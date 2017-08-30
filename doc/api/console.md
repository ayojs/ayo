# Console

<!--introduced_in=v0.10.13-->

> Stability: 2 - Stable

The `console` module provides a simple debugging console that is similar to the
JavaScript console mechanism provided by web browsers.

The module exports two specific components:

* A `Console` class with methods such as `console.log()`, `console.error()` and
  `console.warn()` that can be used to write to any Node.js stream.
* A global `console` instance configured to write to [`process.stdout`][] and
  [`process.stderr`][].  The global `console` can be used without calling
  `require('console')`.

***Warning***: The global console object's methods are neither consistently
synchronous like the browser APIs they resemble, nor are they consistently
asynchronous like all other Node.js streams. See the [note on process I/O][] for
more information.

Example using the global `console`:

```js
console.log('hello world');
// Prints: hello world, to stdout
console.log('hello %s', 'world');
// Prints: hello world, to stdout
console.error(new Error('Whoops, something bad happened'));
// Prints: [Error: Whoops, something bad happened], to stderr

const name = 'Will Robinson';
console.warn(`Danger ${name}! Danger!`);
// Prints: Danger Will Robinson! Danger!, to stderr
```

Example using the `Console` class:

```js
const out = getStreamSomehow();
const err = getStreamSomehow();
const myConsole = new console.Console(out, err);

myConsole.log('hello world');
// Prints: hello world, to out
myConsole.log('hello %s', 'world');
// Prints: hello world, to out
myConsole.error(new Error('Whoops, something bad happened'));
// Prints: [Error: Whoops, something bad happened], to err

const name = 'Will Robinson';
myConsole.warn(`Danger ${name}! Danger!`);
// Prints: Danger Will Robinson! Danger!, to err
```

## Class: Console
<!-- YAML
changes:
  - version: v8.0.0
    pr-url: https://github.com/nodejs/node/pull/9744
    description: Errors that occur while writing to the underlying streams
                 will now be ignored.
-->

<!--type=class-->

The `Console` class can be used to create a simple logger with configurable
output streams and can be accessed using either `require('console').Console`
or `console.Console` (or their destructured counterparts):

```js
const { Console } = require('console');
```

```js
const { Console } = console;
```

### new Console(stdout[, stderr])
* `stdout` {Writable}
* `stderr` {Writable}

Creates a new `Console` by passing one or two writable stream instances.
`stdout` is a writable stream to print log or info output. `stderr`
is used for warning or error output. If `stderr` is not passed, warning and error
output will be sent to `stdout`.

```js
const output = fs.createWriteStream('./stdout.log');
const errorOutput = fs.createWriteStream('./stderr.log');
// custom simple logger
const logger = new Console(output, errorOutput);
// use it like console
const count = 5;
logger.log('count: %d', count);
// in stdout.log: count 5
```

The global `console` is a special `Console` whose output is sent to
[`process.stdout`][] and [`process.stderr`][]. It is equivalent to calling:

```js
new Console(process.stdout, process.stderr);
```

### console.assert(value[, message][, ...args])
<!-- YAML
added: v0.1.101
-->
* `value` {any}
* `message` {any}
* `...args` {any}

A simple assertion test that verifies whether `value` is truthy. If it is not,
an `AssertionError` is thrown. If provided, the error `message` is formatted
using [`util.format()`][] and used as the error message.

```js
console.assert(true, 'does nothing');
// OK
console.assert(false, 'Whoops %s', 'didn\'t work');
// AssertionError: Whoops didn't work
```

*Note*: The `console.assert()` method is implemented differently in Node.js
than the `console.assert()` method [available in browsers][web-api-assert].

Specifically, in browsers, calling `console.assert()` with a falsy
assertion will cause the `message` to be printed to the console without
interrupting execution of subsequent code. In Node.js, however, a falsy
assertion will cause an `AssertionError` to be thrown.

Functionality approximating that implemented by browsers can be implemented
by extending Node.js' `console` and overriding the `console.assert()` method.

