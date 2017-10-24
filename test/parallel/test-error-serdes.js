// Flags: --expose-internals
'use strict';
require('../common');
const assert = require('assert');
const errors = require('internal/errors');
const { serializeError, deserializeError } = require('internal/error-serdes');

function cycle(err) {
  return deserializeError(serializeError(err));
}

assert.strictEqual(cycle(0), 0);
assert.strictEqual(cycle(-1), -1);
assert.strictEqual(cycle(1.4), 1.4);
assert.strictEqual(cycle(null), null);
assert.strictEqual(cycle(undefined), undefined);
assert.strictEqual(cycle('foo'), 'foo');

{
  const err = cycle(new Error('foo'));
  assert(err instanceof Error);
  assert.strictEqual(err.name, 'Error');
  assert.strictEqual(err.message, 'foo');
  assert(/^Error: foo\n/.test(err.stack));
}

assert.strictEqual(cycle(new RangeError('foo')).name, 'RangeError');
assert.strictEqual(cycle(new TypeError('foo')).name, 'TypeError');
assert.strictEqual(cycle(new ReferenceError('foo')).name, 'ReferenceError');
assert.strictEqual(cycle(new URIError('foo')).name, 'URIError');
assert.strictEqual(cycle(new EvalError('foo')).name, 'EvalError');
assert.strictEqual(cycle(new SyntaxError('foo')).name, 'SyntaxError');

class SubError extends Error {}

assert.strictEqual(cycle(new SubError('foo')).name, 'Error');

assert.deepStrictEqual(cycle({ message: 'foo' }), { message: 'foo' });
assert.strictEqual(cycle(Function), '[Function: Function]');

{
  const err = new errors.TypeError('ERR_INVALID_ARG_TYPE', 'object', 'object');
  assert(/^TypeError \[ERR_INVALID_ARG_TYPE\]:/.test(err));
  assert.strictEqual(err.name, 'TypeError [ERR_INVALID_ARG_TYPE]');
  assert.strictEqual(err.code, 'ERR_INVALID_ARG_TYPE');
}
