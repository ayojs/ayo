// Copyright Joyent, Inc. and other Node contributors.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.

'use strict';
const common = require('../common');
const assert = require('assert');
const child = require('child_process');
const path = require('path');

const f = __filename;
const failures = [];

const slashRE = /\//g;
const backslashRE = /\\/g;

// path.basename tests
assert.strictEqual(path.basename(f), 'test-path.js');
assert.strictEqual(path.basename(f, '.js'), 'test-path');
assert.strictEqual(path.basename('.js', '.js'), '');
assert.strictEqual(path.basename(''), '');
assert.strictEqual(path.basename('/dir/basename.ext'), 'basename.ext');
assert.strictEqual(path.basename('/basename.ext'), 'basename.ext');
assert.strictEqual(path.basename('basename.ext'), 'basename.ext');
assert.strictEqual(path.basename('basename.ext/'), 'basename.ext');
assert.strictEqual(path.basename('basename.ext//'), 'basename.ext');
assert.strictEqual(path.basename('aaa/bbb', '/bbb'), 'bbb');
assert.strictEqual(path.basename('aaa/bbb', 'a/bbb'), 'bbb');
assert.strictEqual(path.basename('aaa/bbb', 'bbb'), 'bbb');
assert.strictEqual(path.basename('aaa/bbb//', 'bbb'), 'bbb');
assert.strictEqual(path.basename('aaa/bbb', 'bb'), 'b');
assert.strictEqual(path.basename('aaa/bbb', 'b'), 'bb');
assert.strictEqual(path.basename('/aaa/bbb', '/bbb'), 'bbb');
assert.strictEqual(path.basename('/aaa/bbb', 'a/bbb'), 'bbb');
assert.strictEqual(path.basename('/aaa/bbb', 'bbb'), 'bbb');
assert.strictEqual(path.basename('/aaa/bbb//', 'bbb'), 'bbb');
assert.strictEqual(path.basename('/aaa/bbb', 'bb'), 'b');
assert.strictEqual(path.basename('/aaa/bbb', 'b'), 'bb');
assert.strictEqual(path.basename('/aaa/bbb'), 'bbb');
assert.strictEqual(path.basename('/aaa/'), 'aaa');
assert.strictEqual(path.basename('/aaa/b'), 'b');
assert.strictEqual(path.basename('/a/b'), 'b');
assert.strictEqual(path.basename('//a'), 'a');

// On Windows a backslash acts as a path separator.
assert.strictEqual(path.win32.basename('\\dir\\basename.ext'), 'basename.ext');
assert.strictEqual(path.win32.basename('\\basename.ext'), 'basename.ext');
assert.strictEqual(path.win32.basename('basename.ext'), 'basename.ext');
assert.strictEqual(path.win32.basename('basename.ext\\'), 'basename.ext');
assert.strictEqual(path.win32.basename('basename.ext\\\\'), 'basename.ext');
assert.strictEqual(path.win32.basename('foo'), 'foo');
assert.strictEqual(path.win32.basename('aaa\\bbb', '\\bbb'), 'bbb');
assert.strictEqual(path.win32.basename('aaa\\bbb', 'a\\bbb'), 'bbb');
assert.strictEqual(path.win32.basename('aaa\\bbb', 'bbb'), 'bbb');
assert.strictEqual(path.win32.basename('aaa\\bbb\\\\\\\\', 'bbb'), 'bbb');
assert.strictEqual(path.win32.basename('aaa\\bbb', 'bb'), 'b');
assert.strictEqual(path.win32.basename('aaa\\bbb', 'b'), 'bb');
assert.strictEqual(path.win32.basename('C:'), '');
assert.strictEqual(path.win32.basename('C:.'), '.');
assert.strictEqual(path.win32.basename('C:\\'), '');
assert.strictEqual(path.win32.basename('C:\\dir\\base.ext'), 'base.ext');
assert.strictEqual(path.win32.basename('C:\\basename.ext'), 'basename.ext');
assert.strictEqual(path.win32.basename('C:basename.ext'), 'basename.ext');
assert.strictEqual(path.win32.basename('C:basename.ext\\'), 'basename.ext');
assert.strictEqual(path.win32.basename('C:basename.ext\\\\'), 'basename.ext');
assert.strictEqual(path.win32.basename('C:foo'), 'foo');
assert.strictEqual(path.win32.basename('file:stream'), 'file:stream');

