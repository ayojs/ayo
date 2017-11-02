/* eslint documented-errors: "error" */
/* eslint alphabetize-errors: "error" */

'use strict';

// The whole point behind this internal module is to allow Node.js to no
// longer be forced to treat every error message change as a semver-major
// change. The NodeError classes here all expose a `code` property whose
// value statically and permanently identifies the error. While the error
// message may change, the code should not.

const kCode = Symbol('code');
const messages = new Map();

const { kMaxLength } = process.binding('buffer');
const { defineProperty } = Object;

// Lazily loaded
var util = null;

function makeNodeError(Base) {
  return class NodeError extends Base {
    constructor(key, ...args) {
      super(message(key, args));
      defineProperty(this, kCode, {
        configurable: true,
        enumerable: false,
        value: key,
        writable: true
      });
    }

    get name() {
      return `${super.name} [${this[kCode]}]`;
    }

    set name(value) {
      defineProperty(this, 'name', {
        configurable: true,
        enumerable: true,
        value,
        writable: true
      });
    }

    get code() {
      return this[kCode];
    }

    set code(value) {
      defineProperty(this, 'code', {
        configurable: true,
        enumerable: true,
        value,
        writable: true
      });
    }
  };
}

class AssertionError extends Error {
  constructor(options) {
    if (typeof options !== 'object' || options === null) {
      throw new exports.TypeError('ERR_INVALID_ARG_TYPE', 'options', 'object');
    }
    var { actual, expected, message, operator, stackStartFunction } = options;
    if (message) {
      super(message);
    } else {
      if (actual && actual.stack && actual instanceof Error)
        actual = `${actual.name}: ${actual.message}`;
      if (expected && expected.stack && expected instanceof Error)
        expected = `${expected.name}: ${expected.message}`;
      if (util === null) util = require('util');
      super(`${util.inspect(actual).slice(0, 128)} ` +
        `${operator} ${util.inspect(expected).slice(0, 128)}`);
    }

    this.generatedMessage = !message;
    this.name = 'AssertionError [ERR_ASSERTION]';
    this.code = 'ERR_ASSERTION';
    this.actual = actual;
    this.expected = expected;
    this.operator = operator;
    Error.captureStackTrace(this, stackStartFunction);
  }
}

// This is defined here instead of using the assert module to avoid a
// circular dependency. The effect is largely the same.
function internalAssert(condition, message) {
  if (!condition) {
    throw new AssertionError({
      message,
      actual: false,
      expected: true,
      operator: '=='
    });
  }
}

function message(key, args) {
  const msg = messages.get(key);
  internalAssert(msg, `An invalid error message key was used: ${key}.`);
  let fmt;
  if (typeof msg === 'function') {
    fmt = msg;
  } else {
    if (util === null) util = require('util');
    fmt = util.format;
    if (args === undefined || args.length === 0)
      return msg;
    args.unshift(msg);
  }
  return String(fmt.apply(null, args));
}

// Utility function for registering the error codes. Only used here. Exported
// *only* to allow for testing.
function E(sym, val) {
  messages.set(sym, typeof val === 'function' ? val : String(val));
}

module.exports = exports = {
  message,
  Error: makeNodeError(Error),
  TypeError: makeNodeError(TypeError),
  RangeError: makeNodeError(RangeError),
  URIError: makeNodeError(URIError),
  AssertionError,
  E // This is exported only to facilitate testing.
};

