# OpenID Connect — Workflow-Dokumentation

Dieses Dokument beschreibt den vollständigen Workflow für die OpenID Connect (OIDC) Integration auf der QML-Client-Seite: von der Konfiguration über die Benutzerregistrierung bis hin zum Login und Token-Refresh.

---

## Übersicht

ImtCore unterstützt die Authentifizierung über externe OpenID Connect Provider (z.B. Google, Facebook, Apple) zusätzlich zur lokalen Benutzername/Passwort-Anmeldung. Die Implementierung verwendet den **Authorization Code Flow mit PKCE** (Proof Key for Code Exchange), der für öffentliche Clients (Desktop- und Web-Anwendungen) empfohlen wird.

### Architektur-Übersicht

```
┌─────────────────────────────────────────────────────────────┐
│  QML Client                                                 │
│  ┌──────────────────┐   ┌──────────────────────────────┐   │
│  │ AuthorizationPage│   │ AuthorizationController       │   │
│  │  (Login-UI)      │──▶│  (Singleton)                  │   │
│  └──────────────────┘   │  ┌────────────────────────┐   │   │
│                         │  │ OpenIdConnectProvider   │   │   │
│                         │  │  - Provider-Verwaltung  │   │   │
│                         │  │  - PKCE-Generierung     │   │   │
│                         │  │  - Code-Austausch (GQL) │   │   │
│                         │  └────────────────────────┘   │   │
│                         └──────────────────────────────────┘ │
│                              │            ▲                  │
│                              ▼            │                  │
│  ┌──────────────────────────────────────────────────────┐   │
│  │ OpenIdProviderListView                               │   │
│  │  - Zeigt verfügbare Provider (Google, Facebook, ...) │   │
│  │  - Provider-spezifisches Styling                     │   │
│  └──────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
          │                                    ▲
          ▼                                    │
┌──────────────────┐              ┌─────────────────────────┐
│ OpenID Provider  │              │ ImtCore Backend (GQL)   │
│ (Google, FB, ..) │              │  - ExchangeOpenIdCode   │
│  - Authorization │              │  - RefreshOpenIdToken   │
│  - Token-Endpkt  │              │  - GetOpenIdProviders   │
└──────────────────┘              └─────────────────────────┘
```

---

## 1. Voraussetzungen

### 1.1 Server-Seite

Der ImtCore-Backend-Server muss folgende GQL-Endpunkte bereitstellen (definiert in `Sdl/imtauth/1.0/OpenIdConnect.sdl`):

| Endpunkt | Typ | Beschreibung |
|----------|-----|-------------|
| `GetOpenIdProviders` | Query | Liefert die Liste der konfigurierten OIDC-Provider |
| `ExchangeOpenIdCode` | Mutation | Tauscht den Authorization Code gegen Tokens |
| `RefreshOpenIdToken` | Mutation | Erneuert abgelaufene Tokens |

### 1.2 Provider-Registrierung

Jeder OpenID Provider muss auf dem Server konfiguriert werden mit:

- **providerId** — Eindeutige Kennung (z.B. `"google"`, `"facebook"`, `"apple"`)
- **providerType** — Enum: `GOOGLE`, `FACEBOOK`, `APPLE` oder `GENERIC`
- **clientId** — Die Client-ID aus der Provider-Entwicklerkonsole
- **authorizationEndpoint** — URL für die Benutzer-Autorisierung
- **tokenEndpoint** — URL für den Token-Austausch
- **scopes** — Gewünschte Berechtigungen (Standard: `"openid profile email"`)

#### Bekannte Provider-Endpunkte

| Provider | Authorization Endpoint | Token Endpoint | Scopes |
|----------|----------------------|----------------|--------|
| Google | `https://accounts.google.com/o/oauth2/v2/auth` | `https://oauth2.googleapis.com/token` | `openid profile email` |
| Facebook | `https://www.facebook.com/v18.0/dialog/oauth` | `https://graph.facebook.com/v18.0/oauth/access_token` | `openid email public_profile` |
| Apple | `https://appleid.apple.com/auth/authorize` | `https://appleid.apple.com/auth/token` | `openid name email` |

