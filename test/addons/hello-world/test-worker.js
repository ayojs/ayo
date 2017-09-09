'use strict';
const common = require('../../common');
const assert = require('assert');
const path = require('path');
const { Worker } = require('worker');
const binding = path.resolve(__dirname, `./build/${common.buildType}/binding`);

const w = new Worker(`require(${JSON.stringify(binding)});`, { eval: true });
w.on('error', common.mustCall((err) => {
  assert(String(err).includes(
    'Error: Native modules need to explicitly indicate ' +
    'multi-isolate/multi-thread support by using ' +
    '`NODE_MODULE_WORKER_ENABLED` or `NAPI_MODULE_WORKER_ENABLED` to ' +
    'register themselves.'));
}));
