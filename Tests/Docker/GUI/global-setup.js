const { chromium } = require('@playwright/test');
const { waitForPageStability, login } = require('./utils');

/**
 * Global setup for Playwright tests with multi-user support
 * 
 * Creates separate storageState files for each user defined in the config.
 * Each authorized project (authorized-user0, authorized-user1, etc.) gets its own auth state.
 * 
 * CONFIGURATION:
 * Set LOGIN_PATHS environment variable with JSON containing login UI element paths:
 * export LOGIN_PATHS='{"username":["objectName=usernameInput"],"password":["objectName=passwordInput"],"submit":["objectName=loginButton"]}'
 * 
 * Alternatively, copy this file to your application and hardcode loginPaths:
 * const loginPaths = { username: [...], password: [...], submit: [...] };
 */
module.exports = async (config) => {
  // Get global settings
  const globalUse = config?.use || {};
  const baseURL = globalUse.baseURL || process.env.BASE_URL || 'http://host.docker.internal:7776';
  const viewport = globalUse.viewport || { width: 1400, height: 800 };

  // Get loginPaths from environment variable or use empty object (will be caught below)
  let loginPaths = {};
  if (process.env.LOGIN_PATHS) {
    try {
      loginPaths = JSON.parse(process.env.LOGIN_PATHS);
    } catch (e) {
      console.error('Error parsing LOGIN_PATHS environment variable:', e.message);
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

  // Check if loginPaths are configured
  if (!loginPaths.username || !loginPaths.password || !loginPaths.submit) {
    console.log('\n' + '='.repeat(70));
    console.log('⚠️  LOGIN CONFIGURATION MISSING');
    console.log('='.repeat(70));
    console.log(`Creating empty storageState files for ${authorizedProjects.length} user(s)...`);
    console.log('\nℹ️  Tests will run WITHOUT authentication (as guest).\n');
    console.log('To enable authentication, configure LOGIN_PATHS:');
    console.log('');
    console.log('Option 1 - Environment Variable:');
    console.log('  export LOGIN_PATHS=\'{"username":["objectName=usernameInput"],"password":["objectName=passwordInput"],"submit":["objectName=loginButton"]}\'');
    console.log('');
    console.log('Option 2 - Customize this file:');
    console.log('  1. Copy to your application:');
    console.log('     cp $IMTCORE_DIR/Tests/Docker/GUI/global-setup.js YourApp/Tests/GUI/');
    console.log('  2. Hardcode loginPaths in the copy');
    console.log('  3. Update playwright.config.js globalSetup path');
    console.log('='.repeat(70) + '\n');

    const fs = require('fs');
    // Create empty storageState files
    for (const project of authorizedProjects) {
      const { storageState } = project.use;
      const emptyState = { cookies: [], origins: [] };
      fs.writeFileSync(`./${storageState}`, JSON.stringify(emptyState, null, 2));
      console.log(`✓ Created empty ${storageState}`);
    }
    console.log('\n✓ Setup complete. Tests will run as guest users.\n');
    return;
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
