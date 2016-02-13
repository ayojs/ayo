'use strict';

require('../common');

// When using OpenSSL this test should put the locking_callback to good use
require('./index.js').runTestsInsideWorker([
  'test/parallel/test-crypto-authenticated.js',
  'test/parallel/test-crypto-binary-default.js',
  'test/parallel/test-crypto-certificate.js',
  'test/parallel/test-crypto-cipher-decipher.js',
  'test/parallel/test-crypto-dh.js',
  'test/parallel/test-crypto-dh-odd-key.js',
  'test/parallel/test-crypto-ecb.js',
  'test/parallel/test-crypto-from-binary.js',
  'test/parallel/test-crypto-hash.js',
  'test/parallel/test-crypto-hash-stream-pipe.js',
  'test/parallel/test-crypto-hmac.js',
  'test/parallel/test-crypto.js',
  'test/parallel/test-crypto-padding-aes256.js',
  'test/parallel/test-crypto-padding.js',
  'test/parallel/test-crypto-pbkdf2.js',
  'test/parallel/test-crypto-random.js',
  'test/parallel/test-crypto-rsa-dsa.js',
  'test/parallel/test-crypto-sign-verify.js',
  'test/parallel/test-crypto-stream.js'
]);
