'use strict';
require('../common');

require('./index.js').runTestsInsideWorker([
  'test/parallel/test-vm-basic.js',
  'test/parallel/test-vm-cached-data.js',
  'test/parallel/test-vm-context-async-script.js',
  'test/parallel/test-vm-context.js',
  'test/parallel/test-vm-context-property-forwarding.js',
  'test/parallel/test-vm-create-context-accessors.js',
  'test/parallel/test-vm-create-context-arg.js',
  'test/parallel/test-vm-create-context-circular-reference.js',
  'test/parallel/test-vm-cross-context.js',
  'test/parallel/test-vm-debug-context.js',
  'test/parallel/test-vm-deleting-property.js',
  'test/parallel/test-vm-function-declaration.js',
  'test/parallel/test-vm-function-redefinition.js',
  'test/parallel/test-vm-global-assignment.js',
  'test/parallel/test-vm-global-define-property.js',
  'test/parallel/test-vm-global-identity.js',
  'test/parallel/test-vm-harmony-symbols.js',
  'test/parallel/test-vm-indexed-properties.js',
  'test/parallel/test-vm-is-context.js',
  'test/parallel/test-vm-low-stack-space.js',
  'test/parallel/test-vm-new-script-new-context.js',
  'test/parallel/test-vm-new-script-this-context.js',
  'test/parallel/test-vm-preserves-property.js',
  'test/parallel/test-vm-property-not-on-sandbox.js',
  'test/parallel/test-vm-proxies.js',
  'test/parallel/test-vm-sigint-existing-handler.js',
  'test/parallel/test-vm-sigint.js',
  'test/parallel/test-vm-static-this.js',
  'test/parallel/test-vm-strict-mode.js',
  'test/parallel/test-vm-symbols.js',
  'test/parallel/test-vm-syntax-error-message.js',
  'test/parallel/test-vm-syntax-error-stderr.js',
  'test/parallel/test-vm-timeout.js'
]);
