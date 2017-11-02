'use strict';

const common = require('../common');
if (!common.hasCrypto)
  common.skip('missing crypto');
const assert = require('assert');
const h2 = require('http2');
const net = require('net');

// Tests behaviour of the proxied socket in Http2ServerRequest
// & Http2ServerResponse - this proxy socket should mimic the
// behaviour of http1 but against the http2 api & model

const errMsg = {
  code: 'ERR_HTTP2_NO_SOCKET_MANIPULATION',
  type: Error,
  message: 'HTTP/2 sockets should not be directly manipulated ' +
           '(e.g. read and written)'
};

const server = h2.createServer();

server.on('request', common.mustCall(function(request, response) {
  assert.ok(request.socket instanceof net.Socket);
  assert.ok(response.socket instanceof net.Socket);
  assert.strictEqual(request.socket, response.socket);

  assert.ok(request.socket.readable);
  request.resume();
  assert.ok(request.socket.writable);
  assert.strictEqual(request.socket.destroyed, false);

  request.socket.setTimeout(987);
  assert.strictEqual(request.stream.session._idleTimeout, 987);
  request.socket.setTimeout(0);

  common.expectsError(() => request.socket.read(), errMsg);
  common.expectsError(() => request.socket.write(), errMsg);
  common.expectsError(() => request.socket.pause(), errMsg);
  common.expectsError(() => request.socket.resume(), errMsg);

  // should have correct this context for socket methods & getters
  assert.ok(request.socket.address() != null);
  assert.ok(request.socket.remotePort);

  request.on('end', common.mustCall(() => {
    assert.strictEqual(request.socket.readable, false);
    assert.doesNotThrow(() => response.socket.destroy());
  }));
  response.on('finish', common.mustCall(() => {
    assert.ok(request.socket);
    assert.strictEqual(response.socket, undefined);
    assert.ok(request.socket.destroyed);
    assert.strictEqual(request.socket.readable, false);
    process.nextTick(() => {
      assert.strictEqual(request.socket.writable, false);
      server.close();
    });
  }));

  // properties that do not exist on the proxy are retrieved from the socket
  assert.ok(request.socket._server);
  assert.strictEqual(request.socket.connecting, false);

  // socket events are bound and emitted on Http2Stream
  request.socket.on('streamClosed', common.mustCall());
  request.socket.once('streamClosed', common.mustCall());
  request.socket.on('testEvent', common.mustCall());
  request.socket.emit('testEvent');
}));

server.listen(0, common.mustCall(function() {
  const port = server.address().port;
  const url = `http://localhost:${port}`;
  const client = h2.connect(url, common.mustCall(() => {
    const headers = {
      ':path': '/',
      ':method': 'GET',
      ':scheme': 'http',
      ':authority': `localhost:${port}`
    };
    const request = client.request(headers);
    request.on('end', common.mustCall(() => {
      client.destroy();
    }));
    request.end();
    request.resume();
  }));
}));
