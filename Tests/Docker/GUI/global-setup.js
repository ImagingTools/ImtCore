const { chromium } = require('playwright');
const { waitForPageStability, login } = require('./utils');

/**
 * Global setup for Playwright tests with multi-user support
 * 
 * Creates separate storageState files for each user defined in TEST_USERS.
 * Single authorized project is used.
 * 
 * DEFAULT CONFIGURATION:
 * Uses standard ImtCore authorization page structure:
 *   - Login Input: ["AuthorizationPage", "LoginInput"]
 *   - Password Input: ["AuthorizationPage", "PasswordInput"]  
 *   - Login Button: ["AuthorizationPage", "LoginButton"]
 * 
 * CUSTOM CONFIGURATION:
 * Override defaults by setting LOGIN_PATHS environment variable with JSON:
 * export LOGIN_PATHS='{"username":["objectName=usernameInput"],"password":["objectName=passwordInput"],"submit":["objectName=loginButton"]}'
 * 
 * Or copy this file to your application and modify the loginPaths object directly.
 */
function parseTestUsers() {
  const testUsers = process.env.TEST_USERS || '';
  if (!testUsers) return [];

  return testUsers.split(',').map(userPass => {
    const [username, password] = userPass.trim().split(':');
    if (!username || !password) {
      throw new Error(`Invalid TEST_USERS format: "${userPass}". Expected "username:password"`);
    }
    return { username, password };
  });
}

module.exports = async (config) => {
  // Get global settings
  const globalUse = config?.use || {};
  const baseURL = globalUse.baseURL || process.env.BASE_URL || 'http://host.docker.internal:7776';
  const viewport = globalUse.viewport || { width: 1400, height: 800 };

  // Get loginPaths from environment variable or use default paths
  // Default paths work for applications using standard ImtCore authorization page structure
  let loginPaths = {
    username: ['AuthorizationPage', 'LoginInput'],
    password: ['AuthorizationPage', 'PasswordInput'],
    submit: ['AuthorizationPage', 'LoginButton']
  };
  
  // Override with LOGIN_PATHS environment variable if provided
  if (process.env.LOGIN_PATHS) {
    try {
      loginPaths = JSON.parse(process.env.LOGIN_PATHS);
    } catch (e) {
      console.error('Error parsing LOGIN_PATHS environment variable:', e.message);
      console.log('Using default loginPaths instead.');
    }
  }

  const users = parseTestUsers();

  if (users.length === 0) {
    console.log('No users found in TEST_USERS. Skipping global setup.');
    return;
  }

  // Validate loginPaths structure (should always be valid with defaults, but check anyway)
  if (!loginPaths.username || !loginPaths.password || !loginPaths.submit) {
    console.error('\n' + '='.repeat(70));
    console.error('❌ ERROR: Invalid loginPaths configuration');
    console.error('='.repeat(70));
    console.error('loginPaths must have username, password, and submit properties.');
    console.error('Current loginPaths:', JSON.stringify(loginPaths, null, 2));
    console.error('='.repeat(70) + '\n');
    throw new Error('Invalid loginPaths configuration');
  }

  console.log('\n' + '='.repeat(70));
  console.log(`🔐 Setting up authentication for ${users.length} user(s)`);
  console.log('='.repeat(70));
  console.log(`Base URL: ${baseURL}`);
  console.log(`Login paths configuration:`);
  console.log(`  Username: ${JSON.stringify(loginPaths.username)}`);
  console.log(`  Password: ${JSON.stringify(loginPaths.password)}`);
  console.log(`  Submit:   ${JSON.stringify(loginPaths.submit)}`);
  console.log('='.repeat(70) + '\n');

  const browser = await chromium.launch();

  try {
    for (const [index, user] of users.entries()) {
      const { username, password } = user;

      console.log(`Authenticating user${index}: ${username}...`);

      const context = await browser.newContext({ viewport });
      const page = await context.newPage();

      try {
        await page.goto(baseURL, { waitUntil: 'networkidle', timeout: 30000 });
        await waitForPageStability(page);

        // Wait for authorization page to appear
        const authPageSelector = '[objectName="AuthorizationPage"]';
        await page.waitForSelector(authPageSelector, { timeout: 10000 });
        
        await login(page, username, password, undefined, loginPaths);

        await waitForPageStability(page);

        const storageState = `storageState-${username}.json`;
        await context.storageState({ path: `./${storageState}` });

        console.log(`✓ Created ${storageState} for ${username}`);
      } catch (error) {
        console.error(`✗ Failed to authenticate ${username}.`);
        console.error(`  Make sure the application is running at: ${baseURL}`);
        console.error(`  And that the authorization page structure matches the loginPaths configuration.`);
        
        // Create empty storageState file so tests can run (as guest)
        const storageState = `storageState-${username}.json`;
        await context.storageState({ path: `./${storageState}` });
        console.log(`  Created empty ${storageState} (tests will run as guest)`);
      }

      await context.close();
    }
  } finally {
    await browser.close();
  }

  console.log('\n✓ Authentication setup complete!\n');
};