// @ts-check
const { defineConfig } = require('@playwright/test');

/**
 * Playwright configuration for Lisa application testing
 *
 * Notes:
 * - baseURL now has a safe default for Docker-on-Windows runs
 * - viewport is defined in config and can be consumed by global-setup.js via config.projects[0].use.viewport
 * - auth credentials are passed via "use" as custom fields so global-setup.js can read them from config
 * - fixed testMatch/testIgnore patterns: since testDir is './GUI', patterns must be relative to that dir
 */
module.exports = defineConfig({
  testDir: './GUI',

  /* Global setup (login + storageState.json) */
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
    ['html', { outputFolder: 'test-results/playwright-report' }],
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

    /* Custom fields for global-setup.js (read from config first, env fallback) */
    testUsername: process.env.TEST_USERNAME || 'su',
    testPassword: process.env.TEST_PASSWORD || '1',

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

  /* Configure projects */
  projects: [
    {
      name: 'authorized',
      testMatch: /authorized\/.*\.test\.js/,
      use: {
        storageState: 'storageState.json',
      },
    },
    {
      name: 'guest',
      testIgnore: /authorized\/.*\.test\.js/,
      use: {},
    },
  ],
});