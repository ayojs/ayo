'use strict';

require('../common');
const assert = require('assert');
const { Worker, isMainThread, postMessage } = require('worker');
let checks = 0;

if (isMainThread) {
  const timer = setInterval(function() {}, 1000);
  const aWorker = new Worker(__filename);
  aWorker.on('exit', function() {
    checks++;
  });
  aWorker.on('message', function() {
    checks++;
    setTimeout(function() {
      checks++;
      aWorker.terminate(function() {
        checks++;
        clearInterval(timer);
      });
    }, 5);
  });
  process.on('beforeExit', function() {
    assert.strictEqual(4, checks);
  });
  aWorker.unref();
  aWorker.postMessage();
} else {
  require('worker').on('workerMessage', function() {
    setTimeout(function() {
      postMessage();
    }, 1);
  });
}
