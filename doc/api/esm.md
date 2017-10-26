# ECMAScript Modules

<!--introduced_in=v9.x.x-->

> Stability: 1 - Experimental

<!--name=esm-->

Node.js contains support for ES Modules based upon the
[Node.js EP for ES Modules][].

Not all features of the EP are complete and will be landing as both VM support
and implementation is ready. Error messages are still being polished.

## Enabling

<!-- type=misc -->

The `--experimental-modules` flag can be used to enable features for loading
ESM modules.

Once this has been set, files ending with `.mjs` will be able to be loaded
as ES Modules.

```sh
node --experimental-modules my-app.mjs
```

## Features

<!-- type=misc -->

### Supported

Only the CLI argument for the main entry point to the program can be an entry
point into an ESM graph. In the future `import()` can be used to create entry
points into ESM graphs at run time.

### Unsupported

| Feature | Reason |
| --- | --- |
| `require('./foo.mjs')` | ES Modules have differing resolution and timing, use language standard `import()` |
| `import()` | pending newer V8 release used in Node.js |
| `import.meta` | pending V8 implementation |

## Notable differences between `import` and `require`

### No NODE_PATH

`NODE_PATH` is not part of resolving `import` specifiers. Please use symlinks
if this behavior is desired.

### No `require.extensions`

`require.extensions` is not used by `import`. The expectation is that loader
hooks can provide this workflow in the future.

### No `require.cache`

`require.cache` is not used by `import`. It has a separate cache.

### URL based paths

ESM are resolved and cached based upon [URL](https://url.spec.whatwg.org/)
semantics. This means that files containing special characters such as `#` and
`?` need to be escaped.

Modules will be loaded multiple times if the `import` specifier used to resolve
them have a different query or fragment.

```js
import './foo?query=1'; // loads ./foo with query of "?query=1"
import './foo?query=2'; // loads ./foo with query of "?query=2"
```

For now, only modules using the `file:` protocol can be loaded.

## Interop with existing modules

All CommonJS, JSON, and C++ modules can be used with `import`.

Modules loaded this way will only be loaded once, even if their query
or fragment string differs between `import` statements.

When loaded via `import` these modules will provide a single `default` export
representing the value of `module.exports` at the time they finished evaluating.

```js
import fs from 'fs';
fs.readFile('./foo.txt', (err, body) => {
  if (err) {
    console.error(err);
  } else {
    console.log(body);
  }
});
```

## Loader hooks

<!-- type=misc -->

To customize the default module resolution, loader hooks can optionally be
provided via a `--loader ./loader-name.mjs` argument to Node.

When hooks are used they only apply to ES module loading and not to any
CommonJS modules loaded.

### Resolve hook

The resolve hook returns the resolved file URL and module format for a
given module specifier and parent file URL:

```js
import url from 'url';

export async function resolve(specifier, parentModuleURL, defaultResolver) {
  return {
    url: new URL(specifier, parentModuleURL).href,
    format: 'esm'
  };
}
```

The default NodeJS ES module resolution function is provided as a third
argument to the resolver for easy compatibility workflows.

In addition to returning the resolved file URL value, the resolve hook also
returns a `format` property specifying the module format of the resolved
module. This can be one of `"esm"`, `"cjs"`, `"json"`, `"builtin"` or
`"addon"`.

For example a dummy loader to load JavaScript restricted to browser resolution
rules with only JS file extension and Node builtin modules support could
be written:

```js
import url from 'url';
import path from 'path';
import process from 'process';

const builtins = new Set(
  Object.keys(process.binding('natives')).filter((str) =>
    /^(?!(?:internal|node|v8)\/)/.test(str))
);
const JS_EXTENSIONS = new Set(['.js', '.mjs']);

export function resolve(specifier, parentModuleURL/*, defaultResolve */) {
  if (builtins.has(specifier)) {
    return {
      url: specifier,
      format: 'builtin'
    };
  }
  if (/^\.{0,2}[/]/.test(specifier) !== true && !specifier.startsWith('file:')) {
    // For node_modules support:
    // return defaultResolve(specifier, parentModuleURL);
    throw new Error(
      `imports must begin with '/', './', or '../'; '${specifier}' does not`);
  }
  const resolved = new url.URL(specifier, parentModuleURL);
  const ext = path.extname(resolved.pathname);
  if (!JS_EXTENSIONS.has(ext)) {
    throw new Error(
      `Cannot load file with non-JavaScript file extension ${ext}.`);
  }
  return {
    url: resolved.href,
    format: 'esm'
  };
}
```

With this loader, running:

```console
NODE_OPTIONS='--experimental-modules --loader ./custom-loader.mjs' node x.js
```

would load the module `x.js` as an ES module with relative resolution support
(with `node_modules` loading skipped in this example).

### Dynamic instantiate hook

To create a custom dynamic module that doesn't correspond to one of the
existing `format` interpretations, the `dynamicInstantiate` hook can be used.
This hook is called only for modules that return `format: "dynamic"` from
the `resolve` hook.

```js
export async function dynamicInstantiate(url) {
  return {
    exports: ['customExportName'],
    execute: (exports) => {
      // get and set functions provided for pre-allocated export names
      exports.customExportName.set('value');
    }
  };
}
```

With the list of module exports provided upfront, the `execute` function will
then be called at the exact point of module evalutation order for that module
in the import tree.

[Node.js EP for ES Modules]: https://github.com/nodejs/node-eps/blob/master/002-es-modules.md
