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
const inspect = require('util').inspect;

// test using assert
const qs = require('querystring');

function createWithNoPrototype(properties) {
  const noProto = Object.create(null);
  properties.forEach((property) => {
    noProto[property.key] = property.value;
  });
  return noProto;
}
// folding block, commented to pass gjslint
// {{{
// [ wonkyQS, canonicalQS, obj ]
const qsTestCases = [
  ['__proto__=1',
   '__proto__=1',
   createWithNoPrototype([{ key: '__proto__', value: '1' }])],
  ['__defineGetter__=asdf',
   '__defineGetter__=asdf',
   JSON.parse('{"__defineGetter__":"asdf"}')],
  ['foo=918854443121279438895193',
   'foo=918854443121279438895193',
   { 'foo': '918854443121279438895193' }],
  ['foo=bar', 'foo=bar', { 'foo': 'bar' }],
  ['foo=bar&foo=quux', 'foo=bar&foo=quux', { 'foo': ['bar', 'quux'] }],
  ['foo=1&bar=2', 'foo=1&bar=2', { 'foo': '1', 'bar': '2' }],
  ['my+weird+field=q1%212%22%27w%245%267%2Fz8%29%3F',
   'my%20weird%20field=q1!2%22\'w%245%267%2Fz8)%3F',
   { 'my weird field': 'q1!2"\'w$5&7/z8)?' }],
  ['foo%3Dbaz=bar', 'foo%3Dbaz=bar', { 'foo=baz': 'bar' }],
  ['foo=baz=bar', 'foo=baz%3Dbar', { 'foo': 'baz=bar' }],
  ['str=foo&arr=1&arr=2&arr=3&somenull=&undef=',
   'str=foo&arr=1&arr=2&arr=3&somenull=&undef=',
   { 'str': 'foo',
     'arr': ['1', '2', '3'],
     'somenull': '',
     'undef': '' }],
  [' foo = bar ', '%20foo%20=%20bar%20', { ' foo ': ' bar ' }],
  ['foo=%zx', 'foo=%25zx', { 'foo': '%zx' }],
  ['foo=%EF%BF%BD', 'foo=%EF%BF%BD', { 'foo': '\ufffd' }],
  // See: https://github.com/joyent/node/issues/1707
  ['hasOwnProperty=x&toString=foo&valueOf=bar&__defineGetter__=baz',
   'hasOwnProperty=x&toString=foo&valueOf=bar&__defineGetter__=baz',
   { hasOwnProperty: 'x',
     toString: 'foo',
     valueOf: 'bar',
     __defineGetter__: 'baz' }],
  // See: https://github.com/joyent/node/issues/3058
  ['foo&bar=baz', 'foo=&bar=baz', { foo: '', bar: 'baz' }],
  ['a=b&c&d=e', 'a=b&c=&d=e', { a: 'b', c: '', d: 'e' }],
  ['a=b&c=&d=e', 'a=b&c=&d=e', { a: 'b', c: '', d: 'e' }],
  ['a=b&=c&d=e', 'a=b&=c&d=e', { a: 'b', '': 'c', d: 'e' }],
  ['a=b&=&c=d', 'a=b&=&c=d', { a: 'b', '': '', c: 'd' }],
  ['&&foo=bar&&', 'foo=bar', { foo: 'bar' }],
  ['&', '', {}],
  ['&&&&', '', {}],
  ['&=&', '=', { '': '' }],
  ['&=&=', '=&=', { '': [ '', '' ] }],
  ['=', '=', { '': '' }],
  ['+', '%20=', { ' ': '' }],
  ['+=', '%20=', { ' ': '' }],
  ['+&', '%20=', { ' ': '' }],
  ['=+', '=%20', { '': ' ' }],
  ['+=&', '%20=', { ' ': '' }],
  ['a&&b', 'a=&b=', { 'a': '', 'b': '' }],
  ['a=a&&b=b', 'a=a&b=b', { 'a': 'a', 'b': 'b' }],
  ['&a', 'a=', { 'a': '' }],
  ['&=', '=', { '': '' }],
  ['a&a&', 'a=&a=', { a: [ '', '' ] }],
  ['a&a&a&', 'a=&a=&a=', { a: [ '', '', '' ] }],
  ['a&a&a&a&', 'a=&a=&a=&a=', { a: [ '', '', '', '' ] }],
  ['a=&a=value&a=', 'a=&a=value&a=', { a: [ '', 'value', '' ] }],
  ['foo+bar=baz+quux', 'foo%20bar=baz%20quux', { 'foo bar': 'baz quux' }],
  ['+foo=+bar', '%20foo=%20bar', { ' foo': ' bar' }],
  ['a+', 'a%20=', { 'a ': '' }],
  ['=a+', '=a%20', { '': 'a ' }],
  ['a+&', 'a%20=', { 'a ': '' }],
  ['=a+&', '=a%20', { '': 'a ' }],
  ['%20+', '%20%20=', { '  ': '' }],
  ['=%20+', '=%20%20', { '': '  ' }],
  ['%20+&', '%20%20=', { '  ': '' }],
  ['=%20+&', '=%20%20', { '': '  ' }],
  [null, '', {}],
  [undefined, '', {}]
];

