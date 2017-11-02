'use strict';

const common = require('../common.js');
const PORT = common.PORT;

const path = require('path');
const fs = require('fs');

const file = path.join(path.resolve(__dirname, '../fixtures'), 'alice.html');

const bench = common.createBenchmark(main, {
  requests: [100, 1000, 10000, 100000],
  streams: [100, 200, 1000],
  clients: [1, 2],
  benchmarker: ['h2load']
}, { flags: ['--no-warnings', '--expose-http2'] });

function main(conf) {
  const n = +conf.requests;
  const m = +conf.streams;
  const c = +conf.clients;
  const http2 = require('http2');
  const server = http2.createServer();
  server.on('stream', (stream) => {
    const out = fs.createReadStream(file);
    stream.respond();
    out.pipe(stream);
    stream.on('error', (err) => {});
  });
  server.listen(PORT, () => {
    bench.http({
      path: '/',
      requests: n,
      maxConcurrentStreams: m,
      clients: c,
      threads: c
    }, () => { server.close(); });
  });
}
