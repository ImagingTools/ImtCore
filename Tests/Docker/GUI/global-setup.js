const { chromium } = require('@playwright/test');
const { waitForPageStability, login } = require('./utils');

/**
 * Global setup for Playwright tests with multi-user support
 * 
 * Creates separate storageState files for each user defined in the config.
 * Each authorized project (authorized-user0, authorized-user1, etc.) gets its own auth state.
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

      await login(page, username, password);

      await waitForPageStability(page);

      await context.storageState({ path: `./${storageState}` });

      console.log(`✓ Created ${storageState} for ${username}`);

      await context.close();
    }
  } finally {
    await browser.close();
  }

  console.log('Global setup complete!');
};
