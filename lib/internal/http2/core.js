'use strict';

/* eslint-disable no-use-before-define */

require('internal/util').assertCrypto();

const binding = process.binding('http2');
const assert = require('assert');
const { Buffer } = require('buffer');
const EventEmitter = require('events');
const net = require('net');
const tls = require('tls');
const util = require('util');
const fs = require('fs');
const errors = require('internal/errors');
const { StreamWrap } = require('_stream_wrap');
const { Duplex } = require('stream');
const { URL } = require('url');
const { onServerStream,
        Http2ServerRequest,
        Http2ServerResponse,
} = require('internal/http2/compat');
const { utcDate } = require('internal/http');
const { promisify } = require('internal/util');
const { isUint8Array } = require('internal/util/types');
const { _connectionListener: httpConnectionListener } = require('http');
const { createPromise, promiseResolve } = process.binding('util');
const debug = util.debuglog('http2');


const {
  assertIsObject,
  assertValidPseudoHeaderResponse,
  assertValidPseudoHeaderTrailer,
  assertWithinRange,
  getDefaultSettings,
  getSessionState,
  getSettings,
  getStreamState,
  isPayloadMeaningless,
  kSocket,
  mapToHeaders,
  NghttpError,
  sessionName,
  toHeaderObject,
  updateOptionsBuffer,
  updateSettingsBuffer
} = require('internal/http2/util');

const {
  _unrefActive,
  enroll,
  unenroll
} = require('timers');

const { WriteWrap } = process.binding('stream_wrap');
const { constants } = binding;

const NETServer = net.Server;
const TLSServer = tls.Server;

const kInspect = require('internal/util').customInspectSymbol;

const kAuthority = Symbol('authority');
const kDestroySocket = Symbol('destroy-socket');
const kHandle = Symbol('handle');
const kID = Symbol('id');
const kInit = Symbol('init');
const kLocalSettings = Symbol('local-settings');
const kOptions = Symbol('options');
const kOwner = Symbol('owner');
const kProceed = Symbol('proceed');
const kProtocol = Symbol('protocol');
const kProxySocket = Symbol('proxy-socket');
const kRemoteSettings = Symbol('remote-settings');
const kServer = Symbol('server');
const kSession = Symbol('session');
const kState = Symbol('state');
const kType = Symbol('type');

const kDefaultSocketTimeout = 2 * 60 * 1000;
const kRenegTest = /TLS session renegotiation disabled for this socket/;

const {
  paddingBuffer,
  PADDING_BUF_FRAME_LENGTH,
  PADDING_BUF_MAX_PAYLOAD_LENGTH,
  PADDING_BUF_RETURN_VALUE
} = binding;

const {
  NGHTTP2_CANCEL,
  NGHTTP2_DEFAULT_WEIGHT,
  NGHTTP2_FLAG_END_STREAM,
  NGHTTP2_HCAT_PUSH_RESPONSE,
  NGHTTP2_HCAT_RESPONSE,
  NGHTTP2_INTERNAL_ERROR,
  NGHTTP2_NO_ERROR,
  NGHTTP2_PROTOCOL_ERROR,
  NGHTTP2_REFUSED_STREAM,
  NGHTTP2_SESSION_CLIENT,
  NGHTTP2_SESSION_SERVER,
  NGHTTP2_ERR_NOMEM,
  NGHTTP2_ERR_STREAM_ID_NOT_AVAILABLE,
  NGHTTP2_ERR_INVALID_ARGUMENT,
  NGHTTP2_ERR_STREAM_CLOSED,

  HTTP2_HEADER_AUTHORITY,
  HTTP2_HEADER_DATE,
  HTTP2_HEADER_METHOD,
  HTTP2_HEADER_PATH,
  HTTP2_HEADER_SCHEME,
  HTTP2_HEADER_STATUS,
  HTTP2_HEADER_CONTENT_LENGTH,

  NGHTTP2_SETTINGS_HEADER_TABLE_SIZE,
  NGHTTP2_SETTINGS_ENABLE_PUSH,
  NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS,
  NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE,
  NGHTTP2_SETTINGS_MAX_FRAME_SIZE,
  NGHTTP2_SETTINGS_MAX_HEADER_LIST_SIZE,

  HTTP2_METHOD_GET,
  HTTP2_METHOD_HEAD,
  HTTP2_METHOD_CONNECT,

  HTTP_STATUS_CONTINUE,
  HTTP_STATUS_RESET_CONTENT,
  HTTP_STATUS_OK,
  HTTP_STATUS_NO_CONTENT,
  HTTP_STATUS_NOT_MODIFIED,
  HTTP_STATUS_SWITCHING_PROTOCOLS,

  STREAM_OPTION_EMPTY_PAYLOAD,
  STREAM_OPTION_GET_TRAILERS
} = constants;

// Top level to avoid creating a closure
function emit(self, ...args) {
  self.emit(...args);
}

// Called when a new block of headers has been received for a given
// stream. The stream may or may not be new. If the stream is new,
// create the associated Http2Stream instance and emit the 'stream'
// event. If the stream is not new, emit the 'headers' event to pass
// the block of headers on.
function onSessionHeaders(id, cat, flags, headers) {
  const owner = this[kOwner];
  const type = owner[kType];
  _unrefActive(owner);
  debug(`[${sessionName(type)}] headers were received on ` +
        `stream ${id}: ${cat}`);
  const streams = owner[kState].streams;

  const endOfStream = !!(flags & NGHTTP2_FLAG_END_STREAM);
  let stream = streams.get(id);

  // Convert the array of header name value pairs into an object
  const obj = toHeaderObject(headers);

  if (stream === undefined) {
    // owner[kType] can be only one of two possible values
    if (type === NGHTTP2_SESSION_SERVER) {
      stream = new ServerHttp2Stream(owner, id,
                                     { readable: !endOfStream },
                                     obj);
      if (obj[HTTP2_HEADER_METHOD] === HTTP2_METHOD_HEAD) {
        // For head requests, there must not be a body...
        // end the writable side immediately.
        stream.end();
        stream[kState].headRequest = true;
      }
    } else {
      stream = new ClientHttp2Stream(owner, id, { readable: !endOfStream });
    }
    streams.set(id, stream);
    process.nextTick(emit, owner, 'stream', stream, obj, flags, headers);
  } else {
    let event;
    const status = obj[HTTP2_HEADER_STATUS];
    if (cat === NGHTTP2_HCAT_RESPONSE) {
      if (!endOfStream &&
          status !== undefined &&
          status >= 100 &&
          status < 200) {
        event = 'headers';
      } else {
        event = 'response';
      }
    } else if (cat === NGHTTP2_HCAT_PUSH_RESPONSE) {
      event = 'push';
      // cat === NGHTTP2_HCAT_HEADERS:
    } else if (!endOfStream && status !== undefined && status >= 200) {
      event = 'response';
    } else {
      event = endOfStream ? 'trailers' : 'headers';
    }
    debug(`[${sessionName(type)}] emitting stream '${event}' event`);
    process.nextTick(emit, stream, event, obj, flags, headers);
  }
  if (endOfStream) {
    stream.push(null);
  }
}

// Called to determine if there are trailers to be sent at the end of a
// Stream. The 'getTrailers' callback is invoked and passed a holder object.
// The trailers to return are set on that object by the handler. Once the
// event handler returns, those are sent off for processing. Note that this
// is a necessarily synchronous operation. We need to know immediately if
// there are trailing headers to send.
function onSessionTrailers(id) {
  const owner = this[kOwner];
  debug(`[${sessionName(owner[kType])}] checking for trailers`);
  const streams = owner[kState].streams;
  const stream = streams.get(id);
  // It should not be possible for the stream not to exist at this point.
  // If it does not exist, there is something very very wrong.
  assert(stream !== undefined,
         'Internal HTTP/2 Failure. Stream does not exist. Please ' +
         'report this as a bug in Node.js');
  const trailers = Object.create(null);
  stream[kState].getTrailers.call(stream, trailers);
  const headersList = mapToHeaders(trailers, assertValidPseudoHeaderTrailer);
  if (!Array.isArray(headersList)) {
    process.nextTick(emit, stream, 'error', headersList);
    return;
  }
  return headersList;
}

// Called when the stream is closed. The streamClosed event is emitted on the
// Http2Stream instance. Note that this event is distinctly different than the
// require('stream') interface 'close' event which deals with the state of the
// Readable and Writable sides of the Duplex.
function onSessionStreamClose(id, code) {
  const owner = this[kOwner];
  debug(`[${sessionName(owner[kType])}] session is closing the stream ` +
        `${id}: ${code}`);
  const stream = owner[kState].streams.get(id);
  if (stream === undefined)
    return;
  _unrefActive(owner);
  // Set the rst state for the stream
  abort(stream);
  const state = stream[kState];
  state.rst = true;
  state.rstCode = code;

  if (state.fd !== undefined) {
    debug(`Closing fd ${state.fd} for stream ${id}`);
    fs.close(state.fd, afterFDClose.bind(stream));
  }

  setImmediate(stream.destroy.bind(stream));
}

function afterFDClose(err) {
  if (err)
    process.nextTick(emit, this, 'error', err);
}

// Called when an error event needs to be triggered
function onSessionError(error) {
  const owner = this[kOwner];
  _unrefActive(owner);
  process.nextTick(emit, owner, 'error', error);
}

// Receives a chunk of data for a given stream and forwards it on
// to the Http2Stream Duplex for processing.
function onSessionRead(nread, buf, handle) {
  const owner = this[kOwner];
  const streams = owner[kState].streams;
  const id = handle.id;
  const stream = streams.get(id);
  // It should not be possible for the stream to not exist at this point.
  // If it does not, something is very very wrong
  assert(stream !== undefined,
         'Internal HTTP/2 Failure. Stream does not exist. Please ' +
         'report this as a bug in Node.js');
  _unrefActive(owner); // Reset the session timeout timer
  _unrefActive(stream); // Reset the stream timeout timer
  if (nread >= 0 && !stream.destroyed) {
    // prevent overflowing the buffer while pause figures out the
    // stream needs to actually pause and streamOnPause runs
    if (!stream.push(buf))
      owner[kHandle].streamReadStop(id);
    return;
  }

  // Last chunk was received. End the readable side.
  stream.push(null);
}

// Called when the remote peer settings have been updated.
// Resets the cached settings.
function onSettings(ack) {
  const owner = this[kOwner];
  debug(`[${sessionName(owner[kType])}] new settings received`);
  _unrefActive(owner);
  let event = 'remoteSettings';
  if (ack) {
    if (owner[kState].pendingAck > 0)
      owner[kState].pendingAck--;
    owner[kLocalSettings] = undefined;
    event = 'localSettings';
  } else {
    owner[kRemoteSettings] = undefined;
  }
  // Only emit the event if there are listeners registered
  if (owner.listenerCount(event) > 0) {
    const settings = event === 'localSettings' ?
      owner.localSettings : owner.remoteSettings;
    process.nextTick(emit, owner, event, settings);
  }
}

