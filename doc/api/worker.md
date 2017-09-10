# Worker

<!--introduced_in=REPLACEME-->

> Stability: 1 - Experimental

The `worker` module provides a way to create multiple environments running
on independent threads, and to create message channels between those. It
can be accessed using:

```js
const worker = require('worker');
```

Workers are useful for performing CPU-intensive JavaScript operations; do not
use them for I/O, since Ayo’s built-in mechanisms for performing operations
asynchronously already treat it more efficiently than Worker threads can.

Workers can also, unlike child processes or when using the `cluster` module,
share memory efficiently by transferring `ArrayBuffer` instances or sharing
`SharedArrayBuffer` instances between them.

## Example

```js
const { Worker, isMainThread, postMessage, workerData } = require('worker');

if (isMainThread) {
  module.exports = async function parseJSAsync(script) {
    return new Promise((resolve, reject) => {
      const worker = new Worker(__filename, {
        workerData: script
      });
      worker.on('message', resolve);
      worker.on('error', reject);
      worker.on('exit', (code) => {
        if (code !== 0)
          reject(new Error(`Worker stopped with exit code ${code}`));
      });
    });
  };
} else {
  const { parse } = require('some-js-parsing-library');
  const script = workerData;
  postMessage(parse(script));
}
```

## Event: 'workerMessage'
<!-- YAML
added: REPLACEME
-->

If this thread was spawned by a `Worker`, this event emits messages sent using
[`worker.postMessage()`][]. See the [`port.on('message')`][] event for
more details.

Listeners on this event will receive a clone of the `value` parameter as passed
to `postMessage()` and no further arguments.

*Note*: This event is emitted on the `worker` module as returned by
`require('worker')` itself.

## worker.isMainThread
<!-- YAML
added: REPLACEME
-->

* {boolean}

Is `true` if this code is not running inside of a [`Worker`][] thread.

## worker.postMessage(value[, transferList])
<!-- YAML
added: REPLACEME
-->

* `value` {any}
* `transferList` {Object[]}

* Returns: {undefined}

This method is available if this is a `Worker` thread, which can be tested
using [`require('worker').isMainThread`][].

Send a message to the parent thread’s `Worker` instance that will be received
via [`worker.on('message')`][]. See [`port.postMessage()`][] for
more details.

### worker.ref()
<!-- YAML
added: REPLACEME
-->

Opposite of `unref`, calling `ref` on a previously `unref`d worker will *not*
let the program exit if it's the only active handle left (the default behavior).
If the worker is `ref`d calling `ref` again will have no effect.

### worker.unref()
<!-- YAML
added: REPLACEME
-->

Calling `unref` on a worker will allow the thread to exit if this is the only
active handle in the event system. If the worker is already `unref`d calling
`unref` again will have no effect.

## worker.threadId
<!-- YAML
added: REPLACEME
-->

* {integer}

An integer identifier for the current thread. On the corresponding worker object
(if there is any), it is available as [`worker.threadId`][].

## worker.workerData
<!-- YAML
added: REPLACEME
-->

An arbitrary JavaScript value that contains a clone of the data passed
to this thread’s `Worker` constructor.


## Class: MessageChannel
<!-- YAML
added: REPLACEME
-->


Instances of the `worker.MessageChannel` class represent an asynchronous,
two-way communications channel.
The `MessageChannel` has no methods of its own. `new MessageChannel()`
yields an object with `port1` and `port2` properties, which refer to linked
[`MessagePort`][] instances.

```js
const { MessageChannel } = require('worker');

const { port1, port2 } = new MessageChannel();
port1.on('message', (message) => console.log('received', message));
port2.postMessage({ foo: 'bar' });
// prints: received { foo: 'bar' }
```

## Class: MessagePort
<!-- YAML
added: REPLACEME
-->

* Extends: {EventEmitter}

Instances of the `worker.MessagePort` class represent one end of an
asynchronous, two-way communications channel. It can be used to transfer
structured data, memory regions and other `MessagePort`s between different
[`Worker`][]s or [`vm` context][vm]s.

For transferring `MessagePort` instances between VM contexts, see
[`vm.moveMessagePortToContext()`][].

*Note*: With the exception of `MessagePort`s being [`EventEmitter`][]s rather
than `EventTarget`s, this implementation matches [browser `MessagePort`][]s.

### Event: 'close'
<!-- YAML
added: REPLACEME
-->

The `'close'` event is emitted once either side of the channel has been
disconnected.

### Event: 'error'
<!-- YAML
added: REPLACEME
-->

The `'error'` event is emitted if the worker thread throws an uncaught
expection. In that case, the worker will be terminated.

### Event: 'message'
<!-- YAML
added: REPLACEME
-->

* `value` {any} The transmitted value

The `'message'` event is emitted for any incoming message, containing the cloned
input of [`port.postMessage()`][].

