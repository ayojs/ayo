'use strict';
const common = require('../common');
if (!common.hasCrypto)
  common.skip('missing crypto');

const assert = require('assert');
const fs = require('fs');
const path = require('path');
const exec = require('child_process').exec;
const crypto = require('crypto');
const fixtures = require('../common/fixtures');

// Test certificates
const certPem = fixtures.readSync('test_cert.pem', 'ascii');
const keyPem = fixtures.readSync('test_key.pem', 'ascii');
const modSize = 1024;

// Test signing and verifying
{
  const s1 = crypto.createSign('SHA1')
                   .update('Test123')
                   .sign(keyPem, 'base64');
  let s1stream = crypto.createSign('SHA1');
  s1stream.end('Test123');
  s1stream = s1stream.sign(keyPem, 'base64');
  assert.strictEqual(s1, s1stream, `${s1} should equal ${s1stream}`);

  const verified = crypto.createVerify('SHA1')
                         .update('Test')
                         .update('123')
                         .verify(certPem, s1, 'base64');
  assert.strictEqual(verified, true, 'sign and verify (base 64)');
}

{
  const s2 = crypto.createSign('SHA256')
                   .update('Test123')
                   .sign(keyPem, 'latin1');
  let s2stream = crypto.createSign('SHA256');
  s2stream.end('Test123');
  s2stream = s2stream.sign(keyPem, 'latin1');
  assert.strictEqual(s2, s2stream, `${s2} should equal ${s2stream}`);

  let verified = crypto.createVerify('SHA256')
                       .update('Test')
                       .update('123')
                       .verify(certPem, s2, 'latin1');
  assert.strictEqual(verified, true, 'sign and verify (latin1)');

  const verStream = crypto.createVerify('SHA256');
  verStream.write('Tes');
  verStream.write('t12');
  verStream.end('3');
  verified = verStream.verify(certPem, s2, 'latin1');
  assert.strictEqual(verified, true, 'sign and verify (stream)');
}

{
  const s3 = crypto.createSign('SHA1')
                   .update('Test123')
                   .sign(keyPem, 'buffer');
  let verified = crypto.createVerify('SHA1')
                       .update('Test')
                       .update('123')
                       .verify(certPem, s3);
  assert.strictEqual(verified, true, 'sign and verify (buffer)');

  const verStream = crypto.createVerify('SHA1');
  verStream.write('Tes');
  verStream.write('t12');
  verStream.end('3');
  verified = verStream.verify(certPem, s3);
  assert.strictEqual(verified, true, 'sign and verify (stream)');
}