---

## 2. Initialisierung

### 2.1 OpenID Connect aktivieren

Im Anwendungscode muss die OIDC-Initialisierung aufgerufen werden, typischerweise nach dem Start der Anwendung und der Serververbindung:

```qml
// In der Hauptanwendung (z.B. ApplicationMain.qml)
Component.onCompleted: {
    // Redirect-URI setzen (muss mit der Provider-Konfiguration übereinstimmen)
    AuthorizationController.openIdRedirectUri = "https://meine-app.example.com/auth/callback";

    // OIDC-Provider vom Server laden
    AuthorizationController.initOpenId();
}
```

### 2.2 Was passiert bei initOpenId()

1. `AuthorizationController.initOpenId()` wird aufgerufen
2. `OpenIdConnectProvider.loadProviders()` sendet die GQL-Query `GetOpenIdProviders`
3. Der Server antwortet mit der Liste der konfigurierten Provider
4. `openIdEnabled` wird auf `true` gesetzt, falls Provider vorhanden
5. `openIdProviders` wird mit den Provider-Daten befüllt
6. Die `AuthorizationPage` zeigt automatisch die Provider-Buttons an

---

## 3. Benutzer-Workflows

### 3.1 Neuen Benutzer anlegen (Registrierung über OpenID Provider)

Bei der Erstanmeldung über einen OpenID Provider wird automatisch ein neuer Benutzer angelegt. Der Workflow ist identisch zum Login — der Server erkennt, ob der Benutzer bereits existiert.

```
Benutzer                    App (QML)                    Provider              Server
   │                           │                            │                     │
   │  Klickt "Sign in with     │                            │                     │
   │  Google"                  │                            │                     │
   │ ─────────────────────────▶│                            │                     │
   │                           │  buildAuthorizationUrl()   │                     │
   │                           │  (PKCE code_verifier +     │                     │
   │                           │   state generiert)         │                     │
   │                           │                            │                     │
   │  ◀─── Browser/WebView ───▶│  authorizationUrl          │                     │
   │       öffnet sich         │ ──────────────────────────▶│                     │
   │                           │                            │                     │
   │  Gibt Zugangsdaten ein    │                            │                     │
   │  und erteilt Zustimmung   │                            │                     │
   │ ─────────────────────────────────────────────────────▶ │                     │
   │                           │                            │                     │
   │                           │  Redirect mit ?code=...    │                     │
   │                           │  &state=...                │                     │
   │                           │ ◀──────────────────────────│                     │
   │                           │                            │                     │
   │                           │  handleOpenIdCallback()    │                     │
   │                           │  - State validieren        │                     │
   │                           │  - ExchangeOpenIdCode()    │                     │
   │                           │ ──────────────────────────────────────────────▶  │
   │                           │                            │                     │
   │                           │                            │   Server:           │
   │                           │                            │   1. Code einlösen  │
   │                           │                            │   2. Benutzer       │
   │                           │                            │      anlegen/finden │
   │                           │                            │   3. Session + JWT  │
   │                           │                            │      erstellen      │
   │                           │                            │                     │
   │                           │  OpenIdTokenPayload        │                     │
   │                           │  (accessToken, refreshToken│, userId, ...)       │
   │                           │ ◀────────────────────────────────────────────────│
   │                           │                            │                     │
   │  Eingeloggt! ◀────────── │  setLoginData() →          │                     │
   │                           │  loggedIn()                │                     │
```

**Wichtig:** Der Server ist dafür verantwortlich, bei `ExchangeOpenIdCode`:
1. Den Authorization Code beim Provider gegen Tokens einzutauschen
2. Die Benutzerinformationen (E-Mail, Name) vom Provider abzurufen
3. Einen neuen Benutzer anzulegen, falls dieser noch nicht existiert
4. Einen bestehenden Benutzer zu verknüpfen, falls die E-Mail-Adresse bereits bekannt ist
5. Eine interne Session (JWT) zu erstellen und zurückzugeben

