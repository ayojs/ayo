'use strict';

const common = require('../common');
const { Worker } = require('worker');

if (process.isMainThread) {
  const aWorker = new Worker(__filename);
  aWorker.on('message', common.mustCallAtLeast(function() {
    aWorker.postMessage();
    aWorker.postMessage();
    aWorker.postMessage();
    aWorker.postMessage();
    aWorker.terminate(common.mustCall());
  }));
} else {
  process.on('workerMessage', function() {
    while (true)
      process.postMessage({ hello: 'world' });
  });

  process.postMessage();
}
