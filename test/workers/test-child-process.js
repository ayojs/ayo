'use strict';

require('../common');

require('./index.js').runTestsInsideWorker([
  'test/parallel/test-child-process-buffering.js',
  'test/parallel/test-child-process-cwd.js',
  // TODO(addaleax): currently leaves a zombie
  // 'test/parallel/test-child-process-detached.js',
  'test/parallel/test-child-process-disconnect.js',
  'test/parallel/test-child-process-env.js',
  'test/parallel/test-child-process-exec-cwd.js',
  'test/parallel/test-child-process-exec-env.js',
  'test/parallel/test-child-process-exec-error.js',
  'test/parallel/test-child-process-exit-code.js',
  'test/parallel/test-child-process-fork3.js',
  // TODO(addaleax): doesn’t handle tmp dirs correctly for workers
  // 'test/parallel/test-child-process-fork-and-spawn.js',
  'test/parallel/test-child-process-fork-close.js',
  'test/parallel/test-child-process-fork-dgram.js',
  // TODO(addaleax): doesn’t handle tmp dirs correctly for workers
  // 'test/parallel/test-child-process-fork-exec-argv.js',
  // TODO(addaleax): doesn’t handle tmp dirs correctly for workers
  // 'test/parallel/test-child-process-fork-exec-path.js',
  'test/parallel/test-child-process-fork.js',
  'test/parallel/test-child-process-fork-net2.js',
  'test/parallel/test-child-process-fork-net.js',
  'test/parallel/test-child-process-fork-ref2.js',
  'test/parallel/test-child-process-fork-ref.js',
  'test/parallel/test-child-process-internal.js',
  'test/parallel/test-child-process-ipc.js',
  'test/parallel/test-child-process-kill.js',
  'test/parallel/test-child-process-send-utf8.js',
  'test/parallel/test-child-process-set-blocking.js',
  'test/parallel/test-child-process-spawn-error.js',
  'test/parallel/test-child-process-spawnsync-env.js',
  'test/parallel/test-child-process-spawnsync-input.js',
  'test/parallel/test-child-process-spawnsync.js',
  'test/parallel/test-child-process-spawnsync-timeout.js',
  'test/parallel/test-child-process-spawn-typeerror.js',
  'test/parallel/test-child-process-stdin-ipc.js',
  'test/parallel/test-child-process-stdin.js',
  'test/parallel/test-child-process-stdio-big-write-end.js',
  // TODO(addaleax): currently fails
  //'test/parallel/test-child-process-stdio.js',
  'test/parallel/test-child-process-stdout-flush-exit.js',
  'test/parallel/test-child-process-stdout-flush.js'
]);
