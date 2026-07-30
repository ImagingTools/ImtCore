// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0

/**
 * DocumentShareApiClient
 *
 * Abstract contract (interface) for the document share entity:
 *   - share a document with another tenant (read-only or full access)
 *   - list incoming / outgoing shares
 *   - change the granted access mode
 *   - revoke a share
 *   - resolve the access a tenant has on a document
 *
 * Pages depend on this contract — not on any concrete transport (e.g. GQL/SDL).
 *
 * A concrete implementation (e.g. GqlBasedDocumentShareApiClient) provides the
 * actual transport.
 *
 * This file MUST NOT import imtguigql or any SDL module.
 */
QtObject {
	id: root

	property string tenantId: ""

	// Access modes, mirrored from the DocumentShares SDL schema, so that pages
	// do not have to import the SDL module.
	readonly property int accessModeReadOnly: 0
	readonly property int accessModeFullAccess: 1

	// Share lifecycle status, mirrored from the DocumentShares SDL schema.
	readonly property int shareStatusActive: 0
	readonly property int shareStatusRevoked: 1
	readonly property int shareStatusExpired: 2

	signal requestFailed(string message)

	signal documentShared(string shareId)
	signal documentShareAccessModeChanged(string shareId)
	signal documentShareRevoked(string shareId)
	signal documentSharesReceived(var shares)
	signal documentAccessResolved(bool hasAccess, int accessMode)

	property var documentSharesModel: null

	function fetchDocumentShares(tenantId, direction, documentId) {}
	function shareDocument(sourceTenantId, targetTenantId, relationshipId, documentId, documentType, documentName, accessMode, expiresAt) {}
	function setDocumentShareAccessMode(shareId, accessMode) {}
	function revokeDocumentShare(shareId) {}
	function fetchDocumentAccess(tenantId, documentId) {}
}
