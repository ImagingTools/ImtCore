// Captures browser-side console/JS errors so a QML crash isn't silently invisible behind a passing
// screenshot assertion. `pageerror` (uncaught exception) fails the test; console.error/warn text is
// only collected and attached to the report, since this codebase logs legitimate expected warnings.

/**
 * Start capturing console/page errors on a page. Returns a `finish(testInfo)` function - call it once
 * you're done observing to attach collected console text and raise any uncaught page error.
 *
 * @param {import('@playwright/test').Page} page
 * @param {{ onPageError?: (err: Error) => void }} [opts]  onPageError defaults to collecting the error
 *   for finish() to throw. A shared page spanning many tests should pass a custom handler.
 * @returns {(testInfo: import('@playwright/test').TestInfo) => Promise<void>}
 */
function captureConsoleErrors(page, opts = {}) {
  const consoleIssues = [];
  const pageErrors = [];
  // Throwing from a pageerror listener escapes as a worker-level uncaught exception; collect and let
  // finish() throw instead.
  const onPageError = opts.onPageError || ((err) => pageErrors.push(err));

  const consoleListener = (msg) => {
    const type = msg.type();
    if (type === 'error' || type === 'warning') {
      consoleIssues.push(`[${type}] ${msg.text()}`);
    }
  };
  const pageErrorListener = (err) => {
    onPageError(err);
  };

  page.on('console', consoleListener);
  page.on('pageerror', pageErrorListener);

  return async function finish(testInfo) {
    page.off('console', consoleListener);
    page.off('pageerror', pageErrorListener);
    if (consoleIssues.length > 0 && testInfo) {
      await testInfo.attach('browser-console', {
        body: consoleIssues.join('\n'),
        contentType: 'text/plain',
      });
    }
    if (pageErrors.length > 0) {
      throw new Error(
        `browser raised ${pageErrors.length} uncaught page error(s):\n` +
          pageErrors.map((err) => `  ${(err && err.stack) || err}`).join('\n')
      );
    }
  };
}

module.exports = { captureConsoleErrors };
