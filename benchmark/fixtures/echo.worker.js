'use strict';

const worker = require('worker');

worker.on('workerMessage', (msg) => {
  worker.postMessage(msg);
});
