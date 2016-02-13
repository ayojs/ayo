'use strict';

const common = require('../common');
const { Worker } = require('worker');

if (process.isMainThread) {
  const aWorker = new Worker(__filename);
  aWorker.terminate(common.mustCall());
  aWorker.on('message', common.mustNotCall());
} else {
  setInterval(function() {
    process.postMessage({ hello: 'world' });
  }, 5000);
  setImmediate(function f() {
    process.postMessage({ hello: 'world' });
    setImmediate(f);
  });
}
