'use strict';
const common = require('../common');
const assert = require('assert');
const { Worker, isMainThread } = require('worker');

if (isMainThread) {
  const w = new Worker(__filename);
  w.on('message', common.mustNotCall());
  w.on('error', common.mustCall((err) => {
    assert(/^Error: foo$/.test(err));
  }));
} else {
  throw new Error('foo');
}
