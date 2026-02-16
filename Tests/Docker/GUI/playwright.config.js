// @ts-check
const { defineConfig } = require('@playwright/test');

/**
 * Parse TEST_USERS environment variable
 * Format: "user1:pass1,user2:pass2,user3:pass3"
 * Returns: [{username: "user1", password: "pass1"}, ...]
 */
function parseTestUsers() {
  const testUsers = process.env.TEST_USERS || '';
  if (!testUsers) {
    return [];
  }

  return testUsers.split(',').map(userPass => {
    const [username, password] = userPass.trim().split(':');
    if (!username || !password) {
      throw new Error(`Invalid TEST_USERS format: "${userPass}". Expected "username:password"`);
    }
    return { username, password };
  });
}

/**
 * Playwright configuration with dynamic multi-user support
 *
 * Features:
 * - Dynamically generates "authorized-user0", "authorized-user1", etc. projects based on TEST_USERS
 * - Each user gets their own storageState file
 * - Screenshots automatically include user info via project name
 * - Single "guest" project for non-authenticated tests
 */
module.exports = defineConfig({
  testDir: './GUI',

  /* Global setup (login + storageState for each user) */
  globalSetup: require.resolve('./global-setup'),

  /* Maximum time one test can run for */
  timeout: 30 * 1000,

  /* Test timeout for assertions */
  expect: {
    timeout: 5000,
  },

  /* Run tests in files in parallel */
  fullyParallel: true,

  /* Fail the build on CI if you accidentally left test.only in the source code */
  forbidOnly: !!process.env.CI,

  /* Retry on CI only */
  retries: process.env.CI ? 2 : 0,

  /* Opt out of parallel tests on CI */
  workers: process.env.CI ? 1 : undefined,

  /* Reporter to use. See https://playwright.dev/docs/test-reporters */
  reporter: [
    ['html', { outputFolder: 'playwright-report' }],
    ['json', { outputFile: 'test-results/playwright-results.json' }],
    ['junit', { outputFile: 'test-results/playwright-junit.xml' }],
    ['list'],
  ],

  /* Shared settings for all the projects below. */
  use: {
    /* Base URL to use in actions like `await page.goto('/')` */
    baseURL: process.env.BASE_URL || 'http://host.docker.internal:7776',

    /* Viewport (global-setup.js should read this from config) */
    viewport: {
      width: Number(process.env.VIEWPORT_WIDTH || 1400),
      height: Number(process.env.VIEWPORT_HEIGHT || 800),
    },

    /* Collect trace when retrying the failed test */
    trace: 'on-first-retry',

    /* Screenshot on failure */
    screenshot: 'only-on-failure',

    /* Video on failure */
    video: 'retain-on-failure',

    /* Navigation timeout */
    navigationTimeout: 15 * 1000,

    /* Action timeout */
    actionTimeout: 10 * 1000,
  },

  /* Configure projects dynamically based on TEST_USERS */
  projects: [
    // Generate authorized projects for each user
    ...parseTestUsers().map((user, index) => ({
      name: `authorized-user${index}`,
      testMatch: /authorized\/.*\.test\.js/,
      use: {
        storageState: `storageState-user${index}.json`,
        // Store user info in project metadata for access in tests
        userIndex: index,
        username: user.username,
        password: user.password,
      },
    })),
    
    // Guest project (no authentication)
    {
      name: 'guest',
      testIgnore: /authorized\/.*\.test\.js/,
      use: {},
    },
  ],
});