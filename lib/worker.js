'use strict';

const {
  MessagePort,
  MessageChannel,
  threadId,
  Worker
} = require('internal/worker');

const EventEmitter = require('events');

module.exports = Object.create(EventEmitter.prototype);

EventEmitter.call(module.exports);

Object.assign(module.exports, {
  isMainThread,
  MessagePort,
  MessageChannel,
  threadId,
  Worker
});
