'use strict';
const common = require('../common');
const assert = require('assert');
const { Worker } = require('worker');
const { Server } = require('net');
const fs = require('fs');

if (process.isMainThread) {
  const w = new Worker(__filename);
  let fd = null;
  w.on('message', common.mustCall((fd_) => {
    assert.strictEqual(typeof fd_, 'number');
    fd = fd_;
  }));
  w.on('exit', common.mustCall((code) => {
    assert.throws(() => fs.fstatSync(fd),
                  common.expectsError({ code: 'EBADF' }));
  }));
} else {
  const server = new Server();
  server.listen(0);
  process.postMessage(server._handle.fd);
  server.unref();
}
