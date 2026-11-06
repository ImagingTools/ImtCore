// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12

/**
 * ServerTokenApiClient
 *
 * Abstract contract for the tokens of the servers of a product.
 * This file MUST NOT import imtguigql or any SDL module.
 */
QtObject {
	// Model of the tokens of the last requested product; each item has m_id, m_productId, m_scopes and m_revoked.
	property var tokens: null

	// Model of the servers taking tokens; each item has m_productId and m_permissions (m_id, m_name, m_description, m_required).
	property var productServers: null

	signal tokensReceived()
	signal productServersReceived()
	signal tokenCreated(string token)
	signal operationFailed(string message)

	function requestTokens(productId) {}
	function requestProductServers(productId) {}
	function createToken(productId, receiverId, name, description, scopes) {}
	function updateTokenScopes(productId, tokenId, scopes) {}
	function revokeToken(productId, tokenId) {}
}
