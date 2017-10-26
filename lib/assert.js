// Originally from narwhal.js (http://narwhaljs.org)
// Copyright (c) 2009 Thomas Robinson <280north.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the 'Software'), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

'use strict';

const { isDeepEqual, isDeepStrictEqual } =
  require('internal/util/comparisons');
const errors = require('internal/errors');

// The assert module provides functions that throw
// AssertionError's when particular conditions are not met. The
// assert module must conform to the following interface.

const assert = module.exports = ok;

// All of the following functions must throw an AssertionError
// when a corresponding condition is not met, with a message that
// may be undefined if not provided. All assertion methods provide
// both the actual and expected values to the assertion error for
// display purposes.

function innerFail(actual, expected, message, operator, stackStartFunction) {
  if (message instanceof Error) throw message;

  throw new errors.AssertionError({
    message,
    actual,
    expected,
    operator,
    stackStartFunction
  });
}

function fail(actual, expected, message, operator, stackStartFunction) {
  const argsLen = arguments.length;

  if (argsLen === 0) {
    message = 'Failed';
  } else if (argsLen === 1) {
    message = actual;
    actual = undefined;
  } else if (argsLen === 2) {
    operator = '!=';
  }

  innerFail(actual, expected, message, operator, stackStartFunction || fail);
}

assert.fail = fail;

// The AssertionError is defined in internal/error.
// new assert.AssertionError({ message: message,
//                             actual: actual,
//                             expected: expected });
assert.AssertionError = errors.AssertionError;


// Pure assertion tests whether a value is truthy, as determined
// by !!value.
function ok(value, message) {
  if (!value) innerFail(value, true, message, '==', ok);
}
assert.ok = ok;

// The equality assertion tests shallow, coercive equality with ==.
/* eslint-disable no-restricted-properties */
assert.equal = function equal(actual, expected, message) {
  // eslint-disable-next-line eqeqeq
  if (actual != expected) innerFail(actual, expected, message, '==', equal);
};

// The non-equality assertion tests for whether two objects are not
// equal with !=.
assert.notEqual = function notEqual(actual, expected, message) {
  // eslint-disable-next-line eqeqeq
  if (actual == expected) {
    innerFail(actual, expected, message, '!=', notEqual);
  }
};

// The equivalence assertion tests a deep equality relation.
assert.deepEqual = function deepEqual(actual, expected, message) {
  if (!isDeepEqual(actual, expected)) {
    innerFail(actual, expected, message, 'deepEqual', deepEqual);
  }
};
/* eslint-enable */

assert.deepStrictEqual = function deepStrictEqual(actual, expected, message) {
  if (!isDeepStrictEqual(actual, expected)) {
    innerFail(actual, expected, message, 'deepStrictEqual', deepStrictEqual);
  }
};

// The non-equivalence assertion tests for any deep inequality.
assert.notDeepEqual = function notDeepEqual(actual, expected, message) {
  if (isDeepEqual(actual, expected)) {
    innerFail(actual, expected, message, 'notDeepEqual', notDeepEqual);
  }
};

assert.notDeepStrictEqual = notDeepStrictEqual;
function notDeepStrictEqual(actual, expected, message) {
  if (isDeepStrictEqual(actual, expected)) {
    innerFail(actual, expected, message, 'notDeepStrictEqual',
              notDeepStrictEqual);
  }
}

// The strict equality assertion tests strict equality, as determined by ===.
assert.strictEqual = function strictEqual(actual, expected, message) {
  if (actual !== expected) {
    innerFail(actual, expected, message, '===', strictEqual);
  }
};

// The strict non-equality assertion tests for strict inequality, as
// determined by !==.
assert.notStrictEqual = function notStrictEqual(actual, expected, message) {
  if (actual === expected) {
    innerFail(actual, expected, message, '!==', notStrictEqual);
  }
};

function expectedException(actual, expected) {
  if (typeof expected !== 'function') {
    // Should be a RegExp, if not fail hard
    return expected.test(actual);
  }
  // Guard instanceof against arrow functions as they don't have a prototype.
  if (expected.prototype !== undefined && actual instanceof expected) {
    return true;
  }
  if (Error.isPrototypeOf(expected)) {
    return false;
  }
  return expected.call({}, actual) === true;
}

function tryBlock(block) {
  try {
    block();
  } catch (e) {
    return e;
  }
}

function innerThrows(shouldThrow, block, expected, message) {
  var details = '';

  if (typeof block !== 'function') {
    throw new errors.TypeError('ERR_INVALID_ARG_TYPE', 'block', 'function',
                               block);
  }

  if (typeof expected === 'string') {
    message = expected;
    expected = null;
  }

  const actual = tryBlock(block);

  if (shouldThrow === true) {
    if (actual === undefined) {
      if (expected && expected.name) {
        details += ` (${expected.name})`;
      }
      details += message ? `: ${message}` : '.';
      fail(actual, expected, `Missing expected exception${details}`, fail);
    }
    if (expected && expectedException(actual, expected) === false) {
      throw actual;
    }
  } else if (actual !== undefined) {
    if (!expected || expectedException(actual, expected)) {
      details = message ? `: ${message}` : '.';
      fail(actual,
           expected,
           `Got unwanted exception${details}\n${actual.message}`,
           fail);
    }
    throw actual;
  }
}

// Expected to throw an error.
assert.throws = function throws(block, error, message) {
  innerThrows(true, block, error, message);
};

assert.doesNotThrow = function doesNotThrow(block, error, message) {
  innerThrows(false, block, error, message);
};

assert.ifError = function ifError(err) { if (err) throw err; };
