'use strict';

require('../common');

require('./index.js').runTestsInsideWorker([
  'test/parallel/test-zlib-bytes-read.js',
  'test/parallel/test-zlib-close-after-error.js',
  'test/parallel/test-zlib-close-after-write.js',
  'test/parallel/test-zlib-const.js',
  'test/parallel/test-zlib-convenience-methods.js',
  'test/parallel/test-zlib-create-raw.js',
  'test/parallel/test-zlib-deflate-constructors.js',
  'test/parallel/test-zlib-deflate-raw-inherits.js',
  'test/parallel/test-zlib-destroy-pipe.js',
  'test/parallel/test-zlib-dictionary-fail.js',
  'test/parallel/test-zlib-dictionary.js',
  'test/parallel/test-zlib-failed-init.js',
  'test/parallel/test-zlib-flush-drain.js',
  'test/parallel/test-zlib-flush-drain-longblock.js',
  'test/parallel/test-zlib-flush-flags.js',
  'test/parallel/test-zlib-flush.js',
  'test/parallel/test-zlib-flush-multiple-scheduled.js',
  'test/parallel/test-zlib-from-concatenated-gzip.js',
  'test/parallel/test-zlib-from-gzip.js',
  'test/parallel/test-zlib-from-gzip-with-trailing-garbage.js',
  'test/parallel/test-zlib-from-string.js',
  'test/parallel/test-zlib-invalid-input.js',
  'test/parallel/test-zlib.js',
  'test/parallel/test-zlib-not-string-or-buffer.js',
  'test/parallel/test-zlib-params.js',
  'test/parallel/test-zlib-random-byte-pipes.js',
  'test/parallel/test-zlib-sync-no-event.js',
  'test/parallel/test-zlib-truncated.js',
  'test/parallel/test-zlib-unzip-one-byte-chunks.js',
  'test/parallel/test-zlib-write-after-close.js',
  'test/parallel/test-zlib-write-after-flush.js',
  'test/parallel/test-zlib-zero-byte.js'
]);