### 3.2 Login über OpenID Provider (bestehender Benutzer)

Der Login-Workflow ist identisch zur Registrierung. Der einzige Unterschied liegt auf der Server-Seite: Statt einen neuen Benutzer anzulegen, wird der vorhandene Benutzer anhand der Provider-Kennung (Subject-ID) oder E-Mail-Adresse zugeordnet.

### 3.3 Login über die UI

Auf der `AuthorizationPage` erscheint unterhalb des klassischen Login-Formulars ein Trennstrich mit "oder", gefolgt von den verfügbaren Provider-Buttons:

```
┌─────────────────────────────────────┐
│         Willkommen                  │
│                                     │
│  Login:    [________________]       │
│  Passwort: [________________]       │
│                                     │
│  ☐ Angemeldet bleiben               │
│                                     │
│         [ Anmelden ]                │
│                                     │
│  ──────── oder ────────             │
│                                     │
│   [ G  Sign in with Google    ]     │
│   [ f  Sign in with Facebook  ]     │
│   [    Sign in with Apple     ]     │
│                                     │
└─────────────────────────────────────┘
```

Die Provider-Buttons werden nur angezeigt, wenn `AuthorizationController.openIdEnabled === true`, d.h. wenn mindestens ein Provider serverseitig konfiguriert ist.

---

## 4. Token-Verwaltung

### 4.1 Token-Speicherung

Nach erfolgreichem Login werden die Tokens über die bestehende Infrastruktur gespeichert:

- **Desktop:** Qt.labs.settings (`AuthorizationController.storage`)
- **Web:** Browser localStorage

Die gleichen Mechanismen wie bei der Passwort-Anmeldung werden verwendet:
- `SetGlobalAccessToken()` — setzt den Bearer-Token für alle GQL-Requests
- `SetRefreshToken()` — speichert den Refresh-Token
- "Angemeldet bleiben" — speichert den Refresh-Token persistent

### 4.2 Token-Refresh

Wenn der Access-Token abläuft (HTTP 401), wird automatisch ein Refresh durchgeführt:

```
1. XmlHttpRequestProxy erkennt 401 Unauthorized
2. AuthorizationController.refreshTokenGqlSender wird ausgelöst
3. Neuer Access-Token + Refresh-Token werden gesetzt
4. Ursprüngliche Anfrage wird wiederholt
```

Für OpenID-spezifische Token-Erneuerung steht zusätzlich bereit:

```qml
AuthorizationController.refreshOpenIdToken(providerId, currentRefreshToken);
```

### 4.3 Logout

Der Logout-Workflow ist identisch zur Passwort-Anmeldung:

```qml
AuthorizationController.logout();
```

Dies löscht alle gespeicherten Tokens, die Session und die UI-Daten.

---

## 5. QML-Komponenten-Referenz

### 5.1 OpenIdConnectProvider

**Import:** `import imtauthgui 1.0`

Zentrale Komponente für die OIDC-Logik. Verwaltet Provider-Konfigurationen, generiert Authorization-URLs und tauscht Codes gegen Tokens.

| Eigenschaft | Typ | Beschreibung |
|------------|-----|-------------|
| `redirectUri` | `string` | Redirect-URI für den OIDC-Callback |
| `providers` | `var` (Array) | Liste der verfügbaren Provider |
| `configured` | `bool` | `true`, wenn Provider geladen |
| `activeProviderId` | `string` | Aktuell verwendeter Provider |

| Signal | Parameter | Beschreibung |
|--------|-----------|-------------|
| `providersLoaded()` | — | Provider-Liste wurde geladen |
| `loginSucceeded(...)` | accessToken, refreshToken, idToken, userId, username, systemId, permissions | Login erfolgreich |
| `loginFailed(errorMessage)` | Fehlermeldung | Login fehlgeschlagen |