// To declare an error message, use the E(sym, val) function above. The sym
// must be an upper case string. The val can be either a function or a string.
// The return value of the function must be a string.
// Examples:
// E('EXAMPLE_KEY1', 'This is the error value');
// E('EXAMPLE_KEY2', (a, b) => return `${a} ${b}`);
//
// Once an error code has been assigned, the code itself MUST NOT change and
// any given error code must never be reused to identify a different error.
//
// Any error code added here should also be added to the documentation
//
// Note: Please try to keep these in alphabetical order
E('ERR_ARG_NOT_ITERABLE', '%s must be iterable');
E('ERR_ASSERTION', '%s');
E('ERR_ASYNC_CALLBACK', (name) => `${name} must be a function`);
E('ERR_ASYNC_TYPE', (s) => `Invalid name for async "type": ${s}`);
E('ERR_BUFFER_OUT_OF_BOUNDS', bufferOutOfBounds);
E('ERR_BUFFER_TOO_LARGE',
  `Cannot create a Buffer larger than 0x${kMaxLength.toString(16)} bytes`);
E('ERR_CHILD_CLOSED_BEFORE_REPLY', 'Child closed before reply received');
E('ERR_CONSOLE_WRITABLE_STREAM',
  'Console expects a writable stream instance for %s');
E('ERR_CPU_USAGE', 'Unable to obtain cpu usage %s');
E('ERR_CRYPTO_ECDH_INVALID_FORMAT', 'Invalid ECDH format: %s');
E('ERR_CRYPTO_ENGINE_UNKNOWN', 'Engine "%s" was not found');
E('ERR_CRYPTO_FIPS_FORCED',
  'Cannot set FIPS mode, it was forced with --force-fips at startup.');
E('ERR_CRYPTO_FIPS_UNAVAILABLE', 'Cannot set FIPS mode in a non-FIPS build.');
E('ERR_CRYPTO_HASH_DIGEST_NO_UTF16', 'hash.digest() does not support UTF-16');
E('ERR_CRYPTO_HASH_FINALIZED', 'Digest already called');
E('ERR_CRYPTO_HASH_UPDATE_FAILED', 'Hash update failed');
E('ERR_CRYPTO_INVALID_DIGEST', 'Invalid digest: %s');
E('ERR_CRYPTO_INVALID_STATE', 'Invalid state for operation %s');
E('ERR_CRYPTO_SIGN_KEY_REQUIRED', 'No key provided to sign');
E('ERR_CRYPTO_TIMING_SAFE_EQUAL_LENGTH',
  'Input buffers must have the same length');
E('ERR_DNS_SET_SERVERS_FAILED', (err, servers) =>
  `c-ares failed to set servers: "${err}" [${servers}]`);
E('ERR_ENCODING_INVALID_ENCODED_DATA',
  (enc) => `The encoded data was not valid for encoding ${enc}`);
E('ERR_ENCODING_NOT_SUPPORTED',
  (enc) => `The "${enc}" encoding is not supported`);
E('ERR_FALSY_VALUE_REJECTION', 'Promise was rejected with falsy value');
E('ERR_HTTP2_CONNECT_AUTHORITY',
  ':authority header is required for CONNECT requests');
E('ERR_HTTP2_CONNECT_PATH',
  'The :path header is forbidden for CONNECT requests');
E('ERR_HTTP2_CONNECT_SCHEME',
  'The :scheme header is forbidden for CONNECT requests');
E('ERR_HTTP2_FRAME_ERROR',
  (type, code, id) => {
    let msg = `Error sending frame type ${type}`;
    if (id !== undefined)
      msg += ` for stream ${id}`;
    msg += ` with code ${code}`;
    return msg;
  });
E('ERR_HTTP2_HEADERS_AFTER_RESPOND',
  'Cannot specify additional headers after response initiated');
E('ERR_HTTP2_HEADERS_OBJECT', 'Headers must be an object');
E('ERR_HTTP2_HEADERS_SENT', 'Response has already been initiated.');
E('ERR_HTTP2_HEADER_REQUIRED',
  (name) => `The ${name} header is required`);
E('ERR_HTTP2_HEADER_SINGLE_VALUE',
  (name) => `Header field "${name}" must have only a single value`);
E('ERR_HTTP2_INFO_HEADERS_AFTER_RESPOND',
  'Cannot send informational headers after the HTTP message has been sent');
