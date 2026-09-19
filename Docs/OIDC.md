# OIDC Support in ImtCore
# OIDC-Unterstützung in ImtCore
# Поддержка OIDC в ImtCore

---

## Table of Contents / Inhaltsverzeichnis / Оглавление

1. [English](#english)
2. [Deutsch](#deutsch)
3. [Русский](#русский)

---

<a name="english"></a>
# English

## Overview

ImtCore provides a complete OpenID Connect (OIDC) provider implementation built on top of the existing ACF component model. The implementation is fully compliant with the OAuth 2.0 Authorization Framework (RFC 6749) and the OpenID Connect Core 1.0 specification. It coexists with the existing HMAC-SHA256 session system (`CJwtSessionControllerComp`) and adds an independent RS256-based token infrastructure.

## Architecture

The OIDC feature spans three ImtCore modules:

| Module | Location | Responsibility |
|--------|----------|----------------|
| `imtauth` | `Include/imtauth/` | Interfaces, data models, RS256 JWT provider |
| `imtoidc` | `Include/imtoidc/` | REST servlet, scope-to-claims mapper |
| `imtauthdb` | `Include/imtauthdb/` | Database delegates, SQL schemas |

Package registrations:
- **`ImtAuthPck`** — registers `OidcClient`, `OidcAuthorizationCode`, `OidcTokenInfo`, `Rs256JwtTokenProvider` data model components.
- **`ImtAuthDbPck`** — registers `OidcClientDatabaseDelegate`, `OidcTokenDatabaseDelegate` for SQL persistence.
- **`ImtOidcPck`** — registers `OidcServlet` (REST servlet) and `OidcScopeMapper`.

---

## Phase 1 — OIDC Data Structures & Interfaces (`Include/imtauth/`)

### `IOidcClient` / `COidcClient`

**File:** `Include/imtauth/IOidcClient.h`, `Include/imtauth/COidcClient.h/.cpp`

Represents a registered OAuth 2.0 / OIDC client application. Each client has:

| Field | Type | Description |
|-------|------|-------------|
| `ClientId` | `QByteArray` | Unique client identifier (UUID) |
| `ClientSecretHash` | `QByteArray` | SHA-256 hash of the client secret (never stored in plaintext) |
| `ClientName` | `QString` | Human-readable application name |
| `RedirectUris` | `QStringList` | Whitelist of allowed redirect URIs |
| `GrantTypes` | `QByteArrayList` | Allowed grant types (see enum below) |
| `Scopes` | `QByteArrayList` | Allowed OAuth 2.0 scopes |
| `CreatedAt` | `QDateTime` | UTC registration timestamp |

**`OidcGrantType` enum** (declared inside `IOidcClient` with `I_DECLARE_ENUM`):

```
OGT_AUTHORIZATION_CODE   – Authorization Code Flow (user-interactive)
OGT_CLIENT_CREDENTIALS   – Client Credentials Flow (machine-to-machine)
OGT_REFRESH_TOKEN        – Refresh Token exchange
```

`COidcClient` implements full `iser::IChangeable` semantics: `CopyFrom`, `IsEqual`, `CloneMe`, `ResetData`, and `Serialize` (versioned via `CArchiveTag`).

---

### `IOidcAuthorizationCode` / `COidcAuthorizationCode`

**File:** `Include/imtauth/IOidcAuthorizationCode.h`, `Include/imtauth/COidcAuthorizationCode.h/.cpp`

Short-lived one-time-use code issued during the Authorization Code Flow and exchanged at the token endpoint. Supports **PKCE** (Proof Key for Code Exchange, RFC 7636) for public clients.

| Field | Type | Description |
|-------|------|-------------|
| `Code` | `QByteArray` | Opaque authorization code value (UUID) |
| `ClientId` | `QByteArray` | Issuing client |
| `UserId` | `QByteArray` | Authorizing user |
| `Scope` | `QString` | Space-separated scope string |
| `RedirectUri` | `QString` | Redirect URI used in the authorization request |
| `ExpirationDate` | `QDateTime` | Expiry (10 minutes from issuance) |
| `CodeChallenge` | `QByteArray` | PKCE code challenge (Base64url-encoded SHA-256 or plain) |
| `CodeChallengeMethod` | `QByteArray` | `"S256"` or `"plain"` |
| `Used` | `bool` | One-time-use flag — set to `true` after exchange |

---

### `IOidcTokenInfo` / `COidcTokenInfo`

**File:** `Include/imtauth/IOidcTokenInfo.h`, `Include/imtauth/COidcTokenInfo.h/.cpp`

Tracks every issued access or refresh token for revocation and introspection.

| Field | Type | Description |
|-------|------|-------------|
| `TokenId` | `QByteArray` | JWT `jti` claim — unique token ID |
| `ClientId` | `QByteArray` | Client the token was issued to |
| `UserId` | `QByteArray` | Associated user (empty for `client_credentials`) |
| `Scope` | `QString` | Granted scope |
| `TokenType` | `OidcTokenType` | `OTT_ACCESS` or `OTT_REFRESH` |
| `ExpirationDate` | `QDateTime` | UTC expiry |
| `IsRevoked` | `bool` | Revocation flag |
| `CreatedAt` | `QDateTime` | UTC issuance timestamp |

**`OidcTokenType` enum** uses `I_SERIALIZE_ENUM` for serialization.

---

### `IOidcScopeMapper`

**File:** `Include/imtauth/IOidcScopeMapper.h`

Interface for translating OAuth 2.0 scopes into user claims. Implementations query the ImtCore RBAC model.

```cpp
virtual QJsonObject MapScopesToClaims(const QByteArray& userId,
                                       const QByteArrayList& scopes) const = 0;
virtual QByteArrayList GetSupportedScopes() const = 0;
virtual QByteArrayList GetSupportedClaims() const = 0;
```

---

### `IJwtTokenProvider`

**File:** `Include/imtauth/IJwtTokenProvider.h`

Interface for asymmetric (RS256) JWT operations. Separate from the existing `CJwtSessionControllerComp` which uses HMAC-SHA256 for internal sessions.

```cpp
virtual QByteArray GenerateToken(const QJsonObject& claims) const = 0;
virtual bool ValidateToken(const QByteArray& jwt, QJsonObject& claims) const = 0;
virtual QByteArray GetJwks() const = 0;
virtual QByteArray GetKeyId() const = 0;
```

---

### `CRs256JwtTokenProviderComp`

**File:** `Include/imtauth/CRs256JwtTokenProviderComp.h/.cpp`

Concrete ACF component implementing `IJwtTokenProvider` using OpenSSL's EVP API (compatible with OpenSSL 1.x and 3.x).

**ACF configuration parameters (`I_ASSIGN`):**

| Parameter | Required | Default | Description |
|-----------|----------|---------|-------------|
| `PrivateKeyPath` | ✓ | — | Path to PEM file containing the RSA private key |
| `KeyId` | ✗ | `"imtcore-oidc-key-1"` | Key identifier (`kid`) in JWT header and JWKS |
| `TokenLifetime` | ✗ | `3600` | Access token lifetime in seconds |
| `Issuer` | ✓ | — | OIDC Issuer URL (used as `iss` claim) |

**Internal behaviour:**
- On `OnComponentCreated()`, the private key PEM file is loaded and validated.
- `GenerateToken()` constructs the JWT header (`alg: RS256`, `typ: JWT`, `kid`), merges with provided claims, adds `iat`/`exp`/`iss` if absent, and signs using `EVP_DigestSign`.
- `ValidateToken()` verifies the RSA signature and checks `exp`.
- `GetJwks()` extracts the RSA public key's modulus (`n`) and exponent (`e`) as Base64url-encoded values and builds a standard JWKS JSON document. The result is cached after the first call.

---

## Phase 2 — OIDC REST Endpoints (`Include/imtoidc/`)

### `COidcServletComp`

**File:** `Include/imtoidc/COidcServletComp.h/.cpp`

Extends `imtrest::CHttpServletCompBase`. Dispatches HTTP requests to endpoint handlers based on the last path segment of the command ID.

**ACF dependencies (`I_ASSIGN`):**

| Parameter | Interface | Description |
|-----------|-----------|-------------|
| `JwtTokenProvider` | `IJwtTokenProvider` | RS256 token signing & validation |
| `ScopeMapper` | `IOidcScopeMapper` | Scope → claims mapping |
| `CredentialController` | `ICredentialController` | User authentication |
| `UserInfoProvider` | `IUserInfoProvider` | User profile lookup |
| `ClientCollection` | `IObjectCollection` | OIDC client registry |
| `AuthCodeCollection` | `IObjectCollection` | Authorization code store |
| `TokenCollection` | `IObjectCollection` | Token tracking store |
| `IssuerUrl` | attribute | Base OIDC issuer URL |

#### Supported Endpoints

##### `GET /.well-known/openid-configuration`
Returns the OIDC Provider Metadata document. Includes:
- `issuer`, `authorization_endpoint`, `token_endpoint`, `userinfo_endpoint`, `jwks_uri`, `revocation_endpoint`, `introspection_endpoint`
- `response_types_supported`: `["code"]`
- `grant_types_supported`: `["authorization_code", "client_credentials", "refresh_token"]`
- `id_token_signing_alg_values_supported`: `["RS256"]`
- `scopes_supported`, `claims_supported` (from `IOidcScopeMapper`)
- `code_challenge_methods_supported`: `["S256", "plain"]`

##### `GET /oauth/authorize`
Authorization Code endpoint (RFC 6749 §4.1):
1. Validates `client_id` against `ClientCollection`.
2. Verifies `redirect_uri` is whitelisted for the client.
3. Accepts optional PKCE parameters (`code_challenge`, `code_challenge_method`).
4. Generates a UUID authorization code, stores it in `AuthCodeCollection`.
5. Returns `code` + `state` (JSON — ACC wiring must redirect the browser).

##### `POST /oauth/token`
Token endpoint. Supports three grant types:

**`authorization_code`:**
1. Validates the authorization code (existence, expiry, used-flag, client match).
2. Verifies PKCE `code_verifier` if `code_challenge` was used.
3. Marks the code as used (replay prevention).
4. Issues: `access_token` (RS256 JWT), `id_token` (RS256 JWT with user claims from scope mapper), `refresh_token` (RS256 JWT with 24 h lifetime).
5. Stores token metadata in `TokenCollection`.

**`client_credentials`:**
1. Issues an `access_token` with `sub` = `client_id`.
2. No `id_token` or `refresh_token`.

**`refresh_token`:**
1. Validates the refresh JWT (signature + expiry + `token_type` claim).
2. Issues a new `access_token`.

Client authentication is accepted via HTTP Basic Auth (`Authorization: Basic ...`) or form body (`client_id` / `client_secret`). The client secret is validated against the stored SHA-256 hash.

##### `GET /oauth/userinfo`
Validates the Bearer token from the `Authorization` header, extracts `sub` and `scope`, calls `IOidcScopeMapper::MapScopesToClaims()`, and returns the resulting JSON claims object.

##### `GET /oauth/jwks`
Returns the JWKS document from `IJwtTokenProvider::GetJwks()`.

##### `POST /oauth/revoke` (RFC 7009)
Looks up the token by ID in `TokenCollection`, sets `IsRevoked = true`. Always returns HTTP 200 (even if the token is unknown).

##### `POST /oauth/introspect` (RFC 7662)
Validates the JWT signature and expiry, checks `IsRevoked` in `TokenCollection`, and returns an introspection response with `active`, `sub`, `client_id`, `scope`, `exp`, `iat`, `iss`.

---

### `COidcScopeMapperComp`

**File:** `Include/imtoidc/COidcScopeMapperComp.h/.cpp`

Implements `IOidcScopeMapper` by querying the ImtCore RBAC user model.

**Scope → Claims mapping:**

| Scope | Claims produced | Source interface |
|-------|----------------|-----------------|
| `openid` | `sub` | Always included |
| `profile` | `name`, `given_name`, `family_name` | `IPersonBaseInfo` |
| `email` | `email`, `email_verified` | `IContactBaseInfo` |
| `roles` | `roles` (JSON array of role IDs) | `IUserRoleInfo` |
| `permissions` | `permissions` (JSON array of feature IDs) | `IUserInfo::GetPermissions()` |

**`GetSupportedScopes()`** returns: `openid`, `profile`, `email`, `roles`, `permissions`.

---

## Phase 3 — Database Schema & Delegates (`Include/imtauthdb/`)

### SQL Schemas

All tables exist in both **PostgreSQL** and **SQLite** variants, stored as Qt resources (`.qrc`).

#### `OidcClients`

```sql
-- PostgreSQL
CREATE TABLE IF NOT EXISTS "OidcClients" (
    "Id"               uuid PRIMARY KEY,
    "ClientSecretHash" bytea       NOT NULL DEFAULT '',
    "ClientName"       text        NOT NULL DEFAULT '',
    "RedirectUris"     text        NOT NULL DEFAULT '',  -- comma-separated
    "GrantTypes"       text        NOT NULL DEFAULT '',  -- comma-separated
    "Scopes"           text        NOT NULL DEFAULT '',  -- comma-separated
    "CreatedAt"        timestamp without time zone NOT NULL DEFAULT now()
);
```

#### `OidcAuthCodes`

```sql
CREATE TABLE IF NOT EXISTS "OidcAuthCodes" (
    "Id"                   text PRIMARY KEY,         -- UUID
    "ClientId"             uuid        NOT NULL,
    "UserId"               uuid        NOT NULL,
    "Scope"                text        NOT NULL DEFAULT '',
    "RedirectUri"          text        NOT NULL DEFAULT '',
    "ExpiresAt"            timestamp without time zone NOT NULL,
    "CodeChallenge"        text        NOT NULL DEFAULT '',
    "CodeChallengeMethod"  text        NOT NULL DEFAULT 'plain',
    "Used"                 boolean     NOT NULL DEFAULT false
);
```

#### `OidcTokens`

```sql
CREATE TABLE IF NOT EXISTS "OidcTokens" (
    "Id"        uuid PRIMARY KEY,   -- = JWT jti claim
    "ClientId"  uuid        NOT NULL,
    "UserId"    uuid,               -- NULL for client_credentials
    "Scope"     text        NOT NULL DEFAULT '',
    "TokenType" text        NOT NULL DEFAULT 'access', -- 'access' | 'refresh'
    "ExpiresAt" timestamp without time zone NOT NULL,
    "IsRevoked" boolean     NOT NULL DEFAULT false,
    "CreatedAt" timestamp without time zone NOT NULL DEFAULT now()
);
```

Resource file: `Include/imtauthdb/imtauthdb.qrc` (updated to include the 6 new SQL files).

### DB Delegates

#### `COidcClientDbDelegateComp`

**File:** `Include/imtauthdb/COidcClientDbDelegateComp.h/.cpp`

Extends `imtdb::CSqlDatabaseObjectDelegateCompBase`. On `OnComponentCreated()` it creates the `OidcClients` table if it does not exist, using the SQL resource for the active database driver. Uses `imtdb::SqlEncode()` for all string values.

#### `COidcTokenDbDelegateComp`

**File:** `Include/imtauthdb/COidcTokenDbDelegateComp.h/.cpp`

Same pattern for the `OidcTokens` table. The update path only modifies `IsRevoked` (immutable token fields are never changed after issuance).

> **Note:** Authorization codes (`OidcAuthCodes`) share the token collection infrastructure. A dedicated auth-code delegate can be added as a future extension.

---

## Phase 4 — RBAC Integration

`COidcScopeMapperComp` bridges OAuth 2.0 scopes to ImtCore's Puma RBAC model:

- It receives a `UserCollection` dependency (`IObjectCollection`) from which it looks up the user object by ID.
- The user object is cast to `IPersonBaseInfo`, `IContactBaseInfo`, and `IUserRoleInfo` as needed.
- Custom scopes can be supported by subclassing `COidcScopeMapperComp` or providing an alternative `IOidcScopeMapper` implementation.

The token endpoint's `authorization_code` grant integrates with `ICredentialController` (the same interface used by the existing GraphQL `Authorization` mutation) for future user-password pre-authentication before the authorization code is generated.

---

## Phase 5 — Package Registration & Build

### `ImtOidcPck`

**File:** `Impl/ImtOidcPck/ImtOidcPck.h/.cpp`

Exports:
- `OidcServlet` → `COidcServletComp`
- `OidcScopeMapper` → `COidcScopeMapperComp`

### `ImtAuthPck` additions

New component registrations:
- `OidcClient` → `COidcClient` (wrapped via `TModelCompWrap<TMakeComponentWrap<...>>`)
- `OidcAuthorizationCode` → `COidcAuthorizationCode`
- `OidcTokenInfo` → `COidcTokenInfo`
- `Rs256JwtTokenProvider` → `CRs256JwtTokenProviderComp`

### `ImtAuthDbPck` additions

- `OidcClientDatabaseDelegate` → `COidcClientDbDelegateComp`
- `OidcTokenDatabaseDelegate` → `COidcTokenDbDelegateComp`

### CMake

`Include/imtoidc/CMake/CMakeLists.txt`:
```cmake
project(imtoidc)
include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
target_link_libraries(${PROJECT_NAME} imtauth imtrest imtcrypt imtbase)
```

---

## Key Design Decisions

| Decision | Rationale |
|----------|-----------|
| RS256 (asymmetric) instead of HS256 | OIDC specification requires ID tokens to be verifiable by third parties. Asymmetric signing via the JWKS endpoint enables clients to verify tokens without a shared secret. |
| Separate from `CJwtSessionControllerComp` | The existing HS256 session system is unchanged. OIDC tokens have a different lifecycle (revocable, introspectable, storable). |
| PKCE support (RFC 7636) | Protects the authorization code flow for public clients (SPAs, mobile apps) that cannot securely store a client secret. |
| Token storage in `OidcTokens` | Enables revocation (RFC 7009) and introspection (RFC 7662) without parsing the JWT on every request. |
| `IOidcScopeMapper` interface | Allows applications to inject custom scope→claims logic without modifying the servlet. |
| `imtoidc` as separate module | Keeps `imtauth` and `imtauthdb` focused. `imtoidc` → `imtauth + imtrest + imtcrypt`. |

---

## Security Considerations

- Client secrets are stored as **SHA-256 hashes** only (`ClientSecretHash`). The plaintext secret is never persisted.
- Authorization codes are **single-use** (the `Used` flag is set atomically on exchange).
- PKCE `code_verifier` is validated with constant-time-equivalent string comparison.
- The `OidcTokens` table enables server-side **token revocation** independent of token expiry.
- The RSA private key file path is injected via ACF configuration (`PrivateKeyPath`); the key is never embedded in source code.
- OpenSSL EVP API is used for all cryptographic operations, supporting both OpenSSL 1.x and 3.x.

---

## File Inventory

```
Include/imtauth/
  IOidcClient.h
  COidcClient.h / .cpp
  IOidcAuthorizationCode.h
  COidcAuthorizationCode.h / .cpp
  IOidcTokenInfo.h
  COidcTokenInfo.h / .cpp
  IOidcScopeMapper.h
  IJwtTokenProvider.h
  CRs256JwtTokenProviderComp.h / .cpp

Include/imtoidc/
  COidcServletComp.h / .cpp
  COidcScopeMapperComp.h / .cpp
  CMake/CMakeLists.txt

Include/imtauthdb/
  COidcClientDbDelegateComp.h / .cpp
  COidcTokenDbDelegateComp.h / .cpp
  Resources/SQL/Postgres/CreateOidcClientsTable.sql
  Resources/SQL/Postgres/CreateOidcAuthCodesTable.sql
  Resources/SQL/Postgres/CreateOidcTokensTable.sql
  Resources/SQL/SQLite/CreateOidcClientsTable.sql
  Resources/SQL/SQLite/CreateOidcAuthCodesTable.sql
  Resources/SQL/SQLite/CreateOidcTokensTable.sql
  imtauthdb.qrc  (updated)

Impl/ImtAuthPck/
  ImtAuthPck.h / .cpp  (updated)

Impl/ImtAuthDbPck/
  ImtAuthDbPck.h / .cpp  (updated)

Impl/ImtOidcPck/
  ImtOidcPck.h / .cpp
```

---
---

<a name="deutsch"></a>
# Deutsch

## Überblick

ImtCore stellt eine vollständige OpenID Connect (OIDC) Provider-Implementierung bereit, die auf dem bestehenden ACF-Komponentenmodell aufbaut. Die Implementierung ist konform mit dem OAuth 2.0 Authorization Framework (RFC 6749) und der OpenID Connect Core 1.0 Spezifikation. Sie koexistiert mit dem bestehenden HMAC-SHA256-Session-System (`CJwtSessionControllerComp`) und fügt eine unabhängige RS256-basierte Token-Infrastruktur hinzu.

## Architektur

Das OIDC-Feature verteilt sich auf drei ImtCore-Module:

| Modul | Verzeichnis | Verantwortung |
|-------|-------------|---------------|
| `imtauth` | `Include/imtauth/` | Interfaces, Datenmodelle, RS256-JWT-Provider |
| `imtoidc` | `Include/imtoidc/` | REST-Servlet, Scope-zu-Claims-Mapper |
| `imtauthdb` | `Include/imtauthdb/` | Datenbank-Delegates, SQL-Schemata |

Package-Registrierungen:
- **`ImtAuthPck`** — registriert `OidcClient`, `OidcAuthorizationCode`, `OidcTokenInfo`, `Rs256JwtTokenProvider`.
- **`ImtAuthDbPck`** — registriert `OidcClientDatabaseDelegate`, `OidcTokenDatabaseDelegate`.
- **`ImtOidcPck`** — registriert `OidcServlet` (REST-Servlet) und `OidcScopeMapper`.

---

## Phase 1 — OIDC-Datenstrukturen & Interfaces (`Include/imtauth/`)

### `IOidcClient` / `COidcClient`

**Datei:** `Include/imtauth/IOidcClient.h`, `Include/imtauth/COidcClient.h/.cpp`

Repräsentiert eine registrierte OAuth 2.0 / OIDC-Client-Anwendung. Jeder Client besitzt:

| Feld | Typ | Beschreibung |
|------|-----|--------------|
| `ClientId` | `QByteArray` | Eindeutiger Client-Bezeichner (UUID) |
| `ClientSecretHash` | `QByteArray` | SHA-256-Hash des Client-Secrets (nie im Klartext gespeichert) |
| `ClientName` | `QString` | Menschenlesbarer Anwendungsname |
| `RedirectUris` | `QStringList` | Whitelist erlaubter Redirect-URIs |
| `GrantTypes` | `QByteArrayList` | Erlaubte Grant-Typen (siehe Enum unten) |
| `Scopes` | `QByteArrayList` | Erlaubte OAuth 2.0 Scopes |
| `CreatedAt` | `QDateTime` | UTC-Registrierungszeitstempel |

**`OidcGrantType`-Enum** (innerhalb von `IOidcClient` mit `I_DECLARE_ENUM` deklariert):

```
OGT_AUTHORIZATION_CODE   – Authorization Code Flow (nutzerinteraktiv)
OGT_CLIENT_CREDENTIALS   – Client Credentials Flow (Maschine-zu-Maschine)
OGT_REFRESH_TOKEN        – Refresh-Token-Austausch
```

`COidcClient` implementiert vollständige `iser::IChangeable`-Semantik: `CopyFrom`, `IsEqual`, `CloneMe`, `ResetData` und `Serialize` (versioniert über `CArchiveTag`).

---

### `IOidcAuthorizationCode` / `COidcAuthorizationCode`

**Datei:** `Include/imtauth/IOidcAuthorizationCode.h`, `Include/imtauth/COidcAuthorizationCode.h/.cpp`

Kurzlebiger Einmalcode, der beim Authorization Code Flow ausgestellt und am Token-Endpunkt eingelöst wird. Unterstützt **PKCE** (Proof Key for Code Exchange, RFC 7636) für öffentliche Clients.

| Feld | Typ | Beschreibung |
|------|-----|--------------|
| `Code` | `QByteArray` | Opaker Autorisierungscode (UUID) |
| `ClientId` | `QByteArray` | Ausstellender Client |
| `UserId` | `QByteArray` | Autorisierender Nutzer |
| `Scope` | `QString` | Leerzeichen-getrennte Scope-Zeichenkette |
| `RedirectUri` | `QString` | Redirect-URI aus der Autorisierungsanfrage |
| `ExpirationDate` | `QDateTime` | Ablauf (10 Minuten nach Ausstellung) |
| `CodeChallenge` | `QByteArray` | PKCE-Code-Challenge (Base64url-kodiertes SHA-256 oder Plain) |
| `CodeChallengeMethod` | `QByteArray` | `"S256"` oder `"plain"` |
| `Used` | `bool` | Einmal-Nutzungs-Flag — nach dem Einlösen auf `true` gesetzt |

---

### `IOidcTokenInfo` / `COidcTokenInfo`

**Datei:** `Include/imtauth/IOidcTokenInfo.h`, `Include/imtauth/COidcTokenInfo.h/.cpp`

Verfolgt jeden ausgestellten Access- oder Refresh-Token für Widerruf und Introspektion.

| Feld | Typ | Beschreibung |
|------|-----|--------------|
| `TokenId` | `QByteArray` | JWT-`jti`-Claim — eindeutige Token-ID |
| `ClientId` | `QByteArray` | Client, für den der Token ausgestellt wurde |
| `UserId` | `QByteArray` | Zugehöriger Nutzer (leer bei `client_credentials`) |
| `Scope` | `QString` | Gewährte Scopes |
| `TokenType` | `OidcTokenType` | `OTT_ACCESS` oder `OTT_REFRESH` |
| `ExpirationDate` | `QDateTime` | UTC-Ablaufzeitpunkt |
| `IsRevoked` | `bool` | Widerrufs-Flag |
| `CreatedAt` | `QDateTime` | UTC-Ausstellungszeitstempel |

Das **`OidcTokenType`-Enum** verwendet `I_SERIALIZE_ENUM` zur Serialisierung.

---

### `IOidcScopeMapper`

**Datei:** `Include/imtauth/IOidcScopeMapper.h`

Interface zur Übersetzung von OAuth 2.0 Scopes in Nutzer-Claims. Implementierungen befragen das RBAC-Modell von ImtCore.

```cpp
virtual QJsonObject MapScopesToClaims(const QByteArray& userId,
                                       const QByteArrayList& scopes) const = 0;
virtual QByteArrayList GetSupportedScopes() const = 0;
virtual QByteArrayList GetSupportedClaims() const = 0;
```

---

### `IJwtTokenProvider`

**Datei:** `Include/imtauth/IJwtTokenProvider.h`

Interface für asymmetrische (RS256) JWT-Operationen. Separat vom bestehenden `CJwtSessionControllerComp`, der HMAC-SHA256 für interne Sessions verwendet.

```cpp
virtual QByteArray GenerateToken(const QJsonObject& claims) const = 0;
virtual bool ValidateToken(const QByteArray& jwt, QJsonObject& claims) const = 0;
virtual QByteArray GetJwks() const = 0;
virtual QByteArray GetKeyId() const = 0;
```

---

### `CRs256JwtTokenProviderComp`

**Datei:** `Include/imtauth/CRs256JwtTokenProviderComp.h/.cpp`

Konkrete ACF-Komponente, die `IJwtTokenProvider` mittels der OpenSSL EVP-API (kompatibel mit OpenSSL 1.x und 3.x) implementiert.

**ACF-Konfigurationsparameter (`I_ASSIGN`):**

| Parameter | Pflicht | Standard | Beschreibung |
|-----------|---------|----------|--------------|
| `PrivateKeyPath` | ✓ | — | Pfad zur PEM-Datei mit dem RSA-Privatschlüssel |
| `KeyId` | ✗ | `"imtcore-oidc-key-1"` | Schlüsselbezeichner (`kid`) in JWT-Header und JWKS |
| `TokenLifetime` | ✗ | `3600` | Access-Token-Lebensdauer in Sekunden |
| `Issuer` | ✓ | — | OIDC-Issuer-URL (`iss`-Claim) |

**Internes Verhalten:**
- Bei `OnComponentCreated()` wird die private Key-PEM-Datei geladen und validiert.
- `GenerateToken()` erstellt den JWT-Header, fügt fehlende Standard-Claims hinzu und signiert mit `EVP_DigestSign`.
- `ValidateToken()` prüft die RSA-Signatur und den `exp`-Claim.
- `GetJwks()` extrahiert Modulus (`n`) und Exponent (`e`) des öffentlichen RSA-Schlüssels als Base64url-Werte und baut ein JWKS-JSON-Dokument. Das Ergebnis wird nach dem ersten Aufruf gecacht.

---

## Phase 2 — OIDC REST-Endpunkte (`Include/imtoidc/`)

### `COidcServletComp`

**Datei:** `Include/imtoidc/COidcServletComp.h/.cpp`

Erweitert `imtrest::CHttpServletCompBase`. Leitet HTTP-Anfragen anhand des letzten Pfad-Segments der Command-ID an Endpunkt-Handler weiter.

**ACF-Abhängigkeiten (`I_ASSIGN`):**

| Parameter | Interface | Beschreibung |
|-----------|-----------|--------------|
| `JwtTokenProvider` | `IJwtTokenProvider` | RS256-Token-Signierung & -Validierung |
| `ScopeMapper` | `IOidcScopeMapper` | Scope → Claims Abbildung |
| `CredentialController` | `ICredentialController` | Nutzerauthentifizierung |
| `UserInfoProvider` | `IUserInfoProvider` | Nutzerprofil-Abfrage |
| `ClientCollection` | `IObjectCollection` | OIDC-Client-Registrierung |
| `AuthCodeCollection` | `IObjectCollection` | Autorisierungscode-Speicher |
| `TokenCollection` | `IObjectCollection` | Token-Tracking-Speicher |
| `IssuerUrl` | Attribut | Basis-OIDC-Issuer-URL |

#### Unterstützte Endpunkte

##### `GET /.well-known/openid-configuration`
Gibt das OIDC Provider Metadata Dokument zurück. Enthält alle Endpunkt-URLs, unterstützte Grant-Typen, Signierungsalgorithmen, Scopes, Claims und Code-Challenge-Methoden.

##### `GET /oauth/authorize`
Authorization Code Endpunkt (RFC 6749 §4.1):
1. Validiert `client_id` gegen `ClientCollection`.
2. Prüft, ob `redirect_uri` für den Client auf der Whitelist steht.
3. Akzeptiert optionale PKCE-Parameter.
4. Generiert einen UUID-Autorisierungscode und speichert ihn in `AuthCodeCollection`.
5. Gibt `code` + `state` als JSON zurück (ACC-Wiring muss Browser-Weiterleitung übernehmen).

##### `POST /oauth/token`
Token-Endpunkt mit drei Grant-Typen:

**`authorization_code`:** Validiert Code (Existenz, Ablauf, Einmal-Nutzung, Client-Match), prüft PKCE, markiert Code als verbraucht, stellt `access_token`, `id_token` und `refresh_token` aus (alle RS256).

**`client_credentials`:** Stellt `access_token` aus, wobei `sub` = `client_id`. Kein `id_token` oder `refresh_token`.

**`refresh_token`:** Validiert Refresh-JWT, stellt neuen `access_token` aus.

Client-Authentifizierung via HTTP Basic Auth oder Formular-Body.

##### `GET /oauth/userinfo`
Validiert Bearer-Token, extrahiert `sub` und `scope`, ruft `IOidcScopeMapper::MapScopesToClaims()` auf.

##### `GET /oauth/jwks`
Gibt das JWKS-Dokument aus `IJwtTokenProvider::GetJwks()` zurück.

##### `POST /oauth/revoke` (RFC 7009)
Setzt `IsRevoked = true` im Token-Store. Gibt immer HTTP 200 zurück.

##### `POST /oauth/introspect` (RFC 7662)
Validiert JWT, prüft `IsRevoked`, gibt Introspektions-Antwort mit `active`, `sub`, `scope`, `exp` etc. zurück.

---

### `COidcScopeMapperComp`

**Datei:** `Include/imtoidc/COidcScopeMapperComp.h/.cpp`

Implementiert `IOidcScopeMapper` durch Abfrage des RBAC-Nutzermodells von ImtCore.

**Scope → Claims Abbildung:**

| Scope | Erzeugte Claims | Quell-Interface |
|-------|----------------|-----------------|
| `openid` | `sub` | Immer enthalten |
| `profile` | `name`, `given_name`, `family_name` | `IPersonBaseInfo` |
| `email` | `email`, `email_verified` | `IContactBaseInfo` |
| `roles` | `roles` (JSON-Array von Rollen-IDs) | `IUserRoleInfo` |
| `permissions` | `permissions` (JSON-Array von Feature-IDs) | `IUserInfo::GetPermissions()` |

---

## Phase 3 — Datenbankschema & Delegates (`Include/imtauthdb/`)

### SQL-Schemata

Alle Tabellen existieren in **PostgreSQL**- und **SQLite**-Varianten und werden als Qt-Ressourcen (`.qrc`) eingebettet.

**Tabellen:** `OidcClients`, `OidcAuthCodes`, `OidcTokens` (Details siehe englischer Abschnitt).

`imtauthdb.qrc` wurde um die 6 neuen SQL-Dateien erweitert.

### Datenbank-Delegates

#### `COidcClientDbDelegateComp`
Erweitert `imtdb::CSqlDatabaseObjectDelegateCompBase`. Erstellt die `OidcClients`-Tabelle bei Bedarf in `OnComponentCreated()`. Verwendet `imtdb::SqlEncode()` für alle Zeichenkettenwerte.

#### `COidcTokenDbDelegateComp`
Analoges Muster für `OidcTokens`. Der Update-Pfad modifiziert ausschließlich `IsRevoked` (Token-Felder sind nach Ausstellung unveränderlich).

---

## Phase 4 — RBAC-Integration

`COidcScopeMapperComp` verbindet OAuth 2.0 Scopes mit dem Puma-RBAC-Modell von ImtCore:
- Erhält `UserCollection` als Abhängigkeit.
- Nutzt Casts auf `IPersonBaseInfo`, `IContactBaseInfo`, `IUserRoleInfo`.
- Benutzerdefinierte Scopes können durch Subclassing oder alternative `IOidcScopeMapper`-Implementierungen hinzugefügt werden.

---

## Phase 5 — Package-Registrierung & Build

### `ImtOidcPck`
**Datei:** `Impl/ImtOidcPck/ImtOidcPck.h/.cpp`
Exportiert: `OidcServlet`, `OidcScopeMapper`.

### `ImtAuthPck`-Ergänzungen
Neue Registrierungen: `OidcClient`, `OidcAuthorizationCode`, `OidcTokenInfo`, `Rs256JwtTokenProvider`.

### `ImtAuthDbPck`-Ergänzungen
Neue Delegates: `OidcClientDatabaseDelegate`, `OidcTokenDatabaseDelegate`.

### CMake
```cmake
project(imtoidc)
target_link_libraries(${PROJECT_NAME} imtauth imtrest imtcrypt imtbase)
```

---

## Sicherheitsaspekte

- Client-Secrets werden ausschließlich als **SHA-256-Hashes** gespeichert.
- Autorisierungscodes sind **Einmalcodes** (`Used`-Flag).
- PKCE schützt den Authorization Code Flow für öffentliche Clients.
- Die Tabelle `OidcTokens` ermöglicht serverseitigen **Token-Widerruf** unabhängig vom Ablaufzeitpunkt.
- RSA-Privatschlüssel wird per ACF-Konfiguration injiziert — kein Hardcoding.
- OpenSSL EVP-API für alle kryptographischen Operationen (OpenSSL 1.x und 3.x kompatibel).

---
---

<a name="русский"></a>
# Русский

## Обзор

ImtCore предоставляет полноценную реализацию провайдера OpenID Connect (OIDC), построенную на основе существующей компонентной модели ACF. Реализация полностью соответствует спецификациям OAuth 2.0 Authorization Framework (RFC 6749) и OpenID Connect Core 1.0. Она сосуществует с имеющейся системой сессий на основе HMAC-SHA256 (`CJwtSessionControllerComp`) и добавляет независимую инфраструктуру токенов на базе RS256.

## Архитектура

Функциональность OIDC распределена по трём модулям ImtCore:

| Модуль | Расположение | Ответственность |
|--------|-------------|----------------|
| `imtauth` | `Include/imtauth/` | Интерфейсы, модели данных, провайдер RS256 JWT |
| `imtoidc` | `Include/imtoidc/` | REST-сервлет, маппер scope → claims |
| `imtauthdb` | `Include/imtauthdb/` | Делегаты БД, SQL-схемы |

Регистрация пакетов:
- **`ImtAuthPck`** — регистрирует `OidcClient`, `OidcAuthorizationCode`, `OidcTokenInfo`, `Rs256JwtTokenProvider`.
- **`ImtAuthDbPck`** — регистрирует `OidcClientDatabaseDelegate`, `OidcTokenDatabaseDelegate`.
- **`ImtOidcPck`** — регистрирует `OidcServlet` (REST-сервлет) и `OidcScopeMapper`.

---

## Фаза 1 — Структуры данных и интерфейсы OIDC (`Include/imtauth/`)

### `IOidcClient` / `COidcClient`

**Файлы:** `Include/imtauth/IOidcClient.h`, `Include/imtauth/COidcClient.h/.cpp`

Представляет зарегистрированное клиентское приложение OAuth 2.0 / OIDC. Каждый клиент содержит:

| Поле | Тип | Описание |
|------|-----|----------|
| `ClientId` | `QByteArray` | Уникальный идентификатор клиента (UUID) |
| `ClientSecretHash` | `QByteArray` | SHA-256-хеш client secret (никогда не хранится в открытом виде) |
| `ClientName` | `QString` | Человекочитаемое название приложения |
| `RedirectUris` | `QStringList` | Белый список разрешённых redirect URI |
| `GrantTypes` | `QByteArrayList` | Разрешённые типы grant (см. enum ниже) |
| `Scopes` | `QByteArrayList` | Разрешённые OAuth 2.0 scopes |
| `CreatedAt` | `QDateTime` | Временная метка регистрации (UTC) |

**Перечисление `OidcGrantType`** (объявлено внутри `IOidcClient` с `I_DECLARE_ENUM`):

```
OGT_AUTHORIZATION_CODE   – Authorization Code Flow (интерактивный, с участием пользователя)
OGT_CLIENT_CREDENTIALS   – Client Credentials Flow (межмашинный)
OGT_REFRESH_TOKEN        – Обмен Refresh Token
```

`COidcClient` реализует полную семантику `iser::IChangeable`: `CopyFrom`, `IsEqual`, `CloneMe`, `ResetData` и `Serialize` (версионированная через `CArchiveTag`).

---

### `IOidcAuthorizationCode` / `COidcAuthorizationCode`

**Файлы:** `Include/imtauth/IOidcAuthorizationCode.h`, `Include/imtauth/COidcAuthorizationCode.h/.cpp`

Короткоживущий одноразовый код, выдаваемый в ходе Authorization Code Flow и обмениваемый на token endpoint. Поддерживает **PKCE** (Proof Key for Code Exchange, RFC 7636) для публичных клиентов.

| Поле | Тип | Описание |
|------|-----|----------|
| `Code` | `QByteArray` | Непрозрачное значение кода авторизации (UUID) |
| `ClientId` | `QByteArray` | Клиент, выдавший код |
| `UserId` | `QByteArray` | Авторизовавший пользователь |
| `Scope` | `QString` | Строка scopes, разделённых пробелами |
| `RedirectUri` | `QString` | Redirect URI из запроса авторизации |
| `ExpirationDate` | `QDateTime` | Время истечения (10 минут с момента выдачи) |
| `CodeChallenge` | `QByteArray` | PKCE code challenge (Base64url SHA-256 или plain) |
| `CodeChallengeMethod` | `QByteArray` | `"S256"` или `"plain"` |
| `Used` | `bool` | Флаг однократного использования — устанавливается в `true` после обмена |

---

### `IOidcTokenInfo` / `COidcTokenInfo`

**Файлы:** `Include/imtauth/IOidcTokenInfo.h`, `Include/imtauth/COidcTokenInfo.h/.cpp`

Отслеживает каждый выданный access- или refresh-токен для отзыва и интроспекции.

| Поле | Тип | Описание |
|------|-----|----------|
| `TokenId` | `QByteArray` | JWT-клейм `jti` — уникальный идентификатор токена |
| `ClientId` | `QByteArray` | Клиент, которому выдан токен |
| `UserId` | `QByteArray` | Связанный пользователь (пусто для `client_credentials`) |
| `Scope` | `QString` | Предоставленные scopes |
| `TokenType` | `OidcTokenType` | `OTT_ACCESS` или `OTT_REFRESH` |
| `ExpirationDate` | `QDateTime` | Время истечения (UTC) |
| `IsRevoked` | `bool` | Флаг отзыва |
| `CreatedAt` | `QDateTime` | Временная метка выдачи (UTC) |

**Перечисление `OidcTokenType`** использует `I_SERIALIZE_ENUM` для сериализации.

---

### `IOidcScopeMapper`

**Файл:** `Include/imtauth/IOidcScopeMapper.h`

Интерфейс для преобразования OAuth 2.0 scopes в пользовательские claims. Реализации обращаются к RBAC-модели ImtCore.

```cpp
virtual QJsonObject MapScopesToClaims(const QByteArray& userId,
                                       const QByteArrayList& scopes) const = 0;
virtual QByteArrayList GetSupportedScopes() const = 0;
virtual QByteArrayList GetSupportedClaims() const = 0;
```

---

### `IJwtTokenProvider`

**Файл:** `Include/imtauth/IJwtTokenProvider.h`

Интерфейс для асимметричных (RS256) JWT-операций. Отделён от существующего `CJwtSessionControllerComp`, использующего HMAC-SHA256 для внутренних сессий.

```cpp
virtual QByteArray GenerateToken(const QJsonObject& claims) const = 0;
virtual bool ValidateToken(const QByteArray& jwt, QJsonObject& claims) const = 0;
virtual QByteArray GetJwks() const = 0;
virtual QByteArray GetKeyId() const = 0;
```

---

### `CRs256JwtTokenProviderComp`

**Файлы:** `Include/imtauth/CRs256JwtTokenProviderComp.h/.cpp`

Конкретный ACF-компонент, реализующий `IJwtTokenProvider` с помощью OpenSSL EVP API (совместим с OpenSSL 1.x и 3.x).

**Параметры конфигурации ACF (`I_ASSIGN`):**

| Параметр | Обязательный | По умолчанию | Описание |
|----------|-------------|-------------|----------|
| `PrivateKeyPath` | ✓ | — | Путь к PEM-файлу с RSA-приватным ключом |
| `KeyId` | ✗ | `"imtcore-oidc-key-1"` | Идентификатор ключа (`kid`) в заголовке JWT и JWKS |
| `TokenLifetime` | ✗ | `3600` | Время жизни access-токена в секундах |
| `Issuer` | ✓ | — | URL OIDC issuer (клейм `iss`) |

**Внутреннее поведение:**
- При `OnComponentCreated()` загружается и валидируется PEM-файл приватного ключа.
- `GenerateToken()` формирует заголовок JWT, добавляет недостающие стандартные клеймы (`iat`, `exp`, `iss`) и подписывает с помощью `EVP_DigestSign`.
- `ValidateToken()` проверяет RSA-подпись и клейм `exp`.
- `GetJwks()` извлекает модуль (`n`) и показатель степени (`e`) публичного RSA-ключа в виде Base64url-значений и формирует стандартный JWKS JSON-документ. Результат кэшируется после первого вызова.

---

## Фаза 2 — REST-эндпоинты OIDC (`Include/imtoidc/`)

### `COidcServletComp`

**Файлы:** `Include/imtoidc/COidcServletComp.h/.cpp`

Расширяет `imtrest::CHttpServletCompBase`. Направляет HTTP-запросы обработчикам эндпоинтов по последнему сегменту пути в command ID.

**ACF-зависимости (`I_ASSIGN`):**

| Параметр | Интерфейс | Описание |
|----------|-----------|----------|
| `JwtTokenProvider` | `IJwtTokenProvider` | Подпись и валидация RS256-токенов |
| `ScopeMapper` | `IOidcScopeMapper` | Отображение scope → claims |
| `CredentialController` | `ICredentialController` | Аутентификация пользователей |
| `UserInfoProvider` | `IUserInfoProvider` | Получение профиля пользователя |
| `ClientCollection` | `IObjectCollection` | Реестр OIDC-клиентов |
| `AuthCodeCollection` | `IObjectCollection` | Хранилище кодов авторизации |
| `TokenCollection` | `IObjectCollection` | Хранилище токенов |
| `IssuerUrl` | атрибут | Базовый URL OIDC issuer |

#### Поддерживаемые эндпоинты

##### `GET /.well-known/openid-configuration`
Возвращает документ метаданных провайдера OIDC. Содержит URL всех эндпоинтов, поддерживаемые типы grant, алгоритмы подписи, scopes, claims и методы code challenge.

##### `GET /oauth/authorize`
Эндпоинт Authorization Code (RFC 6749 §4.1):
1. Проверяет `client_id` в `ClientCollection`.
2. Проверяет, что `redirect_uri` внесён в белый список клиента.
3. Принимает опциональные параметры PKCE.
4. Генерирует UUID-код авторизации, сохраняет в `AuthCodeCollection`.
5. Возвращает `code` + `state` как JSON (ACC-проводка должна выполнить редирект браузера).

##### `POST /oauth/token`
Эндпоинт токенов. Поддерживает три типа grant:

**`authorization_code`:** Проверяет код (существование, истечение срока, однократность использования, соответствие клиенту), проверяет PKCE, отмечает код как использованный, выдаёт `access_token`, `id_token` и `refresh_token` (все RS256).

**`client_credentials`:** Выдаёт `access_token`, где `sub` = `client_id`. Без `id_token` и `refresh_token`.

**`refresh_token`:** Валидирует Refresh JWT, выдаёт новый `access_token`.

Аутентификация клиента — через HTTP Basic Auth или тело формы.

##### `GET /oauth/userinfo`
Проверяет Bearer-токен из заголовка `Authorization`, извлекает `sub` и `scope`, вызывает `IOidcScopeMapper::MapScopesToClaims()`, возвращает JSON-объект с клеймами.

##### `GET /oauth/jwks`
Возвращает JWKS-документ из `IJwtTokenProvider::GetJwks()`.

##### `POST /oauth/revoke` (RFC 7009)
Устанавливает `IsRevoked = true` в хранилище токенов. Всегда возвращает HTTP 200 (даже если токен не найден).

##### `POST /oauth/introspect` (RFC 7662)
Валидирует JWT, проверяет `IsRevoked`, возвращает ответ интроспекции с полями `active`, `sub`, `client_id`, `scope`, `exp`, `iat`, `iss`.

---

### `COidcScopeMapperComp`

**Файлы:** `Include/imtoidc/COidcScopeMapperComp.h/.cpp`

Реализует `IOidcScopeMapper` путём запроса к RBAC-модели пользователей ImtCore.

**Отображение Scope → Claims:**

| Scope | Генерируемые claims | Источник |
|-------|--------------------|---------| 
| `openid` | `sub` | Всегда включается |
| `profile` | `name`, `given_name`, `family_name` | `IPersonBaseInfo` |
| `email` | `email`, `email_verified` | `IContactBaseInfo` |
| `roles` | `roles` (JSON-массив ID ролей) | `IUserRoleInfo` |
| `permissions` | `permissions` (JSON-массив ID возможностей) | `IUserInfo::GetPermissions()` |

**`GetSupportedScopes()`** возвращает: `openid`, `profile`, `email`, `roles`, `permissions`.

---

## Фаза 3 — Схема БД и делегаты (`Include/imtauthdb/`)

### SQL-схемы

Все таблицы существуют в вариантах **PostgreSQL** и **SQLite**, встроенных как Qt-ресурсы (`.qrc`).

**Таблицы:** `OidcClients`, `OidcAuthCodes`, `OidcTokens`.

Ключевые поля `OidcClients`: `Id` (UUID PK), `ClientSecretHash`, `ClientName`, `RedirectUris`, `GrantTypes`, `Scopes`, `CreatedAt`.

Ключевые поля `OidcAuthCodes`: `Id` (text PK), `ClientId`, `UserId`, `Scope`, `RedirectUri`, `ExpiresAt`, `CodeChallenge`, `CodeChallengeMethod`, `Used`.

Ключевые поля `OidcTokens`: `Id` (UUID PK = JWT `jti`), `ClientId`, `UserId` (NULL для `client_credentials`), `Scope`, `TokenType`, `ExpiresAt`, `IsRevoked`, `CreatedAt`.

Файл `imtauthdb.qrc` обновлён и включает 6 новых SQL-файлов.

### Делегаты БД

#### `COidcClientDbDelegateComp`
**Файлы:** `Include/imtauthdb/COidcClientDbDelegateComp.h/.cpp`

Расширяет `imtdb::CSqlDatabaseObjectDelegateCompBase`. В `OnComponentCreated()` создаёт таблицу `OidcClients`, если она не существует, используя SQL-ресурс для активного драйвера БД. Использует `imtdb::SqlEncode()` для всех строковых значений.

#### `COidcTokenDbDelegateComp`
**Файлы:** `Include/imtauthdb/COidcTokenDbDelegateComp.h/.cpp`

Аналогичный шаблон для `OidcTokens`. Путь обновления изменяет только `IsRevoked` — поля токена неизменны после выдачи.

---

## Фаза 4 — Интеграция с RBAC

`COidcScopeMapperComp` связывает OAuth 2.0 scopes с Puma-RBAC-моделью ImtCore:
- Получает `UserCollection` (`IObjectCollection`) как зависимость.
- Выполняет приведение объекта пользователя к `IPersonBaseInfo`, `IContactBaseInfo`, `IUserRoleInfo` по мере необходимости.
- Пользовательские scopes поддерживаются через создание подкласса `COidcScopeMapperComp` или предоставление альтернативной реализации `IOidcScopeMapper`.

Тип grant `authorization_code` в token endpoint интегрируется с `ICredentialController` — тем же интерфейсом, который используется существующей GraphQL-мутацией `Authorization`.

---

## Фаза 5 — Регистрация пакетов и сборка

### `ImtOidcPck`
**Файлы:** `Impl/ImtOidcPck/ImtOidcPck.h/.cpp`

Экспортирует: `OidcServlet` → `COidcServletComp`, `OidcScopeMapper` → `COidcScopeMapperComp`.

### Дополнения к `ImtAuthPck`
Новые регистрации компонентов: `OidcClient`, `OidcAuthorizationCode`, `OidcTokenInfo`, `Rs256JwtTokenProvider` (обёрнуты через `TModelCompWrap<TMakeComponentWrap<...>>`).

### Дополнения к `ImtAuthDbPck`
Новые делегаты: `OidcClientDatabaseDelegate`, `OidcTokenDatabaseDelegate`.

### CMake
```cmake
project(imtoidc)
target_link_libraries(${PROJECT_NAME} imtauth imtrest imtcrypt imtbase)
```

---

## Ключевые архитектурные решения

| Решение | Обоснование |
|---------|-------------|
| RS256 (асимметричный) вместо HS256 | Спецификация OIDC требует, чтобы ID-токены были верифицируемы третьими сторонами. Асимметричная подпись через JWKS-эндпоинт позволяет клиентам проверять токены без общего секрета. |
| Отделение от `CJwtSessionControllerComp` | Существующая система сессий на HS256 не изменяется. OIDC-токены имеют другой жизненный цикл: они могут быть отозваны, интроспектированы и сохранены в БД. |
| Поддержка PKCE (RFC 7636) | Защищает Authorization Code Flow для публичных клиентов (SPA, мобильные приложения), которые не могут безопасно хранить client secret. |
| Хранение токенов в `OidcTokens` | Обеспечивает отзыв (RFC 7009) и интроспекцию (RFC 7662) без необходимости разбирать JWT при каждом запросе. |
| Интерфейс `IOidcScopeMapper` | Позволяет приложениям подключать пользовательскую логику отображения scope→claims без изменения сервлета. |
| Отдельный модуль `imtoidc` | Сохраняет `imtauth` и `imtauthdb` сфокусированными. Зависимость: `imtoidc` → `imtauth + imtrest + imtcrypt`. |

---

## Аспекты безопасности

- Client secrets хранятся исключительно в виде **SHA-256-хешей** (`ClientSecretHash`). Открытый текст никогда не сохраняется.
- Коды авторизации являются **одноразовыми** (флаг `Used` устанавливается при обмене).
- PKCE защищает Authorization Code Flow для публичных клиентов.
- Таблица `OidcTokens` обеспечивает серверный **отзыв токенов** независимо от времени их истечения.
- Путь к RSA-приватному ключу инжектируется через конфигурацию ACF (`PrivateKeyPath`) — жёсткое кодирование ключей в исходном коде исключено.
- Все криптографические операции выполняются через OpenSSL EVP API (совместимость с OpenSSL 1.x и 3.x).

---

## Список файлов

```
Include/imtauth/
  IOidcClient.h
  COidcClient.h / .cpp
  IOidcAuthorizationCode.h
  COidcAuthorizationCode.h / .cpp
  IOidcTokenInfo.h
  COidcTokenInfo.h / .cpp
  IOidcScopeMapper.h
  IJwtTokenProvider.h
  CRs256JwtTokenProviderComp.h / .cpp

Include/imtoidc/
  COidcServletComp.h / .cpp
  COidcScopeMapperComp.h / .cpp
  CMake/CMakeLists.txt

Include/imtauthdb/
  COidcClientDbDelegateComp.h / .cpp
  COidcTokenDbDelegateComp.h / .cpp
  Resources/SQL/Postgres/CreateOidcClientsTable.sql
  Resources/SQL/Postgres/CreateOidcAuthCodesTable.sql
  Resources/SQL/Postgres/CreateOidcTokensTable.sql
  Resources/SQL/SQLite/CreateOidcClientsTable.sql
  Resources/SQL/SQLite/CreateOidcAuthCodesTable.sql
  Resources/SQL/SQLite/CreateOidcTokensTable.sql
  imtauthdb.qrc  (обновлён)

Impl/ImtAuthPck/
  ImtAuthPck.h / .cpp  (обновлён)

Impl/ImtAuthDbPck/
  ImtAuthDbPck.h / .cpp  (обновлён)

Impl/ImtOidcPck/
  ImtOidcPck.h / .cpp
```

---

## Phase 6 — External OIDC Provider Support (Relying Party)

### Overview

In addition to acting as an OIDC **Provider** (issuing tokens to client apps), ImtCore can also act as an OIDC **Relying Party / Client**, authenticating users via external providers (Google, Apple, Facebook) and creating internal sessions.

### Architecture

| Module | New Files | Responsibility |
|--------|-----------|----------------|
| `imtauth` | `IExternalOidcProvider.h`, `IExternalIdentity.h`, `CExternalIdentity.h/.cpp`, `IExternalOidcAuthController.h`, `CExternalOidcAuthControllerComp.h/.cpp`, `CGoogleOidcProviderComp.h/.cpp`, `CAppleOidcProviderComp.h/.cpp`, `CFacebookOidcProviderComp.h/.cpp` | Interfaces, data model, provider configs, auth controller |
| `imtoidc` | `CExternalOidcServletComp.h/.cpp` | REST endpoints for external auth flows |
| `imtauthdb` | `CExternalIdentityDbDelegateComp.h/.cpp`, `CreateExternalIdentitiesTable.sql` (Postgres + SQLite) | Database persistence for external identities |

### Interfaces

#### `IExternalOidcProvider`
Describes an external OIDC provider configuration:
- `GetProviderId()` — unique name ("google", "apple", "facebook")
- `GetDiscoveryUrl()` — `.well-known/openid-configuration` URL
- `GetAuthorizationEndpoint()` / `GetTokenEndpoint()` / `GetUserInfoEndpoint()`
- `GetClientId()` / `GetClientSecret()` — OAuth app credentials
- `GetScopes()` — requested scopes
- `GetRedirectUri()` — callback URL

#### `IExternalIdentity`
Links an external provider account to an ImtCore user:
- `Id`, `UserId`, `Provider`, `ExternalSubject` (sub claim), `ExternalEmail`, `LinkedAt`, `LastAuthAt`

#### `IExternalOidcAuthController`
Orchestrates the external login flow:
- `GetAvailableProviders()` — list configured providers
- `GetAuthorizationUrl(providerId, state, nonce)` — build redirect URL
- `HandleCallback(providerId, authCode, state)` — exchange code, validate, link user, create session
- `UnlinkProvider(userId, providerId)` — remove provider link
- `GetLinkedProviders(userId)` — list linked providers

### Provider Components

| Component | Discovery URL | Scopes |
|-----------|--------------|--------|
| `CGoogleOidcProviderComp` | `https://accounts.google.com/.well-known/openid-configuration` | `openid profile email` |
| `CAppleOidcProviderComp` | `https://appleid.apple.com/.well-known/openid-configuration` | `openid name email` |
| `CFacebookOidcProviderComp` | N/A (Graph API) | `openid email public_profile` |

Each provider is configured via ACF attributes: `ClientId`, `ClientSecret`, `RedirectUri`, `Scopes`.

### REST Endpoints (`CExternalOidcServletComp`)

| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | `/oauth/external/providers` | List available external providers |
| GET | `/oauth/external/authorize/{provider}` | Get authorization redirect URL |
| GET | `/oauth/external/callback` | Handle provider callback (code exchange) |
| POST | `/oauth/external/unlink/{provider}` | Unlink external provider (requires bearer token) |
| GET | `/oauth/external/identities` | List linked providers for current user (requires bearer token) |

### Database Schema

Table `ExternalIdentities`:

| Column | Postgres Type | Description |
|--------|--------------|-------------|
| `Id` | `uuid PRIMARY KEY` | Record ID |
| `UserId` | `uuid NOT NULL` | Local ImtCore user |
| `Provider` | `text NOT NULL` | Provider name |
| `ExternalSubject` | `text NOT NULL` | Provider's sub claim |
| `ExternalEmail` | `text` | Email from provider |
| `LinkedAt` | `timestamp` | When linked |
| `LastAuthAt` | `timestamp` | Last auth timestamp |

Indexes: unique on `(Provider, ExternalSubject)`, index on `UserId`.

### Package Registration

- **ImtAuthPck**: `ExternalIdentity`, `GoogleOidcProvider`, `AppleOidcProvider`, `FacebookOidcProvider`, `ExternalOidcAuthController`
- **ImtAuthDbPck**: `ExternalIdentityDatabaseDelegate`
- **ImtOidcPck**: `ExternalOidcServlet`

### Security Considerations

- **CSRF Protection**: `state` parameter generated at `/authorize` and validated at `/callback`
- **Nonce Validation**: `nonce` included in authorization request for ID token validation
- **HTTPS Required**: All redirect URIs must use HTTPS in production
- **Token Signature Validation**: External `id_token` signatures should be verified against provider JWKS
- **Account Linking**: External identities are linked by `(Provider, ExternalSubject)` unique constraint
- **Email Verification**: Only verified emails should be used for auto-user creation

### Provider Setup

#### Google
1. Go to [Google Cloud Console](https://console.cloud.google.com/)
2. Create OAuth 2.0 credentials (Web application)
3. Set authorized redirect URI to your ImtCore callback URL
4. Configure `ClientId` and `ClientSecret` in the `CGoogleOidcProviderComp` ACF attributes

#### Apple
1. Go to [Apple Developer Portal](https://developer.apple.com/)
2. Register a Services ID and configure Sign in with Apple
3. Apple uses `client_secret_jwt` with ES256 — generate the JWT client secret
4. Configure `ClientId` (Services ID) and `ClientSecret` (JWT) in `CAppleOidcProviderComp`

#### Facebook
1. Go to [Facebook Developer Dashboard](https://developers.facebook.com/)
2. Create a Facebook Login app and configure OAuth redirect URI
3. Configure `ClientId` (App ID) and `ClientSecret` (App Secret) in `CFacebookOidcProviderComp`