In the following example, a simple module is created that extends and overrides
the default behavior of `console` in Node.js.

<!-- eslint-disable func-name-matching -->
```js
'use strict';

// Creates a simple extension of console with a
// new impl for assert without monkey-patching.
const myConsole = Object.create(console, {
  assert: {
    value: function assert(assertion, message, ...args) {
      try {
        console.assert(assertion, message, ...args);
      } catch (err) {
        console.error(err.stack);
      }
    },
    configurable: true,
    enumerable: true,
    writable: true,
  },
});

module.exports = myConsole;
```

This can then be used as a direct replacement for the built in console:

```js
const console = require('./myConsole');
console.assert(false, 'this message will print, but no error thrown');
console.log('this will also print');
```

### console.clear()
<!-- YAML
added: v8.3.0
-->

When `stdout` is a TTY, calling `console.clear()` will attempt to clear the
TTY. When `stdout` is not a TTY, this method does nothing.

*Note*: The specific operation of `console.clear()` can vary across operating
systems and terminal types. For most Linux operating systems, `console.clear()`
operates similarly to the `clear` shell command. On Windows, `console.clear()`
will clear only the output in the current terminal viewport for the Node.js
binary.

### console.count([label])
<!-- YAML
added: v8.3.0
-->

* `label` {string} The display label for the counter. Defaults to `'default'`.

Maintains an internal counter specific to `label` and outputs to `stdout` the
number of times `console.count()` has been called with the given `label`.

<!-- eslint-skip -->
```js
> console.count()
default: 1
undefined
> console.count('default')
default: 2
undefined
> console.count('abc')
abc: 1
undefined
> console.count('xyz')
xyz: 1
undefined
> console.count('abc')
abc: 2
undefined
> console.count()
default: 3
undefined
>
```

### console.countReset([label = 'default'])
<!-- YAML
added: v8.3.0
-->

* `label` {string} The display label for the counter. Defaults to `'default'`.

Resets the internal counter specific to `label`.

<!-- eslint-skip -->
```js
> console.count('abc');
abc: 1
undefined
> console.countReset('abc');
undefined
> console.count('abc');
abc: 1
undefined
>
```

### console.dir(obj[, options])
<!-- YAML
added: v0.1.101
-->
* `obj` {any}
* `options` {Object}
  * `showHidden` {boolean}
  * `depth` {number}
  * `colors` {boolean}

Uses [`util.inspect()`][] on `obj` and prints the resulting string to `stdout`.
This function bypasses any custom `inspect()` function defined on `obj`. An
optional `options` object may be passed to alter certain aspects of the
formatted string:

- `showHidden` - if `true` then the object's non-enumerable and symbol
properties will be shown too. Defaults to `false`.

- `depth` - tells [`util.inspect()`][] how many times to recurse while
formatting the object. This is useful for inspecting large complicated objects.
Defaults to `2`. To make it recurse indefinitely, pass `null`.

- `colors` - if `true`, then the output will be styled with ANSI color codes.
Defaults to `false`. Colors are customizable; see
[customizing `util.inspect()` colors][].

### console.error([data][, ...args])
<!-- YAML
added: v0.1.100
-->
* `data` {any}
* `...args` {any}

Prints to `stderr` with newline. Multiple arguments can be passed, with the
first used as the primary message and all additional used as substitution
values similar to printf(3) (the arguments are all passed to
[`util.format()`][]).

```js
const code = 5;
console.error('error #%d', code);
// Prints: error #5, to stderr
console.error('error', code);
// Prints: error 5, to stderr
```

If formatting elements (e.g. `%d`) are not found in the first string then
[`util.inspect()`][] is called on each argument and the resulting string
values are concatenated. See [`util.format()`][] for more information.

### console.group([...label])
<!-- YAML
added: REPLACEME
-->

* `label` {any}

Increases indentation of subsequent lines by two spaces.

If one or more `label`s are provided, those are printed first without the
additional indentation.

### console.groupCollapsed()
<!-- YAML
  added: REPLACEME