E('ERR_HTTP2_INFO_STATUS_NOT_ALLOWED',
  'Informational status codes cannot be used');
E('ERR_HTTP2_INVALID_CONNECTION_HEADERS',
  'HTTP/1 Connection specific headers are forbidden: "%s"');
E('ERR_HTTP2_INVALID_HEADER_VALUE', 'Value must not be undefined or null');
E('ERR_HTTP2_INVALID_INFO_STATUS',
  (code) => `Invalid informational status code: ${code}`);
E('ERR_HTTP2_INVALID_PACKED_SETTINGS_LENGTH',
  'Packed settings length must be a multiple of six');
E('ERR_HTTP2_INVALID_PSEUDOHEADER',
  (name) => `"${name}" is an invalid pseudoheader or is used incorrectly`);
E('ERR_HTTP2_INVALID_SESSION', 'The session has been destroyed');
E('ERR_HTTP2_INVALID_SETTING_VALUE',
  (name, value) => `Invalid value for setting "${name}": ${value}`);
E('ERR_HTTP2_INVALID_STREAM', 'The stream has been destroyed');
E('ERR_HTTP2_MAX_PENDING_SETTINGS_ACK',
  (max) => `Maximum number of pending settings acknowledgements (${max})`);
E('ERR_HTTP2_NO_SOCKET_MANIPULATION',
  'HTTP/2 sockets should not be directly manipulated (e.g. read and written)');
E('ERR_HTTP2_OUT_OF_STREAMS',
  'No stream ID is available because maximum stream ID has been reached');
E('ERR_HTTP2_PAYLOAD_FORBIDDEN',
  (code) => `Responses with ${code} status must not have a payload`);
E('ERR_HTTP2_PSEUDOHEADER_NOT_ALLOWED', 'Cannot set HTTP/2 pseudo-headers');
E('ERR_HTTP2_PUSH_DISABLED', 'HTTP/2 client has disabled push streams');
E('ERR_HTTP2_SEND_FILE', 'Only regular files can be sent');
E('ERR_HTTP2_SOCKET_BOUND',
  'The socket is already bound to an Http2Session');
E('ERR_HTTP2_STATUS_101',
  'HTTP status code 101 (Switching Protocols) is forbidden in HTTP/2');
E('ERR_HTTP2_STATUS_INVALID',
  (code) => `Invalid status code: ${code}`);
E('ERR_HTTP2_STREAM_CLOSED', 'The stream is already closed');
E('ERR_HTTP2_STREAM_ERROR',
  (code) => `Stream closed with error code ${code}`);
E('ERR_HTTP2_STREAM_SELF_DEPENDENCY', 'A stream cannot depend on itself');
E('ERR_HTTP2_UNSUPPORTED_PROTOCOL',
  (protocol) => `protocol "${protocol}" is unsupported.`);
E('ERR_HTTP_HEADERS_SENT',
  'Cannot %s headers after they are sent to the client');
E('ERR_HTTP_INVALID_CHAR', 'Invalid character in statusMessage.');
E('ERR_HTTP_INVALID_STATUS_CODE',
  (originalStatusCode) => `Invalid status code: ${originalStatusCode}`);
E('ERR_HTTP_TRAILER_INVALID',
  'Trailers are invalid with this transfer encoding');
E('ERR_INDEX_OUT_OF_RANGE', 'Index out of range');
E('ERR_INSPECTOR_ALREADY_CONNECTED', 'The inspector is already connected');
E('ERR_INSPECTOR_CLOSED', 'Session was closed');
E('ERR_INSPECTOR_NOT_AVAILABLE', 'Inspector is not available');
E('ERR_INSPECTOR_NOT_CONNECTED', 'Session is not connected');
E('ERR_INVALID_ARG_TYPE', invalidArgType);
E('ERR_INVALID_ARG_VALUE',
  (name, value) => {
    return `The value "${String(value)}" is invalid for argument "${name}"`;
  });