// Special tests for RSA_PKCS1_PSS_PADDING
{
  function testPSS(algo, hLen) {
    // Maximum permissible salt length
    const max = modSize / 8 - hLen - 2;

    function getEffectiveSaltLength(saltLength) {
      switch (saltLength) {
        case crypto.constants.RSA_PSS_SALTLEN_DIGEST:
          return hLen;
        case crypto.constants.RSA_PSS_SALTLEN_MAX_SIGN:
          return max;
        default:
          return saltLength;
      }
    }

    const signSaltLengths = [
      crypto.constants.RSA_PSS_SALTLEN_DIGEST,
      getEffectiveSaltLength(crypto.constants.RSA_PSS_SALTLEN_DIGEST),
      crypto.constants.RSA_PSS_SALTLEN_MAX_SIGN,
      getEffectiveSaltLength(crypto.constants.RSA_PSS_SALTLEN_MAX_SIGN),
      0, 16, 32, 64, 128
    ];

    const verifySaltLengths = [
      crypto.constants.RSA_PSS_SALTLEN_DIGEST,
      getEffectiveSaltLength(crypto.constants.RSA_PSS_SALTLEN_DIGEST),
      getEffectiveSaltLength(crypto.constants.RSA_PSS_SALTLEN_MAX_SIGN),
      0, 16, 32, 64, 128
    ];
    const errMessage = /^Error:.*data too large for key size$/;

    signSaltLengths.forEach((signSaltLength) => {
      if (signSaltLength > max) {
        // If the salt length is too big, an Error should be thrown
        assert.throws(() => {
          crypto.createSign(algo)
            .update('Test123')
            .sign({
              key: keyPem,
              padding: crypto.constants.RSA_PKCS1_PSS_PADDING,
              saltLength: signSaltLength
            });
        }, errMessage);
      } else {
        // Otherwise, a valid signature should be generated
        const s4 = crypto.createSign(algo)
                         .update('Test123')
                         .sign({
                           key: keyPem,
                           padding: crypto.constants.RSA_PKCS1_PSS_PADDING,
                           saltLength: signSaltLength
                         });

        let verified;
        verifySaltLengths.forEach((verifySaltLength) => {
          // Verification should succeed if and only if the salt length is
          // correct
          verified = crypto.createVerify(algo)
                           .update('Test123')
                           .verify({
                             key: certPem,
                             padding: crypto.constants.RSA_PKCS1_PSS_PADDING,
                             saltLength: verifySaltLength
                           }, s4);
          const saltLengthCorrect = getEffectiveSaltLength(signSaltLength) ===
                                    getEffectiveSaltLength(verifySaltLength);
          assert.strictEqual(verified, saltLengthCorrect, 'verify (PSS)');
        });

        // Verification using RSA_PSS_SALTLEN_AUTO should always work
        verified = crypto.createVerify(algo)
                         .update('Test123')
                         .verify({
                           key: certPem,
                           padding: crypto.constants.RSA_PKCS1_PSS_PADDING,
                           saltLength: crypto.constants.RSA_PSS_SALTLEN_AUTO
                         }, s4);
        assert.strictEqual(verified, true, 'verify (PSS with SALTLEN_AUTO)');

        // Verifying an incorrect message should never work
        verified = crypto.createVerify(algo)
                         .update('Test1234')
                         .verify({
                           key: certPem,
                           padding: crypto.constants.RSA_PKCS1_PSS_PADDING,
                           saltLength: crypto.constants.RSA_PSS_SALTLEN_AUTO
                         }, s4);
        assert.strictEqual(verified, false, 'verify (PSS, incorrect)');
      }
    });
  }

  testPSS('SHA1', 20);
  testPSS('SHA256', 32);
}

// Test vectors for RSA_PKCS1_PSS_PADDING provided by the RSA Laboratories:
// https://www.emc.com/emc-plus/rsa-labs/standards-initiatives/pkcs-rsa-cryptography-standard.htm
{
  // We only test verification as we cannot specify explicit salts when signing
  function testVerify(cert, vector) {
    const verified = crypto.createVerify('SHA1')
                          .update(Buffer.from(vector.message, 'hex'))
                          .verify({
                            key: cert,
                            padding: crypto.constants.RSA_PKCS1_PSS_PADDING,
                            saltLength: vector.salt.length / 2
                          }, vector.signature, 'hex');
    assert.strictEqual(verified, true, 'verify (PSS)');
  }

  const examples = JSON.parse(fixtures.readSync('pss-vectors.json', 'utf8'));

  for (const key in examples) {
    const example = examples[key];
    const publicKey = example.publicKey.join('\n');
    example.tests.forEach((test) => testVerify(publicKey, test));
  }
}

// Test exceptions for invalid `padding` and `saltLength` values
{
  [null, undefined, NaN, 'boom', {}, [], true, false]
    .forEach((invalidValue) => {
      common.expectsError(() => {
        crypto.createSign('SHA256')
          .update('Test123')
          .sign({
            key: keyPem,
            padding: invalidValue
          });
      }, {
        code: 'ERR_INVALID_OPT_VALUE',
        type: TypeError
      });

      common.expectsError(() => {
        crypto.createSign('SHA256')
          .update('Test123')
          .sign({
            key: keyPem,
            padding: crypto.constants.RSA_PKCS1_PSS_PADDING,
            saltLength: invalidValue
          });
      }, {
        code: 'ERR_INVALID_OPT_VALUE',
        type: TypeError
      });
    });

  assert.throws(() => {
    crypto.createSign('SHA1')
      .update('Test123')
      .sign({
        key: keyPem,
        padding: crypto.constants.RSA_PKCS1_OAEP_PADDING
      });
  }, /^Error:.*illegal or unsupported padding mode$/);
}

