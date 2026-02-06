const { chromium } = require('@playwright/test');
const { waitForPageStability, login } = require('./GUI/utils');

module.exports = async (config) => {
  const use = config?.projects?.[0]?.use || {};

  const baseURL = use.baseURL || process.env.BASE_URL || 'http://host.docker.internal:7776';

  const username = use.testUsername || process.env.TEST_USERNAME || 'su';
  const password = use.testPassword || process.env.TEST_PASSWORD || '1';

  const viewport =
    use.viewport ||
    (process.env.VIEWPORT_WIDTH && process.env.VIEWPORT_HEIGHT
      ? { width: Number(process.env.VIEWPORT_WIDTH), height: Number(process.env.VIEWPORT_HEIGHT) }
      : { width: 1400, height: 800 });

  const browser = await chromium.launch();

  const context = await browser.newContext({
    viewport,
  });

  const page = await context.newPage();

  await page.goto(baseURL);
  await waitForPageStability(page);

  await login(page, username, password);

  await waitForPageStability(page);

  await context.storageState({ path: './storageState.json' });

  await browser.close();
};