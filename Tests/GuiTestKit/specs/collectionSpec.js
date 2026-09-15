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
// Both skip when the user lacks the permission the declaration names (`requires`), checked against the
// permission list the SERVER granted at login - never against a hand-maintained page/permission table,
// and never against whether a button happened to render in time.
// Anything a declaration cannot express stays a hand-written spec using the same page objects.

const { CollectionPage } = require('../pages/CollectionPage');

/**
 * Skip only on an explicit "no".
 *
 * `user.can()` answers from the permissions the server granted at login (see createTest), and returns
 * undefined when they could not be determined. Spelling this as `!user.can(x)` would turn that undefined
 * into a skip - the same mistake as the old timeout-based probe, where "not rendered yet" became "not
 * permitted" and the run went green. Unknown must reach a real attempt and fail there if it is wrong.
 */
function denied(user, permission) {
  return permission ? user.can(permission) === false : false;
}

/**
 * @param {object} fixtures  the app's fixtures/test.js, plus `defineTest`
 * @param {Function} fixtures.defineTest  a one-line shim the SPEC FILE
 *   declares: `defineTest: (...args) => fixtures.test(...args)`. Spread, not `(t, b)`: a tagged test is
 *   `test(title, { tag }, body)`, and a two-parameter shim silently drops the body - Playwright still
 *   parses the tag, so --list and --grep look right while every tagged test fails. Playwright attributes a test to the file
 *   whose stack frame called `test()`, and that attribution is what testIgnore / testMatch /
 *   isolatedSpec match on - register from inside this module and every generated test is filed under
 *   the kit's own path in node_modules, so a spec could no longer be pinned to a user or excluded from
 *   a project by filename. (Baselines are keyed by user + name and are unaffected either way.)
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
 * @param {string} [declaration.stableSort]  header id to sort by before each interaction test. Clearing
 *   the filters clears the sort too, and an unsorted collection has no defined row order - so any
 *   screenshot of one differs run to run. Name a column that gives it one (ideally a unique-valued
 *   one) for any collection large enough to show this.
 * @param {(page) => CollectionPage} [declaration.createPage]  for a page that needs its own subclass
 * @param {object[]} [declaration.scenarios]  one test (and one screenshot) each - see runScenario
 * @param {(ctx: object) => void} [declaration.extra]  hand-written tests for this page's own flows,
 *   registered inside the SAME shared-page block so they cost no extra app boot. Called with
 *   { test, requires, gui, page, collection, user }, where page/collection/user are getters resolved at
 *   test time
 *   (they are assigned in beforeAll). Use it for what a declaration genuinely cannot express - a bind
 *   dialog, a column-configuration flow - not to avoid declaring a standard scenario.
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
    // Its own describe so the fresh-page reload does NOT also fire for the shared-page block below:
    // an outer beforeEach runs for nested describes too, booting a whole app instance per nested test
    // that nothing then uses.
    test.describe('cold load', () => {
      defineTest('landing', async ({ page, user }) => {
        test.skip(denied(user, requires), unavailable);
        const collection = build(page);
        await collection.reload();
        // No probe: the permission check above already decided this. If the user holds it and the app
        // still does not render the page, open() fails loudly rather than the test quietly skipping.
        await collection.open();
        await collection.expectOpen();
        await gui.checkScreenshot(page, `${prefix}-landing`, await collection.masks());
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
        // Clearing the filters clears the SORTING with them, and a collection with no sort comes back
        // from the server in whatever order it likes - the same rows in a different sequence on the
        // next run, which makes every screenshot of that table a coin flip (measured on Lisa's
        // licences: 17-32k differing pixels on a table that was perfectly correct). Declaring
        // `stableSort` puts a known order back before each test. One click, from the just-cleared
        // state, so it is the same direction every time.
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
          // Same spelling as fixtures' own requires(), bound to this block's user - so a flow written
          // here states the permission it needs in one line, exactly like a declared scenario does in
          // its `requires`, and nobody has to remember why it is `=== false` rather than `!`.
          requires(permission) {
            test.skip(specUser.can(permission) === false, `${specUser.key} was not granted ${permission}`);
          },
          // Getters: these are only assigned once beforeAll has run, so a plain value captured here
          // would be undefined for every test in the block.
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
  let calls = 0;
  let from = 0;
  for (;;) {
    const start = source.indexOf(call, from);
    if (start === -1) break;
    calls++;
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
  // "No declaration here" and "there IS one but it could not be read" are different answers, and only
  // the first is safe to act on. A declaration that references a module-level constant does not evaluate
  // in this empty scope; returning null for it would tell a pruning caller the file declares nothing,
  // and every real baseline it owns would look like an orphan.
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
    // Gated on the permission the scenario DECLARES, checked against what the server granted at login -
    // not on whether the button happened to have rendered yet. The old probe returned false on timeout,
    // so a slow command bar skipped the test and the run stayed green.
    test.skip(denied(user, scenario.requires), `${scenario.command} needs ${scenario.requires}`);
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
    // Restore by DEFAULT: neither the page size nor the current page survives clearAllFilters(), so
    // leaving them set silently changes what every later test in this serial block is looking at. Opt
    // out only for a scenario that is deliberately the last one and wants the state left as it is.
    const { size = 50, page: pageNumber = 2, restore = true } = scenario.pagination;
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
