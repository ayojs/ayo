'use strict';
const common = require('../common');
const { Worker } = require('worker');

const w = new Worker(`
const dns = require('dns');
dns.lookup('nonexistent.org', () => {});
require('worker').postMessage('0');
`, { eval: true });

w.on('message', common.mustCall(() => {
  w.terminate(common.mustCall());
}));