Listeners on this event will receive a clone of the `value` parameter as passed
to `postMessage()` and no further arguments.

### port.close()
<!-- YAML
added: REPLACEME
-->

* Returns: {undefined}

Disables further sending of messages on either side of the connection.
You should call this method once you know that no further communication
will happen over this `MessagePort`.

### port.postMessage(value[, transferList])
<!-- YAML
added: REPLACEME
-->

* `value` {any}
* `transferList` {Object[]}

* Returns: {undefined}

Sends a JavaScript value to the receiving side of this channel.
`value` will be transferred in a way
that is compatible with the [HTML structured clone algorithm][]. In particular,
it may contain circular references and objects like typed arrays that `JSON`
is not able to serialize.

`transferList` may be a list of `ArrayBuffer` and `MessagePort` objects.
After transferring, they will not be usable on the sending side of the channel
anymore (even if they are not contained in `value`).

If `value` contains [`SharedArrayBuffer`][] instances, those will be accessible
from either thread.

`value` may still contain `ArrayBuffer` instances that are not in
`transferList`; in that case, the underlying memory is copied rather than moved.

For more information on the serialization and deserialization mechanisms
behind this API, see the [serialization API of the `v8` module][v8.serdes].

*Note*: Because the object cloning uses the structured clone algorithm,
non-enumberable properties, accessors, and prototypes are not preserved.
In particular, [`Buffer`][] objects will be read as plain [`Uint8Array`][]s
on the receiving side.

### port.ref()
<!-- YAML
added: REPLACEME
-->

Opposite of `unref`, calling `ref` on a previously `unref`d port will *not*
let the program exit if it's the only active handle left (the default behavior).
If the port is `ref`d calling `ref` again will have no effect.

### port.unref()
<!-- YAML
added: REPLACEME
-->

Calling `unref` on a port will allow the thread to exit if this is the only
active handle in the event system. If the port is already `unref`d calling
`unref` again will have no effect.

### port.start()
<!-- YAML
added: REPLACEME
-->

* Returns: {undefined}

Starts receiving messages on this `MessagePort`. When using this port
as an event emitter, this will be called automatically once `'message'`
listeners are attached. This means that this method does not need to be used
unless you are using [`vm.moveMessagePortToContext()`][] to move this `port`
into another VM context.

## Class: Worker
<!-- YAML
added: REPLACEME
-->

The `Worker` class represents an independent JavaScript execution thread.
Most Ayo APIs are available inside of it.

Notable differences inside a Worker environment are:

- The [`process.stdin`][], [`process.stdout`][] and [`process.stderr`][]
  properties are set to `null`.
- The [`domain`][] module is not usable inside of workers.
- The [`require('worker').isMainThread`][] property is set to `false`.
- The [`require('worker').postMessage()`][] method is available and the
  [`require('worker').on('workerMessage')`][] event is being emitted.
- [`process.exit()`][] does not stop the whole program, just the single thread,
  and [`process.abort()`][] is not available.
- [`process.chdir()`][] as well as `process` methods that set group or user ids
  are not available.
- [`process.env`][] is a read-only reference to the environment variables.
- [`process.title`][] can not be modified.
- Native addons that were not build with explicit `Worker` support can not be
  loaded.
- Execution may stop at any point as a result of the [`worker.terminate()`][]
  method being invoked.
- IPC channels from parent processes are not accessible.

Creating `Worker` instances inside of other `Worker`s is permitted.

Like [Web Workers][] and the [`cluster` module][], two-way communication can be
achieved through inter-thread message passing. Internally, a `Worker` has a
built-in pair of [`MessagePort`][]s that are already associated with each other
when the `Worker` is created. While the `MessagePort` objects are not directly
exposed, their functionalities are exposed through [`worker.postMessage()`][]
and the [`worker.on('message')`][] event on the `Worker` object for the parent
thread, and [`require('worker').postMessage()`][] and the
[`require('worker').on('workerMessage')`][] on `require('worker')` for the
child thread.

To create custom messaging channels (which is encouraged over using the default
global channel because it facilitates seperation of concerns), users can create
a `MessageChannel` object on either thread and pass one of the
`MessagePort`s on that `MessageChannel` to the other thread through a
pre-existing channel, such as the global one.

See [`port.postMessage()`][] for more information on how messages are passed,
and what kind of JavaScript values can be successfully transported through
the thread barrier.

For example:

```js
const assert = require('assert');
const { Worker, MessageChannel, MessagePort, isMainThread } = require('worker');
if (isMainThread) {
  const worker = new Worker(__filename);
  const subChannel = new MessageChannel();
  worker.postMessage({ hereIsYourPort: subChannel.port1 }, [subChannel.port1]);
  subChannel.port2.on('message', (value) => {
    console.log('received:', value);
  });
} else {
  require('worker').once('workerMessage', (value) => {
    assert(value.hereIsYourPort instanceof MessagePort);
    value.hereIsYourPort.postMessage('the worker is sending this');
    value.hereIsYourPort.close();
  });
}
```