// On unix a backslash is just treated as any other character.
assert.strictEqual(path.posix.basename('\\dir\\basename.ext'),
                   '\\dir\\basename.ext');
assert.strictEqual(path.posix.basename('\\basename.ext'), '\\basename.ext');
assert.strictEqual(path.posix.basename('basename.ext'), 'basename.ext');
assert.strictEqual(path.posix.basename('basename.ext\\'), 'basename.ext\\');
assert.strictEqual(path.posix.basename('basename.ext\\\\'), 'basename.ext\\\\');
assert.strictEqual(path.posix.basename('foo'), 'foo');

// POSIX filenames may include control characters
// c.f. http://www.dwheeler.com/essays/fixing-unix-linux-filenames.html
const controlCharFilename = `Icon${String.fromCharCode(13)}`;
assert.strictEqual(path.posix.basename(`/a/b/${controlCharFilename}`),
                   controlCharFilename);


// path.dirname tests
assert.strictEqual(path.dirname(f).substr(-13),
                   common.isWindows ? 'test\\parallel' : 'test/parallel');

assert.strictEqual(path.posix.dirname('/a/b/'), '/a');
assert.strictEqual(path.posix.dirname('/a/b'), '/a');
assert.strictEqual(path.posix.dirname('/a'), '/');
assert.strictEqual(path.posix.dirname(''), '.');
assert.strictEqual(path.posix.dirname('/'), '/');
assert.strictEqual(path.posix.dirname('////'), '/');
assert.strictEqual(path.posix.dirname('//a'), '//');
assert.strictEqual(path.posix.dirname('foo'), '.');

assert.strictEqual(path.win32.dirname('c:\\'), 'c:\\');
assert.strictEqual(path.win32.dirname('c:\\foo'), 'c:\\');
assert.strictEqual(path.win32.dirname('c:\\foo\\'), 'c:\\');
assert.strictEqual(path.win32.dirname('c:\\foo\\bar'), 'c:\\foo');
assert.strictEqual(path.win32.dirname('c:\\foo\\bar\\'), 'c:\\foo');
assert.strictEqual(path.win32.dirname('c:\\foo\\bar\\baz'), 'c:\\foo\\bar');
assert.strictEqual(path.win32.dirname('\\'), '\\');
assert.strictEqual(path.win32.dirname('\\foo'), '\\');
assert.strictEqual(path.win32.dirname('\\foo\\'), '\\');
assert.strictEqual(path.win32.dirname('\\foo\\bar'), '\\foo');
assert.strictEqual(path.win32.dirname('\\foo\\bar\\'), '\\foo');
assert.strictEqual(path.win32.dirname('\\foo\\bar\\baz'), '\\foo\\bar');
assert.strictEqual(path.win32.dirname('c:'), 'c:');
assert.strictEqual(path.win32.dirname('c:foo'), 'c:');
assert.strictEqual(path.win32.dirname('c:foo\\'), 'c:');
assert.strictEqual(path.win32.dirname('c:foo\\bar'), 'c:foo');
assert.strictEqual(path.win32.dirname('c:foo\\bar\\'), 'c:foo');
assert.strictEqual(path.win32.dirname('c:foo\\bar\\baz'), 'c:foo\\bar');
assert.strictEqual(path.win32.dirname('file:stream'), '.');
assert.strictEqual(path.win32.dirname('dir\\file:stream'), 'dir');
assert.strictEqual(path.win32.dirname('\\\\unc\\share'),
                   '\\\\unc\\share');
assert.strictEqual(path.win32.dirname('\\\\unc\\share\\foo'),
                   '\\\\unc\\share\\');
assert.strictEqual(path.win32.dirname('\\\\unc\\share\\foo\\'),
                   '\\\\unc\\share\\');
assert.strictEqual(path.win32.dirname('\\\\unc\\share\\foo\\bar'),
                   '\\\\unc\\share\\foo');
assert.strictEqual(path.win32.dirname('\\\\unc\\share\\foo\\bar\\'),
                   '\\\\unc\\share\\foo');
assert.strictEqual(path.win32.dirname('\\\\unc\\share\\foo\\bar\\baz'),
                   '\\\\unc\\share\\foo\\bar');