// If the stream exists, an attempt will be made to emit an event
// on the stream object itself. Otherwise, forward it on to the
// session (which may, in turn, forward it on to the server)
function onPriority(id, parent, weight, exclusive) {
  const owner = this[kOwner];
  debug(`[${sessionName(owner[kType])}] priority advisement for stream ` +
        `${id}: \n  parent: ${parent},\n  weight: ${weight},\n` +
        `  exclusive: ${exclusive}`);
  _unrefActive(owner);
  const streams = owner[kState].streams;
  const stream = streams.get(id);
  const emitter = stream === undefined ? owner : stream;
  process.nextTick(emit, emitter, 'priority', id, parent, weight, exclusive);
}

function emitFrameError(self, id, type, code) {
  if (!self.emit('frameError', type, code, id)) {
    const err = new errors.Error('ERR_HTTP2_FRAME_ERROR', type, code, id);
    err.errno = code;
    self.emit('error', err);
  }
}

// Called by the native layer when an error has occurred sending a
// frame. This should be exceedingly rare.
function onFrameError(id, type, code) {
  const owner = this[kOwner];
  debug(`[${sessionName(owner[kType])}] error sending frame type ` +
        `${type} on stream ${id}, code: ${code}`);
  _unrefActive(owner);
  const streams = owner[kState].streams;
  const stream = streams.get(id);
  const emitter = stream !== undefined ? stream : owner;
  process.nextTick(emitFrameError, emitter, id, type, code);
}

function emitGoaway(self, code, lastStreamID, buf) {
  self.emit('goaway', code, lastStreamID, buf);
  // Tear down the session or destroy
  const state = self[kState];
  if (state.destroying || state.destroyed)
    return;
  if (!state.shuttingDown && !state.shutdown) {
    self.shutdown({}, self.destroy.bind(self));
  } else {
    self.destroy();
  }
}

// Called by the native layer when a goaway frame has been received
function onGoawayData(code, lastStreamID, buf) {
  const owner = this[kOwner];
  debug(`[${sessionName(owner[kType])}] goaway data received`);
  process.nextTick(emitGoaway, owner, code, lastStreamID, buf);
}

// Returns the padding to use per frame. The selectPadding callback is set
// on the options. It is invoked with two arguments, the frameLen, and the
// maxPayloadLen. The method must return a numeric value within the range
// frameLen <= n <= maxPayloadLen.
function onSelectPadding(fn) {
  return function getPadding() {
    debug('fetching padding for frame');
    const frameLen = paddingBuffer[PADDING_BUF_FRAME_LENGTH];
    const maxFramePayloadLen = paddingBuffer[PADDING_BUF_MAX_PAYLOAD_LENGTH];
    paddingBuffer[PADDING_BUF_RETURN_VALUE] =
        Math.min(maxFramePayloadLen,
                 Math.max(frameLen,
                          fn(frameLen,
                             maxFramePayloadLen) | 0));
  };
}

// When a ClientHttp2Session is first created, the socket may not yet be
// connected. If request() is called during this time, the actual request
// will be deferred until the socket is ready to go.
function requestOnConnect(headers, options) {
  const session = this[kSession];
  debug(`[${sessionName(session[kType])}] connected.. initializing request`);
  const streams = session[kState].streams;

  validatePriorityOptions(options);

  const headersList = mapToHeaders(headers);
  if (!Array.isArray(headersList)) {
    process.nextTick(emit, this, 'error', headersList);
    return;
  }

  let streamOptions = 0;
  if (options.endStream)
    streamOptions |= STREAM_OPTION_EMPTY_PAYLOAD;

  if (typeof options.getTrailers === 'function') {
    streamOptions |= STREAM_OPTION_GET_TRAILERS;
    this[kState].getTrailers = options.getTrailers;
  }

  // ret will be either the reserved stream ID (if positive)
  // or an error code (if negative)
  const ret = session[kHandle].submitRequest(headersList,
                                             streamOptions,
                                             options.parent | 0,
                                             options.weight | 0,
                                             !!options.exclusive);

  // In an error condition, one of three possible response codes will be
  // possible:
  // * NGHTTP2_ERR_NOMEM - Out of memory, this should be fatal to the process.
  // * NGHTTP2_ERR_STREAM_ID_NOT_AVAILABLE - Maximum stream ID is reached, this
  //   is fatal for the session
  // * NGHTTP2_ERR_INVALID_ARGUMENT - Stream was made dependent on itself, this
  //   impacts on this stream.
  // For the first two, emit the error on the session,
  // For the third, emit the error on the stream, it will bubble up to the
  // session if not handled.
  let err;
  switch (ret) {
    case NGHTTP2_ERR_NOMEM:
      err = new errors.Error('ERR_OUTOFMEMORY');
      process.nextTick(emit, session, 'error', err);
      break;
    case NGHTTP2_ERR_STREAM_ID_NOT_AVAILABLE:
      err = new errors.Error('ERR_HTTP2_OUT_OF_STREAMS');
      process.nextTick(emit, session, 'error', err);
      break;
    case NGHTTP2_ERR_INVALID_ARGUMENT:
      err = new errors.Error('ERR_HTTP2_STREAM_SELF_DEPENDENCY');
      process.nextTick(emit, this, 'error', err);
      break;
    default:
      // Some other, unexpected error was returned. Emit on the session.
      if (ret < 0) {
        err = new NghttpError(ret);
        process.nextTick(emit, session, 'error', err);
        break;
      }
      debug(`[${sessionName(session[kType])}] stream ${ret} initialized`);
      this[kInit](ret);
      streams.set(ret, this);
  }
}

function validatePriorityOptions(options) {
  if (options.weight === undefined) {
    options.weight = NGHTTP2_DEFAULT_WEIGHT;
  } else if (typeof options.weight !== 'number') {
    const err = new errors.TypeError('ERR_INVALID_OPT_VALUE',
                                     'weight',
                                     options.weight);
    Error.captureStackTrace(err, validatePriorityOptions);
    throw err;
  }

  if (options.parent === undefined) {
    options.parent = 0;
  } else if (typeof options.parent !== 'number' || options.parent < 0) {
    const err = new errors.TypeError('ERR_INVALID_OPT_VALUE',
                                     'parent',
                                     options.parent);
    Error.captureStackTrace(err, validatePriorityOptions);
    throw err;
  }

  if (options.exclusive === undefined) {
    options.exclusive = false;
  } else if (typeof options.exclusive !== 'boolean') {
    const err = new errors.TypeError('ERR_INVALID_OPT_VALUE',
                                     'exclusive',
                                     options.exclusive);
    Error.captureStackTrace(err, validatePriorityOptions);
    throw err;
  }

  if (options.silent === undefined) {
    options.silent = false;
  } else if (typeof options.silent !== 'boolean') {
    const err = new errors.TypeError('ERR_INVALID_OPT_VALUE',
                                     'silent',
                                     options.silent);
    Error.captureStackTrace(err, validatePriorityOptions);
    throw err;
  }
}

// Creates the internal binding.Http2Session handle for an Http2Session
// instance. This occurs only after the socket connection has been
// established. Note: the binding.Http2Session will take over ownership
// of the socket. No other code should read from or write to the socket.
function setupHandle(session, socket, type, options) {
  return function() {
    debug(`[${sessionName(type)}] setting up session handle`);
    session[kState].connecting = false;

    updateOptionsBuffer(options);
    const handle = new binding.Http2Session(type);
    handle[kOwner] = session;
    handle.onpriority = onPriority;
    handle.onsettings = onSettings;
    handle.onheaders = onSessionHeaders;
    handle.ontrailers = onSessionTrailers;
    handle.onstreamclose = onSessionStreamClose;
    handle.onerror = onSessionError;
    handle.onread = onSessionRead;
    handle.onframeerror = onFrameError;
    handle.ongoawaydata = onGoawayData;

    if (typeof options.selectPadding === 'function')
      handle.ongetpadding = onSelectPadding(options.selectPadding);

    assert(socket._handle !== undefined,
           'Internal HTTP/2 Failure. The socket is not connected. Please ' +
           'report this as a bug in Node.js');
    handle.consume(socket._handle._externalStream);

    session[kHandle] = handle;

    const settings = typeof options.settings === 'object' ?
      options.settings : Object.create(null);

    session.settings(settings);
    process.nextTick(emit, session, 'connect', session, socket);
  };
}

// Submits a SETTINGS frame to be sent to the remote peer.
function submitSettings(settings) {
  const type = this[kType];
  debug(`[${sessionName(type)}] submitting actual settings`);
  _unrefActive(this);
  this[kLocalSettings] = undefined;

  updateSettingsBuffer(settings);
  const ret = this[kHandle].submitSettings();
  let err;
  switch (ret) {
    case NGHTTP2_ERR_NOMEM:
      err = new errors.Error('ERR_OUTOFMEMORY');
      process.nextTick(emit, this, 'error', err);
      break;
    default:
      // Some other unexpected error was reported.
      if (ret < 0) {
        err = new NghttpError(ret);
        process.nextTick(emit, this, 'error', err);
      }
  }
  debug(`[${sessionName(type)}] settings complete`);
}

// Submits a PRIORITY frame to be sent to the remote peer
// Note: If the silent option is true, the change will be made
// locally with no PRIORITY frame sent.
function submitPriority(stream, options) {
  const type = this[kType];
  debug(`[${sessionName(type)}] submitting actual priority`);
  _unrefActive(this);

  const ret = this[kHandle].submitPriority(stream[kID],
                                           options.parent | 0,
                                           options.weight | 0,
                                           !!options.exclusive,
                                           !!options.silent);

  let err;
  switch (ret) {
    case NGHTTP2_ERR_NOMEM:
      err = new errors.Error('ERR_OUTOFMEMORY');
      process.nextTick(emit, this, 'error', err);
      break;
    default:
      // Some other unexpected error was reported.
      if (ret < 0) {
        err = new NghttpError(ret);
        process.nextTick(emit, stream, 'error', err);
      }
  }
  debug(`[${sessionName(type)}] priority complete`);
}

// Submit an RST-STREAM frame to be sent to the remote peer.
// This will cause the Http2Stream to be closed.
function submitRstStream(stream, code) {
  const type = this[kType];
  debug(`[${sessionName(type)}] submit actual rststream`);
  _unrefActive(this);
  const ret = this[kHandle].submitRstStream(stream[kID], code);
  let err;
  switch (ret) {
    case NGHTTP2_ERR_NOMEM:
      err = new errors.Error('ERR_OUTOFMEMORY');
      process.nextTick(emit, this, 'error', err);
      break;
    default:
      // Some other unexpected error was reported.
      if (ret < 0) {
        err = new NghttpError(ret);
        process.nextTick(emit, stream, 'error', err);
        break;
      }
      stream.destroy();
  }
  debug(`[${sessionName(type)}] rststream complete`);
}

