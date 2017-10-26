'use strict';

const common = require('../common');
if (!common.hasCrypto)
  common.skip('missing crypto');
const assert = require('assert');
const h2 = require('http2');

// Tests behaviour of the proxied socket in Http2ServerRequest
// & Http2ServerResponse - specifically property setters

const errMsg = {
  code: 'ERR_HTTP2_NO_SOCKET_MANIPULATION',
  type: Error,
  message: 'HTTP/2 sockets should not be directly manipulated ' +
           '(e.g. read and written)'
};

const server = h2.createServer();

server.on('request', common.mustCall(function(request, response) {
  const noop = () => {};

  assert.strictEqual(request.stream.destroyed, false);
  request.socket.destroyed = true;
  assert.strictEqual(request.stream.destroyed, true);
  request.socket.destroyed = false;

  assert.strictEqual(request.stream.readable, false);
  request.socket.readable = true;
  assert.strictEqual(request.stream.readable, true);

  assert.strictEqual(request.stream.writable, true);
  request.socket.writable = false;
  assert.strictEqual(request.stream.writable, false);

  const realOn = request.stream.on;
  request.socket.on = noop;
  assert.strictEqual(request.stream.on, noop);
  request.stream.on = realOn;

  const realOnce = request.stream.once;
  request.socket.once = noop;
  assert.strictEqual(request.stream.once, noop);
  request.stream.once = realOnce;

  const realEnd = request.stream.end;
  request.socket.end = noop;
  assert.strictEqual(request.stream.end, noop);
  request.socket.end = common.mustCall();
  request.socket.end();
  request.stream.end = realEnd;

  const realEmit = request.stream.emit;
  request.socket.emit = noop;
  assert.strictEqual(request.stream.emit, noop);
  request.stream.emit = realEmit;

  const realDestroy = request.stream.destroy;
  request.socket.destroy = noop;
  assert.strictEqual(request.stream.destroy, noop);
  request.stream.destroy = realDestroy;

  request.socket.setTimeout = noop;
  assert.strictEqual(request.stream.session.setTimeout, noop);

  assert.strictEqual(request.stream.session.socket._isProcessing, undefined);
  request.socket._isProcessing = true;
  assert.strictEqual(request.stream.session.socket._isProcessing, true);

  common.expectsError(() => request.socket.read = noop, errMsg);
  common.expectsError(() => request.socket.write = noop, errMsg);
  common.expectsError(() => request.socket.pause = noop, errMsg);
  common.expectsError(() => request.socket.resume = noop, errMsg);

  request.stream.on('finish', common.mustCall(() => {
    setImmediate(() => {
      request.socket.setTimeout = noop;
      assert.strictEqual(request.stream.setTimeout, noop);

      assert.strictEqual(request.stream._isProcessing, undefined);
      request.socket._isProcessing = true;
      assert.strictEqual(request.stream._isProcessing, true);
    });
  }));
  response.stream.destroy();
}));

server.listen(0, common.mustCall(function() {
  const port = server.address().port;
  const url = `http://localhost:${port}`;
  const client = h2.connect(url, common.mustCall(function() {
    const headers = {
      ':path': '/',
      ':method': 'GET',
      ':scheme': 'http',
      ':authority': `localhost:${port}`
    };
    const request = client.request(headers);
    request.on('end', common.mustCall(() => {
      client.destroy();
      server.close();
    }));
    request.end();
    request.resume();
  }));
}));