assert.strictEqual(path.win32.dirname('/a/b/'), '/a');
assert.strictEqual(path.win32.dirname('/a/b'), '/a');
assert.strictEqual(path.win32.dirname('/a'), '/');
assert.strictEqual(path.win32.dirname(''), '.');
assert.strictEqual(path.win32.dirname('/'), '/');
assert.strictEqual(path.win32.dirname('////'), '/');
assert.strictEqual(path.win32.dirname('foo'), '.');


// path.extname tests
[
  [f, '.js'],
  ['', ''],
  ['/path/to/file', ''],
  ['/path/to/file.ext', '.ext'],
  ['/path.to/file.ext', '.ext'],
  ['/path.to/file', ''],
  ['/path.to/.file', ''],
  ['/path.to/.file.ext', '.ext'],
  ['/path/to/f.ext', '.ext'],
  ['/path/to/..ext', '.ext'],
  ['/path/to/..', ''],
  ['file', ''],
  ['file.ext', '.ext'],
  ['.file', ''],
  ['.file.ext', '.ext'],
  ['/file', ''],
  ['/file.ext', '.ext'],
  ['/.file', ''],
  ['/.file.ext', '.ext'],
  ['.path/file.ext', '.ext'],
  ['file.ext.ext', '.ext'],
  ['file.', '.'],
  ['.', ''],
  ['./', ''],
  ['.file.ext', '.ext'],
  ['.file', ''],
  ['.file.', '.'],
  ['.file..', '.'],
  ['..', ''],
  ['../', ''],
  ['..file.ext', '.ext'],
  ['..file', '.file'],
  ['..file.', '.'],
  ['..file..', '.'],
  ['...', '.'],
  ['...ext', '.ext'],
  ['....', '.'],
  ['file.ext/', '.ext'],
  ['file.ext//', '.ext'],
  ['file/', ''],
  ['file//', ''],
  ['file./', '.'],
  ['file.//', '.'],
].forEach((test) => {
  const expected = test[1];
  [path.posix.extname, path.win32.extname].forEach((extname) => {
    let input = test[0];
    let os;
    if (extname === path.win32.extname) {
      input = input.replace(slashRE, '\\');
      os = 'win32';
    } else {
      os = 'posix';
    }
    const actual = extname(input);
    const message = `path.${os}.extname(${JSON.stringify(input)})\n  expect=${
      JSON.stringify(expected)}\n  actual=${JSON.stringify(actual)}`;
    if (actual !== expected)
      failures.push(`\n${message}`);
  });
  {
    const input = `C:${test[0].replace(slashRE, '\\')}`;
    const actual = path.win32.extname(input);
    const message = `path.win32.extname(${JSON.stringify(input)})\n  expect=${
      JSON.stringify(expected)}\n  actual=${JSON.stringify(actual)}`;
    if (actual !== expected)
      failures.push(`\n${message}`);
  }
});
assert.strictEqual(failures.length, 0, failures.join(''));

// On Windows, backslash is a path separator.
assert.strictEqual(path.win32.extname('.\\'), '');
assert.strictEqual(path.win32.extname('..\\'), '');
assert.strictEqual(path.win32.extname('file.ext\\'), '.ext');
assert.strictEqual(path.win32.extname('file.ext\\\\'), '.ext');
assert.strictEqual(path.win32.extname('file\\'), '');
assert.strictEqual(path.win32.extname('file\\\\'), '');
assert.strictEqual(path.win32.extname('file.\\'), '.');
assert.strictEqual(path.win32.extname('file.\\\\'), '.');

// On *nix, backslash is a valid name component like any other character.
assert.strictEqual(path.posix.extname('.\\'), '');
assert.strictEqual(path.posix.extname('..\\'), '.\\');
assert.strictEqual(path.posix.extname('file.ext\\'), '.ext\\');
assert.strictEqual(path.posix.extname('file.ext\\\\'), '.ext\\\\');
assert.strictEqual(path.posix.extname('file\\'), '');
assert.strictEqual(path.posix.extname('file\\\\'), '');
assert.strictEqual(path.posix.extname('file.\\'), '.\\');
assert.strictEqual(path.posix.extname('file.\\\\'), '.\\\\');


