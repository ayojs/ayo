'use strict';

const common = require('../common');
const { Worker } = require('worker');

if (process.isMainThread) {
  const aWorker = new Worker(__filename);
  aWorker.terminate(common.mustCall());
  aWorker.on('message', common.mustNotCall());
} else {
  while (true)
    process.postMessage({ hello: 'world' });
}
