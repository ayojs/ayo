'use strict';

const common = require('../common');
const assert = require('assert');
const { Worker } = require('worker');

if (process.isMainThread) {
  const aWorker = new Worker(__filename);
  aWorker.on('exit', common.mustCall((code) => {
    assert.strictEqual(1337, code);
  }));
  aWorker.on('message', common.mustNotCall());
} else {
  setInterval(function() {
    process.postMessage({ hello: 'world' });
  }, 5000);
  setImmediate(function f() {
    process.postMessage({ hello: 'world' });
    setImmediate(f);
  });
  (function() {
    [1337, 2, 3].map(function(value) {
      process.exit(value);
    });
  })();
}