| Funktion | Parameter | Beschreibung |
|----------|-----------|-------------|
| `loadProviders()` | — | Lädt Provider-Liste vom Server |
| `buildAuthorizationUrl(providerId)` | Provider-ID | Erzeugt die Authorization-URL |
| `handleRedirectCallback(urlString)` | Callback-URL | Verarbeitet den OIDC-Redirect |
| `refreshToken(providerId, token)` | Provider-ID, Refresh-Token | Erneuert den Token |

### 5.2 OpenIdProviderListView

**Import:** `import imtauthgui 1.0`

Zeigt die Liste der verfügbaren OpenID Provider als Buttons mit provider-spezifischem Styling.

| Eigenschaft | Typ | Beschreibung |
|------------|-----|-------------|
| `providers` | `var` (Array) | Provider-Daten aus `AuthorizationController.openIdProviders` |

| Signal | Parameter | Beschreibung |
|--------|-----------|-------------|
| `providerSelected(providerId)` | Provider-ID | Benutzer hat einen Provider gewählt |

### 5.3 OpenIdLoginButton

**Import:** `import imtauthgui 1.0`

Generischer OpenID-Login-Button (für eigene Layouts).

| Eigenschaft | Typ | Beschreibung |
|------------|-----|-------------|
| `text` | `string` | Button-Beschriftung |
| `loading` | `bool` | Zeigt Lade-Zustand an |

### 5.4 AuthorizationController — Neue OpenID-API

| Eigenschaft | Typ | Beschreibung |
|------------|-----|-------------|
| `openIdEnabled` | `bool` | Ob OIDC-Provider verfügbar sind |
| `openIdRedirectUri` | `string` | Redirect-URI (muss gesetzt werden) |
| `openIdProviders` | `var` (Array) | Liste der Provider-Daten |

| Signal | Parameter | Beschreibung |
|--------|-----------|-------------|
| `openIdLoginStarted(authorizationUrl)` | Authorization-URL | OIDC-Flow wurde gestartet — URL muss im Browser/WebView geöffnet werden |

| Funktion | Parameter | Beschreibung |
|----------|-----------|-------------|
| `initOpenId()` | — | Lädt die Provider-Konfiguration |
| `loginWithOpenId(providerId)` | Provider-ID | Startet den OIDC-Login-Flow |
| `handleOpenIdCallback(urlString)` | Redirect-URL | Verarbeitet den Callback |
| `refreshOpenIdToken(providerId, token)` | Provider-ID, Token | Erneuert den Token |

---

## 6. SDL-Schema

Die GraphQL-Schnittstelle ist in `Sdl/imtauth/1.0/OpenIdConnect.sdl` definiert:

```graphql
enum OpenIdProviderType {
    GENERIC
    GOOGLE
    FACEBOOK
    APPLE
}

type OpenIdProviderInfo {
    providerId: ID!
    providerType: OpenIdProviderType!
    displayName: String!
    issuer: String!
    authorizationEndpoint: String!
    tokenEndpoint: String!
    userinfoEndpoint: String
    jwksUri: String
    clientId: String!
    scopes: String
}

# Query: Verfügbare Provider abrufen
Query {
    GetOpenIdProviders: OpenIdProvidersPayload!
}

# Mutation: Authorization Code gegen Tokens tauschen
Mutation {
    ExchangeOpenIdCode(input: OpenIdAuthorizationCodeInput): OpenIdTokenPayload!
    RefreshOpenIdToken(input: OpenIdRefreshTokenInput): OpenIdTokenPayload!
}
```

---

## 7. Integrations-Beispiel

### 7.1 Vollständiges Beispiel: OpenID in einer Anwendung

