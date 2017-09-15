'use strict';

const { ModuleWrap } = process.binding('module_wrap');
const debug = require('util').debuglog('esm');
const ArrayJoin = Function.call.bind(Array.prototype.join);
const ArrayMap = Function.call.bind(Array.prototype.map);

const getNamespaceOfModuleWrap = (m) => {
  const tmp = new ModuleWrap('import * as _ from "";_;', '');
  tmp.link(async () => m);
  tmp.instantiate();
  return tmp.evaluate();
};

const createDynamicModule = (exports, url = '', evaluate) => {
  debug(
    `creating ESM facade for ${url} with exports: ${ArrayJoin(exports, ', ')}`
  );
  const names = ArrayMap(exports, (name) => `${name}`);
  // sanitized ESM for reflection purposes
  const src = `export let executor;
  ${ArrayJoin(ArrayMap(names, (name) => `export let $${name}`), ';\n')}
  ;(() => [
    fn => executor = fn,
    { exports: { ${
  ArrayJoin(ArrayMap(names, (name) => `${name}: {
        get: () => $${name},
        set: v => $${name} = v
      }`), ',\n')
} } }
  ]);
  `;
  const reflectiveModule = new ModuleWrap(src, `cjs-facade:${url}`);
  reflectiveModule.instantiate();
  const [setExecutor, reflect] = reflectiveModule.evaluate()();
  // public exposed ESM
  const reexports = `import { executor,
    ${ArrayMap(names, (name) => `$${name}`)}
  } from "";
  export {
    ${ArrayJoin(ArrayMap(names, (name) => `$${name} as ${name}`), ', ')}
  }
  // add await to this later if top level await comes along
  typeof executor === "function" ? executor() : void 0;`;
  if (typeof evaluate === 'function') {
    setExecutor(() => evaluate(reflect));
  }
  const runner = new ModuleWrap(reexports, `${url}`);
  runner.link(async () => reflectiveModule);
  runner.instantiate();
  return {
    module: runner,
    reflect
  };
};

const REFLECT = Reflect;
const OBJECT = Object;
const SET = Set;
const PROXY = Proxy;
const TYPE_ERROR = TypeError;
// Create a Proxy with the following invariants applied to the property keys in exportNames
// 1. no getter/setters are allowed
// 2. no properties may be added/removed
// 3. the Proxy acts as if the prototype is `null`
const createExportInvariantProxy = function ({
  // TARGET SHOULD NOT BE ACCESSIBLE EXCEPT THROUGH THIS PROXY
  target,
  exportNames,
  update
}) {
  const bindings = new SET();
  for (const name of exportNames) {
    if (typeof name !== 'string') {
      throw TYPE_ERROR(`exportNames must only contain strings`);
    }
    bindings.add(name);
  }
  for (const binding of bindings) {
    const desc = REFLECT.getOwnPropertyDescriptor(target, binding);
    if (!desc) {
      throw TYPE_ERROR(`cannot setup invariants on missing property ${binding}`)
    }
    else {
      let valueDescriptor = REFLECT.getOwnPropertyDescriptor(desc, 'value');
      if (!valueDescriptor) {
        throw TYPE_ERROR(`cannot setup invariants on getter/setter for ${binding}.`);
      }
    }
  }
  const handlers = {
    defineProperty(target, property, descriptor) {
      if (bindings.has(property)) {
        let valueDescriptor = REFLECT.getOwnPropertyDescriptor(descriptor, 'value');
        if (valueDescriptor && REFLECT.defineProperty(target, property, descriptor)) {
          update(property, valueDescriptor.value);
          return true;
        }
        return false;
      }
      return REFLECT.defineProperty(target, property, descriptor);
    },
    deleteProperty(target, property) {
      if (bindings.has(property)) return false;
      return REFLECT.deleteProperty(target, property);
    },
    getPrototypeOf(target) {
      return null;
    },
    get(target, property, receiver) {
      // change to receiver to target, internal field goofiness might be going on
      return REFLECT.get(target, property, target);
    },
    setPrototypeOf(target, prototype) {
      return false;
    }
  }
  const proxy = new PROXY(target, handlers);
  return proxy;
}

const createInvariantModule = (target, exportNames, url, evaluate) => {
  const {module, reflect} = createDynamicModule(exportNames, url, evaluate);
  const proxy = createExportInvariantProxy({
    target,
    exportNames,
    update: (name, value) => {
      reflect.exports[name].set(value)
    }
  });
  for (const name of exportNames) {
    Object.defineProperty(proxy, name, Object.getOwnPropertyDescriptor(target, name));
  }
  return {module, target: proxy};
};

module.exports = {
  createInvariantModule,
  createDynamicModule,
  getNamespaceOfModuleWrap,
  ModuleWrap
};