function doShutdown(self, options) {
  const handle = self[kHandle];
  const state = self[kState];
  if (handle === undefined || state.shutdown)
    return; // Nothing to do, possibly because the session shutdown already.
  const ret = handle.submitGoaway(options.errorCode | 0,
                                  options.lastStreamID | 0,
                                  options.opaqueData);
  state.shuttingDown = false;
  state.shutdown = true;
  if (ret < 0) {
    debug(`[${sessionName(self[kType])}] shutdown failed! code: ${ret}`);
    const err = new NghttpError(ret);
    process.nextTick(emit, self, 'error', err);
    return;
  }
  process.nextTick(emit, self, 'shutdown', options);
  debug(`[${sessionName(self[kType])}] shutdown is complete`);
}

// Submit a graceful or immediate shutdown request for the Http2Session.
function submitShutdown(options) {
  const type = this[kType];
  debug(`[${sessionName(type)}] submitting actual shutdown request`);
  if (type === NGHTTP2_SESSION_SERVER &&
      options.graceful === true) {
    // first send a shutdown notice
    this[kHandle].submitShutdownNotice();
    // then, on flip of the event loop, do the actual shutdown
    setImmediate(doShutdown, this, options);
  } else {
    doShutdown(this, options);
  }
}

function finishSessionDestroy(self, socket) {
  const state = self[kState];

  if (!socket.destroyed)
    socket.destroy();

  state.destroying = false;
  state.destroyed = true;

  // Destroy the handle
  const handle = self[kHandle];
  if (handle !== undefined) {
    handle.destroy(state.skipUnconsume);
    debug(`[${sessionName(self[kType])}] nghttp2session handle destroyed`);
  }
  self[kHandle] = undefined;

  process.nextTick(emit, self, 'close');
  debug(`[${sessionName(self[kType])}] nghttp2session destroyed`);
}

const proxySocketHandler = {
  get(session, prop) {
    switch (prop) {
      case 'setTimeout':
        return session.setTimeout.bind(session);
      case 'destroy':
      case 'emit':
      case 'end':
      case 'pause':
      case 'read':
      case 'resume':
      case 'write':
        throw new errors.Error('ERR_HTTP2_NO_SOCKET_MANIPULATION');
      default:
        const socket = session[kSocket];
        const value = socket[prop];
        return typeof value === 'function' ? value.bind(socket) : value;
    }
  },
  getPrototypeOf(session) {
    return Reflect.getPrototypeOf(session[kSocket]);
  },
  set(session, prop, value) {
    switch (prop) {
      case 'setTimeout':
        session.setTimeout = value;
        return true;
      case 'destroy':
      case 'emit':
      case 'end':
      case 'pause':
      case 'read':
      case 'resume':
      case 'write':
        throw new errors.Error('ERR_HTTP2_NO_SOCKET_MANIPULATION');
      default:
        session[kSocket][prop] = value;
        return true;
    }
  }
};

// Upon creation, the Http2Session takes ownership of the socket. The session
// may not be ready to use immediately if the socket is not yet fully connected.
class Http2Session extends EventEmitter {

  // type     { number } either NGHTTP2_SESSION_SERVER or NGHTTP2_SESSION_CLIENT
  // options  { Object }
  // socket   { net.Socket | tls.TLSSocket | stream.Duplex }
  constructor(type, options, socket) {
    super();

    if (!socket._handle || !socket._handle._externalStream) {
      socket = new StreamWrap(socket);
    }

    // No validation is performed on the input parameters because this
    // constructor is not exported directly for users.

    // If the session property already exists on the socket,
    // then it has already been bound to an Http2Session instance
    // and cannot be attached again.
    if (socket[kSession] !== undefined)
      throw new errors.Error('ERR_HTTP2_SOCKET_BOUND');

    socket[kSession] = this;

    this[kState] = {
      streams: new Map(),
      destroyed: false,
      shutdown: false,
      shuttingDown: false,
      pendingAck: 0,
      maxPendingAck: Math.max(1, (options.maxPendingAck | 0) || 10),
      writeQueueSize: 0
    };

    this[kType] = type;
    this[kProxySocket] = null;
    this[kSocket] = socket;

    // Do not use nagle's algorithm
    if (typeof socket.setNoDelay === 'function')
      socket.setNoDelay();

    // Disable TLS renegotiation on the socket
    if (typeof socket.disableRenegotiation === 'function')
      socket.disableRenegotiation();

    socket[kDestroySocket] = socket.destroy;
    socket.destroy = socketDestroy;

    const setupFn = setupHandle(this, socket, type, options);
    if (socket.connecting) {
      this[kState].connecting = true;
      socket.once('connect', setupFn);
    } else {
      setupFn();
    }

    // Any individual session can have any number of active open
    // streams, these may all need to be made aware of changes
    // in state that occur -- such as when the associated socket
    // is closed. To do so, we need to set the max listener count
    // to something more reasonable because we may have any number
    // of concurrent streams (2^31-1 is the upper limit on the number
    // of streams)
    this.setMaxListeners((2 ** 31) - 1);
    debug(`[${sessionName(type)}] http2session created`);
  }

  [kInspect](depth, opts) {
    const state = this[kState];
    const obj = {
      type: this[kType],
      destroyed: state.destroyed,
      destroying: state.destroying,
      shutdown: state.shutdown,
      shuttingDown: state.shuttingDown,
      state: this.state,
      localSettings: this.localSettings,
      remoteSettings: this.remoteSettings
    };
    return `Http2Session ${util.format(obj)}`;
  }

  // The socket owned by this session
  get socket() {
    const proxySocket = this[kProxySocket];
    if (proxySocket === null)
      return this[kProxySocket] = new Proxy(this, proxySocketHandler);
    return proxySocket;
  }

  // The session type
  get type() {
    return this[kType];
  }

  // true if the Http2Session is waiting for a settings acknowledgement
  get pendingSettingsAck() {
    return this[kState].pendingAck > 0;
  }

  // true if the Http2Session has been destroyed
  get destroyed() {
    return this[kState].destroyed;
  }

  // Retrieves state information for the Http2Session
  get state() {
    const handle = this[kHandle];
    return handle !== undefined ?
      getSessionState(handle) :
      Object.create(null);
  }

  // The settings currently in effect for the local peer. These will
  // be updated only when a settings acknowledgement has been received.
  get localSettings() {
    let settings = this[kLocalSettings];
    if (settings !== undefined)
      return settings;

    const handle = this[kHandle];
    if (handle === undefined)
      return Object.create(null);

    settings = getSettings(handle, false); // Local
    this[kLocalSettings] = settings;
    return settings;
  }

  // The settings currently in effect for the remote peer.
  get remoteSettings() {
    let settings = this[kRemoteSettings];
    if (settings !== undefined)
      return settings;

    const handle = this[kHandle];
    if (handle === undefined)
      return Object.create(null);

    settings = getSettings(handle, true); // Remote
    this[kRemoteSettings] = settings;
    return settings;
  }

  // Submits a SETTINGS frame to be sent to the remote peer.
  settings(settings) {
    const state = this[kState];
    if (state.destroyed || state.destroying)
      throw new errors.Error('ERR_HTTP2_INVALID_SESSION');

    // Validate the input first
    assertIsObject(settings, 'settings');
    settings = Object.assign(Object.create(null), settings);
    assertWithinRange('headerTableSize',
                      settings.headerTableSize,
                      0, 2 ** 32 - 1);
    assertWithinRange('initialWindowSize',
                      settings.initialWindowSize,
                      0, 2 ** 32 - 1);
    assertWithinRange('maxFrameSize',
                      settings.maxFrameSize,
                      16384, 2 ** 24 - 1);
    assertWithinRange('maxConcurrentStreams',
                      settings.maxConcurrentStreams,
                      0, 2 ** 31 - 1);
    assertWithinRange('maxHeaderListSize',
                      settings.maxHeaderListSize,
                      0, 2 ** 32 - 1);
    if (settings.enablePush !== undefined &&
        typeof settings.enablePush !== 'boolean') {
      const err = new errors.TypeError('ERR_HTTP2_INVALID_SETTING_VALUE',
                                       'enablePush', settings.enablePush);
      err.actual = settings.enablePush;
      throw err;
    }
    if (state.pendingAck === state.maxPendingAck) {
      throw new errors.Error('ERR_HTTP2_MAX_PENDING_SETTINGS_ACK',
                             this[kState].pendingAck);
    }
    debug(`[${sessionName(this[kType])}] sending settings`);

    state.pendingAck++;
    if (state.connecting) {
      debug(`[${sessionName(this[kType])}] session still connecting, ` +
            'queue settings');
      this.once('connect', submitSettings.bind(this, settings));
      return;
    }
    submitSettings.call(this, settings);
  }

  // Submits a PRIORITY frame to be sent to the remote peer.
  priority(stream, options) {
    const state = this[kState];
    if (state.destroyed || state.destroying)
      throw new errors.Error('ERR_HTTP2_INVALID_SESSION');

    if (!(stream instanceof Http2Stream)) {
      throw new errors.TypeError('ERR_INVALID_ARG_TYPE',
                                 'stream',
                                 'Http2Stream');
    }
    assertIsObject(options, 'options');
    options = Object.assign(Object.create(null), options);
    validatePriorityOptions(options);

    const id = stream[kID];
    debug(`[${sessionName(this[kType])}] sending priority for stream ` +
          `${id}`);

    // A stream cannot be made to depend on itself
    if (options.parent === id) {
      throw new errors.TypeError('ERR_INVALID_OPT_VALUE',
                                 'parent',
                                 options.parent);
    }

    if (id === undefined) {
      debug(`[${sessionName(this[kType])}] session still connecting. queue ` +
            'priority');
      stream.once('ready', submitPriority.bind(this, stream, options));
      return;
    }
    submitPriority.call(this, stream, options);
  }

