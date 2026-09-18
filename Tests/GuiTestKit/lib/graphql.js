// Spy on GraphQL requests to structurally confirm a Save/mutation actually round-tripped and succeeded
// server-side, independent of any screenshot comparison. The command id appears verbatim in the POST
// body, so matching on it identifies the operation without parsing GraphQL.
//
// Start the spy BEFORE the triggering action (the round-trip can complete before your await returns):
//   const spy = gql.spyOn(page, 'UpdateDeviceFromRepresentation');
//   await editor.save();
//   const result = await spy.wait();
//   expect(result.errors, 'server-side save must succeed').toBeFalsy();

/**
 * Begin watching for the next GraphQL POST whose body mentions `operationName`. Must be called before
 * the action that triggers it.
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
  // Absorb the rejection here so a timeout while the action throws can't tear down the worker; re-throw
  // only from wait(), where a caller is listening.
  const settled = responsePromise.then(
    (response) => ({ response }),
    (error) => ({ error })
  );

  return {
    async wait() {
      const { response, error } = await settled;
      if (error) throw error;
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
