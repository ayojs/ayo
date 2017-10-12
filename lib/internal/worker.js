'use strict';

const EventEmitter = require('events');
const util = require('util');

const { MessagePort, MessageChannel } = process.binding('messaging');
util.inherits(MessagePort, EventEmitter);

const debug = util.debuglog('worker');

// A MessagePort consists a handle (that wraps around an
// uv_async_t) which can receive information from other threads and emits
// .onmessage events, and a function used for sending data to a MessagePort
// in some other thread.
function onmessage(payload, flag) {
  debug(`[${process.threadId}] received message`, flag, payload);
  // Emit the flag and deserialized object to userland.
  if (flag === 0 || flag === undefined)
    this.emit('message', payload);
  else
    this.emit('flaggedMessage', flag, payload);
}

Object.defineProperty(MessagePort.prototype, 'onmessage', {
  enumerable: false,
  configurable: true,
  get() { return onmessage; },
  set(value) {
    Object.defineProperty(this, {
      writable: true,
      enumerable: true,
      configurable: true,
      value
    });
    this.ref();
    this.start();
  }
});

function oninit() {
  setupPortReferencing(this, this, 'message');
}

Object.defineProperty(MessagePort.prototype, 'oninit', {
  enumerable: false,
  writable: false,
  value: oninit
});

function onclose() {
  this.emit('close');
}

Object.defineProperty(MessagePort.prototype, 'onclose', {
  enumerable: false,
  writable: false,
  value: onclose
});

function setupPortReferencing(port, eventEmitter, eventName) {
  // Keep track of whether there are any workerMessage listeners:
  // If there are some, ref() the channel so it keeps the event loop alive.
  // If there are none or all are removed, unref() the channel so the worker
  // can shutdown gracefully.
  port.unref();
  eventEmitter.on('newListener', (name) => {
    if (name === eventName && eventEmitter.listenerCount(eventName) === 0) {
      port.ref();
      port.start();
    }
  });
  eventEmitter.on('removeListener', (name) => {
    if (name === eventName && eventEmitter.listenerCount(eventName) === 0) {
      port.stop();
      port.unref();
    }
  });
}

module.exports = {
  MessagePort,
  MessageChannel
};
