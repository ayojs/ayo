'use strict';

require('../common');

require('./index.js').runTestsInsideWorker([
  'test/parallel/test-zlib-close-after-write.js',
  'test/parallel/test-zlib-convenience-methods.js',
  'test/parallel/test-zlib-dictionary-fail.js',
  'test/parallel/test-zlib-dictionary.js',
  'test/parallel/test-zlib-flush.js',
  'test/parallel/test-zlib-from-gzip.js',
  'test/parallel/test-zlib-from-string.js',
  'test/parallel/test-zlib-invalid-input.js',
  'test/parallel/test-zlib.js',
  'test/parallel/test-zlib-params.js',
  'test/parallel/test-zlib-random-byte-pipes.js',
  'test/parallel/test-zlib-write-after-close.js',
  'test/parallel/test-zlib-write-after-flush.js',
  'test/parallel/test-zlib-zero-byte.js'
]);
