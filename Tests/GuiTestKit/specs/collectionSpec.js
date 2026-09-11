// Generates the standard collection-view spec from a declaration, so an app writes what is actually
// its own - which page, which filters, which screenshots - and nothing else.
//
//   const fixtures = require('../fixtures/test');
//   const { defineCollectionSpec } = require('imtcore-gui-testkit/specs/collectionSpec');
//
//   defineCollectionSpec(fixtures, {
//     title: 'Orders / collection',
//     pageId: 'Orders',
//     prefix: 'orders',
//     filters: { customers: 'CustomersFilter', creationDate: 'CreationDateFilter' },
//     maskColumns: ['added', 'timeStamp'],
//     scenarios: [
//       { name: 'filter-text', search: 'test' },
//       { name: 'sort-delivery-id', sort: 'orderId' },
//       { name: 'remove-dialog', command: 'Remove' },
//     ],
//   });
//
// Structure it always produces, because every collection in these apps has it:
//   - a 'cold load' block with one landing screenshot, taken on a fresh page (that is the point of it)
//   - an 'interactions' describe.serial block sharing ONE page for the whole file, since these tests
//     need a clean filter panel rather than a cold WASM boot; clearAllFilters() resets between them
// Both skip when the page is not in this user's menu - asked of the client, never of a permission
// table. Anything a declaration cannot express stays a hand-written spec using the same page objects.

const { CollectionPage } = require('../pages/CollectionPage');

/**
 * @param {object} fixtures  the app's fixtures/test.js, plus `defineTest`
 * @param {Function} fixtures.test
 * @param {Function} fixtures.newUserPage
 * @param {object} fixtures.gui
 * @param {(title: string, body: Function) => void} fixtures.defineTest  MUST be an arrow declared in
 *   the spec file itself: Playwright takes a test's file and line from the stack frame that called
 *   `test()`, so registering from inside this module would file every declared test - and therefore
 *   every baseline, via snapshotPathTemplate's {testFilePath} - under the kit's own path in
 *   node_modules. Verified live.
 * @param {object} declaration
 * @param {string} declaration.title       describe() title
 * @param {string} declaration.pageId      MenuPanel PageId
 * @param {string} declaration.prefix      screenshot name prefix ("orders" -> "orders-landing")
 * @param {Object<string,string>} [declaration.filters]     filter key -> objectName
 * @param {string[]} [declaration.maskColumns]              non-deterministic columns to mask
 * @param {(page) => CollectionPage} [declaration.createPage]  for a page that needs its own subclass
 * @param {object[]} [declaration.scenarios]  one test (and one screenshot) each - see runScenario
 */
function defineCollectionSpec(fixtures, declaration) {
  const { test, newUserPage, gui, defineTest } = fixtures;
  const { title, pageId, prefix, filters, maskColumns, createPage, scenarios = [] } = declaration;

  if (!title || !pageId || !prefix) {
    throw new Error('defineCollectionSpec: `title`, `pageId` and `prefix` are required');
  }
  if (typeof defineTest !== 'function') {
    throw new Error(
      'defineCollectionSpec: `defineTest` is required and must be declared in the spec file itself - ' +
        'see its doc comment (it decides which file Playwright files the tests, and the baselines, under)'
    );
  }
  const names = new Set();
  for (const scenario of scenarios) {
    if (!scenario.name) throw new Error(`defineCollectionSpec(${prefix}): every scenario needs a \`name\``);
    if (names.has(scenario.name)) {
      // Two scenarios sharing a name would silently compare against - and overwrite - one baseline.
      throw new Error(`defineCollectionSpec(${prefix}): duplicate scenario name "${scenario.name}"`);
    }
    names.add(scenario.name);
  }

  const build = createPage || ((page) => new CollectionPage(page, pageId, { filters, maskColumns }));
  const unavailable = `${pageId} is not available to this user`;

  test.describe(title, () => {
    // Its own describe so the fresh-page reload does NOT also fire for the shared-page block below:
    // an outer beforeEach runs for nested describes too, booting a whole app instance per nested test
    // that nothing then uses.
    test.describe('cold load', () => {
      defineTest('landing', async ({ page }) => {
        const collection = build(page);
        await collection.reload();
        test.skip(!(await collection.isAvailable()), unavailable);
        await collection.open();
        await gui.checkScreenshot(page, `${prefix}-landing`, await collection.masks());
      });
    });

    test.describe.serial('interactions', () => {
      let page, collection, available;

      test.beforeAll(async ({ browser }, testInfo) => {
        ({ page } = await newUserPage(browser, testInfo));
        collection = build(page);
        // newUserPage() only opens a blank page - nothing has navigated to the app yet.
        await collection.reload();
        available = await collection.isAvailable();
        if (available) await collection.open();
      });

      test.afterAll(async () => {
        if (page) await page.context().close();
      });

      test.beforeEach(async () => {
        test.skip(!available, unavailable);
        // Collection view state (filters, sorting) is server-persisted per user session, so a prior
        // test's filter would otherwise leak into this one's screenshot.
        await collection.clearAllFilters();
      });

      for (const scenario of scenarios) {
        defineTest(scenario.title || scenario.name, async () => {
          await runScenario({ test, gui, page, collection, prefix, scenario });
        });
      }
    });
  });
}

