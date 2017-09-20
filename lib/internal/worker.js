'use strict';

const Buffer = require('buffer').Buffer;
const EventEmitter = require('events');
const assert = require('assert');
const path = require('path');
const util = require('util');
const errors = require('internal/errors');

const { MessagePort, MessageChannel } = internalBinding('messaging');
util.inherits(MessagePort, EventEmitter);

const {
  Worker: WorkerImpl,
  getEnvMessagePort,
  setSerializeWorkerErrorFunction,
  threadId,
  MESSAGE_FLAG_NONE,
  MESSAGE_FLAG_CUSTOM_OFFSET,
  MESSAGE_FLAG_COULD_NOT_SERIALIZE_ERR,
  MESSAGE_FLAG_OUT_OF_MEMORY,
  MESSAGE_FLAG_ERROR_MESSAGE
} = internalBinding('worker');

setSerializeWorkerErrorFunction(serializeError);
const kHandle = Symbol('kHandle');
const kPort = Symbol('kPort');
const kDispose = Symbol('kDispose');
const kOnExit = Symbol('kOnExit');
const kOnMessage = Symbol('kOnMessage');
const kOnCouldNotSerializeErr = Symbol('kOnCouldNotSerializeErr');
const kOnOutOfMemory = Symbol('kOnOutOfMemory');
const kOnErrorMessage = Symbol('kOnErrorMessage');

const debug = util.debuglog('worker');

// Flags >= MESSAGE_FLAG_CUSTOM_OFFSET are reserved for custom commands in the
// sense that they are not handled in the C++ layer.
// MESSAGE_FLAG_NONE is the default flag for normal user messages sent by
// `postMessage`.
const kUpAndRunning = MESSAGE_FLAG_CUSTOM_OFFSET;
const kLoadScript = MESSAGE_FLAG_CUSTOM_OFFSET + 1;

function setupPortReferencing(port, eventEmitter, eventName) {
  // TODO(addaleax): Merge with oninit() in lib/extras/messaging.js
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


class Worker extends EventEmitter {
  constructor(filename, options = {}) {
    super();
    debug(`[${threadId}] create new worker`, filename, options);
    if (typeof filename !== 'string') {
      throw new errors.TypeError('ERR_INVALID_ARG_TYPE', 'filename',
                                 'string', filename);
    }

    if (!options.eval && !path.isAbsolute(filename)) {
      throw new errors.TypeError('ERR_WORKER_NEED_ABSOLUTE_PATH', filename);
    }

    const resourceLimits = {
      maxSemiSpaceSize: options.maxSemiSpaceSize | 0,
      maxOldSpaceSize: options.maxOldSpaceSize | 0
    };

    // Set up the C++ handle for the worker, as well as some internal wiring.
    this[kHandle] = new WorkerImpl(resourceLimits);
    this[kHandle].onexit = (code) => this[kOnExit](code);
    this[kPort] = this[kHandle].messagePort;
    this[kPort].on('message', (payload) => this.emit('message', payload));
    this[kPort].on('flaggedMessage', (flag, payload) => {
      this[kOnMessage](flag, payload);
    });
    this[kPort].start();
    debug(`[${threadId}] created Worker with ID ${this.threadId}`);

    this[kPort].postMessage([
      filename, !!options.eval, options.workerData
    ], undefined, kLoadScript);
    // Actually start the new thread now that everything is in place.
    this[kHandle].startThread();
  }

  [kOnExit](code) {
    debug(`[${threadId}] hears end event for Worker ${this.threadId}`);
    this[kDispose]();
    this.emit('exit', code);
    this.removeAllListeners();
  }

  [kOnCouldNotSerializeErr]() {
    this.emit('error', new errors.Error('ERR_WORKER_UNSERIALIZABLE_ERROR'));
  }

  [kOnOutOfMemory](serialized) {
    this.emit('error', new errors.Error('ERR_WORKER_OUT_OF_MEMORY'));
  }

  [kOnErrorMessage](serialized) {
    // This is what is called for uncaught exceptions.
    const error = deserializeError(serialized);
    this.emit('error', error);
  }

  [kOnMessage](flag, payload) {
    switch (flag) {
      case MESSAGE_FLAG_NONE:
        // This is a userland message.
        return this.emit('message', payload);
      case kUpAndRunning:
        return this.emit('online');
      case MESSAGE_FLAG_COULD_NOT_SERIALIZE_ERR:
        return this[kOnCouldNotSerializeErr](payload);
      case MESSAGE_FLAG_OUT_OF_MEMORY:
        return this[kOnOutOfMemory](payload);
      case MESSAGE_FLAG_ERROR_MESSAGE:
        return this[kOnErrorMessage](payload);
    }

    assert.fail(`Unknown worker message flag ${flag}`);
  }

  [kDispose]() {
    this[kHandle].onexit = null;
    this[kHandle] = null;
    this[kPort] = null;
  }

  postMessage(payload, transferList = undefined) {
    if (this[kHandle] === null) return;

    this[kPort].postMessage(payload, transferList);
  }

  terminate(callback) {
    if (this[kHandle] === null) return;

    debug(`[${threadId}] terminates Worker with ID ${this.threadId}`);

    if (typeof callback !== 'undefined')
      this.once('exit', (exitCode) => callback(null, exitCode));

    this[kHandle].stopThread();
  }

  ref() {
    if (this[kPort] === null) return;

    this[kPort].ref();
  }

  unref() {
    if (this[kPort] === null) return;

    this[kPort].unref();
  }

  get threadId() {
    if (this[kHandle] === null) return -1;

    return this[kHandle].threadId;
  }
}

function setupChild(evalScript) {
  // Called during bootstrap to set up worker script execution.
  debug(`[${threadId}] is setting up worker child environment`);
  const port = getEnvMessagePort();
  const publicWorker = require('worker');
  assert.strictEqual(typeof publicWorker.on, 'function');
  publicWorker.postMessage = (payload, transferList) => {
    port.postMessage(payload, transferList);
  };

  port.on('message', (payload) => publicWorker.emit('workerMessage', payload));
  port.on('flaggedMessage', (flag, payload) => {
    if (flag === kLoadScript) {
      const [ filename, doEval, workerData ] = payload;
      debug(`[${threadId}] starts worker script ${filename} ` +
            `(eval = ${!!doEval}) at cwd = ${process.cwd()}`);
      publicWorker.workerData = workerData;
      setupPortReferencing(port, publicWorker, 'workerMessage');
      port.postMessage(null, undefined, kUpAndRunning);
      if (doEval) {
        evalScript('[worker eval]', filename);
      } else {
        process.argv[1] = filename; // script filename
        require('module').runMain();
      }
      return;
    }

    assert.fail(`Unknown worker message flag ${flag}`);
  });

  port.start();
}

// TODO(addaleax): These can be improved a lot.
function serializeError(error) {
  return Buffer.from(util.inspect(error), 'utf8');
}

function deserializeError(error) {
  return error.toString('utf8');
}

module.exports = {
  MessagePort,
  MessageChannel,
  threadId,
  Worker,
  setupChild
};