// [ wonkyQS, canonicalQS, obj ]
const qsColonTestCases = [
  ['foo:bar', 'foo:bar', { 'foo': 'bar' }],
  ['foo:bar;foo:quux', 'foo:bar;foo:quux', { 'foo': ['bar', 'quux'] }],
  ['foo:1&bar:2;baz:quux',
   'foo:1%26bar%3A2;baz:quux',
   { 'foo': '1&bar:2', 'baz': 'quux' }],
  ['foo%3Abaz:bar', 'foo%3Abaz:bar', { 'foo:baz': 'bar' }],
  ['foo:baz:bar', 'foo:baz%3Abar', { 'foo': 'baz:bar' }]
];

// [wonkyObj, qs, canonicalObj]
function extendedFunction() {}
extendedFunction.prototype = { a: 'b' };
const qsWeirdObjects = [
  // eslint-disable-next-line no-unescaped-regexp-dot
  [{ regexp: /./g }, 'regexp=', { 'regexp': '' }],
  // eslint-disable-next-line no-unescaped-regexp-dot
  [{ regexp: new RegExp('.', 'g') }, 'regexp=', { 'regexp': '' }],
  [{ fn: function() {} }, 'fn=', { 'fn': '' }],
  [{ fn: new Function('') }, 'fn=', { 'fn': '' }],
  [{ math: Math }, 'math=', { 'math': '' }],
  [{ e: extendedFunction }, 'e=', { 'e': '' }],
  [{ d: new Date() }, 'd=', { 'd': '' }],
  [{ d: Date }, 'd=', { 'd': '' }],
  [
    { f: new Boolean(false), t: new Boolean(true) },
    'f=&t=',
    { 'f': '', 't': '' }
  ],
  [{ f: false, t: true }, 'f=false&t=true', { 'f': 'false', 't': 'true' }],
  [{ n: null }, 'n=', { 'n': '' }],
  [{ nan: NaN }, 'nan=', { 'nan': '' }],
  [{ inf: Infinity }, 'inf=', { 'inf': '' }],
  [{ a: [], b: [] }, '', {}]
];
// }}}

const vm = require('vm');
const foreignObject = vm.runInNewContext('({"foo": ["bar", "baz"]})');

const qsNoMungeTestCases = [
  ['', {}],
  ['foo=bar&foo=baz', { 'foo': ['bar', 'baz'] }],
  ['foo=bar&foo=baz', foreignObject],
  ['blah=burp', { 'blah': 'burp' }],
  ['a=!-._~\'()*', { 'a': '!-._~\'()*' }],
  ['a=abcdefghijklmnopqrstuvwxyz', { 'a': 'abcdefghijklmnopqrstuvwxyz' }],
  ['a=ABCDEFGHIJKLMNOPQRSTUVWXYZ', { 'a': 'ABCDEFGHIJKLMNOPQRSTUVWXYZ' }],
  ['a=0123456789', { 'a': '0123456789' }],
  ['gragh=1&gragh=3&goo=2', { 'gragh': ['1', '3'], 'goo': '2' }],
  ['frappucino=muffin&goat%5B%5D=scone&pond=moose',
   { 'frappucino': 'muffin', 'goat[]': 'scone', 'pond': 'moose' }],
  ['trololol=yes&lololo=no', { 'trololol': 'yes', 'lololo': 'no' }]
];

const qsUnescapeTestCases = [
  ['there is nothing to unescape here',
   'there is nothing to unescape here'],
  ['there%20are%20several%20spaces%20that%20need%20to%20be%20unescaped',
   'there are several spaces that need to be unescaped'],
  ['there%2Qare%0-fake%escaped values in%%%%this%9Hstring',
   'there%2Qare%0-fake%escaped values in%%%%this%9Hstring'],
  ['%20%21%22%23%24%25%26%27%28%29%2A%2B%2C%2D%2E%2F%30%31%32%33%34%35%36%37',
   ' !"#$%&\'()*+,-./01234567']
];

assert.strictEqual('918854443121279438895193',
                   qs.parse('id=918854443121279438895193').id);


