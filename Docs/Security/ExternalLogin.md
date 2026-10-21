# Sign-in with external accounts (Google, Apple, Facebook, OpenID Connect)

Users sign in with an account of an external identity provider instead of (or in addition to) their
password, and link or unlink such accounts in their profile.

## Flow

```
UI client                    App server (Lisa, ProLife)        Puma (auth server)              Provider
   | BeginExternalLogin  ---------> RemoteAuthorizationController -> ExternalLoginController
   | <-- loginId, authorizationUrl (state, nonce, PKCE S256 challenge)
   | opens authorizationUrl in the browser --------------------------------------------------> sign-in
   |                         <------------------------- redirect to CallbackUrl ?code&state ---|
   |                    ExternalLogin/Callback servlet (on Puma, or on the app server, which
   |                    forwards HandleExternalLoginCallback to Puma)
   |                                                  code exchange (client secret + PKCE verifier),
   |                                                  ID token checks, user resolution
   | CompleteExternalLogin(loginId) polled every 2 s -> SignedIn + AuthorizationPayload | Linked | Failed
```

- The server is a confidential client: the code, the provider tokens and the client secret never reach the UI.
- `state` is single-use; the `loginId` the client polls with is a separate secret that never goes to the provider.
- The ID token comes straight from the token endpoint over TLS, so its signature is not verified
  (OpenID Connect Core 3.1.3.7); `iss`, `aud`, `exp` and `nonce` are.
- The browser window shows a short result page; the session reaches the application only through the poll.
- A browser opens a window only within a click, so the QML client prepares the logins in advance
  (`AuthorizationController.prepareExternalLogins`) and the click opens a ready URL.

## Which user signs in

1. The user the external account (provider + `sub`) is linked to.
2. Otherwise, with `LinkByVerifiedEmail` (default on), the only user whose e-mail equals the account's
   **verified** e-mail; the account is linked to that user.
3. Otherwise, with `AllowRegistration` (default off), a new user without password, login = e-mail,
   with the product's default role.
4. Otherwise the sign-in fails: "No user is linked to this ... account".

Disabled users cannot sign in. A user has at most one linked account per provider; an account linked to
another user cannot be linked. The last linked account of a user without password cannot be unlinked.

## GraphQL (Sdl/imtauth/1.0/Authorization.sdl)

| Operation | Access |
|-----------|--------|
| `GetExternalLoginProviders` | anonymous; providers without client ID are not listed |
| `BeginExternalLogin(providerId, productId, link)` | anonymous; `link: true` needs a session token |
| `CompleteExternalLogin(loginId)` | anonymous; a finished result is returned once |
| `HandleExternalLoginCallback(state, code, error)` | used by an app server forwarding the callback |
| `GetExternalIdentities`, `UnlinkExternalIdentity(providerId)` | session token (not a personal access token) |

## Components

| Component | Package | Role |
|-----------|---------|------|
| `ExternalLoginController` | ImtAuthPck | the flow, pending logins (in memory, 10 min), user resolution |
| `GoogleOidcProvider`, `AppleOidcProvider`, `FacebookOidcProvider`, `GenericOidcProvider` | ImtAuthPck | provider descriptions |
| `ExternalIdentityManager`, `ExternalIdentity` | ImtAuthPck | links between accounts and users |
| `ExternalIdentityDatabaseDelegate` | ImtAuthDbPck | table `ExternalIdentities`, unique (`ProviderId`, `Subject`) |
| `ExternalLoginCallbackServlet` | ImtAuthGqlPck | the redirect URI, GET and form POST (Apple) |
| `AuthorizationController.ExternalLoginController` | ImtAuthGqlPck | the GraphQL operations |
| `RemoteAuthorizationController` | ImtAuthGqlPck | forwards the operations and the callback to Puma |

Composites: `Partitura/ImtAuthVoce.arp/ExternalLoginController.acc` (repository, providers, controller) and
`ExternalLoginSettings.acc` (the settings below). Puma wires them in `PumaServerBase.acc` / `PumaSettings.acc`;
the callback servlet is `Puma/ExternalLogin/Callback` on Puma and `Lisa/ExternalLogin/Callback` on Lisa
(handler `AuthorizableServerFramework/ExternalLoginCallbackHandler`).

## Configuration (Puma server settings, "External Sign-in")

Secrets are runtime settings, never `.acc` attributes (the registry is compiled into the executable).

| Setting | Meaning |
|---------|---------|
| `CallbackUrl` | public URL of a callback servlet, registered at every provider as redirect URI |
| `GoogleClientId` / `GoogleClientSecret` | Google Cloud Console, OAuth client of type "Web application" |
| `AppleClientId` / `AppleClientSecret` | Services ID / client secret JWT (ES256, signed with the Sign in with Apple key, valid for at most 6 months - renew it) |
| `FacebookClientId` / `FacebookClientSecret` | App ID / App Secret (Graph API, no ID token) |
| `OidcProviderName`, `OidcAuthorizationEndpoint`, `OidcTokenEndpoint`, `OidcUserInfoEndpoint`, `OidcIssuer`, `OidcClientId`, `OidcClientSecret` | any OpenID Connect provider (Keycloak, Microsoft Entra ID, ...) |

A provider is offered once its client ID is set. The settings are read at start: restart Puma after changing
them. On a developer machine they are in `C:\Users\Public\ImagingTools\Puma\Puma Server\PumaServerSettings.xml`
(`PumaServerPgTestSettings.xml` for the test server).

## Obtaining the provider keys

Every provider needs the redirect URI, which is `CallbackUrl` and must match character by character:
`http://localhost:7788/Puma/ExternalLogin/Callback` on a developer machine, the public HTTPS address of Puma
(or of an application server hosting the callback servlet) in production. Google and Facebook accept
`http://localhost` for development; Apple accepts only HTTPS on a real domain.

