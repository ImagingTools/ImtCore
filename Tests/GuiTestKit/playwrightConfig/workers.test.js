const assert = require('node:assert/strict');
const test = require('node:test');

const { computeWorkers, machineWorkers, GB } = require('./workers');

// Machine profiles the suites actually run on, from a small build agent to a workstation.
const PROFILES = [
  { name: 'small agent: 4 cores, 8 GB (5 free)', cpus: 4, freeMemory: 5 * GB, expected: 1 },
  { name: 'agent: 8 cores, 16 GB (10 free)', cpus: 8, freeMemory: 10 * GB, expected: 2 },
  { name: 'workstation: 20 cores, 32 GB (20.5 free)', cpus: 20, freeMemory: 20.5 * GB, expected: 5 },
  { name: 'memory-starved: 32 cores, 6 GB free', cpus: 32, freeMemory: 6 * GB, expected: 1 },
  { name: 'big server: 64 cores, 128 GB (100 free)', cpus: 64, freeMemory: 100 * GB, expected: 10 },
  { name: 'cpu-bound: 12 cores, 64 GB (60 free)', cpus: 12, freeMemory: 60 * GB, expected: 3 },
];

for (const p of PROFILES) {
  test(`workers for ${p.name}`, () => {
    assert.equal(computeWorkers({ cpus: p.cpus, freeMemory: p.freeMemory }).workers, p.expected);
  });
}

test('never goes below one worker, even with less free memory than the reserve', () => {
  assert.equal(computeWorkers({ cpus: 2, freeMemory: 1 * GB }).workers, 1);
});

test('PLAYWRIGHT_WORKERS overrides the calculation', () => {
  const result = computeWorkers({ cpus: 64, freeMemory: 100 * GB, env: { PLAYWRIGHT_WORKERS: '3' } });
  assert.equal(result.workers, 3);
  assert.match(result.reason, /PLAYWRIGHT_WORKERS=3/);
});

test('an empty PLAYWRIGHT_WORKERS falls back to the calculation', () => {
  assert.equal(computeWorkers({ cpus: 8, freeMemory: 10 * GB, env: { PLAYWRIGHT_WORKERS: '' } }).workers, 2);
});

test('rejects a PLAYWRIGHT_WORKERS that is not a positive integer', () => {
  for (const bad of ['0', '-2', '2.5', 'many']) {
    assert.throws(() => computeWorkers({ cpus: 8, freeMemory: 10 * GB, env: { PLAYWRIGHT_WORKERS: bad } }), /positive integer/);
  }
});

test('the reason names both limits, so a run log shows why it got its worker count', () => {
  const { reason } = computeWorkers({ cpus: 20, freeMemory: 20.5 * GB });
  assert.match(reason, /20 logical cores -> 5/);
  assert.match(reason, /20\.5 GB free -> 11/);
});

test('works on the machine running the tests', () => {
  const { workers } = machineWorkers({});
  assert.ok(Number.isInteger(workers) && workers >= 1 && workers <= 10);
});