function check(actual, expected, input) {
  assert(!(actual instanceof Object));
  const actualKeys = Object.keys(actual).sort();
  const expectedKeys = Object.keys(expected).sort();
  let msg;
  if (typeof input === 'string') {
    msg = `Input: ${inspect(input)}\n` +
          `Actual keys: ${inspect(actualKeys)}\n` +
          `Expected keys: ${inspect(expectedKeys)}`;
  }
  assert.deepStrictEqual(actualKeys, expectedKeys, msg);
  expectedKeys.forEach(function(key) {
    if (typeof input === 'string') {
      msg = `Input: ${inspect(input)}\n` +
            `Key: ${inspect(key)}\n` +
            `Actual value: ${inspect(actual[key])}\n` +
            `Expected value: ${inspect(expected[key])}`;
    } else {
      msg = undefined;
    }
    assert.deepStrictEqual(actual[key], expected[key], msg);
  });
}

// test that the canonical qs is parsed properly.
qsTestCases.forEach(function(testCase) {
  check(qs.parse(testCase[0]), testCase[2], testCase[0]);
});

// test that the colon test cases can do the same
qsColonTestCases.forEach(function(testCase) {
  check(qs.parse(testCase[0], ';', ':'), testCase[2], testCase[0]);
});

// test the weird objects, that they get parsed properly
qsWeirdObjects.forEach(function(testCase) {
  check(qs.parse(testCase[1]), testCase[2], testCase[1]);
});

qsNoMungeTestCases.forEach(function(testCase) {
  assert.deepStrictEqual(testCase[0], qs.stringify(testCase[1], '&', '='));
});

// test the nested qs-in-qs case
{
  const f = qs.parse('a=b&q=x%3Dy%26y%3Dz');
  check(f, createWithNoPrototype([
    { key: 'a', value: 'b' },
    { key: 'q', value: 'x=y&y=z' }
  ]));

  f.q = qs.parse(f.q);
  const expectedInternal = createWithNoPrototype([
    { key: 'x', value: 'y' },
    { key: 'y', value: 'z' }
  ]);
  check(f.q, expectedInternal);
}

// nested in colon
{
  const f = qs.parse('a:b;q:x%3Ay%3By%3Az', ';', ':');
  check(f, createWithNoPrototype([
    { key: 'a', value: 'b' },
    { key: 'q', value: 'x:y;y:z' }
  ]));
  f.q = qs.parse(f.q, ';', ':');
  const expectedInternal = createWithNoPrototype([
    { key: 'x', value: 'y' },
    { key: 'y', value: 'z' }
  ]);
  check(f.q, expectedInternal);
}

// now test stringifying

// basic
qsTestCases.forEach(function(testCase) {
  assert.strictEqual(testCase[1], qs.stringify(testCase[2]));
});

qsColonTestCases.forEach(function(testCase) {
  assert.strictEqual(testCase[1], qs.stringify(testCase[2], ';', ':'));
});

qsWeirdObjects.forEach(function(testCase) {
  assert.strictEqual(testCase[1], qs.stringify(testCase[0]));
});

// invalid surrogate pair throws URIError
common.expectsError(
  () => qs.stringify({ foo: '\udc00' }),
  {
    code: 'ERR_INVALID_URI',
    type: URIError,
    message: 'URI malformed'
  }
);

// coerce numbers to string
assert.strictEqual('foo=0', qs.stringify({ foo: 0 }));
assert.strictEqual('foo=0', qs.stringify({ foo: -0 }));
assert.strictEqual('foo=3', qs.stringify({ foo: 3 }));
assert.strictEqual('foo=-72.42', qs.stringify({ foo: -72.42 }));
assert.strictEqual('foo=', qs.stringify({ foo: NaN }));
assert.strictEqual('foo=', qs.stringify({ foo: Infinity }));

// nested
{
  const f = qs.stringify({
    a: 'b',
    q: qs.stringify({
      x: 'y',
      y: 'z'
    })
  });
  assert.strictEqual(f, 'a=b&q=x%3Dy%26y%3Dz');
}

assert.doesNotThrow(function() {
  qs.parse(undefined);
});

// nested in colon
{
  const f = qs.stringify({
    a: 'b',
    q: qs.stringify({
      x: 'y',
      y: 'z'
    }, ';', ':')
  }, ';', ':');
  assert.strictEqual(f, 'a:b;q:x%3Ay%3By%3Az');
}

// empty string
assert.strictEqual(qs.stringify(), '');
assert.strictEqual(qs.stringify(0), '');
assert.strictEqual(qs.stringify([]), '');
assert.strictEqual(qs.stringify(null), '');
assert.strictEqual(qs.stringify(true), '');

check(qs.parse(), {});

