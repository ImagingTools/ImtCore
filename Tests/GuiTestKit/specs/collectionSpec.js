// Generates the standard collection-view spec from a declaration.
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
// It always produces a 'cold load' block (one landing screenshot on a fresh page) and an
// 'interactions' describe.serial block sharing one page, resetting via clearAllFilters() between tests.
// Both skip when the user lacks `requires`, checked against the permission list the server granted at login.

const { CollectionPage } = require('../pages/CollectionPage');

/**
 * Skip only on an explicit "no".
 *
 * `user.can()` returns undefined when permissions could not be determined; `=== false` (not `!`) keeps
 * "unknown" from becoming a skip, so it reaches a real attempt instead.
 */
function denied(user, permission) {
  return permission ? user.can(permission) === false : false;
}

/**
 * @param {object} fixtures  the app's fixtures/test.js, plus `defineTest`
 * @param {Function} fixtures.defineTest  a one-line shim the spec file declares:
 *   `defineTest: (...args) => fixtures.test(...args)`. Spread, not `(t, b)`, so a tagged
 *   `test(title, { tag }, body)` keeps its body. Registering from the spec file (not this module) is
 *   what lets testIgnore / testMatch / isolatedSpec match on the right filename.
 * @param {Function} fixtures.test
 * @param {Function} fixtures.newUserPage
 * @param {object} fixtures.gui
 * @param {object} declaration
 * @param {string} declaration.title       describe() title
 * @param {string} declaration.pageId      MenuPanel PageId
 * @param {string} declaration.prefix      screenshot name prefix ("orders" -> "orders-landing")
 * @param {string} [declaration.requires]  permission this page needs; the spec skips for a user the
 *   server did not grant it. Omit only for a page open to everyone.
 * @param {Object<string,string>} [declaration.filters]     filter key -> objectName
 * @param {string[]} [declaration.maskColumns]              non-deterministic columns to mask
 * @param {string} [declaration.stableSort]  header id to sort by before each interaction test, so a
 *   cleared (unsorted) collection has a defined row order for its screenshot.
 * @param {(page) => CollectionPage} [declaration.createPage]  for a page that needs its own subclass
 * @param {object[]} [declaration.scenarios]  one test (and one screenshot) each - see runScenario
 * @param {(ctx: object) => void} [declaration.extra]  hand-written tests registered inside the shared-page
 *   block. Called with { test, requires, gui, page, collection, user }; page/collection/user are getters
 *   resolved at test time (assigned in beforeAll).
 */
