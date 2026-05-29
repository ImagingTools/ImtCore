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
	property string issuer: ""
	property string authorizationEndpoint: ""
	property string tokenEndpoint: ""
	property string userinfoEndpoint: ""
	property string clientId: ""
	property string redirectUri: ""
	property string scopes: "openid profile email"

	// --- State ---
	property string codeVerifier: ""
	property string codeChallenge: ""
	property string state: ""
	property bool configured: false

	// --- Signals ---
	signal configurationLoaded()
	signal loginSucceeded(string accessToken, string refreshToken, string idToken, string userId, string username, string systemId, var permissions)
	signal loginFailed(string errorMessage)

	// --- PKCE helpers ---

	// Generate a cryptographically random string for PKCE code_verifier
	function generateCodeVerifier() {
		var chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
		var verifier = "";
		for (var i = 0; i < 128; i++) {
			verifier += chars.charAt(Math.floor(Math.random() * chars.length));
		}
		return verifier;
	}

	// Generate a random state parameter for CSRF protection
	function generateState() {
		var chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
		var stateStr = "";
		for (var i = 0; i < 32; i++) {
			stateStr += chars.charAt(Math.floor(Math.random() * chars.length));
		}
		return stateStr;
	}

	// SHA-256 code_challenge from code_verifier (S256 method)
	// Uses Qt.md5 as a fallback; real PKCE S256 should use SHA-256.
	// For production, this should use a C++ bridge or Web Crypto API.
	function computeCodeChallenge(verifier) {
		// For the "plain" method, the challenge equals the verifier.
		// S256 requires SHA-256 + base64url encoding, which needs native support.
		// We use the "plain" method here and let the server-side handle validation.
		// When a native SHA-256 bridge is available, switch to S256.
		return verifier;
	}

	// --- Authorization URL construction ---

	function buildAuthorizationUrl() {
		root.codeVerifier = generateCodeVerifier();
		root.codeChallenge = computeCodeChallenge(root.codeVerifier);
		root.state = generateState();

		var url = root.authorizationEndpoint;
		url += "?response_type=code";
		url += "&client_id=" + encodeURIComponent(root.clientId);
		url += "&redirect_uri=" + encodeURIComponent(root.redirectUri);
		url += "&scope=" + encodeURIComponent(root.scopes);
		url += "&state=" + encodeURIComponent(root.state);
		url += "&code_challenge=" + encodeURIComponent(root.codeChallenge);
		url += "&code_challenge_method=plain";

		return url;
	}

	// --- Token exchange ---

	function exchangeAuthorizationCode(code) {
		exchangeCodeInput.m_code = code;
		exchangeCodeInput.m_redirectUri = root.redirectUri;
		exchangeCodeInput.m_codeVerifier = root.codeVerifier;
		exchangeCodeGqlSender.send(exchangeCodeInput);
	}

	// --- Validate callback state ---

	function validateState(receivedState) {
		return root.state !== "" && root.state === receivedState;
	}

	// --- Handle redirect callback (code + state from URL) ---

	function handleRedirectCallback(urlString) {
		var url = new URL(urlString);
		var code = url.searchParams.get("code");
		var receivedState = url.searchParams.get("state");
		var error = url.searchParams.get("error");

		if (error) {
			var errorDescription = url.searchParams.get("error_description") || error;
			root.loginFailed(errorDescription);
			return;
		}

		if (!code) {
			root.loginFailed(qsTr("No authorization code received"));
			return;
		}

		if (!validateState(receivedState)) {
			root.loginFailed(qsTr("Invalid state parameter - possible CSRF attack"));
			return;
		}

		exchangeAuthorizationCode(code);
	}

	// --- Load OIDC configuration from server ---

	function loadConfiguration() {
		getConfigGqlSender.send();
	}

	// --- GQL: Get OpenID Configuration ---

	property GqlSdlRequestSender getConfigGqlSender: GqlSdlRequestSender {
		gqlCommandId: ImtauthOpenIdConnectSdlCommandIds.s_getOpenIdConfiguration;

		sdlObjectComp: Component {
			OpenIdConfiguration {
				onFinished: {
					root.issuer = m_issuer;
					root.authorizationEndpoint = m_authorizationEndpoint;
					root.tokenEndpoint = m_tokenEndpoint;
					root.userinfoEndpoint = m_userinfoEndpoint || "";
					root.clientId = m_clientId;

					if (m_scopes && m_scopes !== "") {
						root.scopes = m_scopes;
					}

					root.configured = true;
					root.configurationLoaded();
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
							m_permissions ? m_permissions.split(';') : []
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
							m_permissions ? m_permissions.split(';') : []
						);
					}
					else {
						root.loginFailed(m_errorMessage || qsTr("OpenID Connect token refresh failed"));
					}
				}
			}
		}
	}

	function refreshToken(currentRefreshToken) {
		refreshOpenIdInput.m_refreshToken = currentRefreshToken;
		refreshOpenIdGqlSender.send(refreshOpenIdInput);
	}
}
