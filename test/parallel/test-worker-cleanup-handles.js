'use strict';
const common = require('../common');
const assert = require('assert');
const { Worker, isMainThread, postMessage } = require('worker');
const { Server } = require('net');
const fs = require('fs');

if (isMainThread) {
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
  postMessage(server._handle.fd);
  server.unref();
}
