// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12

/**
 * ServerTokenApiClient
 *
 * Abstract contract for the tokens of the servers of a product.
 * This file MUST NOT import imtguigql or any SDL module.
 */
QtObject {
	// The client product whose tokens are managed
	property string productId: ""

	// Model of the tokens of the product; each item has m_id, m_productId, m_scopes and m_revoked.
	property var tokens: null

	// This server, which issues the tokens for itself: {groups, requiredPermissions},
	// groups as PermissionsTableView.rebuildFromFlatArray takes them; null until it is read.
	property var productServer: null

	signal tokenCreated(string token)
	signal operationFailed(string message)

	function requestTokens() {}
	function requestProductServer() {}
	function createToken(name, description, scopes) {}
	function updateTokenScopes(tokenId, scopes) {}
	function revokeToken(tokenId) {}
}