/**
 * The screenshot names a scenario produces, in order. The generator below takes ITS names from here
 * too, so tooling that needs to know what a declared spec captures (baseline pruning) cannot drift
 * from what the spec actually captures.
 * @returns {string[]}
 */
function scenarioScreenshotNames(prefix, scenario) {
  const name = `${prefix}-${scenario.name}`;
  if (scenario.sort) return [`${name}-1`, `${name}-2`];
  if (scenario.pagination) {
    const { size = 50, page: pageNumber = 2 } = scenario.pagination;
    return [`${name}-${size}`, `${name}-page-${pageNumber}`];
  }
  return [name];
}

/** Every screenshot name a whole declaration produces, including the landing shot. */
function collectionScreenshotNames({ prefix, scenarios = [] }) {
  return [`${prefix}-landing`, ...scenarios.flatMap((scenario) => scenarioScreenshotNames(prefix, scenario))];
}

/**
 * The same, read out of a spec file's SOURCE - for tooling that must know what a spec captures without
 * running Playwright (see prune-orphan-baselines). The declaration is a static object literal, so it is
 * evaluated as one; `createPage`'s arrow body is never called, so the page class it names need not
 * resolve here.
 * @param {string} source
 * @returns {string[]|null} null when this file declares no collection spec
 */
function collectionScreenshotNamesFromSource(source) {
  const call = 'defineCollectionSpec(';
  const names = [];
  let from = 0;
  for (;;) {
    const start = source.indexOf(call, from);
    if (start === -1) break;
    from = start + call.length;
    // The call takes two object arguments; the first is the fixtures bundle. Take the first one that
    // evaluates to a declaration - the fixtures object references identifiers that do not exist here,
    // so it simply fails to evaluate and is skipped.
    let cursor = from;
    for (;;) {
      const open = source.indexOf('{', cursor);
      if (open === -1) break;
      const end = matchingBrace(source, open);
      if (end === -1) break;
      cursor = end + 1;
      let declaration;
      try {
        // eslint-disable-next-line no-eval
        declaration = eval(`(${source.slice(open, end + 1)})`);
      } catch (_) {
        continue;
      }
      if (declaration && declaration.prefix) {
        names.push(...collectionScreenshotNames(declaration));
        from = cursor;
        break;
      }
    }
  }
  return names.length ? names : null;
}

function matchingBrace(source, open) {
  let depth = 0;
  for (let i = open; i < source.length; i++) {
    if (source[i] === '{') depth++;
    else if (source[i] === '}' && --depth === 0) return i;
  }
  return -1;
}

