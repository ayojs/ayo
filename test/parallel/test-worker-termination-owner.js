'use strict';

const common = require('../common');
const { Worker } = require('worker');

// Test that termination of a worker that is in the middle of processing
// messages from its sub-worker works.

if (process.isMainThread) {
  const worker = new Worker(__filename);
  worker.postMessage({ main: true });
  worker.on('message', common.mustCall(() => {
    worker.terminate(common.mustCall());
  }));
} else {
  process.on('workerMessage', function(data) {
    if (data.main) {
      let messagesReceived = 0;
      const subworker = new Worker(__filename);
      subworker.postMessage({ main: false });
      subworker.on('message', function() {
        messagesReceived++;

        if (messagesReceived === 512)
          process.postMessage();
      });
    } else {
      while (true) process.postMessage();
    }
  });
}
