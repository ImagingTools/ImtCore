// How many Playwright workers this machine can carry.
//
// Every worker is a Chrome instance booting the Qt/WASM client, and the same machine also runs the
// Debug test servers and PostgreSQL. Too many workers and Chrome itself stalls: measured on a 20-core,
// 32 GB box at 10 workers, browser.newContext() hung for the whole 120s test timeout in the first
// seconds of a run. A fixed number is therefore wrong for some machine either way, so it is derived from
// the machine: whichever of CPU and free memory runs out first decides.
//
// Measured once, when the config loads. Windows has no load average (os.loadavg() is always zeros), so
// load that appears after the start is not seen. PLAYWRIGHT_WORKERS always wins over the calculation.

const os = require('os');

const GB = 1024 * 1024 * 1024;

const DEFAULTS = {
  // Logical cores per worker: Chrome's renderer + the WASM compile of the client on boot.
  coresPerWorker: 4,
  // Chrome with a booted client, per worker.
  memoryPerWorker: 1.5 * GB,
  // Left free for the test servers, PostgreSQL and the OS.
  reservedMemory: 4 * GB,
  maxWorkers: 10,
};

/**
 * Pure calculation, so it can be tested for any machine without being on it.
 * @param {{cpus: number, freeMemory: number, env?: Object, limits?: Partial<typeof DEFAULTS>}} machine
 * @returns {{workers: number, reason: string}}
 */
function computeWorkers({ cpus, freeMemory, env = {}, limits = {} }) {
  const l = { ...DEFAULTS, ...limits };

  const override = env.PLAYWRIGHT_WORKERS;
  if (override !== undefined && override !== '') {
    const value = Number(override);
    if (!Number.isInteger(value) || value < 1) {
      throw new Error(`PLAYWRIGHT_WORKERS must be a positive integer, got "${override}"`);
    }
    return { workers: value, reason: `PLAYWRIGHT_WORKERS=${value}` };
  }

  const byCpu = Math.floor(cpus / l.coresPerWorker);
  const byMemory = Math.floor((freeMemory - l.reservedMemory) / l.memoryPerWorker);
  const workers = Math.max(1, Math.min(byCpu, byMemory, l.maxWorkers));
  const freeGb = (freeMemory / GB).toFixed(1);
  return {
    workers,
    reason: `${cpus} logical cores -> ${byCpu}, ${freeGb} GB free -> ${Math.max(byMemory, 0)}, cap ${l.maxWorkers}`,
  };
}

/** The calculation for the machine this process runs on. */
function machineWorkers(env = process.env) {
  return computeWorkers({ cpus: os.cpus().length, freeMemory: os.freemem(), env });
}

module.exports = { computeWorkers, machineWorkers, DEFAULTS, GB };
