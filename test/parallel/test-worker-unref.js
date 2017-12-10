'use strict';

require('../common');
const assert = require('assert');
const { Worker, isMainThread, postMessage } = require('worker');
let checks = 0;

if (isMainThread) {
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
    postMessage();
  }, 5);
}