  // Submits an RST-STREAM frame to be sent to the remote peer. This will
  // cause the stream to be closed.
  rstStream(stream, code = NGHTTP2_NO_ERROR) {
    // Do not check destroying here, as the rstStream may be sent while
    // the session is in the middle of being destroyed.
    if (this[kState].destroyed)
      throw new errors.Error('ERR_HTTP2_INVALID_SESSION');

    if (!(stream instanceof Http2Stream)) {
      throw new errors.TypeError('ERR_INVALID_ARG_TYPE',
                                 'stream',
                                 'Http2Stream');
    }

    if (typeof code !== 'number') {
      throw new errors.TypeError('ERR_INVALID_ARG_TYPE',
                                 'code',
                                 'number');
    }

    const state = stream[kState];
    if (state.rst) {
      // rst has already been called, do not call again,
      // skip straight to destroy
      stream.destroy();
      return;
    }
    state.rst = true;
    state.rstCode = code;

    const id = stream[kID];
    debug(`[${sessionName(this[kType])}] initiating rststream for stream ` +
          `${id}: ${code}`);

    if (id === undefined) {
      debug(`[${sessionName(this[kType])}] session still connecting, queue ` +
            'rststream');
      stream.once('ready', submitRstStream.bind(this, stream, code));
      return;
    }
    submitRstStream.call(this, stream, code);
  }

  // Destroy the Http2Session
  destroy() {
    const state = this[kState];
    if (state.destroyed || state.destroying)
      return;
    debug(`[${sessionName(this[kType])}] destroying nghttp2session`);
    state.destroying = true;
    state.destroyed = false;

    // Unenroll the timer
    this.setTimeout(0, sessionOnTimeout);

    // Shut down any still open streams
    const streams = state.streams;
    streams.forEach((stream) => stream.destroy());

    // Disassociate from the socket and server
    const socket = this[kSocket];
    // socket.pause();
    delete this[kProxySocket];
    delete this[kSocket];
    delete this[kServer];

    if (this[kHandle] !== undefined)
      this[kHandle].destroying();

    setImmediate(finishSessionDestroy, this, socket);
  }

  // Graceful or immediate shutdown of the Http2Session. Graceful shutdown
  // is only supported on the server-side
  shutdown(options, callback) {
    const state = this[kState];
    if (state.destroyed || state.destroying)
      throw new errors.Error('ERR_HTTP2_INVALID_SESSION');

    if (state.shutdown || state.shuttingDown)
      return;

    const type = this[kType];

    if (typeof options === 'function') {
      callback = options;
      options = undefined;
    }

    assertIsObject(options, 'options');
    options = Object.assign(Object.create(null), options);

    if (options.opaqueData !== undefined &&
        !isUint8Array(options.opaqueData)) {
      throw new errors.TypeError('ERR_INVALID_OPT_VALUE',
                                 'opaqueData',
                                 options.opaqueData);
    }
    if (type === NGHTTP2_SESSION_SERVER &&
        options.graceful !== undefined &&
        typeof options.graceful !== 'boolean') {
      throw new errors.TypeError('ERR_INVALID_OPT_VALUE',
                                 'graceful',
                                 options.graceful);
    }
    if (options.errorCode !== undefined &&
        typeof options.errorCode !== 'number') {
      throw new errors.TypeError('ERR_INVALID_OPT_VALUE',
                                 'errorCode',
                                 options.errorCode);
    }
    if (options.lastStreamID !== undefined &&
        (typeof options.lastStreamID !== 'number' ||
         options.lastStreamID < 0)) {
      throw new errors.TypeError('ERR_INVALID_OPT_VALUE',
                                 'lastStreamID',
                                 options.lastStreamID);
    }

    debug(`[${sessionName(type)}] initiating shutdown`);
    state.shuttingDown = true;

    if (callback) {
      this.on('shutdown', callback);
    }

    if (state.connecting) {
      debug(`[${sessionName(type)}] session still connecting, queue ` +
            'shutdown');
      this.once('connect', submitShutdown.bind(this, options));
      return;
    }

    debug(`[${sessionName(type)}] sending shutdown`);
    submitShutdown.call(this, options);
  }

  _onTimeout() {
    // This checks whether a write is currently in progress and also whether
    // that write is actually sending data across the write. The kHandle
    // stored `chunksSentSinceLastWrite` is only updated when a timeout event
    // happens, meaning that if a write is ongoing it should never equal the
    // newly fetched, updated value.
    if (this[kState].writeQueueSize > 0) {
      const handle = this[kHandle];
      const chunksSentSinceLastWrite = handle !== undefined ?
        handle.chunksSentSinceLastWrite : null;
      if (chunksSentSinceLastWrite !== null &&
          chunksSentSinceLastWrite !== handle.updateChunksSent()) {
        _unrefActive(this);
        return;
      }
    }

    process.nextTick(emit, this, 'timeout');
  }
}

class ServerHttp2Session extends Http2Session {
  constructor(options, socket, server) {
    super(NGHTTP2_SESSION_SERVER, options, socket);
    this[kServer] = server;
  }

  get server() {
    return this[kServer];
  }
}

class ClientHttp2Session extends Http2Session {
  constructor(options, socket) {
    super(NGHTTP2_SESSION_CLIENT, options, socket);
    debug(`[${sessionName(this[kType])}] clienthttp2session created`);
  }

  // Submits a new HTTP2 request to the connected peer. Returns the
  // associated Http2Stream instance.
  request(headers, options) {
    const state = this[kState];
    if (state.destroyed || state.destroying)
      throw new errors.Error('ERR_HTTP2_INVALID_SESSION');
    debug(`[${sessionName(this[kType])}] initiating request`);
    _unrefActive(this);
    assertIsObject(headers, 'headers');
    assertIsObject(options, 'options');

    headers = Object.assign(Object.create(null), headers);
    options = Object.assign(Object.create(null), options);

    if (headers[HTTP2_HEADER_METHOD] === undefined)
      headers[HTTP2_HEADER_METHOD] = HTTP2_METHOD_GET;

    const connect = headers[HTTP2_HEADER_METHOD] === HTTP2_METHOD_CONNECT;

    if (!connect) {
      if (headers[HTTP2_HEADER_AUTHORITY] === undefined)
        headers[HTTP2_HEADER_AUTHORITY] = this[kAuthority];
      if (headers[HTTP2_HEADER_SCHEME] === undefined)
        headers[HTTP2_HEADER_SCHEME] = this[kProtocol].slice(0, -1);
      if (headers[HTTP2_HEADER_PATH] === undefined)
        headers[HTTP2_HEADER_PATH] = '/';
    } else {
      if (headers[HTTP2_HEADER_AUTHORITY] === undefined)
        throw new errors.Error('ERR_HTTP2_CONNECT_AUTHORITY');
      if (headers[HTTP2_HEADER_SCHEME] !== undefined)
        throw new errors.Error('ERR_HTTP2_CONNECT_SCHEME');
      if (headers[HTTP2_HEADER_PATH] !== undefined)
        throw new errors.Error('ERR_HTTP2_CONNECT_PATH');
    }

    validatePriorityOptions(options);

    if (options.endStream === undefined) {
      // For some methods, we know that a payload is meaningless, so end the
      // stream by default if the user has not specifically indicated a
      // preference.
      options.endStream = isPayloadMeaningless(headers[HTTP2_HEADER_METHOD]);
    } else if (typeof options.endStream !== 'boolean') {
      throw new errors.TypeError('ERR_INVALID_OPT_VALUE',
                                 'endStream',
                                 options.endStream);
    }

    if (options.getTrailers !== undefined &&
      typeof options.getTrailers !== 'function') {
      throw new errors.TypeError('ERR_INVALID_OPT_VALUE',
                                 'getTrailers',
                                 options.getTrailers);
    }

    const stream = new ClientHttp2Stream(this, undefined, {});

    const onConnect = requestOnConnect.bind(stream, headers, options);

    // Close the writable side of the stream if options.endStream is set.
    if (options.endStream)
      stream.end();

    if (state.connecting) {
      debug(`[${sessionName(this[kType])}] session still connecting, queue ` +
            'stream init');
      stream.on('connect', onConnect);
    } else {
      debug(`[${sessionName(this[kType])}] session connected, immediate ` +
            'stream init');
      onConnect();
    }
    return stream;
  }
}

function createWriteReq(req, handle, data, encoding) {
  switch (encoding) {
    case 'utf8':
    case 'utf-8':
      return handle.writeUtf8String(req, data);
    case 'ascii':
      return handle.writeAsciiString(req, data);
    case 'ucs2':
    case 'ucs-2':
    case 'utf16le':
    case 'utf-16le':
      return handle.writeUcs2String(req, data);
    case 'latin1':
    case 'binary':
      return handle.writeLatin1String(req, data);
    case 'buffer':
      return handle.writeBuffer(req, data);
    default:
      return handle.writeBuffer(req, Buffer.from(data, encoding));
  }
}

function trackWriteState(stream, bytes) {
  const session = stream[kSession];
  stream[kState].writeQueueSize += bytes;
  session[kState].writeQueueSize += bytes;
  session[kHandle].chunksSentSinceLastWrite = 0;
}

function afterDoStreamWrite(status, handle, req) {
  const session = handle[kOwner];
  _unrefActive(session);

  const state = session[kState];
  const { bytes } = req;
  state.writeQueueSize -= bytes;

  const stream = state.streams.get(req.stream);
  if (stream !== undefined) {
    _unrefActive(stream);
    stream[kState].writeQueueSize -= bytes;
  }

  if (typeof req.callback === 'function')
    req.callback();
  this.handle = undefined;
}

function onHandleFinish() {
  const session = this[kSession];
  if (session === undefined) return;
  if (this[kID] === undefined) {
    this.once('ready', onHandleFinish);
  } else {
    const handle = session[kHandle];
    if (handle !== undefined) {
      // Shutdown on the next tick of the event loop just in case there is
      // still data pending in the outbound queue.
      assert(handle.shutdownStream(this[kID]) === undefined,
             `The stream ${this[kID]} does not exist. Please report this as ` +
             'a bug in Node.js');
    }
  }
}

function onSessionClose(hadError, code) {
  abort(this);
  // Close the readable side
  this.push(null);
  // Close the writable side
  this.end();
}

function onStreamClosed(code) {
  abort(this);
  // Close the readable side
  this.push(null);
  // Close the writable side
  this.end();
}

function streamOnResume() {
  if (this[kID] === undefined) {
    this.once('ready', streamOnResume);
    return;
  }
  const session = this[kSession];
  if (session) {
    assert(session[kHandle].streamReadStart(this[kID]) === undefined,
           `HTTP/2 Stream ${this[kID]} does not exist. Please report this as ` +
           'a bug in Node.js');
  }
}

function streamOnPause() {
  const session = this[kSession];
  if (session) {
    assert(session[kHandle].streamReadStop(this[kID]) === undefined,
           `HTTP/2 Stream ${this[kID]} does not exist. Please report this as ' +
           'a bug in Node.js`);
  }
}

function handleFlushData(handle, streamID) {
  assert(handle.flushData(streamID) === undefined,
         `HTTP/2 Stream ${streamID} does not exist. Please report this as ` +
         'a bug in Node.js');
}

