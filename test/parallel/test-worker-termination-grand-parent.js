'use strict';

require('../common');
const assert = require('assert');
const { Worker } = require('worker');
const ids = [];

if (process.isMainThread) {
  const aWorker = new Worker(__filename);
  aWorker.postMessage({
    init: true,
    subWorker: false
  });
  aWorker.on('message', function(data) {
    ids.push(data.id);
    if (ids.length === 4) {
      // Terminating the main worker should terminate its 4 sub-workers
      aWorker.terminate();
    }
  });
  process.on('beforeExit', function() {
    assert.deepStrictEqual([0, 1, 2, 3].sort(), ids.sort());
  });
} else {
  process.on('workerMessage', function(data) {
    if (data.init) {
      if (data.subWorker) {
        subWorker(data.id);
      } else {
        mainWorker();
      }
    }
  });
}

function mainWorker() {
  let l = 4;
  while (l--) {
    const worker = new Worker(__filename);
    worker.postMessage({
      init: true,
      subWorker: true,
      id: l
    });
    worker.on('message', function(payload) {
      process.postMessage(payload);
    });
  }
}

function subWorker(id) {
  process.postMessage({ id: id });
  while (true);
}
