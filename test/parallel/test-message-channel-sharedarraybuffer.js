/*global SharedArrayBuffer*/
'use strict';
// Flags: --harmony-sharedarraybuffer --expose-gc

const common = require('../common');
const assert = require('assert');
const { Worker } = require('worker');

{
  const sharedArrayBuffer = new SharedArrayBuffer(12);
  const local = Buffer.from(sharedArrayBuffer);

  const w = new Worker(`
    process.on('workerMessage', ({ sharedArrayBuffer }) => {
      const local = Buffer.from(sharedArrayBuffer);
      local.write('world!', 6);
      process.postMessage();
    });
  `, { eval: true });
  w.on('message', common.mustCall(() => {
    assert.strictEqual(local.toString(), 'Hello world!');
    global.gc();
    w.terminate();
  }));
  w.postMessage({ sharedArrayBuffer });
  // This would be a race condition if the memory regions were overlapping
  local.write('Hello ');
}
