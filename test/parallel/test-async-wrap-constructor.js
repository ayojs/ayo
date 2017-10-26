'use strict';

// This tests that using falsy values in createHook throws an error.

const common = require('../common');
const assert = require('assert');
const async_hooks = require('async_hooks');

for (const badArg of [0, 1, false, true, null, 'hello']) {
  const hookNames = ['init', 'before', 'after', 'destroy', 'promiseResolve'];
  for (const field of hookNames) {
    assert.throws(() => {
      async_hooks.createHook({ [field]: badArg });
    }, common.expectsError({
      code: 'ERR_ASYNC_CALLBACK',
      type: TypeError,
    }));
  }
}
