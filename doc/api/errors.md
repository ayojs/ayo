# Errors

<!--introduced_in=v4.0.0-->
<!--type=misc-->

Applications running in Node.js will generally experience four categories of
errors:

- Standard JavaScript errors such as:
  - {EvalError} : thrown when a call to `eval()` fails.
  - {SyntaxError} : thrown in response to improper JavaScript language
    syntax.
  - {RangeError} : thrown when a value is not within an expected range
  - {ReferenceError} : thrown when using undefined variables
  - {TypeError} : thrown when passing arguments of the wrong type
  - {URIError} : thrown when a global URI handling function is misused.
- System errors triggered by underlying operating system constraints such
  as attempting to open a file that does not exist, attempting to send data
  over a closed socket, etc;
- And User-specified errors triggered by application code.
- Assertion Errors are a special class of error that can be triggered whenever
  Node.js detects an exceptional logic violation that should never occur. These
  are raised typically by the `assert` module.

All JavaScript and System errors raised by Node.js inherit from, or are
instances of, the standard JavaScript {Error} class and are guaranteed
to provide *at least* the properties available on that class.

## Error Propagation and Interception

<!--type=misc-->

Node.js supports several mechanisms for propagating and handling errors that
occur while an application is running. How these errors are reported and
handled depends entirely on the type of Error and the style of the API that is
called.

All JavaScript errors are handled as exceptions that *immediately* generate
and throw an error using the standard JavaScript `throw` mechanism. These
are handled using the [`try / catch` construct][try-catch] provided by the
JavaScript language.

```js
// Throws with a ReferenceError because z is undefined
try {
  const m = 1;
  const n = m + z;
} catch (err) {
  // Handle the error here.
}
```

Any use of the JavaScript `throw` mechanism will raise an exception that
*must* be handled using `try / catch` or the Node.js process will exit
immediately.

With few exceptions, _Synchronous_ APIs (any blocking method that does not
accept a `callback` function, such as [`fs.readFileSync`][]), will use `throw`
to report errors.

Errors that occur within _Asynchronous APIs_ may be reported in multiple ways:

- Most asynchronous methods that accept a `callback` function will accept an
  `Error` object passed as the first argument to that function. If that first
  argument is not `null` and is an instance of `Error`, then an error occurred
  that should be handled.

<!-- eslint-disable no-useless-return -->
  ```js
  const fs = require('fs');
  fs.readFile('a file that does not exist', (err, data) => {
    if (err) {
      console.error('There was an error reading the file!', err);
      return;
    }
    // Otherwise handle the data
  });
  ```
- When an asynchronous method is called on an object that is an `EventEmitter`,
  errors can be routed to that object's `'error'` event.

  ```js
  const net = require('net');
  const connection = net.connect('localhost');

  // Adding an 'error' event handler to a stream:
  connection.on('error', (err) => {
    // If the connection is reset by the server, or if it can't
    // connect at all, or on any sort of error encountered by
    // the connection, the error will be sent here.
    console.error(err);
  });

  connection.pipe(process.stdout);
  ```

- A handful of typically asynchronous methods in the Node.js API may still
  use the `throw` mechanism to raise exceptions that must be handled using
  `try / catch`. There is no comprehensive list of such methods; please
  refer to the documentation of each method to determine the appropriate
  error handling mechanism required.

The use of the `'error'` event mechanism is most common for [stream-based][]
and [event emitter-based][] APIs, which themselves represent a series of
asynchronous operations over time (as opposed to a single operation that may
pass or fail).

For *all* `EventEmitter` objects, if an `'error'` event handler is not
provided, the error will be thrown, causing the Node.js process to report an
unhandled exception and  crash unless either: The [`domain`][domains] module is
used appropriately or a handler has been registered for the
[`process.on('uncaughtException')`][] event.

```js
const EventEmitter = require('events');
const ee = new EventEmitter();

setImmediate(() => {
  // This will crash the process because no 'error' event
  // handler has been added.
  ee.emit('error', new Error('This will crash'));
});
```

Errors generated in this way *cannot* be intercepted using `try / catch` as
they are thrown *after* the calling code has already exited.

Developers must refer to the documentation for each method to determine
exactly how errors raised by those methods are propagated.

### Node.js style callbacks

<!--type=misc-->

Most asynchronous methods exposed by the Node.js core API follow an idiomatic
pattern  referred to as a "Node.js style callback". With this pattern, a
callback function is passed to the method as an argument. When the operation
either completes or an error is raised, the callback function is called with
the Error object (if any) passed as the first argument. If no error was raised,
the first argument will be passed as `null`.

```js
const fs = require('fs');

function nodeStyleCallback(err, data) {
  if (err) {
    console.error('There was an error', err);
    return;
  }
  console.log(data);
}

fs.readFile('/some/file/that/does-not-exist', nodeStyleCallback);
fs.readFile('/some/file/that/does-exist', nodeStyleCallback);
```

The JavaScript `try / catch` mechanism **cannot** be used to intercept errors
generated by asynchronous APIs.  A common mistake for beginners is to try to
use `throw` inside a Node.js style callback:

```js
// THIS WILL NOT WORK:
const fs = require('fs');

try {
  fs.readFile('/some/file/that/does-not-exist', (err, data) => {
    // mistaken assumption: throwing here...
    if (err) {
      throw err;
    }
  });
} catch (err) {
  // This will not catch the throw!
  console.error(err);
}
```