### new Worker(filename, options)

* filename {string} The absolute path to the Worker’s main script.
  If `options.eval` is true, this is a string containing JavaScript code rather
  than a path.
* options {Object}
  * eval {boolean} If true, interpret the first argument to the constructor
    as a script that is executed once the worker is online.
  * data {any} Any JavaScript value that will be cloned and made
    available as [`require('worker').workerData`][]. The cloning will occur as
    described in the [HTML structured clone algorithm][], and an error will be
    thrown if the object can not be cloned (e.g. because it contains
    `function`s).
  * maxSemiSpaceSize {integer} An optional memory limit in MB for the thread’s
    heap’s semi-space, which contains most short-lived objects.
  * maxOldSpaceSize {integer} An optional memory limit in MB for the thread’s
    main heap.

### Event: 'exit'
<!-- YAML
added: REPLACEME
-->

* `exitCode` {integer}

The `'exit'` event is emitted once the worker has stopped. If the worker
exited by calling [`process.exit()`][], the `exitCode` parameter will be the
passed exit code. If the worker was terminated, the `exitCode` parameter will
be `1`.

### Event: 'message'
<!-- YAML
added: REPLACEME
-->

* `value` {any} The transmitted value

The `'message'` event is emitted when the worker thread has invoked
[`require('worker').postMessage()`][]. See the [`port.on('message')`][] event
for more details.

### Event: 'online'
<!-- YAML
added: REPLACEME
-->

The `'online'` event is emitted when the worker thread has started executing
JavaScript code.

### worker.postMessage(value[, transferList])
<!-- YAML
added: REPLACEME
-->

* `value` {any}
* `transferList` {Object[]}

Send a message to the worker that will be received via
[`require('worker').on('workerMessage')`][]. See [`port.postMessage()`][] for
more details.

### worker.terminate([callback])
<!-- YAML
added: REPLACEME
-->

* `callback` {Function}

Stop all JavaScript execution in the worker thread as soon as possible.
`callback` is an optional function that is invoked once this operation is known
to have completed.

*Note*: Currently, not all code in the internals of Ayo.js is prepared to expect
termination at arbitrary points in time and may crash if it encounters that
condition. Consequently, you should currently only call `.terminate()` if
it is known that the Worker thread is not accessing Ayo.js core modules other
than what is exposed in the `worker` module.

### worker.threadId
<!-- YAML
added: REPLACEME
-->

* {integer}

An integer identifier for the referenced thread. Inside the worker thread,
it is available as [`require('worker').threadId`][].

[`Buffer`]: buffer.html
[`EventEmitter`]: events.html
[`MessagePort`]: #worker_class_messageport
[`port.postMessage()`]: #worker_port_postmessage_value_transferlist
[`Worker`]: #worker_class_worker
[`worker.terminate()`]: #worker_worker_terminate_callback
[`worker.postMessage()`]: #worker_worker_postmessage_value_transferlist_1
[`worker.on('message')`]: #worker_event_message_1
[`worker.threadId`]: #worker_worker_threadid_1
[`port.postMessage()`]: #worker_port_postmessage_value_transferlist
[`port.on('message')`]: #worker_event_message
[`process.exit()`]: process.html#process_process_exit
[`process.exit()`]: process.html#process_process_exit
[`process.abort()`]: process.html#process_process_abort
[`process.chdir()`]: process.html#process_process_chdir_directory
[`process.env`]: process.html#process_process_env
[`process.stdin`]: process.html#process_process_stdin
[`process.stderr`]: process.html#process_process_stderr
[`process.stdout`]: process.html#process_process_stdout
[`process.title`]: process.html#process_process_title
[`require('worker').workerData`]: #worker_worker_workerdata
[`require('worker').on('workerMessage')`]: #worker_event_workermessage
[`require('worker').postMessage()`]: #worker_worker_postmessage_value_transferlist
[`require('worker').isMainThread`]: #worker_worker_is_main_thread
[`require('worker').threadId`]: #worker_worker_threadid
[`domain`]: domain.html
[`vm.moveMessagePortToContext()`]: vm.html#vm_vm_movemessageporttocontext_port_context
[`cluster` module]: cluster.html
[vm]: vm.html#vm_vm_executing_javascript
[v8.serdes]: v8.html#v8_serialization_api
[`SharedArrayBuffer`]: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/SharedArrayBuffer
[`Uint8Array`]: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Uint8Array
[browser `MessagePort`]: https://developer.mozilla.org/en-US/docs/Web/API/MessagePort
[HTML structured clone algorithm]: https://developer.mozilla.org/en-US/docs/Web/API/Web_Workers_API/Structured_clone_algorithm
[Web Workers]: https://developer.mozilla.org/en-US/docs/Web/API/Web_Workers_API

