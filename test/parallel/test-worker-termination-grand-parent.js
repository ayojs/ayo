'use strict';

const common = require('../common');
const assert = require('assert');
const { Worker, isMainThread, postMessage } = require('worker');
const ids = [];

if (isMainThread) {
  const aWorker = new Worker(__filename);
  aWorker.postMessage({
    init: true,
    subWorker: false
  });
  aWorker.on('message', common.mustCall((data) => {
    ids.push(data.id);
    if (ids.length === 4) {
      // Terminating the main worker should terminate its 4 sub-workers
      aWorker.terminate();
    }
  }, 4));
  process.on('beforeExit', function() {
    assert.deepStrictEqual([0, 1, 2, 3].sort(), ids.sort());
  });
} else {
  require('worker').on('workerMessage', function(data) {
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
      postMessage(payload);
    });
  }
}

function subWorker(id) {
  postMessage({ id: id });
  while (true);
}
