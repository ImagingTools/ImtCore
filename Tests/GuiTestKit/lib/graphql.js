// Spy on GraphQL requests to structurally confirm a Save/mutation actually round-tripped and
// succeeded server-side, independent of any screenshot pixel comparison - a Save whose server call
// silently failed (network error, validation rejection) but left the UI LOOKING saved (the screenshot
// only captures whatever the client currently renders) would otherwise be invisible to the suite.
//
// Every ImtCore GraphQL call goes through GqlRequestSender.qml's send(), which builds
// `Gql.GqlRequest(type, gqlCommandId)` - the command id (e.g. "UpdateDeviceFromRepresentation",
// "CreateNewDocument") appears verbatim in the outgoing query text, so matching on it in the POST body
// reliably identifies the operation without needing to parse GraphQL syntax.
//
// Usage - start the spy BEFORE the triggering action (the network round-trip can complete before your
// `await` on the action itself returns), then await its result after:
//
//   const spy = gql.spyOn(page, 'UpdateDeviceFromRepresentation');
//   await editor.save();
//   const result = await spy.wait();
//   expect(result.errors, 'server-side save must succeed').toBeFalsy();

/**
 * Begin watching for the next GraphQL POST whose body mentions `operationName`. Must be called before
 * the action that triggers it (mirrors Playwright's own `page.waitForResponse` ordering requirement).
 * @param {import('@playwright/test').Page} page
 * @param {string} operationName  the gqlCommandId, e.g. "UpdateDeviceFromRepresentation"
 * @param {{timeout?: number}} [opts]
 * @returns {{ wait: () => Promise<{status: number, errors: any, data: any}> }}
 */
function spyOn(page, operationName, opts = {}) {
  const timeout = opts.timeout || 15000;
  const responsePromise = page.waitForResponse(
    (resp) => resp.request().method() === 'POST' && matchesOperation(resp.request(), operationName),
    { timeout }
  );

  return {
    async wait() {
      const response = await responsePromise;
      let json;
      try {
        json = await response.json();
      } catch (err) {
        throw new Error(`GraphQL spy on "${operationName}": response body was not JSON (${err.message})`);
      }
      return { status: response.status(), errors: json.errors, data: json.data };
    },
  };
}

function matchesOperation(request, operationName) {
  const postData = request.postData();
  return typeof postData === 'string' && postData.includes(operationName);
}

module.exports = { spyOn };
