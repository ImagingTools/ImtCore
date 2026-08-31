// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtauthProfileSdl 1.0
import imtauthTenantMembershipsSdl 1.0
import imtauthPersonalAccessTokensSdl 1.0
import imtbaseImtCollectionSdl 1.0

/**
 * GqlBasedProfileApiClient
 *
 * GQL/SDL implementation of the ProfileApiClient contract (must mirror
 * ProfileApiClient.qml's signals/properties — this does NOT instantiate that
 * type; see GqlBasedTenantManagementApiClient for the same convention).
 * This is the ONLY place (on the Profile side) that imports the Profile /
 * TenantMemberships / PersonalAccessTokens SDL modules and owns the
 * GqlSdlRequestSender / GqlBasedPermissionsProvider instances.
 *
 * The orchestrator (ProfileView) and its pages depend only on the abstract
 * contract; the embedding view (UserPanel) injects this concrete client.
 *
 * userId / productId are read from the AuthorizationController singleton
 * internally, so callers never pass identity around.
 */
QtObject {
	id: root

	property string context: ""
	readonly property string __userId: AuthorizationController.userTokenProvider.userId
	readonly property string __productId: AuthorizationController.productId

	// Managed imperatively (set/cleared inside getProfile()/setProfile() and their
	// onFinished handlers below) instead of a reactive binding against the request
	// senders' .state — avoids depending on declaration order between this
	// property and the senders it would otherwise reference.
	property bool loading: false

	property GqlBasedPermissionsProvider __permissionsProvider: GqlBasedPermissionsProvider {
		productId: root.__productId
	}

	property var permissionsProvider: __permissionsProvider

	// =========================================================================
	// Abstract contract (must mirror ProfileApiClient.qml)
	// =========================================================================

	property var lastProfile: null
	signal profileReceived(var profile)
	signal profileSaved()

	property var lastOrganizations: []
	signal organizationsReceived(var list)

	signal invitationAccepted()
	signal invitationRejected()
	signal leftTenant(string tenantId)

	signal tokenListReceived(var tokens)
	signal tokenCreated(string token)
	signal tokenDeleted(string message)
	signal tokenRevoked(string message)

	signal profileOperationFailed(string message)
	signal organizationOperationFailed(string message)
	signal tokenOperationFailed(string message)

	// =========================================================================
	// Profile
	// =========================================================================

	property GqlSdlRequestSender getProfileRequest: GqlSdlRequestSender {
		context: root.context
		gqlCommandId: ImtauthProfileSdlCommandIds.s_getProfile
		sdlObjectComp: Component {
			ProfileData {
				onFinished: {
					root.loading = false
					root.lastProfile = this
					root.profileReceived(this)
				}
			}
		}
		inputObjectComp: Component {
			GetProfileInput {
				m_id: root.__userId
				m_productId: root.__productId
			}
		}
		function onError(message, type) {
			root.loading = false
			root.profileOperationFailed(message)
		}
	}

	property SetProfileInput __setProfileInput: SetProfileInput {}
	property GqlSdlRequestSender setProfileRequest: GqlSdlRequestSender {
		context: root.context
		gqlCommandId: ImtauthProfileSdlCommandIds.s_setProfile
		sdlObjectComp: Component {
			SetProfileResponse {
				onFinished: {
					root.loading = false
					root.profileSaved()
				}
			}
		}
		function onError(message, type) {
			root.loading = false
			root.profileOperationFailed(message)
		}
	}

	function getProfile() {
		root.loading = true
		getProfileRequest.send()
	}

	function setProfile(id, name, email) {
		root.__setProfileInput.m_id = id || ""
		root.__setProfileInput.m_name = name || ""
		root.__setProfileInput.m_email = email || ""
		root.loading = true
		setProfileRequest.send(root.__setProfileInput)
	}

	// =========================================================================
	// Organizations
	// =========================================================================

	property GetProfileInput __organizationsInput: GetProfileInput {
		m_id: root.__userId
		m_productId: root.__productId
	}

	property GqlSdlRequestSender __organizationsRequest: GqlSdlRequestSender {
		context: root.context
		requestType: 0
		gqlCommandId: ImtauthProfileSdlCommandIds.s_getUserOrganizations
		sdlObjectComp: Component {
			GetUserOrganizationsPayload {
				onFinished: {
					var orgs = m_organizations
					var list = []
					if (orgs) {
						for (var i = 0; i < orgs.count; i++) {
							var org = orgs.get(i).item
							if (org && org.m_isActive) {
								var displayName = org.m_name || org.m_id || ""
								var isDelegated = org.m_isDelegated || false
								if (isDelegated)
									displayName = displayName + " " + qsTr("(delegated)")
								list.push({
									id: org.m_id || "",
									name: displayName,
									description: org.m_description || "",
									isDelegated: isDelegated
								})
							}
						}
					}
					// Always provide the No Organization entry to allow switching to
					// the global (non-tenant) context.
					list.push({
						id: "",
						name: qsTr("No organization"),
						description: "",
						isDelegated: false
					})
					root.lastOrganizations = list
					root.organizationsReceived(list)
				}
			}
		}
	}

	function getOrganizations() {
		if (!root.__userId)
			return
		__organizationsRequest.send(root.__organizationsInput)
	}

	// =========================================================================
	// Invitations
	// =========================================================================

	property AcceptTenantInvitationInput __acceptInvitationInput: AcceptTenantInvitationInput {}
	property GqlSdlRequestSender __acceptInvitationSender: GqlSdlRequestSender {
		context: root.context
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_acceptTenantInvitation
		sdlObjectComp: Component {
			AcceptTenantInvitationPayload {
				onFinished: {
					if (m_success) {
						AuthorizationController.tenantInvitationAccepted("", "")
						root.invitationAccepted()
					} else if (m_errorMessage && m_errorMessage !== "") {
						root.organizationOperationFailed(m_errorMessage)
					}
				}
			}
		}
		function onError(message, type) {
			root.organizationOperationFailed(message)
		}
	}

	property RejectTenantInvitationInput __rejectInvitationInput: RejectTenantInvitationInput {}
	property GqlSdlRequestSender __rejectInvitationSender: GqlSdlRequestSender {
		context: root.context
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_rejectTenantInvitation
		sdlObjectComp: Component {
			RejectTenantInvitationPayload {
				onFinished: {
					if (m_success) {
						AuthorizationController.tenantInvitationRejected("", "")
						root.invitationRejected()
					} else if (m_errorMessage && m_errorMessage !== "") {
						root.organizationOperationFailed(m_errorMessage)
					}
				}
			}
		}
		function onError(message, type) {
			root.organizationOperationFailed(message)
		}
	}

	function acceptInvitation(invitationId) {
		root.__acceptInvitationInput.m_invitationId = invitationId || ""
		root.__acceptInvitationSender.send(root.__acceptInvitationInput)
	}

	function rejectInvitation(invitationId) {
		root.__rejectInvitationInput.m_invitationId = invitationId || ""
		root.__rejectInvitationSender.send(root.__rejectInvitationInput)
	}

	// =========================================================================
	// Leave organization (find membership -> remove membership)
	// =========================================================================

	property string __leaveTenantId: ""

	property FindMembershipInput __findMembershipForLeaveInput: FindMembershipInput {}
	property GqlSdlRequestSender __findMembershipForLeaveSender: GqlSdlRequestSender {
		context: root.context
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_findMembership
		sdlObjectComp: Component {
			FindMembershipPayload {
				onFinished: {
					if (m_membership && m_membership.m_id && m_membership.m_id !== "") {
						root.__removeMembershipForLeaveInput.m_membershipId = m_membership.m_id
						root.__removeMembershipForLeaveSender.send(root.__removeMembershipForLeaveInput)
					} else if (m_errorMessage && m_errorMessage !== "") {
						root.organizationOperationFailed(m_errorMessage)
					}
				}
			}
		}
		function onError(message, type) {
			root.organizationOperationFailed(message)
		}
	}

	property RemoveMembershipInput __removeMembershipForLeaveInput: RemoveMembershipInput {}
	property GqlSdlRequestSender __removeMembershipForLeaveSender: GqlSdlRequestSender {
		context: root.context
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_removeMembership
		sdlObjectComp: Component {
			RemoveMembershipPayload {
				onFinished: {
					if (m_success) {
						if (root.__leaveTenantId === AuthorizationController.currentTenantId) {
							AuthorizationController.selectTenant("")
						}
						root.leftTenant(root.__leaveTenantId)
					} else if (m_errorMessage && m_errorMessage !== "") {
						root.organizationOperationFailed(m_errorMessage)
					}
				}
			}
		}
		function onError(message, type) {
			root.organizationOperationFailed(message)
		}
	}

	function leaveTenant(tenantId) {
		if (!tenantId)
			return
		root.__leaveTenantId = tenantId
		root.__findMembershipForLeaveInput.m_userId = root.__userId
		root.__findMembershipForLeaveInput.m_tenantId = tenantId
		root.__findMembershipForLeaveSender.send(root.__findMembershipForLeaveInput)
	}

	// =========================================================================
	// Personal Access Tokens
	// =========================================================================

	property CreateTokenInput __createTokenInput: CreateTokenInput {}
	property InputId __inputId: InputId {}
	property UserIdInput __userIdInput: UserIdInput {}

	property GqlSdlRequestSender __getTokenListRequest: GqlSdlRequestSender {
		context: root.context
		gqlCommandId: ImtauthPersonalAccessTokensSdlCommandIds.s_getTokenList
		sdlObjectComp: Component {
			PersonalAccessTokenList {
				onFinished: {
					root.tokenListReceived(this)
				}
			}
		}
	}

	property GqlSdlRequestSender __createTokenRequest: GqlSdlRequestSender {
		context: root.context
		gqlCommandId: ImtauthPersonalAccessTokensSdlCommandIds.s_createToken
		requestType: 1
		sdlObjectComp: Component {
			CreateTokenPayload {
				onFinished: {
					if (m_success) {
						root.tokenCreated(m_token)
						root.getTokenList()
					} else {
						root.tokenOperationFailed(m_message)
					}
				}
			}
		}
	}

	property GqlSdlRequestSender __deleteTokenRequest: GqlSdlRequestSender {
		context: root.context
		gqlCommandId: ImtauthPersonalAccessTokensSdlCommandIds.s_deleteToken
		requestType: 1
		sdlObjectComp: Component {
			DeleteTokenPayload {
				onFinished: {
					if (m_success) {
						root.tokenDeleted(m_message)
						root.getTokenList()
					} else {
						root.tokenOperationFailed(m_message)
					}
				}
			}
		}
	}

	property GqlSdlRequestSender __revokeTokenRequest: GqlSdlRequestSender {
		context: root.context
		gqlCommandId: ImtauthPersonalAccessTokensSdlCommandIds.s_revokeToken
		requestType: 1
		sdlObjectComp: Component {
			RevokeTokenPayload {
				onFinished: {
					if (m_success) {
						root.tokenRevoked(m_message)
						root.getTokenList()
					} else {
						root.tokenOperationFailed(m_message)
					}
				}
			}
		}
	}

	function getTokenList() {
		root.__userIdInput.m_userId = root.__userId
		root.__getTokenListRequest.send(root.__userIdInput)
	}

	function createToken(name, description, scopes, expiresAt) {
		root.__createTokenInput.m_userId = root.__userId
		root.__createTokenInput.m_productId = root.__productId
		root.__createTokenInput.m_name = name
		root.__createTokenInput.m_description = description
		root.__createTokenInput.m_scopes = scopes
		root.__createTokenInput.m_expiresAt = expiresAt
		root.__createTokenRequest.send(root.__createTokenInput)
	}

	function deleteToken(tokenId) {
		root.__inputId.m_id = tokenId
		root.__deleteTokenRequest.send(root.__inputId)
	}

	function revokeToken(tokenId) {
		root.__inputId.m_id = tokenId
		root.__revokeTokenRequest.send(root.__inputId)
	}
}