E('ERR_INVALID_ARRAY_LENGTH',
  (name, len, actual) => {
    internalAssert(typeof actual === 'number', 'actual must be a number');
    return `The array "${name}" (length ${actual}) must be of length ${len}.`;
  });
E('ERR_INVALID_ASYNC_ID', (type, id) => `Invalid ${type} value: ${id}`);
E('ERR_INVALID_BUFFER_SIZE', 'Buffer size must be a multiple of %s');
E('ERR_INVALID_CALLBACK', 'Callback must be a function');
E('ERR_INVALID_CHAR', invalidChar);
E('ERR_INVALID_CURSOR_POS',
  'Cannot set cursor row without setting its column');
E('ERR_INVALID_DOMAIN_NAME', 'Unable to determine the domain name');
E('ERR_INVALID_FD', '"fd" must be a positive integer: %s');
E('ERR_INVALID_FD_TYPE', 'Unsupported fd type: %s');
E('ERR_INVALID_FILE_URL_HOST',
  'File URL host must be "localhost" or empty on %s');
E('ERR_INVALID_FILE_URL_PATH', 'File URL path %s');
E('ERR_INVALID_HANDLE_TYPE', 'This handle type cannot be sent');
E('ERR_INVALID_HTTP_TOKEN', '%s must be a valid HTTP token ["%s"]');
E('ERR_INVALID_IP_ADDRESS', 'Invalid IP address: %s');
E('ERR_INVALID_OPT_VALUE',
  (name, value) => {
    return `The value "${String(value)}" is invalid for option "${name}"`;
  });
E('ERR_INVALID_OPT_VALUE_ENCODING',
  (value) => `The value "${String(value)}" is invalid for option "encoding"`);
E('ERR_INVALID_PERFORMANCE_MARK', 'The "%s" performance mark has not been set');
E('ERR_INVALID_PROTOCOL', (protocol, expectedProtocol) =>
  `Protocol "${protocol}" not supported. Expected "${expectedProtocol}"`);
E('ERR_INVALID_REPL_EVAL_CONFIG',
  'Cannot specify both "breakEvalOnSigint" and "eval" for REPL');
E('ERR_INVALID_SYNC_FORK_INPUT',
  'Asynchronous forks do not support Buffer, Uint8Array or string input: %s');
E('ERR_INVALID_THIS', 'Value of "this" must be of type %s');
E('ERR_INVALID_TUPLE', '%s must be an iterable %s tuple');
E('ERR_INVALID_URI', 'URI malformed');
E('ERR_INVALID_URL', 'Invalid URL: %s');
E('ERR_INVALID_URL_SCHEME',
  (expected) => `The URL must be ${oneOf(expected, 'scheme')}`);
E('ERR_IPC_CHANNEL_CLOSED', 'Channel closed');
E('ERR_IPC_DISCONNECTED', 'IPC channel is already disconnected');
E('ERR_IPC_ONE_PIPE', 'Child process can have only one IPC pipe');
E('ERR_IPC_SYNC_FORK', 'IPC cannot be used with synchronous forks');
E('ERR_METHOD_NOT_IMPLEMENTED', 'The %s method is not implemented');
E('ERR_MISSING_ARGS', missingArgs);
E('ERR_MISSING_MODULE', 'Cannot find module %s');
E('ERR_MODULE_RESOLUTION_LEGACY', '%s not found by import in %s.' +
  ' Legacy behavior in require() would have found it at %s');
