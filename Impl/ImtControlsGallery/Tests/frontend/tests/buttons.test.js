const { test } = require('@playwright/test');
const { reloadPage, checkScreenshot, clickOnPage, clickOnButton} = require('./utils');

test.beforeEach(async ({ page }) => {
  await reloadPage(page)
  await clickOnPage(page, "Buttons");
});

test('Start page', async ({ page }) => {
	await checkScreenshot(page, 'start_page.png')
});

test('Text button click', async ({ page }) => {
	await clickOnButton(page, ["TextButton"])
  await checkScreenshot(page, 'text_button_click.png')
});

test('Round button click', async ({ page }) => {
	await clickOnButton(page, ["RoundButton"])
  await checkScreenshot(page, 'round_button_click.png')
});

