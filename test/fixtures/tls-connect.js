// One shot call to connect a TLS client and server based on options to
// tls.createServer() and tls.connect(), so assertions can be made on both ends
// of the connection.
'use strict';

const common = require('../common');
// Check if Node was compiled --without-ssl and if so exit early
// as the require of tls will otherwise throw an Error.
if (!common.hasCrypto)
  common.skip('missing crypto');

const fixtures = require('../common/fixtures');
const tls = require('tls');
const util = require('util');

exports.assert = require('assert');
exports.debug = util.debuglog('test');
exports.tls = tls;

// Pre-load keys from common fixtures for ease of use by tests.
const keys = exports.keys = {
  agent1: load('agent1', 'ca1'),
  agent2: load('agent2', 'agent2'),
  agent3: load('agent3', 'ca2'),
  agent4: load('agent4', 'ca2'),
  agent5: load('agent5', 'ca2'),
  agent6: load('agent6', 'ca1'),
  agent7: load('agent7', 'fake-cnnic-root'),
  ec: load('ec', 'ec'),
};

function load(cert, issuer) {
  issuer = issuer || cert; // Assume self-signed if no issuer
  const id = {
    key: fixtures.readKey(cert + '-key.pem', 'binary'),
    cert: fixtures.readKey(cert + '-cert.pem', 'binary'),
    ca: fixtures.readKey(issuer + '-cert.pem', 'binary'),
  };
  return id;
}

exports.connect = function connect(options, callback) {
  callback = common.mustCall(callback);

  const server = {};
  const client = {};
  const pair = {
    server: server,
    client: client,
  };

  tls.createServer(options.server, function(conn) {
    server.conn = conn;
    conn.pipe(conn);
    maybeCallback()
  }).listen(0, function() {
    server.server = this;

    const optClient = util._extend({
      port: this.address().port,
    }, options.client);

    tls.connect(optClient)
      .on('secureConnect', function() {
        client.conn = this;
        maybeCallback();
      })
      .on('error', function(err) {
        client.err = err;
        client.conn = this;
        maybeCallback();
      });
  });

  function maybeCallback() {
    if (!callback)
      return;
    if (server.conn && (client.conn || client.err)) {
      const err = pair.client.err || pair.server.err;
      callback(err, pair, cleanup);
      callback = null;

      function cleanup() {
        if (server.server)
          server.server.close();
        if (client.conn)
          client.conn.end();
      }
    }
  }
}