E('ERR_MULTIPLE_CALLBACK', 'Callback called multiple times');
E('ERR_NAPI_CONS_FUNCTION', 'Constructor must be a function');
E('ERR_NAPI_CONS_PROTOTYPE_OBJECT', 'Constructor.prototype must be an object');
E('ERR_NO_CRYPTO', 'Node.js is not compiled with OpenSSL crypto support');
E('ERR_NO_ICU', '%s is not supported on Node.js compiled without ICU');
E('ERR_NO_LONGER_SUPPORTED', '%s is no longer supported');
E('ERR_OUTOFMEMORY', 'Out of memory');
E('ERR_OUT_OF_RANGE', 'The "%s" argument is out of range');
E('ERR_PARSE_HISTORY_DATA', 'Could not parse history data in %s');
E('ERR_REQUIRE_ESM', 'Must use import to load ES Module: %s');
E('ERR_SERVER_ALREADY_LISTEN',
  'Listen method has been called more than once without closing.');
E('ERR_SOCKET_ALREADY_BOUND', 'Socket is already bound');
E('ERR_SOCKET_BAD_BUFFER_SIZE', 'Buffer size must be a positive integer');
E('ERR_SOCKET_BAD_PORT', 'Port should be > 0 and < 65536. Received %s.');
E('ERR_SOCKET_BAD_TYPE',
  'Bad socket type specified. Valid types are: udp4, udp6');
E('ERR_SOCKET_BUFFER_SIZE',
  (reason) => `Could not get or set buffer size: ${reason}`);
E('ERR_SOCKET_CANNOT_SEND', 'Unable to send data');
E('ERR_SOCKET_CLOSED', 'Socket is closed');
E('ERR_SOCKET_DGRAM_NOT_RUNNING', 'Not running');
E('ERR_STDERR_CLOSE', 'process.stderr cannot be closed');
E('ERR_STDOUT_CLOSE', 'process.stdout cannot be closed');
E('ERR_STREAM_CANNOT_PIPE', 'Cannot pipe, not readable');
E('ERR_STREAM_NULL_VALUES', 'May not write null values to stream');
E('ERR_STREAM_PUSH_AFTER_EOF', 'stream.push() after EOF');
E('ERR_STREAM_READ_NOT_IMPLEMENTED', '_read() is not implemented');
E('ERR_STREAM_UNSHIFT_AFTER_END_EVENT', 'stream.unshift() after end event');
E('ERR_STREAM_WRAP', 'Stream has StringDecoder set or is in objectMode');
E('ERR_STREAM_WRITE_AFTER_END', 'write after end');
E('ERR_TLS_CERT_ALTNAME_INVALID',
  'Hostname/IP does not match certificate\'s altnames: %s');
E('ERR_TLS_DH_PARAM_SIZE', (size) =>
  `DH parameter size ${size} is less than 2048`);
E('ERR_TLS_HANDSHAKE_TIMEOUT', 'TLS handshake timeout');
E('ERR_TLS_RENEGOTIATION_FAILED', 'Failed to renegotiate');
E('ERR_TLS_REQUIRED_SERVER_NAME',
  '"servername" is required parameter for Server.addContext');
E('ERR_TLS_SESSION_ATTACK', 'TSL session renegotiation attack detected');
E('ERR_TRANSFORM_ALREADY_TRANSFORMING',
  'Calling transform done when still transforming');
E('ERR_TRANSFORM_WITH_LENGTH_0',
  'Calling transform done when writableState.length != 0');
E('ERR_UNESCAPED_CHARACTERS',
  (name) => `${name} contains unescaped characters`);
E('ERR_UNHANDLED_ERROR',
  (err) => {
    const msg = 'Unhandled error.';
    if (err === undefined) return msg;
    return `${msg} (${err})`;
  });
E('ERR_UNKNOWN_ENCODING', 'Unknown encoding: %s');
E('ERR_UNKNOWN_FILE_EXTENSION', 'Unknown file extension: %s');
E('ERR_UNKNOWN_MODULE_FORMAT', 'Unknown module format: %s');
E('ERR_UNKNOWN_SIGNAL', 'Unknown signal: %s');
E('ERR_UNKNOWN_STDIN_TYPE', 'Unknown stdin file type');
E('ERR_UNKNOWN_STREAM_TYPE', 'Unknown stream file type');
E('ERR_V8BREAKITERATOR', 'Full ICU data not installed. ' +
  'See https://github.com/nodejs/node/wiki/Intl');
