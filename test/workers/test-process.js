'use strict';

require('../common');

require('./index.js').runTestsInsideWorker([
  'test/parallel/test-process-argv-0.js',
  'test/parallel/test-process-binding.js',
  'test/parallel/test-process-config.js',
  // Only main thread can mutate process environment.
  // 'test/parallel/test-process-env.js',
  'test/parallel/test-process-exec-argv.js',
  'test/parallel/test-process-exit-code.js',
  'test/parallel/test-process-exit-from-before-exit.js',
  'test/parallel/test-process-exit.js',
  'test/parallel/test-process-exit-recursive.js',
  'test/parallel/test-process-getgroups.js',
  'test/parallel/test-process-hrtime.js',
  'test/parallel/test-process-kill-null.js',
  'test/parallel/test-process-kill-pid.js',
  // Workers have a different uncaught exception mechanism.
  // 'test/parallel/test-process-next-tick.js',
  'test/parallel/test-process-raw-debug.js',
  'test/parallel/test-process-remove-all-signal-listeners.js',
  'test/parallel/test-process-versions.js',
  'test/parallel/test-process-wrap.js'
]);
