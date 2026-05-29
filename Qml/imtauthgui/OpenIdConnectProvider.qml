import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtauthOpenIdConnectSdl 1.0

QtObject {
	id: root;

	// --- Configuration ---
	property string redirectUri: ""
	property var providers: []
	property bool configured: false

	// --- Per-login state ---
	property string activeProviderId: ""
	property string codeVerifier: ""
	property string codeChallenge: ""
	property string stateToken: ""

	// --- Signals ---
	signal providersLoaded()
	signal loginSucceeded(string accessToken, string refreshToken, string idToken, string userId, string username, string systemId, var permissions)
	signal loginFailed(string errorMessage)

	// --- Random string generation ---
	// NOTE: Math.random() is not cryptographically secure. For production
	// deployments, replace with a C++ bridge using QRandomGenerator::securelySeeded()
	// or use the Web Crypto API (crypto.getRandomValues) on the web platform.

	function generateRandomString(length, chars) {
		var result = "";
		for (var i = 0; i < length; i++) {
			result += chars.charAt(Math.floor(Math.random() * chars.length));
		}
		return result;
	}

	function generateCodeVerifier() {
		return generateRandomString(128, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~");
	}

	function generateState() {
		return generateRandomString(32, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
	}

	// --- PKCE ---
	// NOTE: Currently uses the "plain" code_challenge_method. For full S256
	// security, a native SHA-256 + base64url bridge is required. The server
	// must accept "plain" until S256 is implemented on the client side.

	function computeCodeChallenge(verifier) {
		return verifier;
	}

	// --- Permissions parsing helper ---

	function parsePermissions(permissionsStr) {
		if (permissionsStr && permissionsStr !== "") {
			return permissionsStr.split(';');
		}
		return [];
	}

	// --- URL query parameter extraction (QML-compatible, no URL constructor) ---

	function getQueryParam(urlString, paramName) {
		var queryStart = urlString.indexOf("?");
		if (queryStart < 0) {
			return "";
		}
		var queryString = urlString.substring(queryStart + 1);
		var params = queryString.split("&");
		for (var i = 0; i < params.length; i++) {
			var pair = params[i].split("=");
			if (decodeURIComponent(pair[0]) === paramName) {
				return pair.length > 1 ? decodeURIComponent(pair[1]) : "";
			}
		}
		return "";
	}

	// --- Provider lookup ---

	function findProvider(providerId) {
		for (var i = 0; i < root.providers.length; i++) {
			if (root.providers[i].providerId === providerId) {
				return root.providers[i];
			}
		}
		return null;
	}

	// --- Authorization URL construction ---

	function buildAuthorizationUrl(providerId) {
		var provider = findProvider(providerId);
		if (!provider) {
			root.loginFailed(qsTr("Unknown OpenID provider: %1").arg(providerId));
			return "";
		}

		root.activeProviderId = providerId;
		root.codeVerifier = generateCodeVerifier();
		root.codeChallenge = computeCodeChallenge(root.codeVerifier);
		root.stateToken = generateState();

		var url = provider.authorizationEndpoint;
		url += "?response_type=code";
		url += "&client_id=" + encodeURIComponent(provider.clientId);
		url += "&redirect_uri=" + encodeURIComponent(root.redirectUri);
		url += "&scope=" + encodeURIComponent(provider.scopes || "openid profile email");
		url += "&state=" + encodeURIComponent(root.stateToken);
		url += "&code_challenge=" + encodeURIComponent(root.codeChallenge);
		url += "&code_challenge_method=plain";

		return url;
	}

	// --- Token exchange ---

	function exchangeAuthorizationCode(providerId, code) {
		exchangeCodeInput.m_providerId = providerId;
		exchangeCodeInput.m_code = code;
		exchangeCodeInput.m_redirectUri = root.redirectUri;
		exchangeCodeInput.m_codeVerifier = root.codeVerifier;
		exchangeCodeGqlSender.send(exchangeCodeInput);
	}

	// --- Validate callback state ---

	function validateState(receivedState) {
		return root.stateToken !== "" && root.stateToken === receivedState;
	}

	// --- Handle redirect callback (code + state from URL) ---

	function handleRedirectCallback(urlString) {
		var code = getQueryParam(urlString, "code");
		var receivedState = getQueryParam(urlString, "state");
		var error = getQueryParam(urlString, "error");

		if (error !== "") {
			var errorDescription = getQueryParam(urlString, "error_description");
			root.loginFailed(errorDescription !== "" ? errorDescription : error);
			return;
		}

		if (code === "") {
			root.loginFailed(qsTr("No authorization code received"));
			return;
		}

		if (!validateState(receivedState)) {
			root.loginFailed(qsTr("Invalid state parameter - possible CSRF attack"));
			return;
		}

		exchangeAuthorizationCode(root.activeProviderId, code);
	}

	// --- Load OIDC providers from server ---

	function loadProviders() {
		getProvidersGqlSender.send();
	}

	// --- GQL: Get OpenID Providers ---

	property GqlSdlRequestSender getProvidersGqlSender: GqlSdlRequestSender {
		gqlCommandId: ImtauthOpenIdConnectSdlCommandIds.s_getOpenIdProviders;

		sdlObjectComp: Component {
			OpenIdProvidersPayload {
				onFinished: {
					var providerList = [];
					if (m_providers) {
						for (var i = 0; i < m_providers.count; i++) {
							var p = m_providers.get(i);
							providerList.push({
								providerId: p.m_providerId,
								providerType: p.m_providerType,
								displayName: p.m_displayName,
								issuer: p.m_issuer,
								authorizationEndpoint: p.m_authorizationEndpoint,
								tokenEndpoint: p.m_tokenEndpoint,
								userinfoEndpoint: p.m_userinfoEndpoint || "",
								clientId: p.m_clientId,
								scopes: p.m_scopes || "openid profile email"
							});
						}
					}

					root.providers = providerList;
					root.configured = providerList.length > 0;
					root.providersLoaded();
				}
			}
		}
	}

	// --- GQL: Exchange authorization code ---

	property OpenIdAuthorizationCodeInput exchangeCodeInput: OpenIdAuthorizationCodeInput {}

	property GqlSdlRequestSender exchangeCodeGqlSender: GqlSdlRequestSender {
		requestType: 1; // Mutation
		gqlCommandId: ImtauthOpenIdConnectSdlCommandIds.s_exchangeOpenIdCode;

		sdlObjectComp: Component {
			OpenIdTokenPayload {
				onFinished: {
					if (m_ok) {
						root.loginSucceeded(
							m_accessToken,
							m_refreshToken,
							m_idToken,
							m_userId,
							m_username,
							m_systemId,
							root.parsePermissions(m_permissions)
						);
					}
					else {
						root.loginFailed(m_errorMessage || qsTr("OpenID Connect authentication failed"));
					}
				}
			}
		}
	}

	// --- GQL: Refresh OpenID token ---

	property OpenIdRefreshTokenInput refreshOpenIdInput: OpenIdRefreshTokenInput {}

	property GqlSdlRequestSender refreshOpenIdGqlSender: GqlSdlRequestSender {
		requestType: 1; // Mutation
		gqlCommandId: ImtauthOpenIdConnectSdlCommandIds.s_refreshOpenIdToken;

		sdlObjectComp: Component {
			OpenIdTokenPayload {
				onFinished: {
					if (m_ok) {
						root.loginSucceeded(
							m_accessToken,
							m_refreshToken,
							m_idToken,
							m_userId,
							m_username,
							m_systemId,
							root.parsePermissions(m_permissions)
						);
					}
					else {
						root.loginFailed(m_errorMessage || qsTr("OpenID Connect token refresh failed"));
					}
				}
			}
		}
	}

	function refreshToken(providerId, currentRefreshToken) {
		refreshOpenIdInput.m_providerId = providerId;
		refreshOpenIdInput.m_refreshToken = currentRefreshToken;
		refreshOpenIdGqlSender.send(refreshOpenIdInput);
	}
}
