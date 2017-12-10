'use strict';

const common = require('../common');
const { Worker, isMainThread, postMessage } = require('worker');

if (isMainThread) {
  const aWorker = new Worker(__filename);
  aWorker.terminate(common.mustCall());
  aWorker.on('message', common.mustNotCall());
} else {
  while (true)
    postMessage({ hello: 'world' });
}