function streamOnSessionConnect() {
  const session = this[kSession];
  debug(`[${sessionName(session[kType])}] session connected. emiting stream ` +
        'connect');
  this[kState].connecting = false;
  process.nextTick(emit, this, 'connect');
}

function streamOnceReady() {
  const session = this[kSession];
  debug(`[${sessionName(session[kType])}] stream ${this[kID]} is ready`);
  this.uncork();
}

function abort(stream) {
  if (!stream[kState].aborted &&
      !(stream._writableState.ended || stream._writableState.ending)) {
    stream.emit('aborted');
    stream[kState].aborted = true;
  }
}

// An Http2Stream is a Duplex stream. On the server-side, the Readable side
// provides access to the received request data. On the client-side, the
// Readable side provides access to the received response data. On the
// server side, the writable side is used to transmit response data, while
// on the client side it is used to transmit request data.
class Http2Stream extends Duplex {
  constructor(session, options) {
    options.allowHalfOpen = true;
    options.decodeStrings = false;
    super(options);
    this.cork();
    this[kSession] = session;

    const state = this[kState] = {
      rst: false,
      rstCode: NGHTTP2_NO_ERROR,
      headersSent: false,
      headRequest: false,
      aborted: false,
      closeHandler: onSessionClose.bind(this),
      writeQueueSize: 0
    };

    this.once('ready', streamOnceReady);
    this.once('streamClosed', onStreamClosed);
    this.once('finish', onHandleFinish);
    this.on('resume', streamOnResume);
    this.on('pause', streamOnPause);
    session.once('close', state.closeHandler);

    if (session[kState].connecting) {
      debug(`[${sessionName(session[kType])}] session is still connecting, ` +
            'queuing stream init');
      state.connecting = true;
      session.once('connect', streamOnSessionConnect.bind(this));
    }
    debug(`[${sessionName(session[kType])}] http2stream created`);
  }

  [kInit](id) {
    this[kID] = id;
    this.emit('ready');
  }

  [kInspect](depth, opts) {
    const obj = {
      id: this[kID],
      state: this.state,
      readableState: this._readableState,
      writableState: this._writableState
    };
    return `Http2Stream ${util.format(obj)}`;
  }

  // The id of the Http2Stream, will be undefined if the socket is not
  // yet connected.
  get id() {
    return this[kID];
  }

  // The Http2Session that owns this Http2Stream.
  get session() {
    return this[kSession];
  }

  _onTimeout() {
    // This checks whether a write is currently in progress and also whether
    // that write is actually sending data across the write. The kHandle
    // stored `chunksSentSinceLastWrite` is only updated when a timeout event
    // happens, meaning that if a write is ongoing it should never equal the
    // newly fetched, updated value.
    if (this[kState].writeQueueSize > 0) {
      const handle = this[kSession][kHandle];
      const chunksSentSinceLastWrite = handle !== undefined ?
        handle.chunksSentSinceLastWrite : null;
      if (chunksSentSinceLastWrite !== null &&
          chunksSentSinceLastWrite !== handle.updateChunksSent()) {
        _unrefActive(this);
        _unrefActive(this[kSession]);
        return;
      }
    }

    process.nextTick(emit, this, 'timeout');
  }

  // true if the Http2Stream was aborted abornomally.
  get aborted() {
    return this[kState].aborted;
  }

  // true if dealing with a HEAD request
  get headRequest() {
    return this[kState].headRequest;
  }

  // The error code reported when this Http2Stream was closed.
  get rstCode() {
    return this[kState].rst ? this[kState].rstCode : undefined;
  }

  // State information for the Http2Stream
  get state() {
    const id = this[kID];
    if (this.destroyed || id === undefined)
      return Object.create(null);
    return getStreamState(this[kSession][kHandle], id);
  }

  [kProceed]() {
    assert.fail(null, null,
                'Implementors MUST implement this. Please report this as a ' +
                'bug in Node.js');
  }

  _write(data, encoding, cb) {
    if (this[kID] === undefined) {
      this.once('ready', this._write.bind(this, data, encoding, cb));
      return;
    }
    if (!this[kState].headersSent)
      this[kProceed]();
    const session = this[kSession];
    _unrefActive(this);
    _unrefActive(session);
    const handle = session[kHandle];
    const req = new WriteWrap();
    req.stream = this[kID];
    req.handle = handle;
    req.callback = cb;
    req.oncomplete = afterDoStreamWrite;
    req.async = false;
    const err = createWriteReq(req, handle, data, encoding);
    if (err)
      throw util._errnoException(err, 'write', req.error);
    trackWriteState(this, req.bytes);
  }

  _writev(data, cb) {
    if (this[kID] === undefined) {
      this.once('ready', this._writev.bind(this, data, cb));
      return;
    }
    if (!this[kState].headersSent)
      this[kProceed]();
    const session = this[kSession];
    _unrefActive(this);
    _unrefActive(session);
    const handle = session[kHandle];
    const req = new WriteWrap();
    req.stream = this[kID];
    req.handle = handle;
    req.callback = cb;
    req.oncomplete = afterDoStreamWrite;
    req.async = false;
    const chunks = new Array(data.length << 1);
    for (var i = 0; i < data.length; i++) {
      const entry = data[i];
      chunks[i * 2] = entry.chunk;
      chunks[i * 2 + 1] = entry.encoding;
    }
    const err = handle.writev(req, chunks);
    if (err)
      throw util._errnoException(err, 'write', req.error);
    trackWriteState(this, req.bytes);
  }

  _read(nread) {
    if (this[kID] === undefined) {
      this.once('ready', this._read.bind(this, nread));
      return;
    }
    if (this.destroyed) {
      this.push(null);
      return;
    }
    _unrefActive(this);
    process.nextTick(handleFlushData, this[kSession][kHandle], this[kID]);
  }

  // Submits an RST-STREAM frame to shutdown this stream.
  // If the stream ID has not yet been allocated, the action will
  // defer until the ready event is emitted.
  // After sending the rstStream, this.destroy() will be called making
  // the stream object no longer usable.
  rstStream(code = NGHTTP2_NO_ERROR) {
    if (this.destroyed)
      throw new errors.Error('ERR_HTTP2_INVALID_STREAM');
    const session = this[kSession];
    if (this[kID] === undefined) {
      debug(
        `[${sessionName(session[kType])}] queuing rstStream for new stream`);
      this.once('ready', this.rstStream.bind(this, code));
      return;
    }
    debug(`[${sessionName(session[kType])}] sending rstStream for stream ` +
          `${this[kID]}: ${code}`);
    _unrefActive(this);
    session.rstStream(this, code);
  }

  rstWithNoError() {
    this.rstStream(NGHTTP2_NO_ERROR);
  }

  rstWithProtocolError() {
    this.rstStream(NGHTTP2_PROTOCOL_ERROR);
  }

  rstWithCancel() {
    this.rstStream(NGHTTP2_CANCEL);
  }

  rstWithRefuse() {
    this.rstStream(NGHTTP2_REFUSED_STREAM);
  }

  rstWithInternalError() {
    this.rstStream(NGHTTP2_INTERNAL_ERROR);
  }

  // Note that this (and other methods like additionalHeaders and rstStream)
  // cause nghttp to queue frames up in its internal buffer that are not
  // actually sent on the wire until the next tick of the event loop. The
  // semantics of this method then are: queue a priority frame to be sent and
  // not immediately send the priority frame. There is current no callback
  // triggered when the data is actually sent.
  priority(options) {
    if (this.destroyed)
      throw new errors.Error('ERR_HTTP2_INVALID_STREAM');
    const session = this[kSession];
    if (this[kID] === undefined) {
      debug(`[${sessionName(session[kType])}] queuing priority for new stream`);
      this.once('ready', this.priority.bind(this, options));
      return;
    }
    debug(`[${sessionName(session[kType])}] sending priority for stream ` +
          `${this[kID]}`);
    _unrefActive(this);
    session.priority(this, options);
  }

  // Called by this.destroy().
  // * If called before the stream is allocated, will defer until the
  //   ready event is emitted.
  // * Will submit an RST stream to shutdown the stream if necessary.
  //   This will cause the internal resources to be released.
  // * Then cleans up the resources on the js side
  _destroy(err, callback) {
    const session = this[kSession];
    if (this[kID] === undefined) {
      debug(`[${sessionName(session[kType])}] queuing destroy for new stream`);
      this.once('ready', this._destroy.bind(this, err, callback));
      return;
    }

    const state = this[kState];
    session[kState].writeQueueSize -= state.writeQueueSize;
    state.writeQueueSize = 0;

    const server = session[kServer];
    if (server !== undefined && err) {
      server.emit('streamError', err, this);
    }

    process.nextTick(continueStreamDestroy, this, err, callback);
  }
}

function continueStreamDestroy(self, err, callback) {
  const session = self[kSession];
  const state = self[kState];

  debug(`[${sessionName(session[kType])}] destroying stream ${self[kID]}`);

  // Submit RST-STREAM frame if one hasn't been sent already and the
  // stream hasn't closed normally...
  const rst = state.rst;
  let code = state.rstCode;
  if (!rst && !session.destroyed) {
    code = err instanceof Error ? NGHTTP2_INTERNAL_ERROR : NGHTTP2_NO_ERROR;
    session.rstStream(self, code);
  }

  // Remove the close handler on the session
  session.removeListener('close', state.closeHandler);

  // Unenroll the timer
  self.setTimeout(0);

  setImmediate(finishStreamDestroy, self, session[kHandle]);

  // RST code 8 not emitted as an error as its used by clients to signify
  // abort and is already covered by aborted event, also allows more
  // seamless compatibility with http1
  if (code !== NGHTTP2_NO_ERROR && code !== NGHTTP2_CANCEL && !err) {
    err = new errors.Error('ERR_HTTP2_STREAM_ERROR', code);
  }
  callback(err);
  process.nextTick(emit, self, 'streamClosed', code);
  debug(`[${sessionName(session[kType])}] stream ${self[kID]} destroyed`);
}

function finishStreamDestroy(self, handle) {
  const id = self[kID];
  self[kSession][kState].streams.delete(id);
  delete self[kSession];
  if (handle !== undefined)
    handle.destroyStream(id);
  self.emit('destroy');
}

function processHeaders(headers) {
  assertIsObject(headers, 'headers');
  headers = Object.assign(Object.create(null), headers);
  if (headers[HTTP2_HEADER_STATUS] == null)
    headers[HTTP2_HEADER_STATUS] = HTTP_STATUS_OK;
  headers[HTTP2_HEADER_DATE] = utcDate();

  const statusCode = headers[HTTP2_HEADER_STATUS] |= 0;
  // This is intentionally stricter than the HTTP/1 implementation, which
  // allows values between 100 and 999 (inclusive) in order to allow for
  // backwards compatibility with non-spec compliant code. With HTTP/2,
  // we have the opportunity to start fresh with stricter spec copmliance.
  // This will have an impact on the compatibility layer for anyone using
  // non-standard, non-compliant status codes.
  if (statusCode < 200 || statusCode > 599)
    throw new errors.RangeError('ERR_HTTP2_STATUS_INVALID',
                                headers[HTTP2_HEADER_STATUS]);

  return headers;
}

