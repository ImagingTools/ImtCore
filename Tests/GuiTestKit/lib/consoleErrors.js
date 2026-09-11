// Captures browser-side console/JS errors so a QML crash isn't silently invisible behind a passing
// screenshot assertion (the app can log "Unable to edit product... This product not found!" - see
// ProductEditor.qml's started() - or throw a WASM-side exception without any of that ever surfacing in
// a Playwright assertion, since screenshots only check pixels).
//
// Two severities, deliberately NOT treated the same:
//   - `pageerror` (an uncaught JS/WASM exception) is unambiguous - nothing legitimate produces one, so
//     the default `page` fixture (see createTest.js) hard-fails the test when one occurs.
//   - `console.error`/`console.warn` text is NOT hard-failed: this codebase already has legitimate,
//     expected warn/error calls for handled edge cases (e.g. OrderEditor.qml's
//     "Unknown product type, fallback to Software" warning), so blindly failing on ANY console.error
//     would break tests that never regressed. Instead these are collected and attached to the test
//     report as a plain-text attachment - free diagnostic context you can check when a test fails for
//     another reason, without it being a NEW source of false-positive failures.

/**
 * Start capturing console/page errors on a page. Returns a `finish(testInfo)` function - call it once
 * you're done observing (end of test, or end of a describe.serial block) to attach any collected
 * console.error/warn text to the test report. `pageerror`s are NOT handled by finish() - they throw
 * immediately, synchronously with the page emitting them, so a test genuinely fails at the moment the
 * exception actually happened rather than only being flagged after the fact.
 *
 * @param {import('@playwright/test').Page} page
 * @param {{ onPageError?: (err: Error) => void }} [opts]  onPageError defaults to re-throwing, which
 *   Playwright surfaces as the test's own failure. A shared page spanning many tests (newUserPage's
 *   describe.serial pattern) can't attribute a pageerror to one specific test cleanly, so callers using
 *   that pattern should pass a custom handler (e.g. console.error to CI output) instead of letting this
 *   throw into an unrelated test.
 * @returns {(testInfo: import('@playwright/test').TestInfo) => Promise<void>}
 */
function captureConsoleErrors(page, opts = {}) {
  const consoleIssues = [];
  const onPageError = opts.onPageError || ((err) => { throw err; });

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
  };
}

module.exports = { captureConsoleErrors };
