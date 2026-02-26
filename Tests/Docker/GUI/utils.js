const { expect } = require('@playwright/test');

const defaultConfig = {
  baseUrl: process.env.BASE_URL || 'http://localhost:3000',

  timeouts: {
    delay: 500,
    locatorWait: 5000,
    stabilityTotal: 5000,
  },

  stability: {
    domStableTime: 800,
    checkInterval: 100,
  },

  screenshot: {
    fullPage: true,
    threshold: 0.05,
    maxDiffPixelRatio: 0,
  },

  loginPaths: {
    username: ['AuthorizationPage', 'LoginInput'],
    password: ['AuthorizationPage', 'PasswordInput'],
    submit: ['AuthorizationPage', 'LoginButton'],
  },
};

const delay = (time) => new Promise(resolve => setTimeout(resolve, time));

function createStrPath(path) {
  if (!Array.isArray(path) || path.length === 0) {
    throw new Error('path must be a non-empty array of objectName parts');
  }

  return path.map(p => `[objectName="${p}"]`).join(' ');
}

async function waitForDomStability(page, options = {}) {
  const idleTime = options.idleTime ?? 500;
  const timeout = options.timeout ?? 5000;
  const checkInterval = options.checkInterval ?? 100;

  const startTime = Date.now();
  let lastHtml = '';
  let stableSince = Date.now();

  while (Date.now() - startTime < timeout) {
    try {
      const currentHtml = await page.evaluate(() => document.documentElement.outerHTML);

      if (currentHtml !== lastHtml) {
        lastHtml = currentHtml;
        stableSince = Date.now();
      } else if (Date.now() - stableSince >= idleTime) {
        return;
      }

      await page.waitForTimeout(checkInterval);
    } catch (e) {
      if (String(e?.message || '').includes('Execution context was destroyed')) {
        await page.waitForLoadState('domcontentloaded');
        lastHtml = '';
        stableSince = Date.now();
        continue;
      }
      throw e;
    }
  }
}

async function waitForPageStability(page, config = defaultConfig, options = {}) {
  const maxTotalTime = options.maxTotalTime ?? config.timeouts.stabilityTotal;
  const domStableTime = options.domStableTime ?? config.stability.domStableTime;

  await waitForDomStability(page, {
    idleTime: domStableTime,
    timeout: maxTotalTime,
    checkInterval: config.stability.checkInterval,
  });
}

async function clickAt(page, x, y, config = defaultConfig) {
  await page.mouse.click(x, y);
  await waitForPageStability(page, config);
}

async function clickOnElement(page, path, config = defaultConfig) {
  const locator = page.locator(createStrPath(path) + ' [objectName="MouseArea"][visible]').first();
  await locator.waitFor({ timeout: config.timeouts.locatorWait });

  await locator.scrollIntoViewIfNeeded();

  const element = await locator.elementHandle();
  if (!element) throw new Error('ElementHandle is null for path: ' + JSON.stringify(path));

  const box = await element.boundingBox();
  if (!box) throw new Error('boundingBox is null for path: ' + JSON.stringify(path));

  const x = box.x + box.width / 2;
  const y = box.y + box.height / 2;

  await clickAt(page, x, y, config);
}

async function clickOnPage(page, pageId, config = defaultConfig) {
  await clickOnElement(page, ['MenuPanel', pageId + 'Button'], config);
}

async function clickOnCommand(page, commandId, config = defaultConfig) {
  await clickOnElement(page, ['CommandsView', commandId + 'Button'], config);
}

async function clickOnButton(page, buttonPath, config = defaultConfig) {
  await clickOnElement(page, buttonPath, config);
}

async function fillTextInput(page, text, path, config = defaultConfig) {
  const textInput = page.locator(createStrPath(path) + ' [objectName="TextInput"][visible]').first();
  await textInput.waitFor({ timeout: config.timeouts.locatorWait });

  const rect = await textInput.boundingBox();
  if (!rect) throw new Error('TextInput boundingBox is null for path: ' + JSON.stringify(path));

  await clickAt(page, rect.x + rect.width / 2, rect.y + rect.height / 2, config);

  await page.keyboard.press('Control+A');
  await page.keyboard.press('Backspace');
  await page.keyboard.type(text);
}

async function selectComboBox(page, selectedText, path, config = defaultConfig) {
  await clickOnElement(page, path, config);
  await clickOnElement(page, ['PopupMenuDialog', selectedText], config);
}

async function reloadPage(page, config = defaultConfig, url) {
  await page.goto(url || config.baseUrl);
  await waitForPageStability(page, config);
}

