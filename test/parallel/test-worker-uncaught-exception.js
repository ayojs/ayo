'use strict';
const common = require('../common');
const assert = require('assert');
const { Worker } = require('worker');

if (process.isMainThread) {
  const w = new Worker(__filename);
  w.on('message', common.mustNotCall());
  w.on('error', common.mustCall((err) => {
    // TODO(addaleax): be more specific here
    assert(/foo/.test(err));
  }));
} else {
  throw new Error('foo');
}
