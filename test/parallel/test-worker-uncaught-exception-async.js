'use strict';
const common = require('../common');
const assert = require('assert');
const { Worker, isMainThread } = require('worker');

if (isMainThread) {
  const w = new Worker(__filename);
  w.on('message', common.mustNotCall());
  w.on('error', common.mustCall((err) => {
    // TODO(addaleax): be more specific here
    assert(/foo/.test(err));
  }));
} else {
  setImmediate(() => {
    throw new Error('foo');
  });
}