function processRespondWithFD(fd, headers, offset = 0, length = -1,
                              streamOptions = 0) {
  const session = this[kSession];
  const state = this[kState];
  state.headersSent = true;

  // Close the writable side of the stream
  this.end();

  const ret = session[kHandle].submitFile(this[kID], fd, headers,
                                          offset, length, streamOptions);
  let err;
  switch (ret) {
    case NGHTTP2_ERR_NOMEM:
      err = new errors.Error('ERR_OUTOFMEMORY');
      process.nextTick(emit, session, 'error', err);
      break;
    default:
      if (ret < 0) {
        err = new NghttpError(ret);
        process.nextTick(emit, this, 'error', err);
        break;
      }
      // exact length of the file doesn't matter here, since the
      // stream is closing anyway — just use 1 to signify that
      // a write does exist
      trackWriteState(this, 1);
  }
}

function doSendFD(session, options, fd, headers, streamOptions, err, stat) {
  if (this.destroyed || session.destroyed) {
    abort(this);
    return;
  }
  if (err) {
    process.nextTick(emit, this, 'error', err);
    return;
  }

  const statOptions = {
    offset: options.offset !== undefined ? options.offset : 0,
    length: options.length !== undefined ? options.length : -1
  };

  if (typeof options.statCheck === 'function' &&
      options.statCheck.call(this, stat, headers, statOptions) === false) {
    return;
  }

  const headersList = mapToHeaders(headers,
                                   assertValidPseudoHeaderResponse);
  if (!Array.isArray(headersList)) {
    process.nextTick(emit, this, 'error', headersList);
    return;
  }

  processRespondWithFD.call(this, fd, headersList,
                            statOptions.offset,
                            statOptions.length,
                            streamOptions);
}

function doSendFileFD(session, options, fd, headers, streamOptions, err, stat) {
  if (this.destroyed || session.destroyed) {
    abort(this);
    return;
  }
  const onError = options.onError;

  if (err) {
    if (onError) {
      onError(err);
    } else {
      this.destroy(err);
    }
    return;
  }

  if (!stat.isFile()) {
    err = new errors.Error('ERR_HTTP2_SEND_FILE');
    if (onError) {
      onError(err);
    } else {
      this.destroy(err);
    }
    return;
  }

  const statOptions = {
    offset: options.offset !== undefined ? options.offset : 0,
    length: options.length !== undefined ? options.length : -1
  };

  // Set the content-length by default
  if (typeof options.statCheck === 'function' &&
      options.statCheck.call(this, stat, headers) === false) {
    return;
  }

  statOptions.length =
    statOptions.length < 0 ? stat.size - (+statOptions.offset) :
      Math.min(stat.size - (+statOptions.offset),
               statOptions.length);

  if (headers[HTTP2_HEADER_CONTENT_LENGTH] === undefined)
    headers[HTTP2_HEADER_CONTENT_LENGTH] = statOptions.length;

  const headersList = mapToHeaders(headers,
                                   assertValidPseudoHeaderResponse);
  if (!Array.isArray(headersList)) {
    process.nextTick(emit, this, 'error', headersList);
    return;
  }

  processRespondWithFD.call(this, fd, headersList,
                            options.offset,
                            options.length,
                            streamOptions);
}

function afterOpen(session, options, headers, streamOptions, err, fd) {
  const state = this[kState];
  const onError = options.onError;
  if (this.destroyed || session.destroyed) {
    abort(this);
    return;
  }
  if (err) {
    if (onError) {
      onError(err);
    } else {
      this.destroy(err);
    }
    return;
  }
  state.fd = fd;

  fs.fstat(fd,
           doSendFileFD.bind(this, session, options, fd,
                             headers, streamOptions));
}

function streamOnError(err) {
  // we swallow the error for parity with HTTP1
  // all the errors that ends here are not critical for the project
  debug('ServerHttp2Stream errored, avoiding uncaughtException', err);
}


class ServerHttp2Stream extends Http2Stream {
  constructor(session, id, options, headers) {
    super(session, options);
    this[kInit](id);
    this[kProtocol] = headers[HTTP2_HEADER_SCHEME];
    this[kAuthority] = headers[HTTP2_HEADER_AUTHORITY];
    this.on('error', streamOnError);
    debug(`[${sessionName(session[kType])}] created serverhttp2stream`);
  }

  // true if the HEADERS frame has been sent
  get headersSent() {
    return this[kState].headersSent;
  }

  // true if the remote peer accepts push streams
  get pushAllowed() {
    return this[kSession].remoteSettings.enablePush;
  }

  // create a push stream, call the given callback with the created
  // Http2Stream for the push stream.
  pushStream(headers, options, callback) {
    if (this.destroyed)
      throw new errors.Error('ERR_HTTP2_INVALID_STREAM');

    const session = this[kSession];
    debug(`[${sessionName(session[kType])}] initiating push stream for stream` +
          ` ${this[kID]}`);

    _unrefActive(this);
    const state = session[kState];
    const streams = state.streams;

    if (!session.remoteSettings.enablePush)
      throw new errors.Error('ERR_HTTP2_PUSH_DISABLED');

    if (typeof options === 'function') {
      callback = options;
      options = undefined;
    }

    if (typeof callback !== 'function')
      throw new errors.TypeError('ERR_INVALID_CALLBACK');

    assertIsObject(options, 'options');
    options = Object.assign(Object.create(null), options);
    options.endStream = !!options.endStream;

    assertIsObject(headers, 'headers');
    headers = Object.assign(Object.create(null), headers);

    if (headers[HTTP2_HEADER_METHOD] === undefined)
      headers[HTTP2_HEADER_METHOD] = HTTP2_METHOD_GET;
    if (headers[HTTP2_HEADER_AUTHORITY] === undefined)
      headers[HTTP2_HEADER_AUTHORITY] = this[kAuthority];
    if (headers[HTTP2_HEADER_SCHEME] === undefined)
      headers[HTTP2_HEADER_SCHEME] = this[kProtocol];
    if (headers[HTTP2_HEADER_PATH] === undefined)
      headers[HTTP2_HEADER_PATH] = '/';

    let headRequest = false;
    if (headers[HTTP2_HEADER_METHOD] === HTTP2_METHOD_HEAD) {
      headRequest = true;
      options.endStream = true;
    }

    const headersList = mapToHeaders(headers);
    if (!Array.isArray(headersList)) {
      // An error occurred!
      throw headersList;
    }

    const streamOptions = options.endStream ? STREAM_OPTION_EMPTY_PAYLOAD : 0;

    const ret = session[kHandle].submitPushPromise(this[kID],
                                                   headersList,
                                                   streamOptions);
    let err;
    switch (ret) {
      case NGHTTP2_ERR_NOMEM:
        err = new errors.Error('ERR_OUTOFMEMORY');
        process.nextTick(emit, session, 'error', err);
        break;
      case NGHTTP2_ERR_STREAM_ID_NOT_AVAILABLE:
        err = new errors.Error('ERR_HTTP2_OUT_OF_STREAMS');
        process.nextTick(emit, session, 'error', err);
        break;
      case NGHTTP2_ERR_STREAM_CLOSED:
        err = new errors.Error('ERR_HTTP2_STREAM_CLOSED');
        process.nextTick(emit, this, 'error', err);
        break;
      default:
        if (ret <= 0) {
          err = new NghttpError(ret);
          process.nextTick(emit, this, 'error', err);
          break;
        }
        debug(`[${sessionName(session[kType])}] push stream ${ret} created`);
        options.readable = !options.endStream;

        const stream = new ServerHttp2Stream(session, ret, options, headers);

        // If the push stream is a head request, close the writable side of
        // the stream immediately as there won't be any data sent.
        if (headRequest) {
          stream.end();
          const state = stream[kState];
          state.headRequest = true;
        }

        streams.set(ret, stream);
        process.nextTick(callback, stream, headers, 0);
    }
  }

  // Initiate a response on this Http2Stream
  respond(headers, options) {
    const session = this[kSession];
    if (this.destroyed)
      throw new errors.Error('ERR_HTTP2_INVALID_STREAM');
    debug(`[${sessionName(session[kType])}] initiating response for stream ` +
          `${this[kID]}`);
    _unrefActive(this);
    const state = this[kState];

    if (state.headersSent)
      throw new errors.Error('ERR_HTTP2_HEADERS_SENT');

    assertIsObject(options, 'options');
    options = Object.assign(Object.create(null), options);
    options.endStream = !!options.endStream;

    let streamOptions = 0;
    if (options.endStream)
      streamOptions |= STREAM_OPTION_EMPTY_PAYLOAD;

    if (options.getTrailers !== undefined) {
      if (typeof options.getTrailers !== 'function') {
        throw new errors.TypeError('ERR_INVALID_OPT_VALUE',
                                   'getTrailers',
                                   options.getTrailers);
      }
      streamOptions |= STREAM_OPTION_GET_TRAILERS;
      state.getTrailers = options.getTrailers;
    }

    headers = processHeaders(headers);
    const statusCode = headers[HTTP2_HEADER_STATUS] |= 0;

    // Payload/DATA frames are not permitted in these cases so set
    // the options.endStream option to true so that the underlying
    // bits do not attempt to send any.
    if (statusCode === HTTP_STATUS_NO_CONTENT ||
        statusCode === HTTP_STATUS_RESET_CONTENT ||
        statusCode === HTTP_STATUS_NOT_MODIFIED ||
        state.headRequest === true) {
      options.endStream = true;
    }

    const headersList = mapToHeaders(headers, assertValidPseudoHeaderResponse);
    if (!Array.isArray(headersList)) {
      // An error occurred!
      throw headersList;
    }
    state.headersSent = true;

    // Close the writable side if the endStream option is set
    if (options.endStream)
      this.end();

    const ret = session[kHandle].submitResponse(this[kID],
                                                headersList,
                                                streamOptions);
    let err;
    switch (ret) {
      case NGHTTP2_ERR_NOMEM:
        err = new errors.Error('ERR_OUTOFMEMORY');
        process.nextTick(emit, session, 'error', err);
        break;
      default:
        if (ret < 0) {
          err = new NghttpError(ret);
          process.nextTick(emit, this, 'error', err);
        }
    }
  }

