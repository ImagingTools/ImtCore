// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0

/**
 * ProfileApiClient
 *
 * Abstract contract (interface) for the user Profile operations:
 *   - profile data (load / save)
 *   - organizations (list / switch is done via AuthorizationController)
 *   - tenant invitations (accept / reject) and leaving an organization
 *   - Personal Access Tokens (list / create / delete / revoke)
 *
 * The ProfileView orchestrator and its pages depend on this contract — NOT on any
 * concrete transport (GQL/SDL). A concrete implementation (e.g. GqlBasedProfileApiClient)
 * provides the actual transport.
 *
 * This file MUST NOT import imtguigql or any SDL module.
 */
QtObject {
	id: root

	// True while any request is in flight (drives the ProfileView loading overlay).
	property bool loading: false

	// Provider used by the "New Token" dialog to populate the scopes tree.
	// A concrete implementation exposes a PermissionsProvider-compatible object.
	property var permissionsProvider: null

	// --- Profile ---
	// Cached result of the last successful getProfile() call, so a page that
	// mounts (or re-mounts) after the fetch already completed can read the
	// current value immediately instead of waiting for another round-trip.
	property var lastProfile: null
	signal profileReceived(var profile)
	signal profileSaved()

	// --- Organizations ---
	// list: plain JS array of { id, name, description, isDelegated }
	property var lastOrganizations: []
	signal organizationsReceived(var list)

	// --- Invitations / leave ---
	signal invitationAccepted()
	signal invitationRejected()
	signal leftTenant(string tenantId)

	// --- Personal Access Tokens ---
	// tokens: the PersonalAccessTokenList model (opaque to pages, used as table elements)
	signal tokenListReceived(var tokens)
	signal tokenCreated(string token)
	signal tokenDeleted(string message)
	signal tokenRevoked(string message)

	// --- Errors, scoped by concern so each page can listen only to what's
	// relevant to its own actions (a token-page error must never surface a
	// banner on the Organizations page, and vice versa). ---
	signal profileOperationFailed(string message)
	signal organizationOperationFailed(string message)
	signal tokenOperationFailed(string message)

	// --- Stub methods (overridden by concrete implementations) ---
	function getProfile() {}
	function setProfile(id, name, email) {}

	function getOrganizations() {}

	function acceptInvitation(invitationId) {}
	function rejectInvitation(invitationId) {}
	function leaveTenant(tenantId) {}

	function getTokenList() {}
	function createToken(name, description, scopes, expiresAt) {}
	function deleteToken(tokenId) {}
	function revokeToken(tokenId) {}
}