function defineCollectionSpec(fixtures, declaration) {
  const { test, newUserPage, gui, defineTest } = fixtures;
  const { title, pageId, prefix, requires, filters, maskColumns, createPage, stableSort, scenarios = [], extra } = declaration;

  if (!title || !pageId || !prefix) {
    throw new Error('defineCollectionSpec: `title`, `pageId` and `prefix` are required');
  }
  if (typeof defineTest !== 'function') {
    throw new Error(
      'defineCollectionSpec: `defineTest` is required and must be declared in the spec file itself - ' +
        'see its doc comment (it decides which file Playwright attributes the generated tests to)'
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
  const unavailable = requires ? `needs ${requires}` : `${pageId} is not available to this user`;

  test.describe(title, () => {
    // Its own describe so the fresh-page reload doesn't also fire for the shared-page block below.
    test.describe('cold load', () => {
      defineTest('landing', async ({ page, user }) => {
        test.skip(denied(user, requires), unavailable);
        const collection = build(page);
        await collection.reload();
        // No probe: the permission check above already decided this. open() fails loudly if the page
        // doesn't render for a user who holds the permission.
        await collection.open();
        await collection.expectOpen();
        // Filters and sorting live in the user's SESSION on the server, not in the page, so this
        // collection comes up however the previous spec left it - and the landing shot would be of that
        // residue rather than of the collection. Cleared (and re-sorted) so the shot means one thing.
        await collection.clearAllFilters();
        if (stableSort) await collection.table.sortBy(stableSort);
        await gui.checkScreenshot(page, `${prefix}-landing`, () => collection.masks());
      });
    });

    test.describe.serial('interactions', () => {
      let page, specUser, collection, skip;

      test.beforeAll(async ({ browser }, testInfo) => {
        ({ page, user: specUser } = await newUserPage(browser, testInfo));
        skip = denied(specUser, requires);
        if (skip) return;
        collection = build(page);
        // newUserPage() only opens a blank page - nothing has navigated to the app yet.
        await collection.reload();
        await collection.open();
      });

      test.afterAll(async () => {
        if (page) await page.context().close();
      });

      test.beforeEach(async () => {
        test.skip(skip, unavailable);
        // Collection view state (filters, sorting) is server-persisted per user session, so a prior
        // test's filter would otherwise leak into this one's screenshot.
        await collection.clearAllFilters();
        // Clearing filters clears the sort too, leaving an undefined row order; stableSort puts a known
        // order back before each test. One click from the just-cleared state, so it's the same each time.
        if (stableSort) await collection.table.sortBy(stableSort);
      });

      for (const scenario of scenarios) {
        defineTest(scenario.title || scenario.name, async () => {
          await runScenario({ test, gui, page, user: specUser, collection, prefix, scenario });
        });
      }

      if (extra) {
        extra({
          test: defineTest,
          gui,
          // Bound to this block's user, so a flow here states the permission it needs in one line.
          requires(permission) {
            test.skip(specUser.can(permission) === false, `${specUser.key} was not granted ${permission}`);
          },
          // Getters: only assigned once beforeAll has run, so a plain value captured here would be undefined.
          get page() {
            return page;
          },
          get collection() {
            return collection;
          },
          get user() {
            return specUser;
          },
        });
      }
    });
  });
}

/**
 * The screenshot names a scenario produces, in order. The generator takes its names from here too, so
 * tooling (baseline pruning) can't drift from what the spec actually captures.
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
 * running Playwright (see prune-orphan-baselines). The declaration is evaluated as an object literal.
 * @param {string} source
 * @returns {string[]|null} null when this file declares no collection spec
 */
function collectionScreenshotNamesFromSource(source) {
  const call = 'defineCollectionSpec(';
  const names = [];
  let calls = 0;
  let from = 0;
  for (;;) {
    const start = source.indexOf(call, from);
    if (start === -1) break;
    calls++;
    from = start + call.length;
    // Take the first brace group that evaluates to a declaration; the fixtures object references
    // identifiers that don't exist here, so it fails to evaluate and is skipped.
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
  // "No declaration here" and "there IS one but it could not be read" are different answers; only the
  // first is safe to act on. Throw rather than return null when a found call couldn't be evaluated.
  if (calls > 0 && names.length === 0) {
    throw new Error(
      'collectionScreenshotNamesFromSource: found defineCollectionSpec() but could not evaluate its ' +
        'declaration (a value referencing an identifier outside the literal?) - refusing to report ' +
        '"no screenshots", which a caller would read as "these baselines are orphans"'
    );
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
 *   { pagination: { size, page } }               page size + navigation (state restored afterwards
 *                                                unless `restore: false`)
 *   { command: 'Remove' }                        select row 0, run the command, screenshot, dismiss
 *
 * `optional: true` on a filter scenario skips when the option is not offered to this user - a
 * data-driven filter (customers an org cannot see) legitimately has nothing to pick.
 */
async function runScenario({ test, gui, page, user, collection, prefix, scenario }) {
  const [firstName, secondName] = scenarioScreenshotNames(prefix, scenario);
  const masks = () => collection.masks();

  if (scenario.command) {
    // Gated on the declared permission, checked against what the server granted at login.
    test.skip(denied(user, scenario.requires), `${scenario.command} needs ${scenario.requires}`);
    test.skip(!(await collection.table.hasRows()), 'this collection is empty for this user');
    await collection.selectRow(0);
    await collection.runCommand(scenario.command);
    await gui.checkScreenshot(page, firstName, masks);
    // Shared page, no reload between tests - a modal left open blocks the next test's clicks.
    await gui.dismissDialog(page);
    return;
  }

  if (scenario.sort) {
    // Cleared first so the first click always lands on ASC: a header click TOGGLES the direction when
    // its column is already the sorted one (imtcontrols/Views/TableHeaderDelegate.qml), and stableSort
    // may have just made it exactly that - which flips both shots, sort arrow included.
    await collection.clearAllFilters();
    await collection.table.sortBy(scenario.sort);
    await gui.checkScreenshot(page, firstName, masks);
    await collection.table.sortBy(scenario.sort);
    await gui.checkScreenshot(page, secondName, masks);
    return;
  }

  if (scenario.pagination) {
    // Restore by default: neither the page size nor the current page survives clearAllFilters(), so
    // leaving them set would change what every later test in this serial block sees.
    const { size = 50, page: pageNumber = 2, restore = true } = scenario.pagination;
    await collection.pagination.setPageSize(size);
    await gui.checkScreenshot(page, firstName, masks);
    // A collection that fits on one page legitimately has no page-2 button.
    if (await collection.pagination.hasPage(pageNumber)) {
      await collection.pagination.goToPage(pageNumber);
      await gui.checkScreenshot(page, secondName, masks);
    }
    if (restore) {
      // Restoring the size doesn't reset the page (Pagination.qml only clamps on overflow) - go back first.
      if (await collection.pagination.hasPage(1)) await collection.pagination.goToPage(1);
      await collection.pagination.setPageSize(25);
    }
    return;
  }

  if (scenario.clearAll) {
    for (const step of scenario.apply || []) await applyFilter({ test, collection, step });
    await collection.clearAllFilters();
    await gui.checkScreenshot(page, firstName, masks);
    return;
  }

  await applyFilter({ test, collection, step: scenario });
  await gui.checkScreenshot(page, firstName, masks);
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
