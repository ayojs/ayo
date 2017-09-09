'use strict';
const common = require('../../common');

if (common.isWindows)
  common.skip('dlopen global symbol loading is not supported on this os.');

if (common.isAIX)
  common.skip('this test does not pass on AIX.');

const assert = require('assert');
const path = require('path');
const os = require('os');

const bindingPath = require.resolve(`./build/${common.buildType}/binding`);
process.dlopen(module, bindingPath,
               os.constants.dlopen.RTLD_NOW | os.constants.dlopen.RTLD_GLOBAL);
module.exports.load(path.dirname(bindingPath) + '/ping.so');
assert.strictEqual(module.exports.ping(), 'pong');

assert.doesNotThrow(() => require(`./build/${common.buildType}/binding`));
