// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * ProfileView
 *
 * Thin orchestrator for the user Profile. Composes a MultiPageView over the
 * profile pages (General / Organizations / Access Tokens / Roles & Permissions)
 * above an identity banner. It kicks off the initial profile / organizations
 * fetch and relays cross-cutting concerns (loading, errors, reload triggers) —
 * each page owns and reads its own slice of data directly from `apiClient`
 * rather than having it forwarded down through this container.
 *
 * The view itself does NOT depend on any concrete transport (no GQL/SDL imports).
 * The concrete client (GqlBasedProfileApiClient) is injected by the embedding view
 * (UserPanel) via `apiClient`, mirroring the TenantEditor + apiClient pattern.
 */
ViewBase {
	id: container

	commandsPanelVisible: false
	contentColor: Style.baseColor

	/**
	 * Injected transport implementing the ProfileApiClient contract.
	 * Must be set by the embedding view before the view becomes active.
	 */
	property var apiClient: null

	// Only used to paint the identity banner (avatar + name); each page
	// independently subscribes to the same apiClient signal for its own data.
	property var __headerProfile: null

	// Guards the open-time fetch: Component.onCompleted and onApiClientChanged
	// both fire when apiClient is injected via binding (UserPanel -> Dialog ->
	// ProfileView). Without this flag each open issued GetProfile /
	// GetUserOrganizations twice.
	property bool __initialLoadDone: false

	readonly property string __displayName: container.__headerProfile
		? (container.__headerProfile.m_name || container.__headerProfile.m_username || "")
		: ""

	function __initialsOf(text) {
		if (!text || text === "")
			return "?"
		var parts = text.trim().split(/\s+/)
		if (parts.length === 1)
			return parts[0].substring(0, 2).toUpperCase()
		return (parts[0].charAt(0) + parts[parts.length - 1].charAt(0)).toUpperCase()
	}

	Component.onCompleted: {
		container.__ensureInitialLoad()
	}

	// apiClient is injected via a property binding (UserPanel -> Dialog.contentComp
	// -> ProfileView), several Loader layers deep. If that binding hasn't settled
	// by the time Component.onCompleted runs, the initial fetch would never
	// happen — so also react the moment apiClient actually becomes available.
	// __ensureInitialLoad() is idempotent so the onCompleted + changed race
	// only issues one pair of requests.
	onApiClientChanged: {
		if (!container.apiClient) {
			container.__initialLoadDone = false
			container.__headerProfile = null
			return
		}
		container.__ensureInitialLoad()
	}

	function __ensureInitialLoad() {
		if (!container.apiClient)
			return
		if (container.__initialLoadDone)
			return
		container.__initialLoadDone = true
		container.__headerProfile = container.apiClient.lastProfile
		container.__reload()
	}

	function __reload() {
		if (!container.apiClient)
			return
		container.apiClient.getProfile()
		container.apiClient.getOrganizations()
	}

	Connections {
		target: container.apiClient ? container.apiClient : null

		// Besides updating the banner, explicitly push the freshly received data
		// into any already-loaded pages. Each page also listens to the apiClient
		// directly, but pushing it here too removes any dependency on exactly one
		// propagation path succeeding — in particular it fixes the case where the
		// very first fetch after opening the dialog resolves before that
		// redundancy would otherwise matter.
		function onProfileReceived(profile) {
			container.__headerProfile = profile
			var generalPage = multiPageView.getPageById("General")
			if (generalPage && generalPage.applyProfile)
				generalPage.applyProfile(profile)
			var accessPage = multiPageView.getPageById("Access")
			if (accessPage && accessPage.applyProfile)
				accessPage.applyProfile(profile)
		}
		function onOrganizationsReceived(list) {
			var orgPage = multiPageView.getPageById("Organizations")
			if (orgPage && orgPage.applyOrganizations)
				orgPage.applyOrganizations(list)
		}
		function onProfileSaved() {
			// Refresh from the server's confirmed state — this is the only point
			// where the banner (and General page) should pick up the new name /
			// email, not while the user is still typing. The "Profile saved"
			// notification itself is shown locally by ProfileGeneralPage.
			container.apiClient.getProfile()
		}
		function onInvitationAccepted() {
			container.apiClient.getOrganizations()
		}
		function onInvitationRejected() {
			container.apiClient.getOrganizations()
		}
		function onLeftTenant(tenantId) {
			container.apiClient.getOrganizations()
		}
		// profileOperationFailed / organizationOperationFailed / tokenOperationFailed
		// are shown as in-dialog banners local to the page that triggered them
		// (General / Organizations / Access Tokens respectively), not here.
	}

	// Refresh when the active tenant / membership state changes (same triggers as
	// the previous inline implementation).
	Connections {
		target: AuthorizationController

		function onTenantSelected(tenantId) { container.__reload() }
		function onTenantInvitationAccepted(tenantId, membershipId) { container.__reload() }
		function onTenantInvitationRejected(tenantId, membershipId) { container.__reload() }
		function onTenantOwnershipTransferred(tenantId) { container.__reload() }
		function onTenantMembershipRoleChanged(tenantId, userId, role) { container.__reload() }
		function onTenantMembershipRemoved(tenantId, userId) { container.__reload() }
	}

	// --- Identity banner ---
	Item {
		id: identityBanner
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		height: avatarRow.height + 2 * Style.marginL

		Row {
			id: avatarRow
			anchors.left: parent.left
			anchors.verticalCenter: parent.verticalCenter
			anchors.leftMargin: Style.marginXL
			spacing: Style.marginM

			Rectangle {
				width: 44
				height: 44
				radius: width / 2
				color: Style.imaginToolsAccentColor
				anchors.verticalCenter: parent.verticalCenter

				Text {
					anchors.centerIn: parent
					text: container.__initialsOf(container.__displayName)
					color: Style.baseColor
					font.family: Style.fontFamilyBold
					font.pixelSize: Style.fontSizeM
					font.bold: true
				}
			}

			Column {
				anchors.verticalCenter: parent.verticalCenter
				spacing: 2

				Text {
					text: container.__displayName !== "" ? container.__displayName : qsTr("My Account")
					color: Style.textColor
					font.family: Style.fontFamilyBold
					font.pixelSize: Style.fontSizeL
					font.bold: true
					elide: Text.ElideRight
				}

				Text {
					visible: container.__headerProfile ? container.__headerProfile.m_email !== "" : false
					text: container.__headerProfile ? container.__headerProfile.m_email : ""
					color: Style.inactiveTextColor
					font.family: Style.fontFamily
					font.pixelSize: Style.fontSizeS
				}
			}
		}
	}

	Rectangle {
		id: bannerSeparator
		anchors.top: identityBanner.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		height: 1
		color: Style.borderColor
	}

	MultiPageView {
		id: multiPageView
		anchors.top: bannerSeparator.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		panelWidth: Style.sizeHintXXS

		Component.onCompleted: {
			multiPageView.addPage("General", qsTr("General"), generalPageComp, "Icons/Settings")
			multiPageView.addPage("Organizations", qsTr("Organizations"), organizationsPageComp, "Icons/Organization")
			multiPageView.addPage("AccessTokens", qsTr("Access Tokens"), tokensPageComp, "Icons/Key")
			multiPageView.addPage("Access", qsTr("Roles & Permissions"), accessPageComp, "Icons/Role")
			multiPageView.currentIndex = 0
		}
	}

	Component {
		id: generalPageComp
		ProfileGeneralPage {
			apiClient: container.apiClient
		}
	}

	Component {
		id: organizationsPageComp
		ProfileOrganizationsPage {
			apiClient: container.apiClient
		}
	}

	Component {
		id: tokensPageComp
		ProfileTokensPage {
			apiClient: container.apiClient
		}
	}

	Component {
		id: accessPageComp
		ProfileAccessPage {
			apiClient: container.apiClient
		}
	}

	Loading {
		id: loading
		anchors.fill: parent
		visible: container.apiClient ? container.apiClient.loading : false
		background.color: Style.backgroundColor2
	}
}