// path.join tests
const joinTests = [
  [ [path.posix.join, path.win32.join],
    // arguments                     result
    [[['.', 'x/b', '..', '/b/c.js'], 'x/b/c.js'],
     [[], '.'],
     [['/.', 'x/b', '..', '/b/c.js'], '/x/b/c.js'],
     [['/foo', '../../../bar'], '/bar'],
     [['foo', '../../../bar'], '../../bar'],
     [['foo/', '../../../bar'], '../../bar'],
     [['foo/x', '../../../bar'], '../bar'],
     [['foo/x', './bar'], 'foo/x/bar'],
     [['foo/x/', './bar'], 'foo/x/bar'],
     [['foo/x/', '.', 'bar'], 'foo/x/bar'],
     [['./'], './'],
     [['.', './'], './'],
     [['.', '.', '.'], '.'],
     [['.', './', '.'], '.'],
     [['.', '/./', '.'], '.'],
     [['.', '/////./', '.'], '.'],
     [['.'], '.'],
     [['', '.'], '.'],
     [['', 'foo'], 'foo'],
     [['foo', '/bar'], 'foo/bar'],
     [['', '/foo'], '/foo'],
     [['', '', '/foo'], '/foo'],
     [['', '', 'foo'], 'foo'],
     [['foo', ''], 'foo'],
     [['foo/', ''], 'foo/'],
     [['foo', '', '/bar'], 'foo/bar'],
     [['./', '..', '/foo'], '../foo'],
     [['./', '..', '..', '/foo'], '../../foo'],
     [['.', '..', '..', '/foo'], '../../foo'],
     [['', '..', '..', '/foo'], '../../foo'],
     [['/'], '/'],
     [['/', '.'], '/'],
     [['/', '..'], '/'],
     [['/', '..', '..'], '/'],
     [[''], '.'],
     [['', ''], '.'],
     [[' /foo'], ' /foo'],
     [[' ', 'foo'], ' /foo'],
     [[' ', '.'], ' '],
     [[' ', '/'], ' /'],
     [[' ', ''], ' '],
     [['/', 'foo'], '/foo'],
     [['/', '/foo'], '/foo'],
     [['/', '//foo'], '/foo'],
     [['/', '', '/foo'], '/foo'],
     [['', '/', 'foo'], '/foo'],
     [['', '/', '/foo'], '/foo']
    ]
  ]
];

// Windows-specific join tests
joinTests.push([
  path.win32.join,
  joinTests[0][1].slice(0).concat(
    [// arguments                     result
      // UNC path expected
      [['//foo/bar'], '\\\\foo\\bar\\'],
      [['\\/foo/bar'], '\\\\foo\\bar\\'],
      [['\\\\foo/bar'], '\\\\foo\\bar\\'],
      // UNC path expected - server and share separate
      [['//foo', 'bar'], '\\\\foo\\bar\\'],
      [['//foo/', 'bar'], '\\\\foo\\bar\\'],
      [['//foo', '/bar'], '\\\\foo\\bar\\'],
      // UNC path expected - questionable
      [['//foo', '', 'bar'], '\\\\foo\\bar\\'],
      [['//foo/', '', 'bar'], '\\\\foo\\bar\\'],
      [['//foo/', '', '/bar'], '\\\\foo\\bar\\'],
      // UNC path expected - even more questionable
      [['', '//foo', 'bar'], '\\\\foo\\bar\\'],
      [['', '//foo/', 'bar'], '\\\\foo\\bar\\'],
      [['', '//foo/', '/bar'], '\\\\foo\\bar\\'],
      // No UNC path expected (no double slash in first component)
      [['\\', 'foo/bar'], '\\foo\\bar'],
      [['\\', '/foo/bar'], '\\foo\\bar'],
      [['', '/', '/foo/bar'], '\\foo\\bar'],
      // No UNC path expected (no non-slashes in first component -
      // questionable)
      [['//', 'foo/bar'], '\\foo\\bar'],
      [['//', '/foo/bar'], '\\foo\\bar'],
      [['\\\\', '/', '/foo/bar'], '\\foo\\bar'],
      [['//'], '/'],
      // No UNC path expected (share name missing - questionable).
      [['//foo'], '\\foo'],
      [['//foo/'], '\\foo\\'],
      [['//foo', '/'], '\\foo\\'],
      [['//foo', '', '/'], '\\foo\\'],
      // No UNC path expected (too many leading slashes - questionable)
      [['///foo/bar'], '\\foo\\bar'],
      [['////foo', 'bar'], '\\foo\\bar'],
      [['\\\\\\/foo/bar'], '\\foo\\bar'],
      // Drive-relative vs drive-absolute paths. This merely describes the
      // status quo, rather than being obviously right
      [['c:'], 'c:.'],
      [['c:.'], 'c:.'],
      [['c:', ''], 'c:.'],
      [['', 'c:'], 'c:.'],
      [['c:.', '/'], 'c:.\\'],
      [['c:.', 'file'], 'c:file'],
      [['c:', '/'], 'c:\\'],
      [['c:', 'file'], 'c:\\file']
    ]
  )
]);
joinTests.forEach((test) => {
  if (!Array.isArray(test[0]))
    test[0] = [test[0]];
  test[0].forEach((join) => {
    test[1].forEach((test) => {
      const actual = join.apply(null, test[0]);
      const expected = test[1];
      // For non-Windows specific tests with the Windows join(), we need to try
      // replacing the slashes since the non-Windows specific tests' `expected`
      // use forward slashes
      let actualAlt;
      let os;
      if (join === path.win32.join) {
        actualAlt = actual.replace(backslashRE, '/');
        os = 'win32';
      } else {
        os = 'posix';
      }
      const message =
        `path.${os}.join(${test[0].map(JSON.stringify).join(',')})\n  expect=${
          JSON.stringify(expected)}\n  actual=${JSON.stringify(actual)}`;
      if (actual !== expected && actualAlt !== expected)
        failures.push(`\n${message}`);
    });
  });
});
assert.strictEqual(failures.length, 0, failures.join(''));


