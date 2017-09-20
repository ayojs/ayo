/* eslint-disable indent, strict */
(function(global, binding, v8) {

'use strict';

const { defineProperties, setPrototypeOf } = global.Object;

// A communication channel consisting of a handle (that wraps around an
// uv_async_t) which can receive information from other threads and emits
// .onmessage events, and a function used for sending data to a MessagePort
// in some other thread.
function onmessage(payload, flag) {
  if (flag !== 0 /*MESSAGE_FLAG_NONE*/ &&
      flag < 100 /*MESSAGE_FLAG_CUSTOM_OFFSET*/) {
    // This was not handled in C++, but it is also not a custom message in the
    // sense that it was generated in JS, so some special handling is still
    // required for deserialization.
    // (This is primarily for error situations)
    // debug(`[${threadId}] received raw message`, flag, payload);
    return this.emit('flaggedMessage', flag, payload);
  }

  // debug(`[${threadId}] received message`, flag, payload);
  // Emit the flag and deserialized object to userland.
  if (flag === 0 || flag === undefined)
    this.emit('message', payload);
  else
    this.emit('flaggedMessage', flag, payload);
}

function oninit() {
  // Keep track of whether there are any workerMessage listeners:
  // If there are some, ref() the channel so it keeps the event loop alive.
  // If there are none or all are removed, unref() the channel so the worker
  // can shutdown gracefully.
  this.unref();
  this.on('newListener', (name) => {
    if (name === 'message' && this.listenerCount('message') === 0) {
      this.ref();
      this.start();
    }
  });
  this.on('removeListener', (name) => {
    if (name === 'message' && this.listenerCount('message') === 0) {
      this.stop();
      this.unref();
    }
  });
}

function onclose() {
  this.emit('close');
}

function makeMessagePort(MessagePort) {
  setPrototypeOf(MessagePort, binding.EventEmitter);
  setPrototypeOf(MessagePort.prototype, binding.EventEmitter.prototype);

  defineProperties(MessagePort.prototype, {
    onmessage: {
      enumerable: true,
      writable: false,
      value: onmessage
    },

    oninit: {
      enumerable: true,
      writable: false,
      value: oninit
    },

    onclose: {
      enumerable: true,
      writable: false,
      value: onclose
    }
  });
}

binding.makeMessagePort = makeMessagePort;

});
