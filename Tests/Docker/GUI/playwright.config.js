// @ts-check
const { defineConfig } = require('@playwright/test');
const fs = require('fs');
const path = require('path');

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

const users = parseTestUsers();
const appDir = path.join(__dirname, 'app');
const guestDir = path.join(__dirname, 'app', 'guest');
const authorizedDir = path.join(__dirname, 'app', 'authorized');

const hasGuest = fs.existsSync(guestDir);
const hasAuthorized = fs.existsSync(authorizedDir);

console.log('App dir:', appDir);
console.log('Guest dir exists:', hasGuest);
console.log('Authorized dir exists:', hasAuthorized);
console.log('Users:', users.length);

const projects = [];

if (hasAuthorized && users.length > 0) {
  users.forEach((user, index) => {
    projects.push({
      name: `authorized-user${index}`,
      testDir: authorizedDir,
      use: {
        storageState: `storageState-user${index}.json`,
        // Fix: Pass credentials to project config so global-setup can read them
        username: user.username,
        password: user.password,
        userIndex: index,
      },
    });
  });
}

if (hasGuest) {
  projects.push({
    name: 'guest',
    testDir: guestDir,
    use: {},
  });
}

if (projects.length === 0) {
  projects.push({
    name: 'default',
    testDir: appDir,
    use: {},
  });
}

console.log('Projects:', projects.map(p => p.name).join(', '));

module.exports = defineConfig({
  globalSetup: (hasAuthorized && users.length > 0) 
    ? require.resolve('./global-setup') 
    : undefined,

  timeout: 30 * 1000,
  expect: { timeout: 5000 },
  fullyParallel: true,
  retries: 0,

  reporter: [
    ['html', { outputFolder: process.env.PLAYWRIGHT_HTML_REPORT || 'playwright-report' }],
    ['json', { outputFile: process.env.PLAYWRIGHT_JSON_OUTPUT_FILE || 'test-results/playwright-results.json' }],
    ['junit', { outputFile: process.env.PLAYWRIGHT_JUNIT_OUTPUT_FILE || 'test-results/playwright-junit.xml' }],
    ['list'],
  ],

  use: {
    baseURL: process.env.BASE_URL || 'http://host.docker.internal:7776',
    viewport: {
      width: Number(process.env.VIEWPORT_WIDTH || 1400),
      height: Number(process.env.VIEWPORT_HEIGHT || 800),
    },
    trace: 'on-first-retry',
    screenshot: 'only-on-failure',
    video: 'retain-on-failure',
    navigationTimeout: 15 * 1000,
    actionTimeout: 10 * 1000,
  },

  projects: projects,
});