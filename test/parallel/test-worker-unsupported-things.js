'use strict';
const common = require('../common');
const assert = require('assert');
const { Worker, isMainThread, postMessage } = require('worker');

if (isMainThread) {
  const w = new Worker(__filename);
  w.on('message', common.mustCall((message) => {
    assert.strictEqual(message, true);
  }));
} else {
  assert.throws(() => {
    require('domain');
  }, common.expectsError({
    code: 'ERR_WORKER_DOMAIN',
    type: TypeError
  }));

  assert.doesNotThrow(() => {
    process.umask();
  });
  assert.throws(() => {
    process.umask(0);
  }, TypeError);

  {
    const before = process.title;
    // This should throw...
    process.title += ' in worker';
    assert.strictEqual(process.title, before);
  }

  {
    const before = process.debugPort;
    // This should throw...
    process.debugPort++;
    assert.strictEqual(process.debugPort, before);
  }

  assert.strictEqual(process.stdin, null);
  assert.strictEqual(process.stdout, null);
  assert.strictEqual(process.stderr, null);

  assert.strictEqual('abort' in process, false);
  assert.strictEqual('chdir' in process, false);
  assert.strictEqual('setuid' in process, false);
  assert.strictEqual('seteuid' in process, false);
  assert.strictEqual('setgid' in process, false);
  assert.strictEqual('setegid' in process, false);
  assert.strictEqual('setgroups' in process, false);
  assert.strictEqual('initgroups' in process, false);

  assert.strictEqual('_startProfilerIdleNotifier' in process, false);
  assert.strictEqual('_stopProfilerIdleNotifier' in process, false);
  assert.strictEqual('_debugProcess' in process, false);
  assert.strictEqual('_debugPause' in process, false);
  assert.strictEqual('_debugEnd' in process, false);

  postMessage(true);
}
