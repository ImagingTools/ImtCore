const { chromium } = require('@playwright/test');
const { waitForPageStability, login } = require('./utils');

/**
 * Global setup for Playwright tests with multi-user support
 * 
 * Creates separate storageState files for each user defined in the config.
 * Each authorized project (authorized-user0, authorized-user1, etc.) gets its own auth state.
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

  // Find all authorized projects (skip guest project)
  const authorizedProjects = config?.projects?.filter(p => 
    p.name.startsWith('authorized-user')
  ) || [];

  if (authorizedProjects.length === 0) {
    console.log('No authorized projects found. Skipping global setup.');
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

  console.log(`Setting up authentication for ${authorizedProjects.length} user(s)...`);

  const browser = await chromium.launch();

  try {
    // Create storageState for each user
    for (const project of authorizedProjects) {
      const { username, password, storageState, userIndex } = project.use;

      if (!username || !password) {
        console.warn(`Skipping ${project.name}: missing username or password`);
        continue;
      }

      console.log(`Authenticating user${userIndex}: ${username}...`);

      const context = await browser.newContext({ viewport });
      const page = await context.newPage();

      await page.goto(baseURL);
      await waitForPageStability(page);

      await login(page, username, password, undefined, loginPaths);

      await waitForPageStability(page);

      await context.storageState({ path: `./${storageState}` });

      console.log(`✓ Created ${storageState} for ${username}`);

      await context.close();
    }
  } finally {
    await browser.close();
  }

  console.log('\n✓ Authentication setup complete!\n');
};
