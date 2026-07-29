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
| Client refresh | `AuthorizationController.qml` | Reactive single-flight refresh / logout (no timers) |
| Client WS | `SubscriptionManager.qml` | On `Unauthorized`/`Forbidden` asks for refresh |

## Defaults

| Setting | Default | Meaning |
| --- | --- | --- |
| `JwtTime` | 3600 s | Access-token lifetime |
| `RefreshTokenTime` | 86400 s | Session / refresh lifetime (sliding on refresh) |
| `TokenCacheTtl` | 300 s | Auth-manager cache entry TTL (capped by JWT `exp`) |
| Client proactive refresh | none | Client is reactive only (no JWT timers) |

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

## Log messages to look for

### Auth manager (`CAuthenticationManagerComp`)

- `ResolveUserId: token ***xxxx expired (client should refresh)` — normal after JWT lifetime.
- `ResolveUserId: slave returned JS_NONE … returning INTERNAL_ERROR, not FORBIDDEN` — remote flake.
- `ResolveUserId: token ***xxxx rejected as invalid` — bad signature / permanently invalid JWT.
- `ValidateJwt: slave rejected token … with state N` — same for direct `ValidateJwt` callers.

### Remote slave (`CRemoteJwtSessionControllerComp`)

- `Remote ValidateJwt: request failed for token ***xxxx: <error>` — GQL/network failure.
- `Remote ValidateJwt: response missing 'state'` — auth service returned an incomplete payload.

### Local JWT store (`CJwtSessionControllerComp`)

- `JWT rejected: token expired by time, exp = … (now = …, age = …s)` — access token past `exp`.
- `JWT rejected: no session record found for session-ID …` — session removed / never existed.
- `JWT rejected: session '…' expired at … — the client was idle longer than RefreshTokenTime`.
- `Refresh rejected: …` — refresh token unknown, already rotated, or session dead.

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

## Client auth rules (reactive only)

Client does **not** parse JWT `exp` and has **no** refresh timers. Server lifetimes decide when auth fails.

1. Any HTTP **401/403** or WS auth error → `handleAuthFailure`
2. If a **refresh token** is present → single-flight `RefreshToken` mutation; on success retry queued HTTP + re-register failed WS subscriptions
3. If refresh is missing or rejected → **forced logout** → login page
4. Concurrent failures share one refresh (single-flight)
