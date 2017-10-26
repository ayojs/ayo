'use strict';

const common = require('../common.js');
let icu;
try {
  icu = process.binding('icu');
} catch (err) {}
const punycode = require('punycode');

const bench = common.createBenchmark(main, {
  method: ['punycode'].concat(icu !== undefined ? ['icu'] : []),
  n: [1024],
  val: [
    'افغانستا.icom.museum',
    'الجزائر.icom.museum',
    'österreich.icom.museum',
    'বাংলাদেশ.icom.museum',
    'беларусь.icom.museum',
    'belgië.icom.museum',
    'българия.icom.museum',
    'تشادر.icom.museum',
    '中国.icom.museum',
    'القمر.icom.museum',
    'κυπρος.icom.museum',
    'českárepublika.icom.museum',
    'مصر.icom.museum',
    'ελλάδα.icom.museum',
    'magyarország.icom.museum',
    'ísland.icom.museum',
    'भारत.icom.museum',
    'ايران.icom.museum',
    'éire.icom.museum',
    'איקו״ם.ישראל.museum',
    '日本.icom.museum',
    'الأردن.icom.museum'
  ]
});

function usingPunycode(val) {
  punycode.toUnicode(punycode.toASCII(val));
}

function usingICU(val) {
  icu.toUnicode(icu.toASCII(val));
}

function runPunycode(n, val) {
  var i = 0;
  for (; i < n; i++)
    usingPunycode(val);
  bench.start();
  for (i = 0; i < n; i++)
    usingPunycode(val);
  bench.end(n);
}

function runICU(n, val) {
  var i = 0;
  bench.start();
  for (; i < n; i++)
    usingICU(val);
  bench.end(n);
}

function main(conf) {
  const n = +conf.n;
  const val = conf.val;
  switch (conf.method) {
    // '' is a default case for tests
    case '':
    case 'punycode':
      runPunycode(n, val);
      break;
    case 'icu':
      if (icu !== undefined) {
        runICU(n, val);
        break;
      }
      // fallthrough
    default:
      throw new Error('Unexpected method');
  }
}
