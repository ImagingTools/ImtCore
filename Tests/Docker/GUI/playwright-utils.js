/**
 * Playwright-specific test utilities
 * 
 * This module wraps the core utilities from utils.js and adds Playwright-specific
 * functionality like screenshot assertions.
 * 
 * Import this module in your Playwright test files instead of utils.js directly:
 *   const { test } = require('@playwright/test');
 *   const { checkScreenshot, login, clickOnButton } = require('./playwright-utils');
 */

const { expect } = require('@playwright/test');
const utils = require('./utils');

/**
 * Take a screenshot and compare it to the baseline
 * @param {import('@playwright/test').Page} page - Playwright page object
 * @param {string} filename - Screenshot filename
 * @param {Array} masks - Array of mask definitions to hide dynamic content
 * @param {Object} config - Configuration object (defaults to utils.defaultConfig)
 */
async function checkScreenshot(page, filename, masks = [], config = utils.defaultConfig) {
  try {
    await utils.applyMasks(page, masks);
    await utils.waitForPageStability(page, config);
    await expect(page).toHaveScreenshot(filename, config.screenshot);
  } finally {
    await utils.clearMasks(page);
  }
}

// Re-export all utilities from utils.js
module.exports = {
  ...utils,
  checkScreenshot,
};