```qml
import QtQuick 2.12
import imtauthgui 1.0
import imtgui 1.0

ApplicationMain {
    id: app;

    // Redirect-URI konfigurieren
    Component.onCompleted: {
        AuthorizationController.openIdRedirectUri = getRedirectUri();
        AuthorizationController.initOpenId();
    }

    function getRedirectUri() {
        if (Qt.platform.os === "web") {
            return Qt.resolvedUrl("/auth/callback");
        }
        return "imtcore://auth/callback";
    }

    // Reagieren auf openIdLoginStarted: Browser/WebView öffnen
    Connections {
        target: AuthorizationController

        function onOpenIdLoginStarted(authorizationUrl) {
            if (Qt.platform.os === "web") {
                // Web: Redirect im selben Fenster
                Qt.openUrlExternally(authorizationUrl);
            } else {
                // Desktop: Externen Browser öffnen
                Qt.openUrlExternally(authorizationUrl);
            }
        }
    }

    // Deep-Link / URL-Callback verarbeiten
    // (Implementierung plattformabhängig, z.B. über URL-Schema-Handler)
    function handleDeepLink(url) {
        AuthorizationController.handleOpenIdCallback(url);
    }
}
```

### 7.2 Provider-spezifische Besonderheiten

#### Google
- Unterstützt PKCE vollständig
- Scopes: `openid profile email`
- Erfordert eine OAuth 2.0 Client-ID aus der [Google Cloud Console](https://console.cloud.google.com/)

#### Facebook
- OAuth 2.0 mit eingeschränkter OpenID Connect Unterstützung
- Scopes: `openid email public_profile`
- Erfordert eine App-ID aus dem [Facebook Developer Portal](https://developers.facebook.com/)

#### Apple
- Erfordert besondere Behandlung: Apple sendet Benutzerinformationen nur beim ersten Login
- Scopes: `openid name email`
- Erfordert einen Service Identifier aus dem [Apple Developer Portal](https://developer.apple.com/)
- Apple erfordert einen `response_mode=form_post` Parameter (serverseitig zu behandeln)

---

## 8. Sicherheitshinweise

### 8.1 PKCE

Die aktuelle Implementierung verwendet die **plain** Code-Challenge-Methode. Für Produktionsumgebungen wird empfohlen, die **S256**-Methode zu implementieren, die eine SHA-256-Hashfunktion erfordert. Dafür ist eine native C++ Bridge oder die Web Crypto API (`crypto.subtle.digest`) erforderlich.

### 8.2 State-Parameter

Der `state`-Parameter schützt gegen CSRF-Angriffe. Er wird bei jedem Login-Versuch neu generiert und beim Callback validiert.

### 8.3 Token-Sicherheit

- Access-Tokens und Refresh-Tokens werden nur clientseitig im lokalen Speicher abgelegt (Qt Settings / localStorage)
- Tokens werden niemals in URLs übertragen (außer dem einmaligen Authorization Code)
- Der eigentliche Token-Austausch erfolgt serverseitig über den sicheren Token-Endpunkt

### 8.4 Zufallszahlen

Die aktuelle Implementierung verwendet `Math.random()` für die Generierung von Code-Verifier und State. Für höhere Sicherheitsanforderungen sollte ein kryptographisch sicherer Zufallsgenerator über eine C++ Bridge (`QRandomGenerator::securelySeeded()`) oder die Web Crypto API bereitgestellt werden.

---

## 9. Fehlerbehebung

| Problem | Mögliche Ursache | Lösung |
|---------|-----------------|--------|
| Keine Provider-Buttons sichtbar | Server hat keine Provider konfiguriert | `GetOpenIdProviders` prüfen |
| "Invalid state parameter" | CSRF-Schutz ausgelöst, oder State abgelaufen | Erneut versuchen |
| "No authorization code received" | Provider hat keinen Code gesendet | Redirect-URI prüfen |
| Login erfolgreich, aber keine Berechtigungen | Server-Mapping nicht konfiguriert | Provider-Benutzer → Rollen-Mapping prüfen |
| Callback wird nicht erkannt | Redirect-URI stimmt nicht überein | URI auf Client und Provider-Seite vergleichen |