// Test thrown TypeErrors
const typeErrorTests = [true, false, 7, null, {}, undefined, [], NaN];

function fail(fn) {
  const args = Array.from(arguments).slice(1);

  assert.throws(() => {
    fn.apply(null, args);
  }, common.expectsError({ code: 'ERR_INVALID_ARG_TYPE', type: TypeError }));
}

typeErrorTests.forEach((test) => {
  [path.posix, path.win32].forEach((namespace) => {
    fail(namespace.join, test);
    fail(namespace.resolve, test);
    fail(namespace.normalize, test);
    fail(namespace.isAbsolute, test);
    fail(namespace.relative, test, 'foo');
    fail(namespace.relative, 'foo', test);
    fail(namespace.parse, test);
    fail(namespace.dirname, test);
    fail(namespace.basename, test);
    fail(namespace.extname, test);

    // undefined is a valid value as the second argument to basename
    if (test !== undefined) {
      fail(namespace.basename, 'foo', test);
    }
  });
});


// path.normalize tests
assert.strictEqual(path.win32.normalize('./fixtures///b/../b/c.js'),
                   'fixtures\\b\\c.js');
assert.strictEqual(path.win32.normalize('/foo/../../../bar'), '\\bar');
assert.strictEqual(path.win32.normalize('a//b//../b'), 'a\\b');
assert.strictEqual(path.win32.normalize('a//b//./c'), 'a\\b\\c');
assert.strictEqual(path.win32.normalize('a//b//.'), 'a\\b');
assert.strictEqual(path.win32.normalize('//server/share/dir/file.ext'),
                   '\\\\server\\share\\dir\\file.ext');
assert.strictEqual(path.win32.normalize('/a/b/c/../../../x/y/z'), '\\x\\y\\z');
assert.strictEqual(path.win32.normalize('C:'), 'C:.');
assert.strictEqual(path.win32.normalize('C:..\\abc'), 'C:..\\abc');
assert.strictEqual(path.win32.normalize('C:..\\..\\abc\\..\\def'),
                   'C:..\\..\\def');
assert.strictEqual(path.win32.normalize('C:\\.'), 'C:\\');
assert.strictEqual(path.win32.normalize('file:stream'), 'file:stream');
assert.strictEqual(path.win32.normalize('bar\\foo..\\..\\'), 'bar\\');
assert.strictEqual(path.win32.normalize('bar\\foo..\\..'), 'bar');
assert.strictEqual(path.win32.normalize('bar\\foo..\\..\\baz'), 'bar\\baz');
assert.strictEqual(path.win32.normalize('bar\\foo..\\'), 'bar\\foo..\\');
assert.strictEqual(path.win32.normalize('bar\\foo..'), 'bar\\foo..');