### Google

1. https://console.cloud.google.com - create or select a project.
2. APIs & Services -> OAuth consent screen: type External, application name, support e-mail, scopes
   `openid`, `email`, `profile`. While the app is in "Testing", only the listed test users can sign in;
   publish it for everybody.
3. APIs & Services -> Credentials -> Create credentials -> OAuth client ID, type "Web application",
   Authorized redirect URIs: `CallbackUrl`.
4. Client ID -> `GoogleClientId`, Client secret -> `GoogleClientSecret`.

### Facebook

1. https://developers.facebook.com -> My Apps -> Create App, use case "Authenticate and request data from
   users with Facebook Login".
2. Add the product Facebook Login; Facebook Login -> Settings -> Valid OAuth Redirect URIs: `CallbackUrl`.
3. App settings -> Basic: App ID -> `FacebookClientId`, App Secret -> `FacebookClientSecret`.
4. In Development mode only the app's administrators and testers (App Roles) can sign in. Switching to Live
   needs a privacy policy URL and data deletion instructions. `email` and `public_profile` need no app review.

### Apple

Needs a paid Apple Developer Program membership and a domain with HTTPS.

1. https://developer.apple.com/account -> Certificates, Identifiers & Profiles.
2. Identifiers -> App IDs: an App ID with "Sign In with Apple" enabled.
3. Identifiers -> Services IDs: a Services ID, e.g. `com.imagingtools.prolife.signin` -> `AppleClientId`.
   Enable "Sign In with Apple" -> Configure: the App ID of step 2, Domains, Return URLs = `CallbackUrl`.
4. Keys -> +: a key with "Sign In with Apple"; download the `.p8` file (possible only once) and note the
   Key ID. The Team ID is shown at the top right of the account page.
5. `AppleClientSecret` is a JWT generated from the key: header `alg: ES256`, `kid: <Key ID>`; claims
   `iss: <Team ID>`, `sub: <Services ID>`, `aud: https://appleid.apple.com`, `iat: now`,
   `exp: at most 6 months later`, signed with the `.p8` key. Generate a new one before it expires, or the
   sign-in with Apple stops working.

### Any OpenID Connect provider (Keycloak, Microsoft Entra ID, ...)

Register a confidential (web) client with redirect URI `CallbackUrl` and scopes `openid email profile`.
Take `authorization_endpoint`, `token_endpoint`, `userinfo_endpoint` and `issuer` from the provider's
`/.well-known/openid-configuration` into the `Oidc*` settings, the client ID and secret into
`OidcClientId` / `OidcClientSecret`, and the button text into `OidcProviderName`.

Keep the secrets in the server settings only - never in `.acc` files, the repository or logs.

## Turning the mode on per application

The client offers external accounts only when the application server enables it: attribute
`ExternalLoginEnabled` of `ApplicationInfoController` (default `false`), exported through
`ApplicationHandlers.acc` / `ApplicationSettingsController.acc` / `RootDataController.acc` like
`TenantsEnabled`, and delivered with `GetApplicationInfo` (`externalLoginEnabled`). The client reads it as
`ApplicationInfoProvider.externalLoginEnabled` / `AuthorizationController.externalLoginEnabled`.
ProLife sets it in `ProLifeHandlers.acc` and `ProLifeApplicationSettings.acc`; Lisa and the other products keep it off.

## UI (Qml/imtauthgui)

- `AuthorizationPage.qml`: "or continue with" separator and one `ExternalLoginButton` per provider offered by
  the server (brand logo + "Continue with ..."), waiting state with Cancel. With the mode on, "Sign in" becomes
  the full-width primary button; with it off the page is unchanged.
- `ExternalLoginButton.qml`: icon by provider type - `Icons/Google`, `Icons/Apple`, `Icons/Facebook`
  (`Include/imtgui/Resources/Icons/Template`), `Icons/Key` for a generic OpenID Connect provider.
  The Google and Facebook logos keep their brand colors (only the template color `#D9D9D9` is themed);
  the Apple logo uses the template color and follows the theme.
- `ProfileLinkedAccountsPage.qml`: profile tab "Linked Accounts" (shown only when providers are configured),
  Link / Unlink per provider; transport through `ProfileApiClient` (`getLinkedAccounts`, `unlinkAccount`).
- `AuthorizationController.qml`: `loadExternalLoginProviders`, `loginWithExternalProvider`,
  `linkExternalAccount`, `cancelExternalLogin`, `prepareExternalLogins`.

## Tests

- `Tests/ExternalLoginTest` (QtTest, 28 cases): the controller against an in-process mock provider -
  PKCE, nonce/aud/iss/exp checks, single-use state, linking rules, registration, settings.
- `Puma/Tests/TenantApiPostman/ws/external-login.js` (19 cases, part of Run-CiTests.ps1): the whole flow
  against a running Puma with `ws/mock-idp.js`; `ws/external-login-settings.js` writes the server settings.
- `ProLife/Tests/ProLifeApiPostman` (GetApplicationInfo case): `externalLoginEnabled` is true for ProLife.
- No GUI specs yet; the web client was checked by hand (ProLife: sign-in, profile link/unlink, failed
  sign-in, Cancel; Lisa: no provider buttons).

## Not covered

- ImtCore as an OpenID Connect provider for third-party applications (discovery, `/authorize`, `/token`,
  JWKS) - part of the branch of PR #446, but not of the described feature.
- Generating the Apple client secret from the key; it is a setting renewed by the operator.
- Pending logins are kept in memory of one Puma process (no cluster support).
