'use strict';
const common = require('../common');
const assert = require('assert');
const { Worker, isMainThread, postMessage } = require('worker');

if (isMainThread) {
  const w = new Worker(__filename);
  w.on('message', common.mustCall((message) => {
    assert.strictEqual(message, 'Hello, world!');
  }));
} else {
  setImmediate(() => {
    process.nextTick(() => {
      postMessage('Hello, world!');
    });
  });
}