  // Initiate a response using an open FD. Note that there are fewer
  // protections with this approach. For one, the fd is not validated by
  // default. In respondWithFile, the file is checked to make sure it is a
  // regular file, here the fd is passed directly. If the underlying
  // mechanism is not able to read from the fd, then the stream will be
  // reset with an error code.
  respondWithFD(fd, headers, options) {
    const session = this[kSession];
    if (this.destroyed)
      throw new errors.Error('ERR_HTTP2_INVALID_STREAM');
    debug(`[${sessionName(session[kType])}] initiating response for stream ` +
          `${this[kID]}`);
    _unrefActive(this);
    const state = this[kState];

    if (state.headersSent)
      throw new errors.Error('ERR_HTTP2_HEADERS_SENT');

    assertIsObject(options, 'options');
    options = Object.assign(Object.create(null), options);

    if (options.offset !== undefined && typeof options.offset !== 'number')
      throw new errors.TypeError('ERR_INVALID_OPT_VALUE',
                                 'offset',
                                 options.offset);

    if (options.length !== undefined && typeof options.length !== 'number')
      throw new errors.TypeError('ERR_INVALID_OPT_VALUE',
                                 'length',
                                 options.length);

    if (options.statCheck !== undefined &&
        typeof options.statCheck !== 'function') {
      throw new errors.TypeError('ERR_INVALID_OPT_VALUE',
                                 'statCheck',
                                 options.statCheck);
    }

    let streamOptions = 0;
    if (options.getTrailers !== undefined) {
      if (typeof options.getTrailers !== 'function') {
        throw new errors.TypeError('ERR_INVALID_OPT_VALUE',
                                   'getTrailers',
                                   options.getTrailers);
      }
      streamOptions |= STREAM_OPTION_GET_TRAILERS;
      state.getTrailers = options.getTrailers;
    }

    if (typeof fd !== 'number')
      throw new errors.TypeError('ERR_INVALID_ARG_TYPE',
                                 'fd', 'number');

    headers = processHeaders(headers);
    const statusCode = headers[HTTP2_HEADER_STATUS] |= 0;
    // Payload/DATA frames are not permitted in these cases
    if (statusCode === HTTP_STATUS_NO_CONTENT ||
        statusCode === HTTP_STATUS_RESET_CONTENT ||
        statusCode === HTTP_STATUS_NOT_MODIFIED) {
      throw new errors.Error('ERR_HTTP2_PAYLOAD_FORBIDDEN', statusCode);
    }

    if (options.statCheck !== undefined) {
      fs.fstat(fd,
               doSendFD.bind(this, session, options, fd,
                             headers, streamOptions));
      return;
    }

    const headersList = mapToHeaders(headers,
                                     assertValidPseudoHeaderResponse);
    if (!Array.isArray(headersList)) {
      process.nextTick(emit, this, 'error', headersList);
      return;
    }

    processRespondWithFD.call(this, fd, headersList,
                              options.offset,
                              options.length,
                              streamOptions);
  }

  // Initiate a file response on this Http2Stream. The path is passed to
  // fs.open() to acquire the fd with mode 'r', then the fd is passed to
  // fs.fstat(). Assuming fstat is successful, a check is made to ensure
  // that the file is a regular file, then options.statCheck is called,
  // giving the user an opportunity to verify the details and set additional
  // headers. If statCheck returns false, the operation is aborted and no
  // file details are sent.
  respondWithFile(path, headers, options) {
    const session = this[kSession];
    if (this.destroyed)
      throw new errors.Error('ERR_HTTP2_INVALID_STREAM');
    debug(`[${sessionName(session[kType])}] initiating response for stream ` +
          `${this[kID]}`);
    _unrefActive(this);
    const state = this[kState];

    if (state.headersSent)
      throw new errors.Error('ERR_HTTP2_HEADERS_SENT');

    assertIsObject(options, 'options');
    options = Object.assign(Object.create(null), options);

    if (options.offset !== undefined && typeof options.offset !== 'number')
      throw new errors.TypeError('ERR_INVALID_OPT_VALUE',
                                 'offset',
                                 options.offset);

    if (options.length !== undefined && typeof options.length !== 'number')
      throw new errors.TypeError('ERR_INVALID_OPT_VALUE',
                                 'length',
                                 options.length);

    if (options.statCheck !== undefined &&
        typeof options.statCheck !== 'function') {
      throw new errors.TypeError('ERR_INVALID_OPT_VALUE',
                                 'statCheck',
                                 options.statCheck);
    }

    let streamOptions = 0;
    if (options.getTrailers !== undefined) {
      if (typeof options.getTrailers !== 'function') {
        throw new errors.TypeError('ERR_INVALID_OPT_VALUE',
                                   'getTrailers',
                                   options.getTrailers);
      }
      streamOptions |= STREAM_OPTION_GET_TRAILERS;
      state.getTrailers = options.getTrailers;
    }

    headers = processHeaders(headers);
    const statusCode = headers[HTTP2_HEADER_STATUS] |= 0;
    // Payload/DATA frames are not permitted in these cases
    if (statusCode === HTTP_STATUS_NO_CONTENT ||
        statusCode === HTTP_STATUS_RESET_CONTENT ||
        statusCode === HTTP_STATUS_NOT_MODIFIED) {
      throw new errors.Error('ERR_HTTP2_PAYLOAD_FORBIDDEN', statusCode);
    }

    fs.open(path, 'r',
            afterOpen.bind(this, session, options, headers, streamOptions));
  }

  // Sends a block of informational headers. In theory, the HTTP/2 spec
  // allows sending a HEADER block at any time during a streams lifecycle,
  // but the HTTP request/response semantics defined in HTTP/2 places limits
  // such that HEADERS may only be sent *before* or *after* DATA frames.
  // If the block of headers being sent includes a status code, it MUST be
  // a 1xx informational code and it MUST be sent before the request/response
  // headers are sent, or an error will be thrown.
  additionalHeaders(headers) {
    if (this.destroyed)
      throw new errors.Error('ERR_HTTP2_INVALID_STREAM');

    if (this[kState].headersSent)
      throw new errors.Error('ERR_HTTP2_HEADERS_AFTER_RESPOND');

    const session = this[kSession];
    debug(`[${sessionName(session[kType])}] sending additional headers`);

    assertIsObject(headers, 'headers');
    headers = Object.assign(Object.create(null), headers);
    if (headers[HTTP2_HEADER_STATUS] != null) {
      const statusCode = headers[HTTP2_HEADER_STATUS] |= 0;
      if (statusCode === HTTP_STATUS_SWITCHING_PROTOCOLS)
        throw new errors.Error('ERR_HTTP2_STATUS_101');
      if (statusCode < 100 || statusCode >= 200) {
        throw new errors.RangeError('ERR_HTTP2_INVALID_INFO_STATUS',
                                    headers[HTTP2_HEADER_STATUS]);
      }
    }

    _unrefActive(this);

    const headersList = mapToHeaders(headers,
                                     assertValidPseudoHeaderResponse);
    if (!Array.isArray(headersList)) {
      throw headersList;
    }

    const ret = session[kHandle].sendHeaders(this[kID], headersList);
    let err;
    switch (ret) {
      case NGHTTP2_ERR_NOMEM:
        err = new errors.Error('ERR_OUTOFMEMORY');
        process.nextTick(emit, session, 'error', err);
        break;
      default:
        if (ret < 0) {
          err = new NghttpError(ret);
          process.nextTick(emit, this, 'error', err);
        }
    }
  }
}

ServerHttp2Stream.prototype[kProceed] = ServerHttp2Stream.prototype.respond;

class ClientHttp2Stream extends Http2Stream {
  constructor(session, id, options) {
    super(session, options);
    this[kState].headersSent = true;
    if (id !== undefined)
      this[kInit](id);
    this.on('headers', handleHeaderContinue);
    debug(`[${sessionName(session[kType])}] clienthttp2stream created`);
  }
}

function handleHeaderContinue(headers) {
  if (headers[HTTP2_HEADER_STATUS] === HTTP_STATUS_CONTINUE) {
    this.emit('continue');
  }
}

const setTimeout = {
  configurable: true,
  enumerable: true,
  writable: true,
  value: function(msecs, callback) {
    if (typeof msecs !== 'number') {
      throw new errors.TypeError('ERR_INVALID_ARG_TYPE',
                                 'msecs',
                                 'number');
    }
    if (msecs === 0) {
      unenroll(this);
      if (callback !== undefined) {
        if (typeof callback !== 'function')
          throw new errors.TypeError('ERR_INVALID_CALLBACK');
        this.removeListener('timeout', callback);
      }
    } else {
      enroll(this, msecs);
      _unrefActive(this);
      if (callback !== undefined) {
        if (typeof callback !== 'function')
          throw new errors.TypeError('ERR_INVALID_CALLBACK');
        this.once('timeout', callback);
      }
    }
    return this;
  }
};
Object.defineProperty(Http2Stream.prototype, 'setTimeout', setTimeout);
Object.defineProperty(Http2Session.prototype, 'setTimeout', setTimeout);

// --------------------------------------------------------------------

// Set as a replacement for socket.prototype.destroy upon the
// establishment of a new connection.
function socketDestroy(error) {
  const session = this[kSession];
  const type = session[kType];
  debug(`[${sessionName(type)}] socket destroy called`);
  delete this[kServer];
  // destroy the session first so that it will stop trying to
  // send data while we close the socket.
  session.destroy();
  this.destroy = this[kDestroySocket];
  debug(`[${sessionName(type)}] destroying the socket`);
  this.destroy(error);
}

// When an Http2Session emits an error, first try to forward it to the
// server as a sessionError; failing that, forward it to the socket as
// a sessionError; failing that, destroy, remove the error listener, and
// re-emit the error event
function sessionOnError(error) {
  debug(`[${sessionName(this[kType])}] server session error: ${error.message}`);
  if (this[kServer] !== undefined && this[kServer].emit('sessionError', error))
    return;
  if (this[kSocket] !== undefined && this[kSocket].emit('sessionError', error))
    return;
  this.destroy();
  this.removeListener('error', sessionOnError);
  this.emit('error', error);
}

// When a Socket emits an error, forward it to the session as a
// socketError; failing that, remove the listener and call destroy
function socketOnError(error) {
  const session = this[kSession];
  const type = session && session[kType];
  debug(`[${sessionName(type)}] server socket error: ${error.message}`);
  if (kRenegTest.test(error.message))
    return this.destroy();
  if (session !== undefined &&
      session.emit('socketError', error, this))
    return;
  this.removeListener('error', socketOnError);
  this.destroy(error);
}

// Handles the on('stream') event for a session and forwards
// it on to the server object.
function sessionOnStream(stream, headers, flags, rawHeaders) {
  debug(`[${sessionName(this[kType])}] emit server stream event`);
  this[kServer].emit('stream', stream, headers, flags, rawHeaders);
}