async function applyMasks(page, masks = []) {
  if (!Array.isArray(masks)) {
    throw new Error('masks must be an array');
  }

  const rects = [];
  for (const mask of masks) {
    if (!mask) continue;

    let rect = null;

    if (mask.path) {
      const locator = page.locator(createStrPath(mask.path));
      await locator.waitFor({ timeout: 3000 });

      const element = await locator.elementHandle();
      if (!element) {
        throw new Error('Element not found for mask path: ' + mask.path.join(' > '));
      }

      rect = await element.boundingBox();
      if (!rect) {
        throw new Error('boundingBox is null for mask element: ' + mask.path.join(' > '));
      }
    }

    if (!rect && mask.x !== undefined) {
      rect = { x: mask.x, y: mask.y, width: mask.width, height: mask.height };
    }

    if (!rect) {
      throw new Error('Each mask must have either {path} or {x,y,width,height}');
    }

    const pad = mask.padding || 0;
    rects.push({
      x: rect.x - pad,
      y: rect.y - pad,
      width: rect.width + pad * 2,
      height: rect.height + pad * 2,
    });
  }

  await page.evaluate(({ rects }) => {
    document.querySelectorAll('div[data-mask="true"]').forEach(m => m.remove());

    for (const r of rects) {
      const mask = document.createElement('div');
      Object.assign(mask.style, {
        position: 'fixed',
        top: r.y + 'px',
        left: r.x + 'px',
        width: r.width + 'px',
        height: r.height + 'px',
        backgroundColor: '#000000',
        zIndex: '9999',
        pointerEvents: 'none',
      });
      mask.setAttribute('data-mask', 'true');
      document.body.appendChild(mask);
    }
  }, { rects });

  await page.waitForFunction(() => document.querySelectorAll('div[data-mask="true"]').length >= 0);
}

async function clearMasks(page) {
  await page.evaluate(() => {
    document.querySelectorAll('div[data-mask="true"]').forEach(m => m.remove());
  });
}

async function checkScreenshot(page, filename, masks = [], config = defaultConfig) {
  try {
    await applyMasks(page, masks);
    await waitForPageStability(page, config);
    await expect(page).toHaveScreenshot(filename, config.screenshot);
  } finally {
    await clearMasks(page);
  }
}

async function login(page, username, password, config = defaultConfig, loginPaths = {}) {
  const paths = { ...config.loginPaths, ...loginPaths };

  await reloadPage(page, config);

  if (!paths.username || !paths.password || !paths.submit) {
    throw new Error(
      'loginPaths are not configured. Provide {username, password, submit} paths with objectName arrays.'
    );
  }

  await fillTextInput(page, username, paths.username, config);
  await fillTextInput(page, password, paths.password, config);
  await delay(config.timeouts.delay);
  await clickOnButton(page, paths.submit, config);
}

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

async function runWithEachUser(page, testFn, config = defaultConfig, loginPaths = {}) {
  const users = parseTestUsers();
  
  if (users.length === 0) {
    throw new Error('No users defined in TEST_USERS environment variable');
  }

  for (const user of users) {
    console.log(`Running test with user: ${user.username}`);
    await testFn(page, user, config, loginPaths);
  }
}

function getUserScreenshotName(testInfo, baseFilename) {
  const projectName = testInfo.project.name;
  
  const match = projectName.match(/^authorized-user(\d+)$/);
  
  if (!match) {
    return baseFilename;
  }
  
  const userIndex = match[1];
  
  const lastDot = baseFilename.lastIndexOf('.');
  if (lastDot === -1) {
    return `${baseFilename}-user${userIndex}`;
  }
  
  const nameWithoutExt = baseFilename.substring(0, lastDot);
  const ext = baseFilename.substring(lastDot);
  return `${nameWithoutExt}-user${userIndex}${ext}`;
}

function getUserInfoFromTest(testInfo) {
  const project = testInfo.project;
  const use = project.use;
  
  if (use.userIndex !== undefined && use.username && use.password) {
    return {
      userIndex: use.userIndex,
      username: use.username,
      password: use.password,
    };
  }
  
  return null;
}

module.exports = {
  defaultConfig,
  delay,
  reloadPage,
  clickAt,
  checkScreenshot,
  login,
  parseTestUsers,
  runWithEachUser,
  getUserScreenshotName,
  getUserInfoFromTest,
  waitForPageStability,
  clickOnPage,
  clickOnCommand,
  selectComboBox,
  fillTextInput,
  clickOnButton,
};