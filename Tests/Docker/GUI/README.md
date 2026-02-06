# GUI Test Utilities

This directory contains utilities and helpers for GUI testing with Playwright.

## Files

### utils.js

Common Playwright test utilities that are automatically available in the test container at `/app/tests/GUI/utils.js`.

#### Available Functions

- `login(page, username, password)` - Automated login flow
- `clickOnElement(page, selector)` - Click on element with wait
- `clickOnPage(page, x, y)` - Click at coordinates
- `clickOnCommand(page, command)` - Execute command via click
- `fillTextInput(page, selector, text)` - Fill text input
- `selectComboBox(page, selector, value)` - Select from dropdown
- `waitForPageStability(page)` - Wait for DOM to stabilize
- `checkScreenshot(page, name, options)` - Screenshot comparison with masking
- `reloadPage(page)` - Reload page and wait
- `delay(ms)` - Sleep utility

#### Usage in Tests

```javascript
const { login, checkScreenshot, waitForPageStability } = require('../GUI/utils.js');

test('login test', async ({ page }) => {
  await login(page, 'user@example.com', 'password');
  await waitForPageStability(page);
  await checkScreenshot(page, 'dashboard');
});
```

### playwright.config.js

Default Playwright configuration file that is automatically copied to `/app/tests/playwright.config.js` in the container.

This configuration:
- Sets test directory to `./GUI`
- Configures timeouts and retries
- Sets up reporters (HTML, JSON, JUnit)
- Defines browser projects
- Uses environment variables for configuration

Applications can override this by copying their own `playwright.config.js` file to the container.

## Adding More Utilities

You can add more GUI test utilities to this directory. They will be available to all Playwright tests running in the container.