// Test throws exception when key options is null
{
  common.expectsError(() => {
    crypto.createSign('SHA1').update('Test123').sign(null, 'base64');
  }, {
    code: 'ERR_CRYPTO_SIGN_KEY_REQUIRED',
    type: Error
  });
}

// RSA-PSS Sign test by verifying with 'openssl dgst -verify'
{
  if (!common.opensslCli)
    common.skip('node compiled without OpenSSL CLI.');

  const pubfile = fixtures.path('keys', 'rsa_public_2048.pem');
  const privkey = fixtures.readKey('rsa_private_2048.pem');

  const msg = 'Test123';
  const s5 = crypto.createSign('SHA256')
    .update(msg)
    .sign({
      key: privkey,
      padding: crypto.constants.RSA_PKCS1_PSS_PADDING
    });

  common.refreshTmpDir();

  const sigfile = path.join(common.tmpDir, 's5.sig');
  fs.writeFileSync(sigfile, s5);
  const msgfile = path.join(common.tmpDir, 's5.msg');
  fs.writeFileSync(msgfile, msg);

  const cmd =
    `"${common.opensslCli}" dgst -sha256 -verify "${pubfile}" -signature "${
      sigfile}" -sigopt rsa_padding_mode:pss -sigopt rsa_pss_saltlen:-2 "${
      msgfile}"`;

  exec(cmd, common.mustCall((err, stdout, stderr) => {
    assert(stdout.includes('Verified OK'));
  }));
}

[1, [], {}, undefined, null, true, Infinity].forEach((i) => {
  common.expectsError(
    () => crypto.createSign(),
    {
      code: 'ERR_INVALID_ARG_TYPE',
      type: TypeError,
      message: 'The "algorithm" argument must be of type string'
    }
  );
  common.expectsError(
    () => crypto.createVerify(),
    {
      code: 'ERR_INVALID_ARG_TYPE',
      type: TypeError,
      message: 'The "algorithm" argument must be of type string'
    }
  );
});

{
  const sign = crypto.createSign('SHA1');
  const verify = crypto.createVerify('SHA1');

  [1, [], {}, undefined, null, true, Infinity].forEach((i) => {
    common.expectsError(
      () => sign.update(i),
      {
        code: 'ERR_INVALID_ARG_TYPE',
        type: TypeError,
        message: 'The "data" argument must be one of type string, Buffer, ' +
                 'TypedArray, or DataView'
      }
    );
    common.expectsError(
      () => verify.update(i),
      {
        code: 'ERR_INVALID_ARG_TYPE',
        type: TypeError,
        message: 'The "data" argument must be one of type string, Buffer, ' +
                 'TypedArray, or DataView'
      }
    );
    common.expectsError(
      () => sign._write(i, 'utf8', () => {}),
      {
        code: 'ERR_INVALID_ARG_TYPE',
        type: TypeError,
        message: 'The "data" argument must be one of type string, Buffer, ' +
                 'TypedArray, or DataView'
      }
    );
    common.expectsError(
      () => verify._write(i, 'utf8', () => {}),
      {
        code: 'ERR_INVALID_ARG_TYPE',
        type: TypeError,
        message: 'The "data" argument must be one of type string, Buffer, ' +
                 'TypedArray, or DataView'
      }
    );
  });

  [
    Uint8Array, Uint16Array, Uint32Array, Float32Array, Float64Array
  ].forEach((i) => {
    // These should all just work
    sign.update(new i());
    verify.update(new i());
  });

  [1, {}, [], Infinity].forEach((i) => {
    common.expectsError(
      () => sign.sign(i),
      {
        code: 'ERR_INVALID_ARG_TYPE',
        type: TypeError,
        message: 'The "key" argument must be one of type string, Buffer, ' +
                 'TypedArray, or DataView'
      }
    );

    common.expectsError(
      () => verify.verify(i),
      {
        code: 'ERR_INVALID_ARG_TYPE',
        type: TypeError,
        message: 'The "key" argument must be one of type string, Buffer, ' +
                 'TypedArray, or DataView'
      }
    );

    common.expectsError(
      () => verify.verify('test', i),
      {
        code: 'ERR_INVALID_ARG_TYPE',
        type: TypeError,
        message: 'The "signature" argument must be one of type string, ' +
                 'Buffer, TypedArray, or DataView'
      }
    );
  });
}
