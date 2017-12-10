'use strict';
const common = require('../common');
const path = require('path');
const { Worker } = require('worker');

common.crashOnUnhandledRejection();

exports.runTestInsideWorker = function(testFilePath, workerData) {
  return new Promise(function(resolve, reject) {
    const worker = new Worker(path.resolve(__dirname, '../../', testFilePath), {
      keepAlive: false,
      workerData
    });
    worker.on('exit', function(exitCode) {
      if (exitCode === 0)
        resolve();
      else
        reject(new Error(`${testFilePath} exited with code ${exitCode}`));
    });

    worker.on('error', function(e) {
      reject(new Error(`Running ${testFilePath} inside worker failed:\n` +
                       e/*.stack*/));
    });
  });
};

exports.runTestsInsideWorker = async function(tests) {
  const parallelism = +process.env.JOBS || require('os').cpus().length;
  const testsPerThread = Math.ceil(tests.length / parallelism);
  await Promise.all([...Array(parallelism).keys()].map(async (i) => {
    const shareOfTests = tests.slice(i * testsPerThread,
                                     (i + 1) * testsPerThread);
    for (const testFile of shareOfTests) {
      await exports.runTestInsideWorker(testFile, { testThreadId: i });
    }
  }));
};
