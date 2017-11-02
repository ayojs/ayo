#!/usr/bin/env python
# Copyright 2017 the V8 project authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""
Use this script to update V8 in a Node.js checkout.

Requirements:
  - Node.js checkout in which V8 should be updated.
  - V8 checkout at the commit to which Node.js should be updated.

Usage:
  $ update_node.py <path_to_v8> <path_to_node>

  This will synchronize the content of <path_to_node>/deps/v8 with <path_to_v8>,
  and a few V8 dependencies require in Node.js. It will also update .gitignore
  appropriately.

Optional flags:
  --gclient     Run `gclient sync` on the V8 checkout before updating.
  --commit      Create commit with the updated V8 in the Node.js checkout.
  --with-patch  Also include currently staged files in the V8 checkout.
"""

import argparse
import os
import shutil
import subprocess
import sys

TARGET_SUBDIR = os.path.join("deps", "v8")

SUB_REPOSITORIES = [ ["base", "trace_event", "common"],
                     ["testing", "gtest"],
                     ["third_party", "jinja2"],
                     ["third_party", "markupsafe"] ]

DELETE_FROM_GITIGNORE = [ "/base",
                          "/testing/gtest",
                          "/third_party/jinja2",
                          "/third_party/markupsafe" ]

# Node.js requires only a single header file from gtest to build V8.
# Both jinja2 and markupsafe are required to generate part of the inspector.
ADD_TO_GITIGNORE = [ "/testing/gtest/*",
                     "!/testing/gtest/include",
                     "/testing/gtest/include/*",
                     "!/testing/gtest/include/gtest",
                     "/testing/gtest/include/gtest/*",
                     "!/testing/gtest/include/gtest/gtest_prod.h",
                     "!/third_party/jinja2",
                     "!/third_party/markupsafe" ]

def RunGclient(path):
  assert os.path.isdir(path)
  print ">> Running gclient sync"
  subprocess.check_call(["gclient", "sync", "--nohooks"], cwd=path)

def UninitGit(path):
  target = os.path.join(path, ".git")
  if os.path.isdir(target):
    print ">> Cleaning up %s" % path
    shutil.rmtree(target)

def CommitPatch(options):
  """Makes a dummy commit for the changes in the index.

  On trybots, bot_updated applies the patch to the index. We commit it to make
  the fake git clone fetch it into node.js. We can leave the commit, as
  bot_update will ensure a clean state on each run.
  """
  print ">> Committing patch"
  subprocess.check_call(
      ["git", "-c", "user.name=fake", "-c", "user.email=fake@chromium.org",
       "commit", "--allow-empty", "-m", "placeholder-commit"],
      cwd=options.v8_path,
  )

def UpdateTarget(repository, options):
  source = os.path.join(options.v8_path, *repository)
  target = os.path.join(options.node_path, TARGET_SUBDIR, *repository)
  print ">> Updating target directory %s" % target
  print ">>     from active branch at %s" % source
  if not os.path.exists(target):
    os.makedirs(target)
  # Remove possible remnants of previous incomplete runs.
  UninitGit(target)

  git_commands = [
    ["git", "init"],                             # initialize target repo
    ["git", "remote", "add", "origin", source],  # point to the source repo
    ["git", "fetch", "origin", "HEAD"],          # sync to the current branch
    ["git", "reset", "--hard", "FETCH_HEAD"],    # reset to the current branch
    ["git", "clean", "-fd"],                     # delete removed files
  ]
  try:
    for command in git_commands:
      subprocess.check_call(command, cwd=target)
  except:
    raise
  finally:
    UninitGit(target)

def UpdateGitIgnore(options):
  file_name = os.path.join(options.node_path, TARGET_SUBDIR, ".gitignore")
  assert os.path.isfile(file_name)
  print ">> Updating .gitignore with lines"
  with open(file_name) as gitignore:
    content = gitignore.readlines()
  content = [x.strip() for x in content]
  for x in DELETE_FROM_GITIGNORE:
    if x in content:
      print "- %s" % x
      content.remove(x)
  for x in ADD_TO_GITIGNORE:
    if x not in content:
      print "+ %s" % x
      content.append(x)
  content.sort(key=lambda x: x[1:] if x.startswith("!") else x)
  with open(file_name, "w") as gitignore:
    for x in content:
      gitignore.write("%s\n" % x)

def CreateCommit(options):
  print ">> Creating commit."
  # Find git hash from source.
  githash = subprocess.check_output(["git", "rev-parse", "--short", "HEAD"],
                                    cwd=options.v8_path).strip()
  # Create commit at target.
  git_commands = [
    ["git", "checkout", "-b", "update_v8_to_%s" % githash],  # new branch
    ["git", "add", "."],                                     # add files
    ["git", "commit", "-m", "Update V8 to %s" % githash]     # new commit
  ]
  for command in git_commands:
    subprocess.check_call(command, cwd=options.node_path)

def ParseOptions(args):
  parser = argparse.ArgumentParser(description="Update V8 in Node.js")
  parser.add_argument("v8_path", help="Path to V8 checkout")
  parser.add_argument("node_path", help="Path to Node.js checkout")
  parser.add_argument("--gclient", action="store_true", help="Run gclient sync")
  parser.add_argument("--commit", action="store_true", help="Create commit")
  parser.add_argument("--with-patch", action="store_true",
                      help="Apply also staged files")
  options = parser.parse_args(args)
  assert os.path.isdir(options.v8_path)
  options.v8_path = os.path.abspath(options.v8_path)
  assert os.path.isdir(options.node_path)
  options.node_path = os.path.abspath(options.node_path)
  return options

def Main(args):
  options = ParseOptions(args)
  if options.gclient:
    RunGclient(options.v8_path)
  # Commit patch on trybots to main V8 repository.
  if options.with_patch:
    CommitPatch(options)
  # Update main V8 repository.
  UpdateTarget([""], options)
  # Patch .gitignore before updating sub-repositories.
  UpdateGitIgnore(options)
  for repo in SUB_REPOSITORIES:
    UpdateTarget(repo, options)
  if options.commit:
    CreateCommit(options)

if __name__ == "__main__":
  Main(sys.argv[1:])
