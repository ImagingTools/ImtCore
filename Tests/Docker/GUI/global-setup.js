const { chromium } = require('@playwright/test');
const { waitForPageStability, login } = require('./utils');

/**
 * Global setup for Playwright tests with multi-user support
 * 
 * Creates separate storageState files for each user defined in the config.
 * Each authorized project (authorized-user0, authorized-user1, etc.) gets its own auth state.
 * 
 * Note: This setup assumes the application has a standard login flow.
 * If your application requires custom login logic, you should:
 * 1. Copy this file to your application's Tests/GUI/ directory
 * 2. Import and configure the login paths for your application
 * 3. Update the playwright.config.js globalSetup path to point to your custom file
 */
module.exports = async (config) => {
  // Get global settings
  const globalUse = config?.use || {};
  const baseURL = globalUse.baseURL || process.env.BASE_URL || 'http://host.docker.internal:7776';
  const viewport = globalUse.viewport || { width: 1400, height: 800 };

  // Find all authorized projects (skip guest project)
  const authorizedProjects = config?.projects?.filter(p => 
    p.name.startsWith('authorized-user')
  ) || [];

  if (authorizedProjects.length === 0) {
    console.log('No authorized projects found. Skipping global setup.');
    return;
  }

  console.log(`Setting up authentication for ${authorizedProjects.length} user(s)...`);
  console.log('');
  console.log('='.repeat(60));
  console.log('IMPORTANT: Login configuration required!');
  console.log('='.repeat(60));
  console.log('');
  console.log('This global-setup.js is a template from ImtCore.');
  console.log('To enable authentication for your application:');
  console.log('');
  console.log('1. Copy this file to your application Tests/GUI/ directory:');
  console.log('   cp $IMTCORE_DIR/Tests/Docker/GUI/global-setup.js \\');
  console.log('      YourApp/Tests/GUI/global-setup.js');
  console.log('');
  console.log('2. Edit your copy and configure login paths:');
  console.log('');
  console.log('   const loginConfig = {');
  console.log('     loginPaths: {');
  console.log('       username: ["userNameInput"],  // objectName of username field');
  console.log('       password: ["passwordInput"],  // objectName of password field');
  console.log('       submit: ["loginButton"]       // objectName of login button');
  console.log('     }');
  console.log('   };');
  console.log('');
  console.log('   await login(page, username, password, loginConfig);');
  console.log('');
  console.log('3. Update playwright.config.js globalSetup to use your copy:');
  console.log('   globalSetup: "./GUI/global-setup.js"');
  console.log('');
  console.log('='.repeat(60));
  console.log('');
  console.log('Skipping authentication setup (no login paths configured).');
  console.log('Tests will run without authentication.');
  console.log('');

  // For now, skip authentication but still create empty storageState files
  // so tests can run (they just won't be authenticated)
  const browser = await chromium.launch();

  try {
    for (const project of authorizedProjects) {
      const { storageState, userIndex, username } = project.use;
      
      console.log(`Creating empty storageState for user${userIndex}: ${username}...`);
      
      const context = await browser.newContext({ viewport });
      await context.storageState({ path: `./${storageState}` });
      await context.close();
      
      console.log(`✓ Created ${storageState} (unauthenticated)`);
    }
  } finally {
    await browser.close();
  }

  console.log('');
  console.log('Global setup complete (authentication skipped).');
};
