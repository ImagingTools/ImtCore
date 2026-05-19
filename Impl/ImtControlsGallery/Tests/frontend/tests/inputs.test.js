const { test } = require('@playwright/test');
const { reloadPage, checkScreenshot, clickOnPage, clickOnButton, fillTextInput, selectComboBox} = require('./utils');

test.beforeEach(async ({ page }) => {
  await reloadPage(page)
  await clickOnPage(page, "Inputs");
});

test('Start page', async ({ page }) => {
	await checkScreenshot(page, 'inputs_page.png')
});

test('Text input', async ({ page }) => {
	await fillTextInput(page, "EXAMPLE TEXT", ["TextField"])
  await checkScreenshot(page, 'text_input.png')
});

test('ComboBox click', async ({ page }) => {
	await selectComboBox(page, "name1", ["ComboBox"])
  await checkScreenshot(page, 'combo_box_click.png')
});

