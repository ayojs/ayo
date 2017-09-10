# Worker

> Stability: 1 - Experimental

The `worker` module provides a way to create multiple environments running
on independent threads, and to create message channels between those. It
can be accessed using:

```js
const worker = require('worker');
```

Workers are useful for performing CPU-intensive JavaScript operations; do not
use them for I/O, since Node’s built-in mechanisms for performing operations
asynchronously already treat it more efficiently than Worker threads can.

Workers can also, unlike child processes, share memory efficiently by
transferring `ArrayBuffer` instances or sharing `SharedArrayBuffer` instances
between them.

## Example

```js
const { Worker } = require('worker');

if (process.isMainThread) {
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
  const script = process.workerData;
  process.postMessage(parse(script));
}
```

## Class: MessageChannel
<!-- YAML
added: REPLACEME
-->

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

Instances of the `worker.MessagePort` class represent an asynchronous
communications channel. It can be used to transfer structured data,
memory regions and other `MessagePort`s between different [`Worker`][]s
or [`vm` context][vm]s.

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

The `'message'` event is emitted for any incoming message, containing the cloned
input of [`port.postMessage()`][].

### port.close()
<!-- YAML
added: REPLACEME
-->

* Returns: {undefined}

Disables further sending of messages on either side of the connection.

### port.postMessage(value[, transferList])
<!-- YAML
added: REPLACEME
-->

* Returns: {undefined}

Sends a JavaScript value to the receiving side of this channel.
`value` will be transferred in a way
that is compatible with the [HTML structured clone algorithm][]. In particular,
it may contain circular references and objects like typed arrays that `JSON`
is not able to serialize.

`transferList` may be a list of `ArrayBuffer` and `MessagePort` objects.
After transferring, they will not be usable on the sending side of the channel
anymore.

If `value` contains [`SharedArrayBuffer`][] instances, those will be accessible
from either thread.

### port.start()
<!-- YAML
added: REPLACEME
-->

* Returns: {undefined}

Starts receiving messages on this `MessagePort`. When using this port
as an event emitter, this will be called automatically once `message` listeners
are attached.

## Class: Worker
<!-- YAML
added: REPLACEME
-->

The `Worker` class represents an independent JavaScript execution thread.
Most Node APIs are available inside of it.

Notable differences inside a Worker environment are:

- The [`process.stdin`][], [`process.stdout`][] and [`process.stderr`][]
  properties are set to `null`.
- The [`domain`][] module is not usable inside of workers.
- The [`process.isMainThread`][] property is set to `false`.
- The [`process.postMessage()`][] method is available and the
  [process.on('`workerMessage`')][] is being emitted on the `process` object.
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

### new Worker(filename, options)

* filename {string} The absolute path to the Worker’s main script.
  If `options.eval` is true, this is a string containing JavaScript code rather
  than a path.
* options {Object}
  * eval {boolean} If true, interpret the first argument to the constructor
    as a script that is executed once the worker is online.
  * data {value} An arbitrary JavaScript value that will be cloned and made
    available as [`process.workerData`][]. The cloning will occur as described
    in the [HTML structured clone algorithm][], and an error will be thrown
    if the object can not be cloned (e.g. because it contains `function`s).
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

The `'message'` event is emitted when the worker thread has invoked
[`process.postMessage()`][]. See the [`port.on('message')`][] event for
more details.

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

Send a message to the worker that will be received via
[`process.on('workerMessage')`][]. See [`port.postMessage()`][] for
more details.

### worker.terminate([callback])
<!-- YAML
added: REPLACEME
-->

Stop all JavaScript execution in the worker thread as soon as possible.
`callback` is an optional function that is invoked once this operation is known
to have completed.

### worker.threadId
<!-- YAML
added: REPLACEME
-->

* {integer}

An integer identifier for the referenced thread. Inside the worker thread,
it is available as [`process.threadId`][].

[`EventEmitter`]: events.html
[`MessagePort`]: #worker_class_messageport
[`port.postMessage()`]: #worker_port_postmessage_value_transferlist
[`Worker`]: #worker_class_worker
[`worker.terminate()`]: #worker_worker_terminate_callback
[`process.isMainThread`]: process.html#process_process_is_main_thread
[`process.exit()`]: process.html#process_process_exit
[`process.abort()`]: process.html#process_process_abort
[`process.chdir()`]: process.html#process_process_chdir_directory
[`process.env`]: process.html#process_process_env
[`process.postMessage()`]: process.html#process_process_post_message_value_transferlist
[`process.stdin`]: process.html#process_process_stdin
[`process.stderr`]: process.html#process_process_stderr
[`process.stdout`]: process.html#process_process_stdout
[`process.threadId`]: process.html#process_process_threadid
[`process.title`]: process.html#process_process_title
[`process.workerData`]: process.html#process_process_workerdata
[`process.on('workerMessage')`]: process.html#process_event_workermessage
[`domain`]: domain.html
[`vm.moveMessagePortToContext()`]: vm.html#vm_vm_movemessageporttocontext_port_context
[vm]: vm.html#vm_vm_executing_javascript
[`SharedArrayBuffer`]: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/SharedArrayBuffer
[browser `MessagePort`]: https://developer.mozilla.org/en-US/docs/Web/API/MessagePort
[HTML structured clone algorithm]: https://developer.mozilla.org/en-US/docs/Web/API/Web_Workers_API/Structured_clone_algorithm
