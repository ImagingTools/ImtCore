# Testing JQML v4 applications (Playwright and others)

The application is drawn on a canvas, so a test has no DOM tree to look into. Every build, release ones too,
publishes `window.qmlTest`: an API that finds the items of the running application by their `objectName`, reads them and drives
them with the pointer and the keyboard. `Tools/JQML/v4/playwright/qml.mjs` is a client for Playwright on top of it.

It needs no flag: the page has `window.qmlTest` as soon as it is loaded (`started` is true when the first document is built).

## Finding items

`find(scope, selector)` looks for the items **below `scope`**: a handle of an item that was found before, or `null` for the
whole application. This is the way to deal with an `objectName` that many items have: find the parents first and search from them.

```js
const forms = qmlTest.find(null, 'form');              // two forms with the same name
const ok = qmlTest.find(forms[1].id, 'ok');            // the button of the second one only
```

The selector is a small CSS-like text:

| Selector | Meaning |
| --- | --- |
| `save`, `#save`, `"Save as"` | the `objectName` (`*` and `?` are wildcards: `row*`) |
| `@Rectangle`, `@ImtButton`, `@ImtButton#ok` | the type: the QML type, or the name of the component where it is used |
| `[text="OK"]`, `[width>0]`, `[model.count>=3]` | a property: `=`, `!=`, `*=` (contains), `^=`, `$=`, `>`, `<`, `>=`, `<=`; `[prop]` is "has it" |
| `:visible`, `:enabled`, `:disabled` | the state; it follows a name, a type or a quoted name: `save:visible`, `row*:enabled`, `@Text:visible`, `"Save as":disabled` |
| `a b` / `a > b` / `> b` | `b` anywhere under `a` / directly under `a` / directly under the scope |

A name has letters of any alphabet, digits and `_ - . : $ * ?` (`ПользовательElementView`, `#ПриборучетаButton`); a name with a space or any
other character is quoted. A `:` that is not followed by `visible`, `enabled` or `disabled` stays in the name (`ns:name`).

The items come in the order of the tree. Hidden items and items without a size are found as well (use `:visible`).
Items that do not exist are not found: a row of a list that was not built yet has to be scrolled to first.

## The API of `window.qmlTest`

All the arguments and results are JSON. An item is a handle `{ id, objectName, type, qmlType }`; the handle of a destroyed item
fails with a message that starts with `STALE:`.

* `find(scope, selector, { limit })`, `count(scope, selector)`, `parent(id)`, `children(id)`, `closest(id, selector)`
* `info(id)`: `text`, `visible`, `enabled`, `opacity`, `box` and `visibleBox` (in page pixels, like `getBoundingClientRect`), `parent`, `focus`
* `get(id, name)`, `set(id, name, value)`, `call(id, name, args)`
* `actionability(id)`: whether a click can reach the item, and why not (`not visible`, `is disabled`, `is covered by MouseArea 'cover'`, ...)
* `click(id, { button, clickCount, position, force })`, `hover`, `focus`, `fill(id, text)`, `type(text)`, `press('Control+a')`, `wheel`, `scrollIntoView`
* `tree(id, { depth, named, boxes })`, `dump(id)`: the tree as text, for the message of a failing test
* `whenIdle({ timeout })`: resolves when no frame is wanted and no animation, picture or request is in progress
* `errors()`, `logs()`, `started`

Input goes through the scene as synthetic pointer and key events, so the handlers of the application run as for a real user.
`click` first scrolls the item into the views that hold it (`Flickable`, `ListView`) and checks it can be hit; `force: true` skips this.

## Playwright

```js
import { qml } from 'Tools/JQML/v4/playwright/qml.mjs';

const app = qml(page);
await app.ready();                                     // started and idle

const form = app.locator('loginForm');
await form.locator('login').fill('su');
await form.locator('password').fill('secret');
await form.locator('okButton').click();                // waits until the button exists and can be clicked

const row = app.locator('table').locator('row').nth(2);
await row.locator('title').waitForText('Pump 3');
console.log(await app.dump());                         // the tree, when something is wrong
```

A locator is a description, not an item: it is resolved at every action, so it works when the item is rebuilt. `locator()` on a
locator searches only below the item it found. A locator without `nth()` that finds several items fails (it does not guess), and the
message lists them. Actions wait up to `timeout` (5 s by default, `qml(page, { timeout })`) for the item to exist and to be actionable.

## Tests of this repository

* `node test/testapi.mjs`: selectors, scoped search, info, actionability, clicks, keys, scrolling, stale handles
* `node test/playwright-client.mjs`: the client with a fake `page` that runs on the same API (real Playwright is not needed)

The page was also checked in the built-in browser. The client was not run with real Playwright.
A page in a background tab gets no animation frames: the API goes on by a timer then, but the canvas may have no size.