-->

An alias for [`console.group()`][].

### console.groupEnd()
<!-- YAML
added: REPLACEME
-->

Decreases indentation of subsequent lines by two spaces.

### console.info([data][, ...args])
<!-- YAML
added: v0.1.100
-->
* `data` {any}
* `...args` {any}

The `console.info()` function is an alias for [`console.log()`][].

### console.log([data][, ...args])
<!-- YAML
added: v0.1.100
-->
* `data` {any}
* `...args` {any}

Prints to `stdout` with newline. Multiple arguments can be passed, with the
first used as the primary message and all additional used as substitution
values similar to printf(3) (the arguments are all passed to
[`util.format()`][]).

```js
const count = 5;
console.log('count: %d', count);
// Prints: count: 5, to stdout
console.log('count:', count);
// Prints: count: 5, to stdout
```

See [`util.format()`][] for more information.

### console.time(label)
<!-- YAML
added: v0.1.104
-->
* `label` {string} Defaults to `'default'`.

Starts a timer that can be used to compute the duration of an operation. Timers
are identified by a unique `label`. Use the same `label` when calling
[`console.timeEnd()`][] to stop the timer and output the elapsed time in
milliseconds to `stdout`. Timer durations are accurate to the sub-millisecond.

### console.timeEnd(label)
<!-- YAML
added: v0.1.104
changes:
  - version: v6.0.0
    pr-url: https://github.com/nodejs/node/pull/5901
    description: This method no longer supports multiple calls that don’t map
                 to individual `console.time()` calls; see below for details.
-->
* `label` {string} Defaults to `'default'`.

Stops a timer that was previously started by calling [`console.time()`][] and
prints the result to `stdout`:

```js
console.time('100-elements');
for (let i = 0; i < 100; i++) {}
console.timeEnd('100-elements');
// prints 100-elements: 225.438ms
```

*Note*: As of Node.js v6.0.0, `console.timeEnd()` deletes the timer to avoid
leaking it. On older versions, the timer persisted. This allowed
`console.timeEnd()` to be called multiple times for the same label. This
functionality was unintended and is no longer supported.

### console.trace([message][, ...args])
<!-- YAML
added: v0.1.104
-->
* `message` {any}
* `...args` {any}

Prints to `stderr` the string `'Trace :'`, followed by the [`util.format()`][]
formatted message and stack trace to the current position in the code.

```js
console.trace('Show me');
// Prints: (stack trace will vary based on where trace is called)
//  Trace: Show me
//    at repl:2:9
//    at REPLServer.defaultEval (repl.js:248:27)
//    at bound (domain.js:287:14)
//    at REPLServer.runBound [as eval] (domain.js:300:12)
//    at REPLServer.<anonymous> (repl.js:412:12)
//    at emitOne (events.js:82:20)
//    at REPLServer.emit (events.js:169:7)
//    at REPLServer.Interface._onLine (readline.js:210:10)
//    at REPLServer.Interface._line (readline.js:549:8)
//    at REPLServer.Interface._ttyWrite (readline.js:826:14)
```

### console.warn([data][, ...args])
<!-- YAML
added: v0.1.100
-->
* `data` {any}
* `...args` {any}

The `console.warn()` function is an alias for [`console.error()`][].

[`console.error()`]: #console_console_error_data_args
[`console.group()`]: #console_console_group_label
[`console.log()`]: #console_console_log_data_args
[`console.time()`]: #console_console_time_label
[`console.timeEnd()`]: #console_console_timeend_label
[`process.stderr`]: process.html#process_process_stderr
[`process.stdout`]: process.html#process_process_stdout
[`util.format()`]: util.html#util_util_format_format_args
[`util.inspect()`]: util.html#util_util_inspect_object_options
[customizing `util.inspect()` colors]: util.html#util_customizing_util_inspect_colors
[note on process I/O]: process.html#process_a_note_on_process_i_o
[web-api-assert]: https://developer.mozilla.org/en-US/docs/Web/API/console/assert
