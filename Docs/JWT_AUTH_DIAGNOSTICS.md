# JWT authentication diagnostics

This note describes how access-token validation works end-to-end, what log lines
to expect after idle timeouts, and the status-code mapping used by the GraphQL
HTTP and WebSocket entry points.

## Components

| Layer | Component | Role |
| --- | --- | --- |
| Access JWT create / validate | `CJwtSessionControllerComp` | Signs JWT, checks `exp` + session store |
| Remote slave (app servers) | `CRemoteJwtSessionControllerComp` | Validates via GQL to the auth service |
| Request-path cache | `CAuthenticationManagerComp` | Caches validated tokens, builds `IGqlContext` |
| HTTP entry | `CHttpGraphQLServletComp` | Maps context errors → HTTP status |
| WebSocket entry | `CWebSocketServletComp` | Maps context errors → WS `error` payload |
| Client refresh | `AuthorizationController.qml` | Single-flight refresh ahead of the expiry and on 401 / logout |
| Client WS | `SubscriptionManager.qml` | On `Unauthorized`/`Forbidden` asks for refresh |
| Access changes | `CAccessChangePublisherComp` (`OnAccessChanged`) | Tells a session its permissions changed; the client reads them again |

## Defaults

| Setting | Default | Meaning |
| --- | --- | --- |
| `JwtTime` | 3600 s | Access-token lifetime |
| `RefreshTokenTime` | 86400 s | Session / refresh lifetime (sliding on refresh) |
| `TokenCacheTtl` | 300 s | Auth-manager cache entry TTL (capped by JWT `exp`) |
| Client proactive refresh | 80 % of `exp - iat` | `AuthorizationController.refreshAheadRatio`; a token without `iat` is refreshed on 401 only |
| `RefreshTokenReuseLeeway` | 30 s | Applies when the successor has also been rotated: refuse reuse within the leeway, otherwise attempt to remove the session |

## Status mapping (`ResolveUserId`)

| Slave / local outcome | Context status | HTTP | Client expectation |
| --- | --- | --- | --- |
| Cache hit / `JS_OK` | `CCS_OK` | 200 | Continue |
| `JS_EXPIRED` (token or session) | `CCS_UNAUTHORIZED` | **401** | Refresh access token |
| `JS_INVALID` (signature / malformed) | `CCS_FORBIDDEN` | **403** | Refresh once; logout if refresh fails |
| `JS_NONE` (remote transport / setup failure) | `CCS_INTERNAL_ERROR` | **500** | Do **not** force-logout |
| Slave / PAT manager missing | `CCS_INTERNAL_ERROR` | **500** | Operational misconfiguration |
| Empty token | anonymous context | 200 | Permission checks see no user |

**Important:** remote validation failures used to be folded into `JS_NONE` → 403 →
client `logoutForce()`. They are now 500 so transient auth-service issues after
idle do not log the user out.

A fresh session-level denial evicts cached token validations and computed
authorizations for that session. An older validation, refresh, or authorization
reply cannot refill those caches. Expiry of just one access token does not
revoke its siblings. A forbidden authorization forces that credential to be
checked again; an unavailable source is not evidence that a session was closed.
A computed authorization is cached only until the earliest of its cache TTL,
JWT `exp` and the source's `sessionExpiresAt` (when supplied).

## Log messages to look for

### Auth manager (`CAuthenticationManagerComp`)

- `ResolveUserId: token expired (client should refresh)` — normal after JWT lifetime.
- `ResolveUserId: slave returned JS_NONE … returning INTERNAL_ERROR, not FORBIDDEN` — remote flake.
- `ResolveUserId: token rejected as invalid` — bad signature / permanently invalid JWT.
- `ValidateJwt: slave rejected token … with state N` — same for direct `ValidateJwt` callers.

### Remote slave (`CRemoteJwtSessionControllerComp`)

- `Remote ValidateJwt: request failed for token ***xxxx: <error>` — GQL/network failure.
- `Remote ValidateJwt: response missing 'state'` — auth service returned an incomplete payload.