E('ERR_VALID_PERFORMANCE_ENTRY_TYPE',
  'At least one valid performance entry type is required');
E('ERR_VALUE_OUT_OF_RANGE', (start, end, value) => {
  return `The value of "${start}" must be ${end}. Received "${value}"`;
});
E('ERR_WORKER_DOMAIN', 'Domains are not available inside worker threads');
E('ERR_WORKER_NEED_ABSOLUTE_PATH',
  'The worker script filename must be an absolute path. Received "%s"');
E('ERR_WORKER_OUT_OF_MEMORY', 'The worker script ran out of memory');
E('ERR_WORKER_UNSERIALIZABLE_ERROR',
  'Serializing an uncaught exception failed');
E('ERR_WORKER_UNSUPPORTED_EXTENSION',
  'The worker script extension must be ".js" or ".mjs". Received "%s"');
E('ERR_ZLIB_BINDING_CLOSED', 'zlib binding closed');
E('ERR_ZLIB_INITIALIZATION_FAILED', 'Initialization failed');

function invalidArgType(name, expected, actual) {
  internalAssert(name, 'name is required');

  // determiner: 'must be' or 'must not be'
  let determiner;
  if (typeof expected === 'string' && expected.startsWith('not ')) {
    determiner = 'must not be';
    expected = expected.replace(/^not /, '');
  } else {
    determiner = 'must be';
  }

  let msg;
  if (Array.isArray(name)) {
    var names = name.map((val) => `"${val}"`).join(', ');
    msg = `The ${names} arguments ${determiner} ${oneOf(expected, 'type')}`;
  } else if (name.endsWith(' argument')) {
    // for the case like 'first argument'
    msg = `The ${name} ${determiner} ${oneOf(expected, 'type')}`;
  } else {
    const type = name.includes('.') ? 'property' : 'argument';
    msg = `The "${name}" ${type} ${determiner} ${oneOf(expected, 'type')}`;
  }

  // if actual value received, output it
  if (arguments.length >= 3) {
    msg += `. Received type ${actual !== null ? typeof actual : 'null'}`;
  }
  return msg;
}

function missingArgs(...args) {
  internalAssert(args.length > 0, 'At least one arg needs to be specified');
  let msg = 'The ';
  const len = args.length;
  args = args.map((a) => `"${a}"`);
  switch (len) {
    case 1:
      msg += `${args[0]} argument`;
      break;
    case 2:
      msg += `${args[0]} and ${args[1]} arguments`;
      break;
    default:
      msg += args.slice(0, len - 1).join(', ');
      msg += `, and ${args[len - 1]} arguments`;
      break;
  }
  return `${msg} must be specified`;
}

function oneOf(expected, thing) {
  internalAssert(expected, 'expected is required');
  internalAssert(typeof thing === 'string', 'thing is required');
  if (Array.isArray(expected)) {
    const len = expected.length;
    internalAssert(len > 0,
                   'At least one expected value needs to be specified');
    expected = expected.map((i) => String(i));
    if (len > 2) {
      return `one of ${thing} ${expected.slice(0, len - 1).join(', ')}, or ` +
             expected[len - 1];
    } else if (len === 2) {
      return `one of ${thing} ${expected[0]} or ${expected[1]}`;
    } else {
      return `of ${thing} ${expected[0]}`;
    }
  } else {
    return `of ${thing} ${String(expected)}`;
  }
}

function bufferOutOfBounds(name, isWriting) {
  if (isWriting) {
    return 'Attempt to write outside buffer bounds';
  } else {
    return `"${name}" is outside of buffer bounds`;
  }
}

function invalidChar(name, field) {
  let msg = `Invalid character in ${name}`;
  if (field) {
    msg += ` ["${field}"]`;
  }
  return msg;
}
