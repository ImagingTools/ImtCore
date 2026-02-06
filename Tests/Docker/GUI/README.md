# GUI Test Utilities

This directory contains utilities and helpers for GUI testing with Playwright.

## Files

### utils.js

Common Playwright test utilities that are automatically available in the test container at `/app/tests/GUI/utils.js`.

#### Available Functions

**Basic Utilities:**
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

**Multi-User Testing:**
- `parseTestUsers()` - Parse TEST_USERS environment variable into array
- `runWithEachUser(page, testFn, config, loginPaths)` - Run test for each user
- `getUserScreenshotName(testInfo, baseFilename)` - Get user-specific screenshot filename
- `getUserInfoFromTest(testInfo)` - Get current user info from testInfo

#### Usage in Tests

**Basic Test:**
```javascript
const { login, checkScreenshot, waitForPageStability } = require('../GUI/utils.js');

test('login test', async ({ page }) => {
  await login(page, 'user@example.com', 'password');
  await waitForPageStability(page);
  await checkScreenshot(page, 'dashboard');
});
```

**Multi-User Test with Dynamic Projects:**
```javascript
const { getUserScreenshotName, getUserInfoFromTest } = require('../GUI/utils.js');

// Test runs automatically for each user (authorized-user0, authorized-user1, etc.)
test('dashboard test', async ({ page }, testInfo) => {
  // Get current user info
  const userInfo = getUserInfoFromTest(testInfo);
  console.log(`Testing with user: ${userInfo.username}`);
  
  // Screenshots automatically include user in filename
  const screenshotName = getUserScreenshotName(testInfo, 'dashboard.png');
  await page.screenshot({ path: screenshotName });
  // Creates: dashboard-user0.png, dashboard-user1.png, etc.
});
```

### playwright.config.js

Enhanced Playwright configuration with **dynamic multi-user support**.

#### Multi-User Project Generation

The configuration automatically generates separate Playwright projects for each user defined in `TEST_USERS`:

```bash
# Single user
export TEST_USERS="admin@example.com:admin123"
# Creates: authorized-user0 project

# Multiple users  
export TEST_USERS="admin@example.com:admin123,user@example.com:user123,viewer@example.com:viewer123"
# Creates: authorized-user0, authorized-user1, authorized-user2 projects
```

#### Features

- **Dynamic Projects**: Automatically creates `authorized-user0`, `authorized-user1`, etc. based on TEST_USERS
- **Separate Auth States**: Each user gets their own `storageState-user0.json`, `storageState-user1.json`, etc.
- **User-Specific Screenshots**: Project name automatically includes user index for screenshot differentiation
- **Guest Project**: Single `guest` project for non-authenticated tests
- **Test Matching**: `authorized/*.test.js` files run for each user, others run as guest

#### Project Structure

```javascript
projects: [
  {
    name: 'authorized-user0',  // admin@example.com
    testMatch: /authorized\/.*\.test\.js/,
    use: {
      storageState: 'storageState-user0.json',
      userIndex: 0,
      username: 'admin@example.com',
      password: 'admin123',
    },
  },
  {
    name: 'authorized-user1',  // user@example.com
    testMatch: /authorized\/.*\.test\.js/,
    use: {
      storageState: 'storageState-user1.json',
      userIndex: 1,
      username: 'user@example.com',
      password: 'user123',
    },
  },
  // ... more user projects ...
  {
    name: 'guest',
    testIgnore: /authorized\/.*\.test\.js/,
    use: {},
  },
]
```

### global-setup.js

Global setup script that runs before all tests.

#### Multi-User Authentication

Automatically creates authentication state files for each user:
- Logs in each user defined in the projects
- Saves separate `storageState-user{N}.json` files
- Each authorized project loads its own auth state
- Guest project runs without authentication

Console output example:
```
Setting up authentication for 3 user(s)...
Authenticating user0: admin@example.com...
✓ Created storageState-user0.json for admin@example.com
Authenticating user1: user@example.com...
✓ Created storageState-user1.json for user@example.com
Authenticating user2: viewer@example.com...
✓ Created storageState-user2.json for viewer@example.com
Global setup complete!
```

## Adding More Utilities

You can add more GUI test utilities to this directory. They will be available to all Playwright tests running in the container.
