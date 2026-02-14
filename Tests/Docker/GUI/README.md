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

#### Multi-User Authentication Setup

**IMPORTANT:** The `global-setup.js` file in ImtCore is a **template** that requires configuration for your application's login flow.

##### How to Configure Authentication

1. **Copy the template to your application:**
   ```bash
   cp $IMTCORE_DIR/Tests/Docker/GUI/global-setup.js \
      YourApp/Tests/GUI/global-setup.js
   ```

2. **Edit your copy and configure login paths:**
   ```javascript
   const { chromium } = require('@playwright/test');
   const { waitForPageStability, login } = require('./utils');

   module.exports = async (config) => {
     // ... existing code ...

     // Configure login paths for your application
     const loginConfig = {
       loginPaths: {
         username: ["userNameInput"],  // objectName of username field
         password: ["passwordInput"],  // objectName of password field  
         submit: ["loginButton"]       // objectName of login button
       }
     };

     // Inside the authentication loop:
     await login(page, username, password, loginConfig);
     
     // ... rest of code ...
   };
   ```

3. **Update playwright.config.js to use your custom global-setup:**
   ```javascript
   module.exports = defineConfig({
     // Change from ImtCore template to your custom file
     globalSetup: './GUI/global-setup.js',  // Your application's copy
     // ... rest of config ...
   });
   ```

##### Without Custom Configuration

If you don't customize `global-setup.js`, tests will run but won't be authenticated:
- Empty `storageState` files are created
- Tests run as unauthenticated users
- You'll see instructions in console output about configuring login

##### After Configuration

Once configured, you'll see:
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

## Updating Reference Screenshots

To update Playwright visual regression baseline images:

1. **Set UPDATE_SNAPSHOTS environment variable:**
   ```bash
   # Linux/Mac
   export UPDATE_SNAPSHOTS="true"
   
   # Windows (cmd)
   set UPDATE_SNAPSHOTS=true
   
   # Windows (PowerShell)
   $env:UPDATE_SNAPSHOTS="true"
   ```

2. **Run tests:** The Playwright command will include `--update-snapshots` flag automatically
   ```bash
   ./run-tests.sh  # or run-tests.bat on Windows
   ```

3. **Commit updated screenshots:** After tests complete, commit the new baseline images to your repository

**What happens:**
- Playwright runs with `--update-snapshots` flag
- All `toHaveScreenshot()` assertions update their baseline images
- User-specific screenshots (dashboard-user0.png, dashboard-user1.png, etc.) all get updated
- After update, set `UPDATE_SNAPSHOTS=false` or remove the variable for normal testing

## Adding More Utilities

You can add more GUI test utilities to this directory. They will be available to all Playwright tests running in the container.