assert.strictEqual(path.posix.normalize('./fixtures///b/../b/c.js'),
                   'fixtures/b/c.js');
assert.strictEqual(path.posix.normalize('/foo/../../../bar'), '/bar');
assert.strictEqual(path.posix.normalize('a//b//../b'), 'a/b');
assert.strictEqual(path.posix.normalize('a//b//./c'), 'a/b/c');
assert.strictEqual(path.posix.normalize('a//b//.'), 'a/b');
assert.strictEqual(path.posix.normalize('/a/b/c/../../../x/y/z'), '/x/y/z');
assert.strictEqual(path.posix.normalize('///..//./foo/.//bar'), '/foo/bar');
assert.strictEqual(path.posix.normalize('bar/foo../../'), 'bar/');
assert.strictEqual(path.posix.normalize('bar/foo../..'), 'bar');
assert.strictEqual(path.posix.normalize('bar/foo../../baz'), 'bar/baz');
assert.strictEqual(path.posix.normalize('bar/foo../'), 'bar/foo../');
assert.strictEqual(path.posix.normalize('bar/foo..'), 'bar/foo..');


// path.resolve tests
const resolveTests = [
  [ path.win32.resolve,
    // arguments                               result
    [[['c:/blah\\blah', 'd:/games', 'c:../a'], 'c:\\blah\\a'],
     [['c:/ignore', 'd:\\a/b\\c/d', '\\e.exe'], 'd:\\e.exe'],
     [['c:/ignore', 'c:/some/file'], 'c:\\some\\file'],
     [['d:/ignore', 'd:some/dir//'], 'd:\\ignore\\some\\dir'],
     [['.'], process.cwd()],
     [['//server/share', '..', 'relative\\'], '\\\\server\\share\\relative'],
     [['c:/', '//'], 'c:\\'],
     [['c:/', '//dir'], 'c:\\dir'],
     [['c:/', '//server/share'], '\\\\server\\share\\'],
     [['c:/', '//server//share'], '\\\\server\\share\\'],
     [['c:/', '///some//dir'], 'c:\\some\\dir'],
     [['C:\\foo\\tmp.3\\', '..\\tmp.3\\cycles\\root.js'],
      'C:\\foo\\tmp.3\\cycles\\root.js']
    ]
  ],
  [ path.posix.resolve,
    // arguments                    result
    [[['/var/lib', '../', 'file/'], '/var/file'],
     [['/var/lib', '/../', 'file/'], '/file'],
     [['a/b/c/', '../../..'], process.cwd()],
     [['.'], process.cwd()],
     [['/some/dir', '.', '/absolute/'], '/absolute'],
     [['/foo/tmp.3/', '../tmp.3/cycles/root.js'], '/foo/tmp.3/cycles/root.js']
    ]
  ]
];
resolveTests.forEach((test) => {
  const resolve = test[0];
  test[1].forEach((test) => {
    const actual = resolve.apply(null, test[0]);
    let actualAlt;
    const os = resolve === path.win32.resolve ? 'win32' : 'posix';
    if (resolve === path.win32.resolve && !common.isWindows)
      actualAlt = actual.replace(backslashRE, '/');
    else if (resolve !== path.win32.resolve && common.isWindows)
      actualAlt = actual.replace(slashRE, '\\');

    const expected = test[1];
    const message =
      `path.${os}.resolve(${test[0].map(JSON.stringify).join(',')})\n  expect=${
        JSON.stringify(expected)}\n  actual=${JSON.stringify(actual)}`;
    if (actual !== expected && actualAlt !== expected)
      failures.push(`\n${message}`);
  });
});
assert.strictEqual(failures.length, 0, failures.join(''));

if (common.isWindows) {
  // Test resolving the current Windows drive letter from a spawned process.
  // See https://github.com/nodejs/node/issues/7215
  const currentDriveLetter = path.parse(process.cwd()).root.substring(0, 2);
  const resolveFixture = path.join(common.fixturesDir, 'path-resolve.js');
  const spawnResult = child.spawnSync(
    process.argv[0], [resolveFixture, currentDriveLetter]);
  const resolvedPath = spawnResult.stdout.toString().trim();
  assert.strictEqual(resolvedPath.toLowerCase(), process.cwd().toLowerCase());
}