This will not work because the callback function passed to `fs.readFile()` is
called asynchronously. By the time the callback has been called, the
surrounding code (including the `try { } catch (err) { }` block will have
already exited. Throwing an error inside the callback **can crash the Node.js
process** in most cases. If [domains][] are enabled, or a handler has been
registered with `process.on('uncaughtException')`, such errors can be
intercepted.

## Class: Error

<!--type=class-->

A generic JavaScript `Error` object that does not denote any specific
circumstance of why the error occurred. `Error` objects capture a "stack trace"
detailing the point in the code at which the `Error` was instantiated, and may
provide a text description of the error.

For crypto only, `Error` objects will include the OpenSSL error stack in a
separate property called `opensslErrorStack` if it is available when the error
is thrown.

All errors generated by Node.js, including all System and JavaScript errors,
will either be instances of, or inherit from, the `Error` class.

### new Error(message)

* `message` {string}

Creates a new `Error` object and sets the `error.message` property to the
provided text message. If an object is passed as `message`, the text message
is generated by calling `message.toString()`. The `error.stack` property will
represent the point in the code at which `new Error()` was called. Stack traces
are dependent on [V8's stack trace API][]. Stack traces extend only to either
(a) the beginning of  *synchronous code execution*, or (b) the number of frames
given by the property `Error.stackTraceLimit`, whichever is smaller.

### Error.captureStackTrace(targetObject[, constructorOpt])

* `targetObject` {Object}
* `constructorOpt` {Function}

Creates a `.stack` property on `targetObject`, which when accessed returns
a string representing the location in the code at which
`Error.captureStackTrace()` was called.

```js
const myObject = {};
Error.captureStackTrace(myObject);
myObject.stack;  // similar to `new Error().stack`
```

The first line of the trace will be prefixed with `${myObject.name}: ${myObject.message}`.

The optional `constructorOpt` argument accepts a function. If given, all frames
above `constructorOpt`, including `constructorOpt`, will be omitted from the
generated stack trace.

The `constructorOpt` argument is useful for hiding implementation
details of error generation from an end user. For instance:

```js
function MyError() {
  Error.captureStackTrace(this, MyError);
}

// Without passing MyError to captureStackTrace, the MyError
// frame would show up in the .stack property. By passing
// the constructor, we omit that frame, and retain all frames below it.
new MyError().stack;
```

### Error.stackTraceLimit

* {number}

The `Error.stackTraceLimit` property specifies the number of stack frames
collected by a stack trace (whether generated by `new Error().stack` or
`Error.captureStackTrace(obj)`).

The default value is `10` but may be set to any valid JavaScript number. Changes
will affect any stack trace captured *after* the value has been changed.

If set to a non-number value, or set to a negative number, stack traces will
not capture any frames.

### error.code

* {string}

The `error.code` property is a string label that identifies the kind of error.
See [Node.js Error Codes][] for details about specific codes.

### error.message

* {string}

The `error.message` property is the string description of the error as set by
calling `new Error(message)`. The `message` passed to the constructor will also
appear in the first line of the stack trace of the `Error`, however changing
this property after the `Error` object is created *may not* change the first
line of the stack trace (for example, when `error.stack` is read before this
property is changed).

```js
const err = new Error('The message');
console.error(err.message);
// Prints: The message
```

### error.stack

* {string}

The `error.stack` property is a string describing the point in the code at which
the `Error` was instantiated.

For example:

```txt
Error: Things keep happening!
   at /home/gbusey/file.js:525:2
   at Frobnicator.refrobulate (/home/gbusey/business-logic.js:424:21)
   at Actor.<anonymous> (/home/gbusey/actors.js:400:8)
   at increaseSynergy (/home/gbusey/actors.js:701:6)
```

The first line is formatted as `<error class name>: <error message>`, and
is followed by a series of stack frames (each line beginning with "at ").
Each frame describes a call site within the code that lead to the error being
generated. V8 attempts to display a name for each function (by variable name,
function name, or object method name), but occasionally it will not be able to
find a suitable name. If V8 cannot determine a name for the function, only
location information will be displayed for that frame. Otherwise, the
determined function name will be displayed with location information appended
in parentheses.

It is important to note that frames are **only** generated for JavaScript
functions. If, for example, execution synchronously passes through a C++ addon
function called `cheetahify`, which itself calls a JavaScript function, the
frame representing the `cheetahify` call will **not** be present in the stack
traces:

```js
const cheetahify = require('./native-binding.node');

function makeFaster() {
  // cheetahify *synchronously* calls speedy.
  cheetahify(function speedy() {
    throw new Error('oh no!');
  });
}

makeFaster();
// will throw:
//   /home/gbusey/file.js:6
//       throw new Error('oh no!');
//           ^
//   Error: oh no!
//       at speedy (/home/gbusey/file.js:6:11)
//       at makeFaster (/home/gbusey/file.js:5:3)
//       at Object.<anonymous> (/home/gbusey/file.js:10:1)
//       at Module._compile (module.js:456:26)
//       at Object.Module._extensions..js (module.js:474:10)
//       at Module.load (module.js:356:32)
//       at Function.Module._load (module.js:312:12)
//       at Function.Module.runMain (module.js:497:10)
//       at startup (node.js:119:16)
//       at node.js:906:3
```

The location information will be one of:

* `native`, if the frame represents a call internal to V8 (as in `[].forEach`).
* `plain-filename.js:line:column`, if the frame represents a call internal
   to Node.js.
* `/absolute/path/to/file.js:line:column`, if the frame represents a call in
  a user program, or its dependencies.

The string representing the stack trace is lazily generated when the
`error.stack` property is **accessed**.

The number of frames captured by the stack trace is bounded by the smaller of
`Error.stackTraceLimit` or the number of available frames on the current event
loop tick.

System-level errors are generated as augmented `Error` instances, which are
detailed [here](#errors_system_errors).

## Class: AssertionError

A subclass of `Error` that indicates the failure of an assertion. Such errors
commonly indicate inequality of actual and expected value.

For example:

```js
assert.strictEqual(1, 2);
// AssertionError [ERR_ASSERTION]: 1 === 2
```

## Class: RangeError

A subclass of `Error` that indicates that a provided argument was not within the
set or range of acceptable values for a function; whether that is a numeric
range, or outside the set of options for a given function parameter.

For example:

```js
require('net').connect(-1);
// throws "RangeError: "port" option should be >= 0 and < 65536: -1"
```

Node.js will generate and throw `RangeError` instances *immediately* as a form
of argument validation.

## Class: ReferenceError

A subclass of `Error` that indicates that an attempt is being made to access a
variable that is not defined. Such errors commonly indicate typos in code, or
an otherwise broken program.

While client code may generate and propagate these errors, in practice, only V8
will do so.

```js
doesNotExist;
// throws ReferenceError, doesNotExist is not a variable in this program.
```

Unless an application is dynamically generating and running code,
`ReferenceError` instances should always be considered a bug in the code
or its dependencies.

## Class: SyntaxError

A subclass of `Error` that indicates that a program is not valid JavaScript.
These errors may only be generated and propagated as a result of code
evaluation. Code evaluation may happen as a result of `eval`, `Function`,
`require`, or [vm][]. These errors are almost always indicative of a broken
program.

```js
try {
  require('vm').runInThisContext('binary ! isNotOk');
} catch (err) {
  // err will be a SyntaxError
}
```

`SyntaxError` instances are unrecoverable in the context that created them –
they may only be caught by other contexts.

## Class: TypeError

A subclass of `Error` that indicates that a provided argument is not an
allowable type. For example, passing a function to a parameter which expects a
string would be considered a TypeError.

```js
require('url').parse(() => { });
// throws TypeError, since it expected a string
```

Node.js will generate and throw `TypeError` instances *immediately* as a form
of argument validation.

## Exceptions vs. Errors

<!--type=misc-->

A JavaScript exception is a value that is thrown as a result of an invalid
operation or as the target of a `throw` statement. While it is not required
that these values are instances of `Error` or classes which inherit from
`Error`, all exceptions thrown by Node.js or the JavaScript runtime *will* be
instances of Error.

Some exceptions are *unrecoverable* at the JavaScript layer. Such exceptions
will *always* cause the Node.js process to crash. Examples include `assert()`
checks or `abort()` calls in the C++ layer.

## System Errors

System errors are generated when exceptions occur within the program's
runtime environment. Typically, these are operational errors that occur
when an application violates an operating system constraint such as attempting
to read a file that does not exist or when the user does not have sufficient
permissions.

System errors are typically generated at the syscall level: an exhaustive list
of error codes and their meanings is available by running `man 2 intro` or
`man 3 errno` on most Unices; or [online][].

In Node.js, system errors are represented as augmented `Error` objects with
added properties.

### Class: System Error

#### error.code

* {string}

The `error.code` property is a string representing the error code, which is
typically `E` followed by a sequence of capital letters.

#### error.errno

* {string|number}

The `error.errno` property is a number or a string.
The number is a **negative** value which corresponds to the error code defined
in [`libuv Error handling`]. See uv-errno.h header file
(`deps/uv/include/uv-errno.h` in the Node.js source tree) for details. In case
of a string, it is the same as `error.code`.

#### error.syscall

* {string}

The `error.syscall` property is a string describing the [syscall][] that failed.

#### error.path

* {string}

When present (e.g. in `fs` or `child_process`), the `error.path` property is a
string containing a relevant invalid pathname.

#### error.address

* {string}

When present (e.g. in `net` or `dgram`), the `error.address` property is a
string describing the address to which the connection failed.

#### error.port

* {number}

When present (e.g. in `net` or `dgram`), the `error.port` property is a number
representing the connection's port that is not available.

### Common System Errors

This list is **not exhaustive**, but enumerates many of the common system
errors encountered when writing a Node.js program. An exhaustive list may be
found [here][online].

- `EACCES` (Permission denied): An attempt was made to access a file in a way
  forbidden by its file access permissions.

- `EADDRINUSE` (Address already in use):  An attempt to bind a server
  ([`net`][], [`http`][], or [`https`][]) to a local address failed due to
  another server on the local system already occupying that address.

- `ECONNREFUSED` (Connection refused): No connection could be made because the
  target machine actively refused it. This usually results from trying to
  connect to a service that is inactive on the foreign host.

- `ECONNRESET` (Connection reset by peer): A connection was forcibly closed by
  a peer. This normally results from a loss of the connection on the remote
  socket due to a timeout or reboot. Commonly encountered via the [`http`][]
  and [`net`][] modules.

- `EEXIST` (File exists): An existing file was the target of an operation that
  required that the target not exist.

- `EISDIR` (Is a directory): An operation expected a file, but the given
  pathname was a directory.

- `EMFILE` (Too many open files in system): Maximum number of
  [file descriptors][] allowable on the system has been reached, and
  requests for another descriptor cannot be fulfilled until at least one
  has been closed. This is encountered when opening many files at once in
  parallel, especially on systems (in particular, macOS) where there is a low
  file descriptor limit for processes. To remedy a low limit, run
  `ulimit -n 2048` in the same shell that will run the Node.js process.

- `ENOENT` (No such file or directory): Commonly raised by [`fs`][] operations
  to indicate that a component of the specified pathname does not exist -- no
  entity (file or directory) could be found by the given path.

- `ENOTDIR` (Not a directory): A component of the given pathname existed, but
  was not a directory as expected. Commonly raised by [`fs.readdir`][].

- `ENOTEMPTY` (Directory not empty): A directory with entries was the target
  of an operation that requires an empty directory -- usually [`fs.unlink`][].

- `EPERM` (Operation not permitted): An attempt was made to perform an
  operation that requires elevated privileges.

- `EPIPE` (Broken pipe): A write on a pipe, socket, or FIFO for which there is
  no process to read the data. Commonly encountered at the [`net`][] and
  [`http`][] layers, indicative that the remote side of the stream being
  written to has been closed.

- `ETIMEDOUT` (Operation timed out): A connect or send request failed because
  the connected party did not properly respond after a period of time. Usually
  encountered by [`http`][] or [`net`][] -- often a sign that a `socket.end()`
  was not properly called.

<a id="nodejs-error-codes"></a>
## Node.js Error Codes

<a id="ERR_ARG_NOT_ITERABLE"></a>
### ERR_ARG_NOT_ITERABLE

Used generically to identify that an iterable argument (i.e. a value that works
with `for...of` loops) is required, but not provided to a Node.js API.

<a id="ERR_ASSERTION"></a>
### ERR_ASSERTION

Used as special type of error that can be triggered whenever Node.js detects an
exceptional logic violation that should never occur. These are raised typically
by the `assert` module.

<a id="ERR_ASYNC_CALLBACK"></a>
### ERR_ASYNC_CALLBACK

Used with `AsyncHooks` to indicate an attempt of registering something that is
not a function as a callback.

<a id="ERR_ASYNC_TYPE"></a>
### ERR_ASYNC_TYPE

Used when the type of an asynchronous resource is invalid. Note that users are
also able to define their own types when using the public embedder API.

<a id="ERR_BUFFER_OUT_OF_BOUNDS"></a>
### ERR_BUFFER_OUT_OF_BOUNDS

Used when attempting to perform an operation outside the bounds of a `Buffer`.

<a id="ERR_BUFFER_TOO_LARGE"></a>
### ERR_BUFFER_TOO_LARGE

Used when an attempt has been made to create a `Buffer` larger than the
maximum allowed size.

<a id="ERR_CHILD_CLOSED_BEFORE_REPLY"></a>
### ERR_CHILD_CLOSED_BEFORE_REPLY

Used when a child process is closed before the parent received a reply.

<a id="ERR_CONSOLE_WRITABLE_STREAM"></a>
### ERR_CONSOLE_WRITABLE_STREAM

Used when `Console` is instantiated without `stdout` stream or when `stdout` or
`stderr` streams are not writable.

<a id="ERR_CPU_USAGE"></a>
### ERR_CPU_USAGE

Used when the native call from `process.cpuUsage` cannot be processed properly.

<a id="ERR_CRYPTO_ECDH_INVALID_FORMAT"></a>
### ERR_CRYPTO_ECDH_INVALID_FORMAT

Used when an invalid value for the `format` argument has been passed to the
`crypto.ECDH()` class `getPublicKey()` method.

<a id="ERR_CRYPTO_ENGINE_UNKNOWN"></a>
### ERR_CRYPTO_ENGINE_UNKNOWN

Used when an invalid crypto engine identifier is passed to
[`require('crypto').setEngine()`][].

<a id="ERR_CRYPTO_FIPS_FORCED"></a>
### ERR_CRYPTO_FIPS_FORCED

Used when trying to enable or disable FIPS mode in the crypto module and
the [`--force-fips`][] command-line argument is used.

<a id="ERR_CRYPTO_FIPS_UNAVAILABLE"></a>
### ERR_CRYPTO_FIPS_UNAVAILABLE

Used when trying to enable or disable FIPS mode when FIPS is not available.

<a id="ERR_CRYPTO_HASH_DIGEST_NO_UTF16"></a>
### ERR_CRYPTO_HASH_DIGEST_NO_UTF16

Used when the UTF-16 encoding is used with [`hash.digest()`][]. While the
`hash.digest()` method does allow an `encoding` argument to be passed in,
causing the method to return a string rather than a `Buffer`, the UTF-16
encoding (e.g. `ucs` or `utf16le`) is not supported.

<a id="ERR_CRYPTO_HASH_FINALIZED"></a>
### ERR_CRYPTO_HASH_FINALIZED

Used when [`hash.digest()`][] is called multiple times. The `hash.digest()`
method must be called no more than one time per instance of a `Hash` object.

<a id="ERR_CRYPTO_HASH_UPDATE_FAILED"></a>
### ERR_CRYPTO_HASH_UPDATE_FAILED

Used when [`hash.update()`][] fails for any reason. This should rarely, if
ever, happen.

<a id="ERR_CRYPTO_INVALID_DIGEST"></a>
### ERR_CRYPTO_INVALID_DIGEST

Used when an invalid [crypto digest algorithm][] is specified.

<a id="ERR_CRYPTO_INVALID_STATE"></a>
### ERR_CRYPTO_INVALID_STATE

Used generically when a crypto method is used on an object that is in an
invalid state. For instance, calling [`cipher.getAuthTag()`][] before calling
`cipher.final()`.

<a id="ERR_CRYPTO_SIGN_KEY_REQUIRED"></a>
### ERR_CRYPTO_SIGN_KEY_REQUIRED

Used when a signing `key` is not provided to the [`sign.sign()`][] method.

<a id="ERR_CRYPTO_TIMING_SAFE_EQUAL_LENGTH"></a>
### ERR_CRYPTO_TIMING_SAFE_EQUAL_LENGTH

Used when calling [`crypto.timingSafeEqual()`][] with `Buffer`, `TypedArray`,
or `DataView` arguments of different lengths.

<a id="ERR_DNS_SET_SERVERS_FAILED"></a>
### ERR_DNS_SET_SERVERS_FAILED

Used when `c-ares` failed to set the DNS server.

<a id="ERR_ENCODING_INVALID_ENCODED_DATA"></a>
### ERR_ENCODING_INVALID_ENCODED_DATA

Used by the `util.TextDecoder()` API when the data provided is invalid
according to the encoding provided.

<a id="ERR_ENCODING_NOT_SUPPORTED"></a>
### ERR_ENCODING_NOT_SUPPORTED

Used by the `util.TextDecoder()` API when the encoding provided is not one of
the [WHATWG Supported Encodings][].

<a id="ERR_FALSY_VALUE_REJECTION"></a>
### ERR_FALSY_VALUE_REJECTION

Used by the `util.callbackify()` API when a callbackified `Promise` is rejected
with a falsy value (e.g. `null`).

<a id="ERR_HTTP_HEADERS_SENT"></a>
### ERR_HTTP_HEADERS_SENT

Used when headers have already been sent and another attempt is made to add
more headers.

<a id="ERR_HTTP_INVALID_CHAR"></a>
### ERR_HTTP_INVALID_CHAR

Used when an invalid character is found in an HTTP response status message
(reason phrase).

<a id="ERR_HTTP_INVALID_STATUS_CODE"></a>
### ERR_HTTP_INVALID_STATUS_CODE

Used for status codes outside the regular status code ranges (100-999).

<a id="ERR_HTTP_TRAILER_INVALID"></a>
### ERR_HTTP_TRAILER_INVALID

Used when the `Trailer` header is set even though the transfer encoding does not
support that.

<a id="ERR_HTTP2_CONNECT_AUTHORITY"></a>
### ERR_HTTP2_CONNECT_AUTHORITY

For HTTP/2 requests using the `CONNECT` method, the `:authority` pseudo-header
is required.

<a id="ERR_HTTP2_CONNECT_PATH"></a>
### ERR_HTTP2_CONNECT_PATH

For HTTP/2 requests using the `CONNECT` method, the `:path` pseudo-header is
forbidden.

<a id="ERR_HTTP2_CONNECT_SCHEME"></a>
### ERR_HTTP2_CONNECT_SCHEME

For HTTP/2 requests using the `CONNECT` method, the `:scheme` pseudo-header is
forbidden.

<a id="ERR_HTTP2_FRAME_ERROR"></a>
### ERR_HTTP2_FRAME_ERROR

Used when a failure occurs sending an individual frame on the HTTP/2
session.

<a id="ERR_HTTP2_HEADER_REQUIRED"></a>
### ERR_HTTP2_HEADER_REQUIRED

Used when a required header is missing in an HTTP/2 message.

<a id="ERR_HTTP2_HEADER_SINGLE_VALUE"></a>
### ERR_HTTP2_HEADER_SINGLE_VALUE

Used when multiple values have been provided for an HTTP header field that
required to have only a single value.

<a id="ERR_HTTP2_HEADERS_AFTER_RESPOND"></a>
### ERR_HTTP2_HEADERS_AFTER_RESPOND

Used when trying to specify additional headers after an HTTP/2 response
initiated.

<a id="ERR_HTTP2_HEADERS_OBJECT"></a>
### ERR_HTTP2_HEADERS_OBJECT

Used when an HTTP/2 Headers Object is expected.

<a id="ERR_HTTP2_HEADERS_SENT"></a>
### ERR_HTTP2_HEADERS_SENT

Used when an attempt is made to send multiple response headers.

<a id="ERR_HTTP2_INFO_HEADERS_AFTER_RESPOND"></a>
### ERR_HTTP2_INFO_HEADERS_AFTER_RESPOND

HTTP/2 Informational headers must only be sent *prior* to calling the
`Http2Stream.prototype.respond()` method.

<a id="ERR_HTTP2_INFO_STATUS_NOT_ALLOWED"></a>
### ERR_HTTP2_INFO_STATUS_NOT_ALLOWED

Informational HTTP status codes (`1xx`) may not be set as the response status
code on HTTP/2 responses.

<a id="ERR_HTTP2_INVALID_CONNECTION_HEADERS"></a>
### ERR_HTTP2_INVALID_CONNECTION_HEADERS

HTTP/1 connection specific headers are forbidden to be used in HTTP/2
requests and responses.

<a id="ERR_HTTP2_INVALID_HEADER_VALUE"></a>
### ERR_HTTP2_INVALID_HEADER_VALUE

Used to indicate that an invalid HTTP/2 header value has been specified.

<a id="ERR_HTTP2_INVALID_INFO_STATUS"></a>
### ERR_HTTP2_INVALID_INFO_STATUS

An invalid HTTP informational status code has been specified. Informational
status codes must be an integer between `100` and `199` (inclusive).

<a id="ERR_HTTP2_INVALID_PACKED_SETTINGS_LENGTH"></a>
### ERR_HTTP2_INVALID_PACKED_SETTINGS_LENGTH

Input `Buffer` and `Uint8Array` instances passed to the
`http2.getUnpackedSettings()` API must have a length that is a multiple of
six.

<a id="ERR_HTTP2_INVALID_PSEUDOHEADER"></a>
### ERR_HTTP2_INVALID_PSEUDOHEADER

Only valid HTTP/2 pseudoheaders (`:status`, `:path`, `:authority`, `:scheme`,
and `:method`) may be used.

<a id="ERR_HTTP2_INVALID_SESSION"></a>
### ERR_HTTP2_INVALID_SESSION

Used when any action is performed on an `Http2Session` object that has already
been destroyed.

<a id="ERR_HTTP2_INVALID_SETTING_VALUE"></a>
### ERR_HTTP2_INVALID_SETTING_VALUE

An invalid value has been specified for an HTTP/2 setting.

<a id="ERR_HTTP2_INVALID_STREAM"></a>
### ERR_HTTP2_INVALID_STREAM

Used when an operation has been performed on a stream that has already been
destroyed.

<a id="ERR_HTTP2_MAX_PENDING_SETTINGS_ACK"></a>
### ERR_HTTP2_MAX_PENDING_SETTINGS_ACK

Whenever an HTTP/2 `SETTINGS` frame is sent to a connected peer, the peer is
required to send an acknowledgement that it has received and applied the new
SETTINGS. By default, a maximum number of un-acknowledged `SETTINGS` frame may
be sent at any given time. This error code is used when that limit has been
reached.

<a id="ERR_HTTP2_NO_SOCKET_MANIPULATION"></a>
### ERR_HTTP2_NO_SOCKET_MANIPULATION

Used when attempting to directly manipulate (e.g read, write, pause, resume,
etc.) a socket attached to an `Http2Session`.

<a id="ERR_HTTP2_OUT_OF_STREAMS"></a>
### ERR_HTTP2_OUT_OF_STREAMS

Used when the maximum number of streams on a single HTTP/2 session have been
created.

<a id="ERR_HTTP2_PAYLOAD_FORBIDDEN"></a>
### ERR_HTTP2_PAYLOAD_FORBIDDEN

Used when a message payload is specified for an HTTP response code for which
a payload is forbidden.

<a id="ERR_HTTP2_PSEUDOHEADER_NOT_ALLOWED"></a>
### ERR_HTTP2_PSEUDOHEADER_NOT_ALLOWED

Used to indicate that an HTTP/2 pseudo-header has been used inappropriately.
Pseudo-headers are header key names that begin with the `:` prefix.

<a id="ERR_HTTP2_PUSH_DISABLED"></a>
### ERR_HTTP2_PUSH_DISABLED

Used when push streams have been disabled by the client but an attempt to
create a push stream is made.

<a id="ERR_HTTP2_SEND_FILE"></a>
### ERR_HTTP2_SEND_FILE

Used when an attempt is made to use the
`Http2Stream.prototype.responseWithFile()` API to send a non-regular file.

<a id="ERR_HTTP2_SOCKET_BOUND"></a>
### ERR_HTTP2_SOCKET_BOUND

Used when an attempt is made to connect a `Http2Session` object to a
`net.Socket` or `tls.TLSSocket` that has already been bound to another
`Http2Session` object.

<a id="ERR_HTTP2_STATUS_101"></a>
### ERR_HTTP2_STATUS_101

Use of the `101` Informational status code is forbidden in HTTP/2.

<a id="ERR_HTTP2_STATUS_INVALID"></a>
### ERR_HTTP2_STATUS_INVALID

An invalid HTTP status code has been specified. Status codes must be an integer
between `100` and `599` (inclusive).

<a id="ERR_HTTP2_STREAM_CLOSED"></a>
### ERR_HTTP2_STREAM_CLOSED

Used when an action has been performed on an HTTP/2 Stream that has already
been closed.

<a id="ERR_HTTP2_STREAM_ERROR"></a>
### ERR_HTTP2_STREAM_ERROR

Used when a non-zero error code has been specified in an `RST_STREAM` frame.

<a id="ERR_HTTP2_STREAM_SELF_DEPENDENCY"></a>
### ERR_HTTP2_STREAM_SELF_DEPENDENCY

When setting the priority for an HTTP/2 stream, the stream may be marked as
a dependency for a parent stream. This error code is used when an attempt is
made to mark a stream and dependent of itself.

<a id="ERR_HTTP2_UNSUPPORTED_PROTOCOL"></a>
### ERR_HTTP2_UNSUPPORTED_PROTOCOL

Used when `http2.connect()` is passed a URL that uses any protocol other than
`http:` or `https:`.

<a id="ERR_INDEX_OUT_OF_RANGE"></a>
### ERR_INDEX_OUT_OF_RANGE

Used when a given index is out of the accepted range (e.g. negative offsets).

<a id="ERR_INSPECTOR_ALREADY_CONNECTED"></a>
### ERR_INSPECTOR_ALREADY_CONNECTED

When using the `inspector` module, the `ERR_INSPECTOR_ALREADY_CONNECTED` error
code is used when an attempt is made to connect when the inspector is already
connected.

<a id="ERR_INSPECTOR_CLOSED"></a>
### ERR_INSPECTOR_CLOSED

When using the `inspector` module, the `ERR_INSPECTOR_CLOSED` error code is
used when an attempt is made to use the inspector after the session has
already closed.

<a id="ERR_INSPECTOR_NOT_AVAILABLE"></a>
### ERR_INSPECTOR_NOT_AVAILABLE

Used to identify when the `inspector` module is not available for use.

<a id="ERR_INSPECTOR_NOT_CONNECTED"></a>
### ERR_INSPECTOR_NOT_CONNECTED

When using the `inspector` module, the `ERR_INSPECTOR_NOT_CONNECTED` error code
is used when an attempt is made to use the inspector before it is connected.

<a id="ERR_INVALID_ARG_TYPE"></a>
### ERR_INVALID_ARG_TYPE

Used generically to identify that an argument of the wrong type has been passed
to a Node.js API.

<a id="ERR_INVALID_ARG_VALUE"></a>
### ERR_INVALID_ARG_VALUE

Used generically to identify that an invalid or unsupported value has been
passed for a given argument.

<a id="ERR_INVALID_ARRAY_LENGTH"></a>
### ERR_INVALID_ARRAY_LENGTH

Used when an Array is not of the expected length or in a valid range.

<a id="ERR_INVALID_ASYNC_ID"></a>
### ERR_INVALID_ASYNC_ID

Used with `AsyncHooks` when an invalid `asyncId` or `triggerAsyncId` is passed.
An id less than -1 should never happen.

<a id="ERR_INVALID_BUFFER_SIZE"></a>
### ERR_INVALID_BUFFER_SIZE

Used when performing a swap on a `Buffer` but it's size is not compatible with the operation.

<a id="ERR_INVALID_CALLBACK"></a>
### ERR_INVALID_CALLBACK

Used generically to identify that a callback function is required and has not
been provided to a Node.js API.

<a id="ERR_INVALID_CHAR"></a>
### ERR_INVALID_CHAR

Used when invalid characters are detected in headers.

<a id="ERR_INVALID_CURSOR_POS"></a>
### ERR_INVALID_CURSOR_POS

The `'ERR_INVALID_CURSOR_POS'` is thrown specifically when a cursor on a given
stream is attempted to move to a specified row without a specified column.

<a id="ERR_INVALID_DOMAIN_NAME"></a>
### ERR_INVALID_DOMAIN_NAME

Used when `hostname` can not be parsed from a provided URL.

<a id="ERR_INVALID_FD"></a>
### ERR_INVALID_FD

Used when a file descriptor ('fd') is not valid (e.g. it has a negative value).

<a id="ERR_INVALID_FD_TYPE"></a>
### ERR_INVALID_FD_TYPE

Used when a file descriptor ('fd') type is not valid.

<a id="ERR_INVALID_FILE_URL_HOST"></a>
### ERR_INVALID_FILE_URL_HOST

Used when a Node.js API that consumes `file:` URLs (such as certain functions in
the [`fs`][] module) encounters a file URL with an incompatible host. Currently,
this situation can only occur on Unix-like systems, where only `localhost` or an
empty host is supported.

<a id="ERR_INVALID_FILE_URL_PATH"></a>
### ERR_INVALID_FILE_URL_PATH

Used when a Node.js API that consumes `file:` URLs (such as certain
functions in the [`fs`][] module) encounters a file URL with an incompatible
path. The exact semantics for determining whether a path can be used is
platform-dependent.

<a id="ERR_INVALID_HANDLE_TYPE"></a>
### ERR_INVALID_HANDLE_TYPE

Used when an attempt is made to send an unsupported "handle" over an IPC
communication channel to a child process. See [`subprocess.send()`] and
[`process.send()`] for more information.

<a id="ERR_INVALID_HTTP_TOKEN"></a>
### ERR_INVALID_HTTP_TOKEN

Used when `options.method` received an invalid HTTP token.

<a id="ERR_INVALID_IP_ADDRESS"></a>
### ERR_INVALID_IP_ADDRESS

Used when an IP address is not valid.

<a id="ERR_INVALID_OPT_VALUE"></a>
### ERR_INVALID_OPT_VALUE

Used generically to identify when an invalid or unexpected value has been
passed in an options object.

<a id="ERR_INVALID_OPT_VALUE_ENCODING"></a>
### ERR_INVALID_OPT_VALUE_ENCODING

Used when an invalid or unknown file encoding is passed.

<a id="ERR_INVALID_PERFORMANCE_MARK"></a>
### ERR_INVALID_PERFORMANCE_MARK

Used by the Performance Timing API (`perf_hooks`) when a performance mark is
invalid.

<a id="ERR_INVALID_PROTOCOL"></a>
### ERR_INVALID_PROTOCOL

Used when an invalid `options.protocol` is passed.

<a id="ERR_INVALID_REPL_EVAL_CONFIG"></a>
### ERR_INVALID_REPL_EVAL_CONFIG

Used when both `breakEvalOnSigint` and `eval` options are set
in the REPL config, which is not supported.

<a id="ERR_INVALID_SYNC_FORK_INPUT"></a>
### ERR_INVALID_SYNC_FORK_INPUT

Used when a `Buffer`, `Uint8Array` or `string` is provided as stdio input to a
synchronous fork. See the documentation for the
[`child_process`](child_process.html) module for more information.

<a id="ERR_INVALID_THIS"></a>
### ERR_INVALID_THIS

Used generically to identify that a Node.js API function is called with an
incompatible `this` value.

Example:

```js
const { URLSearchParams } = require('url');
const urlSearchParams = new URLSearchParams('foo=bar&baz=new');

const buf = Buffer.alloc(1);
urlSearchParams.has.call(buf, 'foo');
// Throws a TypeError with code 'ERR_INVALID_THIS'
```

<a id="ERR_INVALID_TUPLE"></a>
### ERR_INVALID_TUPLE

Used when an element in the `iterable` provided to the [WHATWG][WHATWG URL
API] [`URLSearchParams` constructor][`new URLSearchParams(iterable)`] does not
represent a `[name, value]` tuple – that is, if an element is not iterable, or
does not consist of exactly two elements.

<a id="ERR_INVALID_URI"></a>
### ERR_INVALID_URI

Used when an invalid URI is passed.

<a id="ERR_INVALID_URL"></a>
### ERR_INVALID_URL

Used when an invalid URL is passed to the [WHATWG][WHATWG URL API]
[`URL` constructor][`new URL(input)`] to be parsed. The thrown error object
typically has an additional property `'input'` that contains the URL that failed
to parse.

<a id="ERR_INVALID_URL_SCHEME"></a>
### ERR_INVALID_URL_SCHEME

Used generically to signify an attempt to use a URL of an incompatible scheme
(aka protocol) for a specific purpose. It is currently only used in the
[WHATWG URL API][] support in the [`fs`][] module (which only accepts URLs with
`'file'` scheme), but may be used in other Node.js APIs as well in the future.

<a id="ERR_IPC_CHANNEL_CLOSED"></a>
### ERR_IPC_CHANNEL_CLOSED

Used when an attempt is made to use an IPC communication channel that has
already been closed.

<a id="ERR_IPC_DISCONNECTED"></a>
### ERR_IPC_DISCONNECTED

Used when an attempt is made to disconnect an already disconnected IPC
communication channel between two Node.js processes. See the documentation for
the [`child_process`](child_process.html) module for more information.

<a id="ERR_IPC_ONE_PIPE"></a>
### ERR_IPC_ONE_PIPE

Used when an attempt is made to create a child Node.js process using more than
one IPC communication channel. See the documentation for the
[`child_process`](child_process.html) module for more information.

<a id="ERR_IPC_SYNC_FORK"></a>
### ERR_IPC_SYNC_FORK

Used when an attempt is made to open an IPC communication channel with a
synchronous forked Node.js process. See the documentation for the
[`child_process`](child_process.html) module for more information.

<a id="ERR_METHOD_NOT_IMPLEMENTED"></a>
### ERR_METHOD_NOT_IMPLEMENTED

Used when a method is required but not implemented.

<a id="ERR_MISSING_ARGS"></a>
### ERR_MISSING_ARGS

Used when a required argument of a Node.js API is not passed. This is only used
for strict compliance with the API specification (which in some cases may accept
`func(undefined)` but not `func()`). In most native Node.js APIs,
`func(undefined)` and `func()` are treated identically, and the
[`ERR_INVALID_ARG_TYPE`][] error code may be used instead.

<a id="ERR_MISSING_MODULE"></a>
### ERR_MISSING_MODULE

> Stability: 1 - Experimental

Used when an [ES6 module][] cannot be resolved.

<a id="ERR_MODULE_RESOLUTION_LEGACY"></a>
### ERR_MODULE_RESOLUTION_LEGACY

> Stability: 1 - Experimental

Used when a failure occurs resolving imports in an [ES6 module][].

<a id="ERR_MULTIPLE_CALLBACK"></a>
### ERR_MULTIPLE_CALLBACK

Used when a callback is called more then once.

*Note*: A callback is almost always meant to only be called once as the query
can either be fulfilled or rejected but not both at the same time. The latter
would be possible by calling a callback more then once.

<a id="ERR_NAPI_CONS_FUNCTION"></a>
### ERR_NAPI_CONS_FUNCTION

Used by the `N-API` when a constructor passed is not a function.

<a id="ERR_NAPI_CONS_PROTOTYPE_OBJECT"></a>
### ERR_NAPI_CONS_PROTOTYPE_OBJECT

Used by the `N-API` when `Constructor.prototype` is not an object.

<a id="ERR_NO_CRYPTO"></a>
### ERR_NO_CRYPTO

Used when an attempt is made to use crypto features while Node.js is not
compiled with OpenSSL crypto support.

<a id="ERR_NO_ICU"></a>
### ERR_NO_ICU

Used when an attempt is made to use features that require [ICU][], while
Node.js is not compiled with ICU support.

<a id="ERR_NO_LONGER_SUPPORTED"></a>
### ERR_NO_LONGER_SUPPORTED

Used when a Node.js API is called in an unsupported manner.

For example: `Buffer.write(string, encoding, offset[, length])`

<a id="ERR_OUTOFMEMORY"></a>
### ERR_OUTOFMEMORY

Used generically to identify that an operation caused an out of memory
condition.

<a id="ERR_OUT_OF_RANGE"></a>
### ERR_OUT_OF_RANGE

Used generically when an input argument value values outside an acceptable
range.

<a id="ERR_PARSE_HISTORY_DATA"></a>
### ERR_PARSE_HISTORY_DATA

Used by the `REPL` module when it cannot parse data from the REPL history file.

<a id="ERR_REQUIRE_ESM"></a>
### ERR_REQUIRE_ESM

> Stability: 1 - Experimental

Used when an attempt is made to `require()` an [ES6 module][].

<a id="ERR_SERVER_ALREADY_LISTEN"></a>
### ERR_SERVER_ALREADY_LISTEN

Used when the [`server.listen()`][] method is called while a `net.Server` is
already listening. This applies to all instances of `net.Server`, including
HTTP, HTTPS, and HTTP/2 Server instances.

<a id="ERR_SOCKET_ALREADY_BOUND"></a>
### ERR_SOCKET_ALREADY_BOUND

Used when an attempt is made to bind a socket that has already been bound.

<a id="ERR_SOCKET_BAD_BUFFER_SIZE"></a>
### ERR_SOCKET_BAD_BUFFER_SIZE

Used when an invalid (negative) size is passed for either the `recvBufferSize`
or `sendBufferSize` options in [`dgram.createSocket()`][].

<a id="ERR_SOCKET_BAD_PORT"></a>
### ERR_SOCKET_BAD_PORT

Used when an API function expecting a port > 0 and < 65536 receives an invalid
value.

<a id="ERR_SOCKET_BAD_TYPE"></a>
### ERR_SOCKET_BAD_TYPE

Used when an API function expecting a socket type (`udp4` or `udp6`) receives an
invalid value.

<a id="ERR_SOCKET_BUFFER_SIZE"></a>
### ERR_SOCKET_BUFFER_SIZE

Used when using [`dgram.createSocket()`][] and the size of the receive or send
`Buffer` cannot be determined.

<a id="ERR_SOCKET_CANNOT_SEND"></a>
### ERR_SOCKET_CANNOT_SEND

Used when data cannot be sent on a socket.

<a id="ERR_SOCKET_CLOSED"></a>
### ERR_SOCKET_CLOSED

Used when an attempt is made to operate on an already closed socket.

<a id="ERR_SOCKET_DGRAM_NOT_RUNNING"></a>
### ERR_SOCKET_DGRAM_NOT_RUNNING

Used when a call is made and the UDP subsystem is not running.

<a id="ERR_STDERR_CLOSE"></a>
### ERR_STDERR_CLOSE

Used when an attempt is made to close the `process.stderr` stream. By design,
Node.js does not allow `stdout` or `stderr` Streams to be closed by user code.

<a id="ERR_STDOUT_CLOSE"></a>
### ERR_STDOUT_CLOSE

Used when an attempt is made to close the `process.stdout` stream. By design,
Node.js does not allow `stdout` or `stderr` Streams to be closed by user code.

<a id="ERR_STREAM_CANNOT_PIPE"></a>
### ERR_STREAM_CANNOT_PIPE

Used when an attempt is made to call [`stream.pipe()`][] on a
[`Writable`][] stream.

<a id="ERR_STREAM_NULL_VALUES"></a>
### ERR_STREAM_NULL_VALUES

Used when an attempt is made to call [`stream.write()`][] with a `null`
chunk.

<a id="ERR_STREAM_PUSH_AFTER_EOF"></a>
### ERR_STREAM_PUSH_AFTER_EOF

Used when an attempt is made to call [`stream.push()`][] after a `null`(EOF)
has been pushed to the stream.

<a id="ERR_STREAM_READ_NOT_IMPLEMENTED"></a>
### ERR_STREAM_READ_NOT_IMPLEMENTED

Used when an attempt is made to use a readable stream that has not implemented
[`readable._read()`][].

<a id="ERR_STREAM_UNSHIFT_AFTER_END_EVENT"></a>
### ERR_STREAM_UNSHIFT_AFTER_END_EVENT

Used when an attempt is made to call [`stream.unshift()`][] after the
`end` event has been emitted.

<a id="ERR_STREAM_WRAP"></a>
### ERR_STREAM_WRAP

Used to prevent an abort if a string decoder was set on the Socket or if in
`objectMode`.

Example
```js
const Socket = require('net').Socket;
const instance = new Socket();

instance.setEncoding('utf8');
```

<a id="ERR_STREAM_WRITE_AFTER_END"></a>
### ERR_STREAM_WRITE_AFTER_END

Used when an attempt is made to call [`stream.write()`][] after
`stream.end()` has been called.

<a id="ERR_TLS_CERT_ALTNAME_INVALID"></a>
### ERR_TLS_CERT_ALTNAME_INVALID

Used with TLS, when the hostname/IP of the peer does not match any of the
subjectAltNames in its certificate.

<a id="ERR_TLS_DH_PARAM_SIZE"></a>
### ERR_TLS_DH_PARAM_SIZE

Used with TLS when the parameter offered for the Diffie-Hellman (`DH`)
key-agreement protocol is too small. By default, the key length must be greater
than or equal to 1024 bits to avoid vulnerabilities, even though it is strongly
recommended to use 2048 bits or larger for stronger security.

<a id="ERR_TLS_HANDSHAKE_TIMEOUT"></a>
### ERR_TLS_HANDSHAKE_TIMEOUT

A TLS error emitted by the server whenever a TLS/SSL handshake times out. In
this case, the server must also abort the connection.

<a id="ERR_TLS_RENEGOTIATION_FAILED"></a>
### ERR_TLS_RENEGOTIATION_FAILED

Used when a TLS renegotiation request has failed in a non-specific way.

<a id="ERR_TLS_REQUIRED_SERVER_NAME"></a>
### ERR_TLS_REQUIRED_SERVER_NAME

Used with TLS, when calling the `server.addContext()` method without providing
a hostname in the first parameter.

<a id="ERR_TLS_SESSION_ATTACK"></a>
### ERR_TLS_SESSION_ATTACK

Used when an excessive amount of TLS renegotiations is detected, which is a
potential vector for denial-of-service attacks.

<a id="ERR_TRANSFORM_ALREADY_TRANSFORMING"></a>
### ERR_TRANSFORM_ALREADY_TRANSFORMING

Used in Transform streams when the stream finishes while it is still
transforming.

<a id="ERR_TRANSFORM_WITH_LENGTH_0"></a>
### ERR_TRANSFORM_WITH_LENGTH_0

Used in Transform streams when the stream finishes with data still in the write
buffer.

<a id="ERR_UNESCAPED_CHARACTERS"></a>
### ERR_UNESCAPED_CHARACTERS

Used when a string that contains unescaped characters was received.

<a id="ERR_UNHANDLED_ERROR"></a>
### ERR_UNHANDLED_ERROR

Used when an unhandled "error" occurs (for instance, when an `'error'` event
is emitted by an `EventEmitter` but an `'error'` handler is not registered).

<a id="ERR_UNKNOWN_ENCODING"></a>
### ERR_UNKNOWN_ENCODING

Used when an invalid or unknown encoding option is passed to an API.

<a id="ERR_UNKNOWN_FILE_EXTENSION"></a>
### ERR_UNKNOWN_FILE_EXTENSION

> Stability: 1 - Experimental

Used when attempting to load a module with an unknown or unsupported file
extension.

<a id="ERR_UNKNOWN_MODULE_FORMAT"></a>
### ERR_UNKNOWN_MODULE_FORMAT

> Stability: 1 - Experimental

Used when attempting to load a module with an unknown or unsupported format.

<a id="ERR_UNKNOWN_SIGNAL"></a>
### ERR_UNKNOWN_SIGNAL

Used when an invalid or unknown process signal is passed to an API expecting a
valid signal (such as [`subprocess.kill()`][]).

<a id="ERR_UNKNOWN_STDIN_TYPE"></a>
### ERR_UNKNOWN_STDIN_TYPE

Used when an attempt is made to launch a Node.js process with an unknown `stdin`
file type. Errors of this kind cannot *typically* be caused by errors in user
code, although it is not impossible. Occurrences of this error are most likely
an indication of a bug within Node.js itself.

<a id="ERR_UNKNOWN_STREAM_TYPE"></a>
### ERR_UNKNOWN_STREAM_TYPE

Used when an attempt is made to launch a Node.js process with an unknown
`stdout` or `stderr` file type. Errors of this kind cannot *typically* be caused
by errors in user code, although it is not impossible. Occurrences of this error
are most likely an indication of a bug within Node.js itself.

<a id="ERR_V8BREAKITERATOR"></a>
### ERR_V8BREAKITERATOR

Used when the V8 BreakIterator API is used but the full ICU data set is not
installed.

<a id="ERR_VALID_PERFORMANCE_ENTRY_TYPE"></a>
### ERR_VALID_PERFORMANCE_ENTRY_TYPE

Used by the Performance Timing API (`perf_hooks`) when no valid performance
entry types were found.

<a id="ERR_VALUE_OUT_OF_RANGE"></a>
### ERR_VALUE_OUT_OF_RANGE

Used when a given value is out of the accepted range.

<a id="ERR_ZLIB_BINDING_CLOSED"></a>
### ERR_ZLIB_BINDING_CLOSED

Used when an attempt is made to use a `zlib` object after it has already been
closed.

<a id="ERR_ZLIB_INITIALIZATION_FAILED"></a>
### ERR_ZLIB_INITIALIZATION_FAILED

Used when creation of a [`zlib`][] object fails due to incorrect configuration.

[`--force-fips`]: cli.html#cli_force_fips
[`cipher.getAuthTag()`]: crypto.html#crypto_cipher_getauthtag
[`crypto.timingSafeEqual()`]: crypto.html#crypto_crypto_timingsafeequal_a_b
[`dgram.createSocket()`]: dgram.html#dgram_dgram_createsocket_options_callback
[`ERR_INVALID_ARG_TYPE`]: #ERR_INVALID_ARG_TYPE
[`hash.digest()`]: crypto.html#crypto_hash_digest_encoding
[`hash.update()`]: crypto.html#crypto_hash_update_data_inputencoding
[`readable._read()`]: stream.html#stream_readable_read_size_1
[`sign.sign()`]: crypto.html#crypto_sign_sign_privatekey_outputformat
[`stream.pipe()`]: stream.html#stream_readable_pipe_destination_options
[`stream.push()`]: stream.html#stream_readable_push_chunk_encoding
[`stream.unshift()`]: stream.html#stream_readable_unshift_chunk
[`stream.write()`]: stream.html#stream_writable_write_chunk_encoding_callback
[`Writable`]: stream.html#stream_class_stream_writable
[`subprocess.kill()`]: child_process.html#child_process_subprocess_kill_signal
[`subprocess.send()`]: child_process.html#child_process_subprocess_send_message_sendhandle_options_callback
[`fs.readFileSync`]: fs.html#fs_fs_readfilesync_path_options
[`fs.readdir`]: fs.html#fs_fs_readdir_path_options_callback
[`fs.unlink`]: fs.html#fs_fs_unlink_path_callback
[`fs`]: fs.html
[`http`]: http.html
[`https`]: https.html
[`libuv Error handling`]: http://docs.libuv.org/en/v1.x/errors.html
[`net`]: net.html
[`new URL(input)`]: url.html#url_constructor_new_url_input_base
[`new URLSearchParams(iterable)`]: url.html#url_constructor_new_urlsearchparams_iterable
[`process.on('uncaughtException')`]: process.html#process_event_uncaughtexception
[`process.send()`]: process.html#process_process_send_message_sendhandle_options_callback
[`require('crypto').setEngine()`]: crypto.html#crypto_crypto_setengine_engine_flags
[`server.listen()`]: net.html#net_server_listen
[ES6 module]: esm.html
[Node.js Error Codes]: #nodejs-error-codes
[V8's stack trace API]: https://github.com/v8/v8/wiki/Stack-Trace-API
[WHATWG URL API]: url.html#url_the_whatwg_url_api
[crypto digest algorithm]: crypto.html#crypto_crypto_gethashes
[domains]: domain.html
[event emitter-based]: events.html#events_class_eventemitter
[file descriptors]: https://en.wikipedia.org/wiki/File_descriptor
[ICU]: intl.html#intl_internationalization_support
[online]: http://man7.org/linux/man-pages/man3/errno.3.html
[stream-based]: stream.html
[syscall]: http://man7.org/linux/man-pages/man2/syscall.2.html
[try-catch]: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Statements/try...catch
[vm]: vm.html
[WHATWG Supported Encodings]: util.html#util_whatwg_supported_encodings
[`zlib`]: zlib.html
