/* eslint-disable prefer-assert-methods */
'use strict';
const common = require('../common');
const assert = require('assert');
const vm = require('vm');
const { MessageChannel } = require('worker');

{
  const context = vm.createContext();
  const channel = new MessageChannel();
  context.port = vm.moveMessagePortToContext(channel.port1, context);
  context.global = context;
  const port = channel.port2;
  vm.runInContext('(' + function() {
    function assert(condition) { if (!condition) throw new Error(); }

    {
      assert(port instanceof Object);
      assert(port.onmessage === undefined);
      assert(port.postMessage instanceof Function);
      port.onmessage = function(msg) {
        assert(msg instanceof Object);
        port.postMessage(msg);
      };
      port.start();
    }

    {
      let threw = false;
      try {
        port.postMessage(global);
      } catch (e) {
        assert(e instanceof Object);
        assert(e instanceof Error);
        threw = true;
      }
      assert(threw);
    }
  } + ')()', context);
  port.on('message', common.mustCall((msg) => {
    assert(msg instanceof Object);
    port.close();
  }));
  port.postMessage({});
}
