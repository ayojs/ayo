'use strict';

const common = require('../common');
const { Worker, isMainThread, postMessage } = require('worker');

if (isMainThread) {
  const aWorker = new Worker(__filename);
  aWorker.on('message', common.mustCallAtLeast(function() {
    aWorker.postMessage();
    aWorker.postMessage();
    aWorker.postMessage();
    aWorker.postMessage();
    aWorker.terminate(common.mustCall());
  }));
} else {
  require('worker').on('workerMessage', function() {
    while (true)
      postMessage({ hello: 'world' });
  });

  postMessage();
}