// empty sep
check(qs.parse('a', []), { a: '' });

// empty eq
check(qs.parse('a', null, []), { '': 'a' });

// Test limiting
assert.strictEqual(
  Object.keys(qs.parse('a=1&b=1&c=1', null, null, { maxKeys: 1 })).length,
  1);

// Test limiting with a case that starts from `&`
assert.strictEqual(
  Object.keys(qs.parse('&a', null, null, { maxKeys: 1 })).length,
  0);

// Test removing limit
{
  function testUnlimitedKeys() {
    const query = {};

    for (let i = 0; i < 2000; i++) query[i] = i;

    const url = qs.stringify(query);

    assert.strictEqual(
      Object.keys(qs.parse(url, null, null, { maxKeys: 0 })).length,
      2000);
  }

  testUnlimitedKeys();
}

{
  const b = qs.unescapeBuffer('%d3%f2Ug%1f6v%24%5e%98%cb' +
    '%0d%ac%a2%2f%9d%eb%d8%a2%e6');
  // <Buffer d3 f2 55 67 1f 36 76 24 5e 98 cb 0d ac a2 2f 9d eb d8 a2 e6>
  assert.strictEqual(0xd3, b[0]);
  assert.strictEqual(0xf2, b[1]);
  assert.strictEqual(0x55, b[2]);
  assert.strictEqual(0x67, b[3]);
  assert.strictEqual(0x1f, b[4]);
  assert.strictEqual(0x36, b[5]);
  assert.strictEqual(0x76, b[6]);
  assert.strictEqual(0x24, b[7]);
  assert.strictEqual(0x5e, b[8]);
  assert.strictEqual(0x98, b[9]);
  assert.strictEqual(0xcb, b[10]);
  assert.strictEqual(0x0d, b[11]);
  assert.strictEqual(0xac, b[12]);
  assert.strictEqual(0xa2, b[13]);
  assert.strictEqual(0x2f, b[14]);
  assert.strictEqual(0x9d, b[15]);
  assert.strictEqual(0xeb, b[16]);
  assert.strictEqual(0xd8, b[17]);
  assert.strictEqual(0xa2, b[18]);
  assert.strictEqual(0xe6, b[19]);
}

assert.strictEqual(qs.unescapeBuffer('a+b', true).toString(), 'a b');
assert.strictEqual(qs.unescapeBuffer('a+b').toString(), 'a+b');
assert.strictEqual(qs.unescapeBuffer('a%').toString(), 'a%');
assert.strictEqual(qs.unescapeBuffer('a%2').toString(), 'a%2');
assert.strictEqual(qs.unescapeBuffer('a%20').toString(), 'a ');
assert.strictEqual(qs.unescapeBuffer('a%2g').toString(), 'a%2g');
assert.strictEqual(qs.unescapeBuffer('a%%').toString(), 'a%%');

// Test invalid encoded string
check(qs.parse('%\u0100=%\u0101'), { '%Ā': '%ā' });

// Test custom decode
{
  function demoDecode(str) {
    return str + str;
  }

  check(
    qs.parse('a=a&b=b&c=c', null, null, { decodeURIComponent: demoDecode }),
    { aa: 'aa', bb: 'bb', cc: 'cc' });
  check(
    qs.parse('a=a&b=b&c=c', null, '==', { decodeURIComponent: (str) => str }),
    { 'a=a': '', 'b=b': '', 'c=c': '' });
}

// Test QueryString.unescape
{
  function errDecode(str) {
    throw new Error('To jump to the catch scope');
  }

  check(qs.parse('a=a', null, null, { decodeURIComponent: errDecode }),
        { a: 'a' });
}

// Test custom encode
{
  function demoEncode(str) {
    return str[0];
  }

  const obj = { aa: 'aa', bb: 'bb', cc: 'cc' };
  assert.strictEqual(
    qs.stringify(obj, null, null, { encodeURIComponent: demoEncode }),
    'a=a&b=b&c=c');
}

// Test QueryString.unescapeBuffer
qsUnescapeTestCases.forEach(function(testCase) {
  assert.strictEqual(qs.unescape(testCase[0]), testCase[1]);
  assert.strictEqual(qs.unescapeBuffer(testCase[0]).toString(), testCase[1]);
});

// test overriding .unescape
{
  const prevUnescape = qs.unescape;
  qs.unescape = function(str) {
    return str.replace(/o/g, '_');
  };
  check(
    qs.parse('foo=bor'),
    createWithNoPrototype([{ key: 'f__', value: 'b_r' }]));
  qs.unescape = prevUnescape;
}
// test separator and "equals" parsing order
check(qs.parse('foo&bar', '&', '&'), { foo: '', bar: '' });
