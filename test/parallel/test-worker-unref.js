'use strict';

require('../common');
const assert = require('assert');
const { Worker } = require('worker');
let checks = 0;

if (process.isMainThread) {
  const aWorker = new Worker(__filename);
  aWorker.on('exit', function() {
    checks++;
  });
  aWorker.on('message', function() {
    checks++;
    setTimeout(function() {
      checks++;
      aWorker.terminate();
    }, 5);
  });
  process.on('beforeExit', function() {
    assert.strictEqual(0, checks);
  });
  aWorker.unref();
} else {
  setInterval(function() {
    process.postMessage();
  }, 5);
}
