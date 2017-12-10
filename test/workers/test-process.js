'use strict';

require('../common');

require('./index.js').runTestsInsideWorker([
  'test/parallel/test-process-argv-0.js',
  'test/parallel/test-process-assert.js',
  'test/parallel/test-process-beforeexit.js',
  'test/parallel/test-process-binding.js',
  'test/parallel/test-process-config.js',
  'test/parallel/test-process-constants-noatime.js',
  'test/parallel/test-process-cpuUsage.js',
  'test/parallel/test-process-emit.js',
  'test/parallel/test-process-exec-argv.js',
  'test/parallel/test-process-execpath.js',
  'test/parallel/test-process-exit-code.js',
  'test/parallel/test-process-exit-from-before-exit.js',
  'test/parallel/test-process-exit.js',
  'test/parallel/test-process-exit-recursive.js',
  'test/parallel/test-process-external-stdio-close.js',
  'test/parallel/test-process-external-stdio-close-spawn.js',
  'test/parallel/test-process-getactivehandles.js',
  'test/parallel/test-process-getactiverequests.js',
  'test/parallel/test-process-hrtime.js',
  'test/parallel/test-process-kill-null.js',
  'test/parallel/test-process-kill-pid.js',
  'test/parallel/test-process-no-deprecation.js',
  'test/parallel/test-process-prototype.js',
  'test/parallel/test-process-raw-debug.js',
  'test/parallel/test-process-redirect-warnings-env.js',
  'test/parallel/test-process-redirect-warnings.js',
  'test/parallel/test-process-remove-all-signal-listeners.js',
  'test/parallel/test-process-versions.js',
  'test/parallel/test-process-wrap.js'
]);