function sessionOnPriority(stream, parent, weight, exclusive) {
  debug(`[${sessionName(this[kType])}] priority change received`);
  this[kServer].emit('priority', stream, parent, weight, exclusive);
}

function sessionOnSocketError(error, socket) {
  if (this.listenerCount('socketError') <= 1 && this[kServer] !== undefined)
    this[kServer].emit('socketError', error, socket, this);
}

// When the session times out on the server, attempt a graceful shutdown
function sessionOnTimeout() {
  debug('session timeout');
  process.nextTick(() => {
    const state = this[kState];
    // if destroyed or destryoing, do nothing
    if (state.destroyed || state.destroying)
      return;
    const server = this[kServer];
    const socket = this[kSocket];
    // If server or socket are undefined, then we're already in the process of
    // shutting down, do nothing.
    if (server === undefined || socket === undefined)
      return;
    if (!server.emit('timeout', this)) {
      this.shutdown(
        {
          graceful: true,
          errorCode: NGHTTP2_NO_ERROR
        },
        socket.destroy.bind(socket));
    }
  });
}

function connectionListener(socket) {
  debug('[server] received a connection');
  const options = this[kOptions] || {};

  if (socket.alpnProtocol === false || socket.alpnProtocol === 'http/1.1') {
    // Fallback to HTTP/1.1
    if (options.allowHTTP1 === true)
      return httpConnectionListener.call(this, socket);
    // Let event handler deal with the socket
    if (this.emit('unknownProtocol', socket))
      return;
    // Reject non-HTTP/2 client
    return socket.destroy();
  }

  socket.on('error', socketOnError);
  socket.on('close', socketOnClose);

  // Set up the Session
  const session = new ServerHttp2Session(options, socket, this);

  session.on('error', sessionOnError);
  session.on('stream', sessionOnStream);
  session.on('priority', sessionOnPriority);
  session.on('socketError', sessionOnSocketError);

  if (this.timeout) {
    session.setTimeout(this.timeout);
    session.on('timeout', sessionOnTimeout);
  }

  socket[kServer] = this;

  process.nextTick(emit, this, 'session', session);
}

function initializeOptions(options) {
  assertIsObject(options, 'options');
  options = Object.assign(Object.create(null), options);
  options.allowHalfOpen = true;
  assertIsObject(options.settings, 'options.settings');
  options.settings = Object.assign(Object.create(null), options.settings);
  return options;
}

function initializeTLSOptions(options, servername) {
  options = initializeOptions(options);
  options.ALPNProtocols = ['h2'];
  if (options.allowHTTP1 === true)
    options.ALPNProtocols.push('http/1.1');
  if (servername !== undefined && options.servername === undefined)
    options.servername = servername;
  return options;
}

function onErrorSecureServerSession(err, conn) {
  if (!this.emit('clientError', err, conn))
    conn.destroy(err);
}

class Http2SecureServer extends TLSServer {
  constructor(options, requestListener) {
    options = initializeTLSOptions(options);
    super(options, connectionListener);
    this[kOptions] = options;
    this.timeout = kDefaultSocketTimeout;
    this.on('newListener', setupCompat);
    if (typeof requestListener === 'function')
      this.on('request', requestListener);
    this.on('tlsClientError', onErrorSecureServerSession);
    debug('http2secureserver created');
  }

  setTimeout(msecs, callback) {
    this.timeout = msecs;
    if (callback !== undefined) {
      if (typeof callback !== 'function')
        throw new errors.TypeError('ERR_INVALID_CALLBACK');
      this.on('timeout', callback);
    }
    return this;
  }
}

class Http2Server extends NETServer {
  constructor(options, requestListener) {
    super(connectionListener);
    this[kOptions] = initializeOptions(options);
    this.timeout = kDefaultSocketTimeout;
    this.on('newListener', setupCompat);
    if (typeof requestListener === 'function')
      this.on('request', requestListener);
    debug('http2server created');
  }

  setTimeout(msecs, callback) {
    this.timeout = msecs;
    if (callback !== undefined) {
      if (typeof callback !== 'function')
        throw new errors.TypeError('ERR_INVALID_CALLBACK');
      this.on('timeout', callback);
    }
    return this;
  }
}

function setupCompat(ev) {
  if (ev === 'request') {
    debug('setting up compatibility handler');
    this.removeListener('newListener', setupCompat);
    this.on('stream', onServerStream);
  }
}

function socketOnClose(hadError) {
  const session = this[kSession];
  if (session !== undefined && !session.destroyed) {
    // Skip unconsume because the socket is destroyed.
    session[kState].skipUnconsume = true;
    session.destroy();
  }
}

// If the session emits an error, forward it to the socket as a sessionError;
// failing that, destroy the session, remove the listener and re-emit the error
function clientSessionOnError(error) {
  debug(`client session error: ${error.message}`);
  if (this[kSocket] !== undefined && this[kSocket].emit('sessionError', error))
    return;
  this.destroy();
  this.removeListener('error', socketOnError);
  this.removeListener('error', clientSessionOnError);
}

function connect(authority, options, listener) {
  if (typeof options === 'function') {
    listener = options;
    options = undefined;
  }

  assertIsObject(options, 'options');
  options = Object.assign(Object.create(null), options);

  if (typeof authority === 'string')
    authority = new URL(authority);

  assertIsObject(authority, 'authority', ['string', 'object', 'URL']);

  debug(`connecting to ${authority}`);

  const protocol = authority.protocol || options.protocol || 'https:';
  const port = '' + (authority.port !== '' ?
    authority.port : (authority.protocol === 'http:' ? 80 : 443));
  const host = authority.hostname || authority.host || 'localhost';

  let socket;
  if (typeof options.createConnection === 'function') {
    socket = options.createConnection(authority, options);
  } else {
    switch (protocol) {
      case 'http:':
        socket = net.connect(port, host);
        break;
      case 'https:':
        socket = tls.connect(port, host, initializeTLSOptions(options, host));
        break;
      default:
        throw new errors.Error('ERR_HTTP2_UNSUPPORTED_PROTOCOL', protocol);
    }
  }

  socket.on('error', socketOnError);
  socket.on('close', socketOnClose);

  const session = new ClientHttp2Session(options, socket);

  session.on('error', clientSessionOnError);

  session[kAuthority] = `${options.servername || host}:${port}`;
  session[kProtocol] = protocol;

  if (typeof listener === 'function')
    session.once('connect', listener);
  return session;
}

// Support util.promisify
Object.defineProperty(connect, promisify.custom, {
  value: (authority, options) => {
    const promise = createPromise();
    const server = connect(authority,
                           options,
                           () => promiseResolve(promise, server));
    return promise;
  }
});

function createSecureServer(options, handler) {
  if (options == null || typeof options !== 'object') {
    throw new errors.TypeError('ERR_INVALID_ARG_TYPE',
                               'options',
                               'object');
  }
  debug('creating http2secureserver');
  return new Http2SecureServer(options, handler);
}

function createServer(options, handler) {
  if (typeof options === 'function') {
    handler = options;
    options = Object.create(null);
  }
  debug('creating htt2pserver');
  return new Http2Server(options, handler);
}

// Returns a Base64 encoded settings frame payload from the given
// object. The value is suitable for passing as the value of the
// HTTP2-Settings header frame.
function getPackedSettings(settings) {
  assertIsObject(settings, 'settings');
  settings = settings || Object.create(null);
  assertWithinRange('headerTableSize',
                    settings.headerTableSize,
                    0, 2 ** 32 - 1);
  assertWithinRange('initialWindowSize',
                    settings.initialWindowSize,
                    0, 2 ** 32 - 1);
  assertWithinRange('maxFrameSize',
                    settings.maxFrameSize,
                    16384, 2 ** 24 - 1);
  assertWithinRange('maxConcurrentStreams',
                    settings.maxConcurrentStreams,
                    0, 2 ** 31 - 1);
  assertWithinRange('maxHeaderListSize',
                    settings.maxHeaderListSize,
                    0, 2 ** 32 - 1);
  if (settings.enablePush !== undefined &&
      typeof settings.enablePush !== 'boolean') {
    const err = new errors.TypeError('ERR_HTTP2_INVALID_SETTING_VALUE',
                                     'enablePush', settings.enablePush);
    err.actual = settings.enablePush;
    throw err;
  }
  updateSettingsBuffer(settings);
  return binding.packSettings();
}

function getUnpackedSettings(buf, options = {}) {
  if (!isUint8Array(buf)) {
    throw new errors.TypeError('ERR_INVALID_ARG_TYPE', 'buf',
                               ['Buffer', 'Uint8Array']);
  }
  if (buf.length % 6 !== 0)
    throw new errors.RangeError('ERR_HTTP2_INVALID_PACKED_SETTINGS_LENGTH');
  const settings = Object.create(null);
  let offset = 0;
  while (offset < buf.length) {
    const id = buf.readUInt16BE(offset);
    offset += 2;
    const value = buf.readUInt32BE(offset);
    switch (id) {
      case NGHTTP2_SETTINGS_HEADER_TABLE_SIZE:
        settings.headerTableSize = value;
        break;
      case NGHTTP2_SETTINGS_ENABLE_PUSH:
        settings.enablePush = value;
        break;
      case NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS:
        settings.maxConcurrentStreams = value;
        break;
      case NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE:
        settings.initialWindowSize = value;
        break;
      case NGHTTP2_SETTINGS_MAX_FRAME_SIZE:
        settings.maxFrameSize = value;
        break;
      case NGHTTP2_SETTINGS_MAX_HEADER_LIST_SIZE:
        settings.maxHeaderListSize = value;
        break;
    }
    offset += 4;
  }

  if (options != null && options.validate) {
    assertWithinRange('headerTableSize',
                      settings.headerTableSize,
                      0, 2 ** 32 - 1);
    assertWithinRange('enablePush',
                      settings.enablePush,
                      0, 1);
    assertWithinRange('initialWindowSize',
                      settings.initialWindowSize,
                      0, 2 ** 32 - 1);
    assertWithinRange('maxFrameSize',
                      settings.maxFrameSize,
                      16384, 2 ** 24 - 1);
    assertWithinRange('maxConcurrentStreams',
                      settings.maxConcurrentStreams,
                      0, 2 ** 31 - 1);
    assertWithinRange('maxHeaderListSize',
                      settings.maxHeaderListSize,
                      0, 2 ** 32 - 1);
  }

  if (settings.enablePush !== undefined) {
    settings.enablePush = !!settings.enablePush;
  }

  return settings;
}

// Exports
module.exports = {
  constants,
  getDefaultSettings,
  getPackedSettings,
  getUnpackedSettings,
  createServer,
  createSecureServer,
  connect,
  Http2Session,
  Http2Stream,
  Http2ServerRequest,
  Http2ServerResponse
};

/* eslint-enable no-use-before-define */
