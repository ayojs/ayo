'use strict';

const common = require('../common');
const assert = require('assert');
const spawn = require('child_process').spawn;
const node = process.execPath;

if (process.argv[2] === 'child') {
  throw new Error('child error');
} else {
  run('', null);
  run('--abort-on-uncaught-exception', ['SIGABRT', 'SIGTRAP', 'SIGILL']);
}

function run(flags, signals) {
  const args = [__filename, 'child'];
  if (flags)
    args.unshift(flags);

  const child = spawn(node, args);
  child.on('exit', common.mustCall(function(code, sig) {
    if (common.isWindows) {
      if (signals)
        assert.strictEqual(code, 0xC0000005);
      else
        assert.strictEqual(code, 1);
    } else if (signals) {
      assert(signals.includes(sig), `Unexpected signal ${sig}`);
    } else {
      assert.strictEqual(sig, null);
    }
  }));
}