// path.isAbsolute tests
assert.strictEqual(path.win32.isAbsolute('/'), true);
assert.strictEqual(path.win32.isAbsolute('//'), true);
assert.strictEqual(path.win32.isAbsolute('//server'), true);
assert.strictEqual(path.win32.isAbsolute('//server/file'), true);
assert.strictEqual(path.win32.isAbsolute('\\\\server\\file'), true);
assert.strictEqual(path.win32.isAbsolute('\\\\server'), true);
assert.strictEqual(path.win32.isAbsolute('\\\\'), true);
assert.strictEqual(path.win32.isAbsolute('c'), false);
assert.strictEqual(path.win32.isAbsolute('c:'), false);
assert.strictEqual(path.win32.isAbsolute('c:\\'), true);
assert.strictEqual(path.win32.isAbsolute('c:/'), true);
assert.strictEqual(path.win32.isAbsolute('c://'), true);
assert.strictEqual(path.win32.isAbsolute('C:/Users/'), true);
assert.strictEqual(path.win32.isAbsolute('C:\\Users\\'), true);
assert.strictEqual(path.win32.isAbsolute('C:cwd/another'), false);
assert.strictEqual(path.win32.isAbsolute('C:cwd\\another'), false);
assert.strictEqual(path.win32.isAbsolute('directory/directory'), false);
assert.strictEqual(path.win32.isAbsolute('directory\\directory'), false);

assert.strictEqual(path.posix.isAbsolute('/home/foo'), true);
assert.strictEqual(path.posix.isAbsolute('/home/foo/..'), true);
assert.strictEqual(path.posix.isAbsolute('bar/'), false);
assert.strictEqual(path.posix.isAbsolute('./baz'), false);


// path.relative tests
const relativeTests = [
  [ path.win32.relative,
    // arguments                     result
    [['c:/blah\\blah', 'd:/games', 'd:\\games'],
     ['c:/aaaa/bbbb', 'c:/aaaa', '..'],
     ['c:/aaaa/bbbb', 'c:/cccc', '..\\..\\cccc'],
     ['c:/aaaa/bbbb', 'c:/aaaa/bbbb', ''],
     ['c:/aaaa/bbbb', 'c:/aaaa/cccc', '..\\cccc'],
     ['c:/aaaa/', 'c:/aaaa/cccc', 'cccc'],
     ['c:/', 'c:\\aaaa\\bbbb', 'aaaa\\bbbb'],
     ['c:/aaaa/bbbb', 'd:\\', 'd:\\'],
     ['c:/AaAa/bbbb', 'c:/aaaa/bbbb', ''],
     ['c:/aaaaa/', 'c:/aaaa/cccc', '..\\aaaa\\cccc'],
     ['C:\\foo\\bar\\baz\\quux', 'C:\\', '..\\..\\..\\..'],
     ['C:\\foo\\test', 'C:\\foo\\test\\bar\\package.json', 'bar\\package.json'],
     ['C:\\foo\\bar\\baz-quux', 'C:\\foo\\bar\\baz', '..\\baz'],
     ['C:\\foo\\bar\\baz', 'C:\\foo\\bar\\baz-quux', '..\\baz-quux'],
     ['\\\\foo\\bar', '\\\\foo\\bar\\baz', 'baz'],
     ['\\\\foo\\bar\\baz', '\\\\foo\\bar', '..'],
     ['\\\\foo\\bar\\baz-quux', '\\\\foo\\bar\\baz', '..\\baz'],
     ['\\\\foo\\bar\\baz', '\\\\foo\\bar\\baz-quux', '..\\baz-quux'],
     ['C:\\baz-quux', 'C:\\baz', '..\\baz'],
     ['C:\\baz', 'C:\\baz-quux', '..\\baz-quux'],
     ['\\\\foo\\baz-quux', '\\\\foo\\baz', '..\\baz'],
     ['\\\\foo\\baz', '\\\\foo\\baz-quux', '..\\baz-quux'],
     ['C:\\baz', '\\\\foo\\bar\\baz', '\\\\foo\\bar\\baz'],
     ['\\\\foo\\bar\\baz', 'C:\\baz', 'C:\\baz']
    ]
  ],
  [ path.posix.relative,
    // arguments          result
    [['/var/lib', '/var', '..'],
     ['/var/lib', '/bin', '../../bin'],
     ['/var/lib', '/var/lib', ''],
     ['/var/lib', '/var/apache', '../apache'],
     ['/var/', '/var/lib', 'lib'],
     ['/', '/var/lib', 'var/lib'],
     ['/foo/test', '/foo/test/bar/package.json', 'bar/package.json'],
     ['/Users/a/web/b/test/mails', '/Users/a/web/b', '../..'],
     ['/foo/bar/baz-quux', '/foo/bar/baz', '../baz'],
     ['/foo/bar/baz', '/foo/bar/baz-quux', '../baz-quux'],
     ['/baz-quux', '/baz', '../baz'],
     ['/baz', '/baz-quux', '../baz-quux']
    ]
  ]
];
relativeTests.forEach((test) => {
  const relative = test[0];
  test[1].forEach((test) => {
    const actual = relative(test[0], test[1]);
    const expected = test[2];
    const os = relative === path.win32.relative ? 'win32' : 'posix';
    const message = `path.${os}.relative(${
      test.slice(0, 2).map(JSON.stringify).join(',')})\n  expect=${
      JSON.stringify(expected)}\n  actual=${JSON.stringify(actual)}`;
    if (actual !== expected)
      failures.push(`\n${message}`);
  });
});
assert.strictEqual(failures.length, 0, failures.join(''));


