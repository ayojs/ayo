'use strict';
const common = require('../common');
const assert = require('assert');

const { MessageChannel, MessagePort } = require('worker');

{
  const { port1, port2 } = new MessageChannel();
  assert(port1 instanceof MessagePort);
  assert(port2 instanceof MessagePort);

  const input = { a: 1 };
  port1.postMessage(input);
  port2.on('message', common.mustCall((received) => {
    assert.deepStrictEqual(received, input);
    port2.close(common.mustCall());
  }));
}

{
  const { port1, port2 } = new MessageChannel();

  const input = { a: 1 };
  port1.postMessage(input);
  // Check that the message still gets delivered if `port2` has its
  // `on('message')` handler attached at a later point in time.
  setImmediate(() => {
    port2.on('message', common.mustCall((received) => {
      assert.deepStrictEqual(received, input);
      port2.close(common.mustCall());
    }));
  });
}

{
  const { port1, port2 } = new MessageChannel();

  const input = { a: 1 };

  const dummy = common.mustNotCall();
  // Check that the message still gets delivered if `port2` has its
  // `on('message')` handler attached at a later point in time, even if a
  // listener was removed previously.
  port2.addListener('message', dummy);
  setImmediate(() => {
    port2.removeListener('message', dummy);
    port1.postMessage(input);
    setImmediate(() => {
      port2.on('message', common.mustCall((received) => {
        assert.deepStrictEqual(received, input);
        port2.close(common.mustCall());
      }));
    });
  });
}