### Local JWT store (`CJwtSessionControllerComp`)

- `JWT rejected: token expired by time, exp = … (now = …, age = …s)` — access token past `exp`.
- `JWT rejected: session absent or expired` — the store confirmed the denial.
- `JWT validation unavailable: session could not be checked` — a store read failed.
- `Refresh rejected: …` — refresh token unknown, already rotated, or session dead.
- `Refresh token reuse: session removal failed; retaining rotation history` — the
  session was **not** confirmed removed. A later reuse can retry removal.
  Rotation history is local to this controller, not persisted across restarts.

Neither the auth manager nor the local JWT controller logs access tokens,
refresh tokens or token suffixes.

### HTTP servlet (`CHttpGraphQLServletComp`)

- `HTTP GraphQL auth rejected (401 Unauthorized) for Command-ID '…', token ***xxxx: <reason>`
- `HTTP GraphQL auth rejected (403 Forbidden) for Command-ID '…', token ***xxxx: <reason>`
- Critical for internal context creation failures (500).

### WebSocket servlet (`CWebSocketServletComp`)

- `WebSocket subscription auth failed (401 Unauthorized|403 Forbidden|internal error) for command '…', token ***xxxx: <reason>`

Wire payload `message` carries the full reason, with a status prefix the client
can still match:

- `Unauthorized: JWT token expired.`
- `Forbidden: Invalid JWT token.`
- (internal) bare `contextError.message`, e.g. `JWT validation temporarily unavailable.`

### Client (`AuthorizationController` / `SubscriptionManager`)

- `Auth: HTTP 401 Unauthorized` / `Auth: HTTP 403 Forbidden` → reactive `handleAuthFailure`
- `Auth: refreshing access token` / `refresh succeeded` / `refresh failed — logout`
- `Auth: no refresh token — logout`
- `SubscriptionManager: auth failure on subscription — requesting token refresh`

## Idle scenarios

1. **Idle &lt; JWT lifetime, cache expired (≈5 min)**  
   Cache miss → remote `ValidateJwt`. If remote is healthy → 200. If remote fails → **500**, not logout.

2. **Idle &gt; JWT lifetime, session still alive**  
   401 → single-flight refresh → new access token → queued HTTP retries + WS re-register for failed subscriptions.

3. **Idle &gt; RefreshTokenTime**  
   401 → refresh rejected → logout. Server log shows session expired past `RefreshTokenTime`.

## Client auth rules

The client reads `iat` and `exp` of its access token (the lifetime, independent of its own clock) and refreshes at `refreshAheadRatio` of it. A refusal is handled as follows:

1. Any HTTP **401/403** or WS auth error → `handleAuthFailure`
2. If a **refresh token** is present → single-flight `RefreshToken` mutation; on success retry queued HTTP + re-register failed WS subscriptions
3. If refresh is missing or rejected → **forced logout** → login page
4. Concurrent failures share one refresh (single-flight)
5. Before a refresh the client can take a rotated token from shared settings only if those settings identify the same session. Retrying the immediate predecessor while its successor is still current returns that successor. Reuse of an older predecessor after `RefreshTokenReuseLeeway` triggers an attempt to remove the session for all tabs.
6. A request refused with 401 ends once: `finished(1)` after it was sent again, or `onAuthorizationLost()` → `finished(-1)` when it is dropped
7. A logout refused for an expired token refreshes the session with its refresh token and closes it with the new one
8. `OnAccessChanged` (granted or taken permissions) → `GetPermissions` → `accessChanged()`: the pages and commands are read again

## Remote collection changes

The transport does not invalidate user, role or group caches. Remote collection
caches observe their source models and configured dependencies. A write through
a cached collection invalidates its local cache in the write path. If an operation
bypasses that path, as with a remote write through ProLife, the receiving server
invalidates on the `RemoteRepository` model notification. That delivery is
asynchronous: a successful write response does not guarantee that another
server's collection cache already reflects the change. Do not treat
`OnAccessChanged`, which asks the UI to reread permissions, as that guarantee.