// path.sep tests
// windows
assert.strictEqual(path.win32.sep, '\\');
// posix
assert.strictEqual(path.posix.sep, '/');

// path.delimiter tests
// windows
assert.strictEqual(path.win32.delimiter, ';');
// posix
assert.strictEqual(path.posix.delimiter, ':');


// path._makeLong tests
const emptyObj = {};
assert.strictEqual(path.posix._makeLong('/foo/bar'), '/foo/bar');
assert.strictEqual(path.posix._makeLong('foo/bar'), 'foo/bar');
assert.strictEqual(path.posix._makeLong(null), null);
assert.strictEqual(path.posix._makeLong(true), true);
assert.strictEqual(path.posix._makeLong(1), 1);
assert.strictEqual(path.posix._makeLong(), undefined);
assert.strictEqual(path.posix._makeLong(emptyObj), emptyObj);
if (common.isWindows) {
  // These tests cause resolve() to insert the cwd, so we cannot test them from
  // non-Windows platforms (easily)
  assert.strictEqual(path.win32._makeLong('foo\\bar').toLowerCase(),
                     `\\\\?\\${process.cwd().toLowerCase()}\\foo\\bar`);
  assert.strictEqual(path.win32._makeLong('foo/bar').toLowerCase(),
                     `\\\\?\\${process.cwd().toLowerCase()}\\foo\\bar`);
  const currentDeviceLetter = path.parse(process.cwd()).root.substring(0, 2);
  assert.strictEqual(path.win32._makeLong(currentDeviceLetter).toLowerCase(),
                     `\\\\?\\${process.cwd().toLowerCase()}`);
  assert.strictEqual(path.win32._makeLong('C').toLowerCase(),
                     `\\\\?\\${process.cwd().toLowerCase()}\\c`);
}
assert.strictEqual(path.win32._makeLong('C:\\foo'), '\\\\?\\C:\\foo');
assert.strictEqual(path.win32._makeLong('C:/foo'), '\\\\?\\C:\\foo');
assert.strictEqual(path.win32._makeLong('\\\\foo\\bar'),
                   '\\\\?\\UNC\\foo\\bar\\');
assert.strictEqual(path.win32._makeLong('//foo//bar'),
                   '\\\\?\\UNC\\foo\\bar\\');
assert.strictEqual(path.win32._makeLong('\\\\?\\foo'), '\\\\?\\foo');
assert.strictEqual(path.win32._makeLong(null), null);
assert.strictEqual(path.win32._makeLong(true), true);
assert.strictEqual(path.win32._makeLong(1), 1);
assert.strictEqual(path.win32._makeLong(), undefined);
assert.strictEqual(path.win32._makeLong(emptyObj), emptyObj);


if (common.isWindows)
  assert.deepStrictEqual(path, path.win32, 'should be win32 path module');
else
  assert.deepStrictEqual(path, path.posix, 'should be posix path module');