/**
 * One scenario = one test = one screenshot (or a small fixed set, for sort/pagination). Exactly one
 * of the action keys applies:
 *   { search: 'text' }                           type into the collection's search box
 *   { filter: key, option: 'Text' }              pick an option in a declared filter combo
 *   { filter: key, optionIndex: 0 }              ... by position, for catalogue-driven filters
 *   { dateFilter: key, preset: 'Year_Last' }     pick a date-range preset
 *   { clearAll: true, apply: [...] }             apply the listed filters, then clear them all
 *   { sort: 'headerId' }                         sort, screenshot, sort again, screenshot ("-1"/"-2")
 *   { pagination: { size, page, restore } }      page size + navigation
 *   { command: 'Remove' }                        select row 0, run the command, screenshot, dismiss
 *
 * `optional: true` on a filter scenario skips when the option is not offered to this user - a
 * data-driven filter (customers an org cannot see) legitimately has nothing to pick.
 */
async function runScenario({ test, gui, page, collection, prefix, scenario }) {
  const [firstName, secondName] = scenarioScreenshotNames(prefix, scenario);
  const masks = () => collection.masks();

  if (scenario.command) {
    test.skip(
      !(await collection.commands.isAvailable(scenario.command)),
      `${scenario.command} is not available to this user`
    );
    test.skip(!(await collection.table.hasRows()), 'this collection is empty for this user');
    await collection.selectRow(0);
    await collection.runCommand(scenario.command);
    await gui.checkScreenshot(page, firstName, await masks());
    // Shared page, no reload between tests - a modal left open blocks the next test's clicks.
    await gui.dismissDialog(page);
    return;
  }

  if (scenario.sort) {
    await collection.table.sortBy(scenario.sort);
    await gui.checkScreenshot(page, firstName, await masks());
    await collection.table.sortBy(scenario.sort);
    await gui.checkScreenshot(page, secondName, await masks());
    return;
  }

  if (scenario.pagination) {
    const { size = 50, page: pageNumber = 2, restore = false } = scenario.pagination;
    await collection.pagination.setPageSize(size);
    await gui.checkScreenshot(page, firstName, await masks());
    // A collection that fits on one page legitimately has no page-2 button.
    if (await collection.pagination.hasPage(pageNumber)) {
      await collection.pagination.goToPage(pageNumber);
      await gui.checkScreenshot(page, secondName, await masks());
    }
    if (restore) {
      // Neither the size nor the current page survives clearAllFilters(), so both would otherwise leak
      // into every later test in this block. Restoring the size does not reset the page by itself
      // (Pagination.qml only clamps currentIndex when it exceeds the new page count) - go back first.
      if (await collection.pagination.hasPage(1)) await collection.pagination.goToPage(1);
      await collection.pagination.setPageSize(25);
    }
    return;
  }

  if (scenario.clearAll) {
    for (const step of scenario.apply || []) await applyFilter({ test, collection, step });
    await collection.clearAllFilters();
    await gui.checkScreenshot(page, firstName, await masks());
    return;
  }

  await applyFilter({ test, collection, step: scenario });
  await gui.checkScreenshot(page, firstName, await masks());
}

async function applyFilter({ test, collection, step }) {
  if (step.search !== undefined) {
    await collection.search(step.search);
    return;
  }
  if (step.dateFilter) {
    await collection.setDateFilter(step.dateFilter, step.preset);
    return;
  }
  if (step.filter && step.optionIndex !== undefined) {
    await collection.selectFilterOptionByIndex(step.filter, step.optionIndex);
    return;
  }
  if (step.filter && step.option !== undefined) {
    if (step.optional) {
      test.skip(
        !(await collection.hasFilterOption(step.filter, step.option)),
        `"${step.option}" is not offered by the ${step.filter} filter for this user`
      );
    }
    await collection.selectFilterOption(step.filter, step.option);
    return;
  }
  throw new Error(`defineCollectionSpec: scenario "${step.name || JSON.stringify(step)}" declares no action`);
}

module.exports = { defineCollectionSpec, collectionScreenshotNames, collectionScreenshotNamesFromSource };
