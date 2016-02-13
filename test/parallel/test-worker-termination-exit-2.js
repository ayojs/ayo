'use strict';

const common = require('../common');
const assert = require('assert');
const { Worker, isMainThread, postMessage } = require('worker');

if (isMainThread) {
  const aWorker = new Worker(__filename, { keepAlive: false });
  aWorker.on('exit', common.mustCall((code) => {
    assert.strictEqual(1337, code);
  }));
  aWorker.on('message', common.mustCall((data) => {
    assert.strictEqual(data, 0);
  }));
} else {
  process.on('beforeExit', () => {
    setInterval(function() {
      postMessage({ hello: 'world' });
    }, 5000);
    setImmediate(function f() {
      postMessage({ hello: 'world' });
      setImmediate(f);
    });
    process.exit(1337);
  });
  let emits = 0;
  process.on('exit', function() {
    postMessage(emits++);
  });
}
