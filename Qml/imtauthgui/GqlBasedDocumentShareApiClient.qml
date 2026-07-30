// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthDocumentSharesSdl 1.0
import imtauthgui 1.0

/**
 * GqlBasedDocumentShareApiClient
 *
 * GQL/SDL implementation of the abstract DocumentShareApiClient contract.
 * This is the ONLY place that imports the document shares SDL module and owns
 * the GqlSdlRequestSender instances for the document share operations.
 *
 * Pages depend only on the abstract contract; the orchestrator injects this
 * concrete client.
 */
QtObject {
	id: root

	// =========================================================================
	// Configuration
	// =========================================================================

	property string tenantId: AuthorizationController.currentTenantId

	// =========================================================================
	// Abstract contract (must mirror DocumentShareApiClient.qml)
	// =========================================================================

	readonly property int accessModeReadOnly: DocumentShareAccessModeEnum.s_readOnly
	readonly property int accessModeFullAccess: DocumentShareAccessModeEnum.s_fullAccess

	readonly property int shareStatusActive: DocumentShareStatusEnum.s_active
	readonly property int shareStatusRevoked: DocumentShareStatusEnum.s_revoked
	readonly property int shareStatusExpired: DocumentShareStatusEnum.s_expired

	signal requestFailed(string message)

	signal documentShared(string shareId)
	signal documentShareAccessModeChanged(string shareId)
	signal documentShareRevoked(string shareId)
	signal documentSharesReceived(var shares)
	signal documentAccessResolved(bool hasAccess, int accessMode)

	property ListModel documentSharesModel: ListModel {}

	// =========================================================================
	// Share a document
	// =========================================================================

	property ShareDocumentInput __shareDocumentInput: ShareDocumentInput {}
	property GqlSdlRequestSender __shareDocumentSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthDocumentSharesSdlCommandIds.s_shareDocument

		sdlObjectComp: Component {
			ShareDocumentPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.documentShared(m_shareId || "")
					}
				}
			}
		}
	}

	// =========================================================================
	// Change the granted access mode
	// =========================================================================

	property SetDocumentShareAccessModeInput __setAccessModeInput: SetDocumentShareAccessModeInput {}
	property string __pendingAccessModeShareId: ""
	property GqlSdlRequestSender __setAccessModeSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthDocumentSharesSdlCommandIds.s_setDocumentShareAccessMode

		sdlObjectComp: Component {
			SetDocumentShareAccessModePayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.documentShareAccessModeChanged(root.__pendingAccessModeShareId)
					}
				}
			}
		}
	}

	// =========================================================================
	// Revoke a share
	// =========================================================================

	property RevokeDocumentShareInput __revokeShareInput: RevokeDocumentShareInput {}
	property string __pendingRevokeShareId: ""
	property GqlSdlRequestSender __revokeShareSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthDocumentSharesSdlCommandIds.s_revokeDocumentShare

		sdlObjectComp: Component {
			RevokeDocumentSharePayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.documentShareRevoked(root.__pendingRevokeShareId)
					}
				}
			}
		}
	}

	// =========================================================================
	// List shares
	// =========================================================================

	property GetDocumentSharesInput __getSharesInput: GetDocumentSharesInput {
		m_direction: "Incoming"
	}
	property GqlSdlRequestSender __getSharesSender: GqlSdlRequestSender {
		gqlCommandId: ImtauthDocumentSharesSdlCommandIds.s_getDocumentShares

		sdlObjectComp: Component {
			GetDocumentSharesPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.__populateDocumentSharesModel(m_shares)
					}
				}
			}
		}
	}

	// =========================================================================
	// Resolve the access on a document
	// =========================================================================

	property GetDocumentAccessInput __getAccessInput: GetDocumentAccessInput {}
	property GqlSdlRequestSender __getAccessSender: GqlSdlRequestSender {
		gqlCommandId: ImtauthDocumentSharesSdlCommandIds.s_getDocumentAccess

		sdlObjectComp: Component {
			GetDocumentAccessPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.documentAccessResolved(m_hasAccess || false, m_accessMode || DocumentShareAccessModeEnum.s_readOnly)
					}
				}
			}
		}
	}

	function __populateDocumentSharesModel(shares) {
		root.documentSharesModel.clear()
		if (shares) {
			for (var i = 0; i < shares.count; ++i) {
				var share = shares.get(i).item
				if (!share)
					continue
				root.documentSharesModel.append({
					"id": share.m_id || "",
					"shareId": share.m_id || "",
					"sourceTenantId": share.m_sourceTenantId || "",
					"targetTenantId": share.m_targetTenantId || "",
					"relationshipId": share.m_relationshipId || "",
					"documentId": share.m_documentId || "",
					"documentType": share.m_documentType || "",
					"documentName": share.m_documentName || "",
					"accessMode": share.m_accessMode || DocumentShareAccessModeEnum.s_readOnly,
					"status": share.m_status || DocumentShareStatusEnum.s_active,
					"sharedByUserId": share.m_sharedByUserId || "",
					"createdAt": share.m_createdAt || "",
					"updatedAt": share.m_updatedAt || "",
					"expiresAt": share.m_expiresAt || "",
					"revokedAt": share.m_revokedAt || ""
				})
			}
		}
		root.documentSharesReceived(shares || [])
	}

	function fetchDocumentShares(tenantId, direction, documentId) {
		root.__getSharesInput.m_tenantId = tenantId || root.tenantId || ""
		if (direction && direction !== "")
			root.__getSharesInput.m_direction = direction
		root.__getSharesInput.m_documentId = documentId || ""
		root.__getSharesSender.send(root.__getSharesInput)
	}

	function shareDocument(sourceTenantId, targetTenantId, relationshipId, documentId, documentType, documentName, accessMode, expiresAt) {
		root.__shareDocumentInput.m_sourceTenantId = sourceTenantId || root.tenantId || ""
		root.__shareDocumentInput.m_targetTenantId = targetTenantId || ""
		root.__shareDocumentInput.m_relationshipId = relationshipId || ""
		root.__shareDocumentInput.m_documentId = documentId || ""
		root.__shareDocumentInput.m_documentType = documentType || ""
		root.__shareDocumentInput.m_documentName = documentName || ""
		root.__shareDocumentInput.m_accessMode = accessMode || DocumentShareAccessModeEnum.s_readOnly
		root.__shareDocumentInput.m_expiresAt = expiresAt || ""
		root.__shareDocumentSender.send(root.__shareDocumentInput)
	}

	function setDocumentShareAccessMode(shareId, accessMode) {
		root.__pendingAccessModeShareId = shareId || ""
		root.__setAccessModeInput.m_shareId = shareId || ""
		root.__setAccessModeInput.m_accessMode = accessMode || DocumentShareAccessModeEnum.s_readOnly
		root.__setAccessModeSender.send(root.__setAccessModeInput)
	}

	function revokeDocumentShare(shareId) {
		root.__pendingRevokeShareId = shareId || ""
		root.__revokeShareInput.m_shareId = shareId || ""
		root.__revokeShareSender.send(root.__revokeShareInput)
	}

	function fetchDocumentAccess(tenantId, documentId) {
		root.__getAccessInput.m_tenantId = tenantId || root.tenantId || ""
		root.__getAccessInput.m_documentId = documentId || ""
		root.__getAccessSender.send(root.__getAccessInput)
	}
}
