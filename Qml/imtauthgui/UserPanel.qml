import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthProfileSdl 1.0

Item {
	id: userPanel;
	
	property string permissionPath: ""
	// Only the rail row is as wide as the navigation panel; in the classic top
	// panel the row keeps its legacy button-sized geometry.
	width: userPanel.menuPanelRef ? Style.sizeHintXXS : 50;
	height: userPanel.menuPanelRef ? Style.controlHeightL : Style.controlHeightM;
	
	property bool enabled: false;
	
	property bool isExitButton: false;

	// The panel is a row in the navigation rail: folded it is the account mark
	// alone, open it carries the login and the organisation under it.
	property bool collapsed: false;
	property Item menuPanelRef: null;

	// The rail hangs its settings control on this row while there is width for it.
	property bool settingsButtonVisible: false;
	signal settingsClicked();

	// Concrete transport injected into the Profile dialog (mirrors how
	// TenantCollectionView owns tenantManagementApiClient for the TenantEditor).
	property GqlBasedProfileApiClient profileApiClient: GqlBasedProfileApiClient {}

	Component.onCompleted: {
		Events.subscribeEvent("SetUserPanelEnabled", userPanel.setUserPanelEnabled);
		userPanel.__loadOrganizations();
	}
	
	Component.onDestruction: {
		Events.unSubscribeEvent("SetUserPanelEnabled", userPanel.setUserPanelEnabled);
	}

	Connections {
		target: AuthorizationController;
		
		function onLoggedIn(){
			userPanel.enabled = true;
			userPanel.__loadOrganizations();
		}
		
		function onLoggedOut(){
			userPanel.enabled = false;
		}

		function onTenantSelected(tenantId){
			userPanel.__loadOrganizations();
		}

		function onTenantInvitationAccepted(tenantId, membershipId){
			userPanel.__loadOrganizations();
		}

		function onTenantInvitationRejected(tenantId, membershipId){
			userPanel.__loadOrganizations();
		}

		function onTenantsEnabledChanged(){
			userPanel.__loadOrganizations();
		}

		function onOrganizationsChanged(){
			userPanel.__loadOrganizations();
		}

		function onTenantMembershipRemoved(tenantId, userId){
			userPanel.__loadOrganizations();
		}

		function onTenantOwnershipTransferred(tenantId){
			userPanel.__loadOrganizations();
		}

		// An open menu shows an accepted or a new invitation at once
		function onPendingInvitationsChanged(){
			contextMenuModel.fillModel();
		}
	}

	Connections {
		target: userPanel.profileApiClient;

		function onOrganizationOperationFailed(message){
			PopupManager.addErrorMessage(message, true);
		}
	}

	// The reasons to read the organizations come in bursts (a login selects an organization too): one request serves them all
	Timer {
		id: organizationsReloadTimer
		interval: 100
		onTriggered: {
			// The session can end while the read waits
			if (!AuthorizationController.tenantsEnabled || !AuthorizationController.userTokenProvider.userId)
				return
			organizationsRequest.send(userPanel.organizationsInput)
			// The invitations follow the organizations: a membership change can answer or bring one
			AuthorizationController.refreshPendingInvitations()
		}
	}
	
	function setUserPanelEnabled(enabled){
		userPanel.enabled = enabled;
	}
	
	function setVisible(visible){
		userPanel.visible = visible;
	}

	function openMenu(){
		if (userPanel.isExitButton){
			AuthorizationController.logout();
			return;
		}

		contextMenuModel.fillModel();
		// A missed notification must not hide an invitation: the open menu takes the answer at once
		AuthorizationController.refreshPendingInvitations();

		ModalDialogManager.openDialog(popupMenu, {"model": contextMenuModel});
	}

	function updateHint(){
		if (!userPanel.menuPanelRef){
			return;
		}

		if (userPanel.collapsed && accountArea.containsMouse){
			userPanel.menuPanelRef.showHint(usernameText.text,
				userPanel.mapToItem(userPanel.menuPanelRef, 0, userPanel.height / 2).y);
		}
		else {
			userPanel.menuPanelRef.hideHint();
		}
	}

	// A page the user has no permission for is not in the rail: a command to it would do nothing
	function __hasPage(pageId){
		if (!userPanel.menuPanelRef || !userPanel.menuPanelRef.model)
			return true
		var pages = userPanel.menuPanelRef.model
		for (var i = 0; i < pages.getItemsCount(); i++){
			if (pages.getData("id", i) === pageId)
				return true
		}
		return false
	}

	// --- Organizations list for context menu ---
	property var __organizationsList: []

	// The server names the relation in English: Owner, Creator, Member, Delegated Member
	function __roleText(role, isDelegated){
		if (isDelegated)
			return qsTr("Delegated")
		if (role === "Owner")
			return qsTr("Owner")
		if (role === "Creator")
			return qsTr("Creator")
		return qsTr("Member")
	}

	function __loadOrganizations() {
		if (!AuthorizationController.tenantsEnabled || !AuthorizationController.userTokenProvider.userId)
			return
		organizationsReloadTimer.restart()
	}

	GqlSdlRequestSender {
		permissionPath: userPanel.permissionPath
		id: organizationsRequest
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
								if (String(org.m_id) === AuthorizationController.currentTenantId && org.m_name) {
									AuthorizationController.updateCurrentTenantName(String(org.m_name))
								}
								var isDelegated = org.m_isDelegated || false
								list.push({
									id: org.m_id || "",
									name: org.m_name || org.m_id || "",
									role: userPanel.__roleText(String(org.m_role || ""), isDelegated),
									isDelegated: isDelegated
								})
							}
						}
					}
					userPanel.__organizationsList = list
					contextMenuModel.fillModel()
				}
			}
		}
	}

	property GetProfileInput organizationsInput: GetProfileInput {
		m_id: AuthorizationController.userTokenProvider.userId
		m_productId: AuthorizationController.productId
	}

	// Same geometry as a navigation row: the marker spans the rail, the avatar sits
	// on the rail's icon line, and the labels start where every page label does.
	Rectangle {
		id: marker;

		anchors.left: parent.left;
		anchors.leftMargin: Style.marginS;
		anchors.top: parent.top;
		anchors.bottom: parent.bottom;

		width: (userPanel.menuPanelRef ? userPanel.menuPanelRef.width : userPanel.width) - 2 * Style.marginS;

		radius: Style.marginS;
		color: accountArea.containsMouse && userPanel.enabled ? Style.menuPanelItemHoverColor : "transparent";
	}

	Rectangle {
		id: accountAvatar;

		anchors.left: parent.left;
		anchors.leftMargin: userPanel.menuPanelRef ? Style.marginL - (width - Style.menuPanelIconSize) / 2 : (userPanel.width - width) / 2;
		anchors.verticalCenter: userPanel.verticalCenter;

		width: Style.iconSizeL - Style.marginXXS;
		height: width;
		radius: width / 2;

		color: Style.imagingToolsAccentColor;

		Text {
			anchors.centerIn: parent;

			text: Functions.initialsOf(AuthorizationController.userTokenProvider.login);
			color: Style.baseColor;
			font.family: Style.fontFamilyBold;
			font.pixelSize: Style.fontSizeS;
			font.bold: true;
		}
	}

	Column {
		id: accountText;

		anchors.left: accountAvatar.right;
		// Pulled back by the amount the avatar overhangs the rail's icon size, so
		// the login starts on the same line as every page label.
		anchors.leftMargin: Style.marginM - (accountAvatar.width - Style.menuPanelIconSize) / 2;
		anchors.right: invitationCount.visible ? invitationCount.left : settingsButton.visible ? settingsButton.left : marker.right;
		anchors.rightMargin: Style.marginXS;
		anchors.verticalCenter: userPanel.verticalCenter;

		// The classic top panel reserves room for the avatar only, so the login
		// and the organisation are shown in the rail row alone.
		visible: userPanel.menuPanelRef !== null && !userPanel.collapsed;

		Text {
			id: usernameText;

			width: parent.width;

			color: Style.textColor;
			font.family: Style.fontFamily;
			font.pixelSize: Style.fontSizeM;
			text: AuthorizationController.userTokenProvider.login;
			elide: Text.ElideRight;
		}

		Text {
			id: tenantText;

			width: parent.width;

			color: Style.inactiveTextColor;
			font.family: Style.fontFamily;
			font.pixelSize: Style.fontSizeS;
			text: AuthorizationController.currentTenantName ? AuthorizationController.currentTenantName : qsTr("Personal space");
			elide: Text.ElideRight;
			visible: AuthorizationController.tenantsEnabled && text !== "";
		}

	}

	MouseArea {
		id: accountArea;
		objectName: "UserPanelButton";

		anchors.fill: marker;

		enabled: userPanel.enabled;
		hoverEnabled: true;
		cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;

		onClicked: {
			userPanel.openMenu();
		}

		onContainsMouseChanged: {
			userPanel.updateHint();
		}
	}

	// Declared after the row's own mouse area so the gear takes the clicks that
	// land on it, while the rest of the row still opens the account menu.
	ToolButton {
		id: settingsButton;
		objectName: "PreferenceButton";

		anchors.right: marker.right;
		anchors.rightMargin: Style.marginXS;
		anchors.verticalCenter: userPanel.verticalCenter;

		width: Style.controlHeightM;
		height: width;

		visible: userPanel.settingsButtonVisible && !userPanel.collapsed;

		tooltipText: qsTr("Settings");
		iconSource: "qrc:/" + Style.getIconPath("Icons/Settings", Icon.State.On, Icon.Mode.Normal);

		decorator: Component {
			ToolButtonDecorator {
				icon.width: Style.iconSizeS;
				radius: height / 2;
			}
		}

		onClicked: {
			userPanel.settingsClicked();
		}
	}

	// The pending invitations as the rail shows a badge: the count before the gear, like the one of a page
	Rectangle {
		id: invitationCount
		objectName: "InvitationCount"

		anchors.right: settingsButton.visible ? settingsButton.left : marker.right
		anchors.rightMargin: Style.marginXS
		anchors.verticalCenter: userPanel.verticalCenter

		height: Style.fontSizeXS + Style.marginXS
		width: Math.max(height, invitationCountText.width + Style.marginS)
		radius: height / 2
		visible: accountText.visible && AuthorizationController.tenantsEnabled && AuthorizationController.pendingInvitationsCount > 0
		color: Style.imagingToolsAccentColor

		Text {
			id: invitationCountText

			anchors.centerIn: parent

			text: AuthorizationController.pendingInvitationsCount
			color: Style.highlightedTextColor
			font.family: Style.fontFamilyBold
			font.pixelSize: Style.fontSizeXS
		}
	}

	// Folded, the row has no room for the count: a dot on the avatar, ringed to stand out of its color
	Rectangle {
		id: invitationDot
		objectName: "InvitationDot"

		anchors.top: accountAvatar.top
		anchors.right: accountAvatar.right
		anchors.topMargin: -Style.marginXXS
		anchors.rightMargin: -Style.marginXXS

		width: Style.spacingM + 2 * border.width
		height: width
		radius: width / 2
		visible: !accountText.visible && AuthorizationController.tenantsEnabled && AuthorizationController.pendingInvitationsCount > 0
		color: Style.imagingToolsAccentColor
		border.width: Style.marginXXS
		border.color: Style.baseColor
		z: accountAvatar.z + 1
	}

	ListModel {
		id: contextMenuModel;
		
		Component.onCompleted: {
			fillModel();
		}
		
		// Every row carries the same keys - a ListModel takes its roles from the
		// first item it is given, so a later row cannot introduce one.
		function appendRow(itemType, id, name, icon, isEnabled, checked, badge, detail){
			contextMenuModel.append({
				"itemType": itemType,
				"id": id,
				"name": name,
				"icon": icon,
				"isEnabled": isEnabled,
				"checked": checked,
				"badge": badge,
				"detail": detail
			});
		}

		// The one place to switch: the invitations, the personal space and the organizations with the role in each
		function fillModel(){
			var currentTenantId = AuthorizationController.currentTenantId || "";

			contextMenuModel.clear();

			contextMenuModel.appendRow("header", "", "", "", false, false, false, "");
			contextMenuModel.appendRow("separator", "", "", "", false, false, false, "");

			if (AuthorizationController.tenantsEnabled){
				var invitations = AuthorizationController.pendingInvitations;
				if (invitations.length > 0){
					contextMenuModel.appendRow("caption", "", qsTr("Invitations: %1").arg(invitations.length), "", false, false, false, "");
					for (var j = 0; j < invitations.length; j++){
						contextMenuModel.appendRow("invitation", invitations[j].id, invitations[j].tenantName || invitations[j].tenantId,
							"", true, false, false, "");
					}
					contextMenuModel.appendRow("separator", "", "", "", false, false, false, "");
				}

				contextMenuModel.appendRow("caption", "", qsTr("Work in"), "", false, false, false, "");
				contextMenuModel.appendRow("action", "NoOrganization", qsTr("Personal space"), "Icons/Account",
					currentTenantId !== "", currentTenantId === "", false, "");

				for (var i = 0; i < userPanel.__organizationsList.length; i++) {
					var orgData = userPanel.__organizationsList[i];
					if (!orgData || !orgData.id)
						continue;

					var isCurrent = orgData.id === currentTenantId;

					contextMenuModel.appendRow("action", "Organization:" + orgData.id, orgData.name || "",
						"Icons/Organization", !isCurrent, isCurrent, false, orgData.role);
				}

				if (userPanel.__hasPage("Tenants"))
					contextMenuModel.appendRow("action", "AllOrganizations", qsTr("All organizations"), "", true, false, false, "");
				contextMenuModel.appendRow("separator", "", "", "", false, false, false, "");
			}

			contextMenuModel.appendRow("action", "Profile", qsTr("Profile"), "Icons/Account", true, false, false, "");
			contextMenuModel.appendRow("action", "Logout", qsTr("Logout"), "Icons/Exit", true, false, false, "");
		}
	}

	Component {
		id: popupMenu;
		
		PopupMenuDialog {
			id: popupMenuDialog;

			itemWidth: AuthorizationController.tenantsEnabled ? Style.sizeHintS : Style.sizeHintXS;
			itemHeight: Style.controlHeightS + Style.spacingXS;
			shownItemsCount: 14;
			radius: Style.radiusM;
			opacity: 0;

			// The row sits at the foot of the rail, so the list has to be placed
			// once its own height is known - otherwise it opens off the bottom.
			onHeightChanged: {
				popupMenuDialog.placeNextToPanel();
			}

			// Only enabled actions can be reached with the keyboard - headers,
			// separators and captions are passed over.
			function isSelectableIndex(index){
				if (index < 0 || index >= popupMenuDialog.model.count){
					return false;
				}

				return popupMenuDialog.model.get(index).itemType === "action" && popupMenuDialog.model.get(index).isEnabled;
			}

			function onKeyboardUp(){
				for (var i = popupMenuDialog.selectedIndex - 1; i >= 0; i--){
					if (popupMenuDialog.isSelectableIndex(i)){
						popupMenuDialog.selectedIndex = i;
						popupMenuDialog.decorator_.contentYCorrection(false);
						return;
					}
				}
			}

			function onKeyboardDown(){
				for (var i = popupMenuDialog.selectedIndex + 1; i < popupMenuDialog.model.count; i++){
					if (popupMenuDialog.isSelectableIndex(i)){
						popupMenuDialog.selectedIndex = i;
						popupMenuDialog.decorator_.contentYCorrection(true);
						return;
					}
				}
			}

			function onReturn(){
				if (!popupMenuDialog.isSelectableIndex(popupMenuDialog.selectedIndex)){
					return;
				}

				popupMenuDialog.finished(popupMenuDialog.model.get(popupMenuDialog.selectedIndex).id, popupMenuDialog.selectedIndex);
			}

			function placeNextToPanel(){
				if (popupMenuDialog.height === 0){
					return;
				}

				let point = userPanel.mapToItem(null, Style.marginS, 0);

				popupMenuDialog.x = point.x;

				if (point.y + userPanel.height + popupMenuDialog.height > ModalDialogManager.activeView.height){
					popupMenuDialog.y = point.y - popupMenuDialog.height - Style.marginXS;
					popupMenuDialog.isUpwards = true;
				}
				else {
					popupMenuDialog.y = point.y + userPanel.height + Style.marginXS;
					popupMenuDialog.isUpwards = false;
				}

				popupMenuDialog.opacity = 1;
			}

			delegate: Component {
				Item {
					width: popupMenuDialog.width;

					height: model.itemType === "header" ? Style.controlHeightL + Style.marginL
						: model.itemType === "separator" ? Style.marginM
						: model.itemType === "caption" ? Style.controlHeightS + Style.marginXS
						: model.itemType === "invitation" ? Style.controlHeightM + Style.marginXS
						: Style.controlHeightS + Style.spacingXS;

					Item {
						id: invitationRow;

						anchors.fill: parent;

						visible: model.itemType === "invitation";

						Text {
							anchors.left: parent.left;
							anchors.leftMargin: Style.marginM + Style.marginXS;
							anchors.right: invitationButtons.left;
							anchors.rightMargin: Style.marginM;
							anchors.verticalCenter: parent.verticalCenter;

							text: model.name;
							textFormat: Text.PlainText;
							color: Style.textColor;
							font.family: Style.fontFamily;
							font.pixelSize: Style.fontSizeM;
							elide: Text.ElideRight;
						}

						Row {
							id: invitationButtons;

							anchors.right: parent.right;
							anchors.rightMargin: Style.marginM;
							anchors.verticalCenter: parent.verticalCenter;

							spacing: Style.spacingS;

							Rectangle {
								width: acceptText.contentWidth + 2 * Style.marginM;
								height: Style.controlHeightS;
								radius: Style.buttonRadius;
								color: Style.buttonPrimaryColor;
								opacity: acceptArea.containsMouse ? 0.85 : 1;

								Text {
									id: acceptText;

									anchors.centerIn: parent;

									text: qsTr("Accept");
									color: Style.buttonPrimaryTextColor;
									font.family: Style.fontFamily;
									font.pixelSize: Style.fontSizeS;
								}

								MouseArea {
									id: acceptArea;

									anchors.fill: parent;

									hoverEnabled: true;
									cursorShape: Qt.PointingHandCursor;

									onClicked: {
										userPanel.profileApiClient.acceptInvitation(model.id);
									}
								}
							}

							Rectangle {
								width: rejectText.contentWidth + 2 * Style.marginM;
								height: Style.controlHeightS;
								radius: Style.buttonRadius;
								color: rejectArea.containsMouse ? Style.menuPanelItemHoverColor : "transparent";
								border.width: 1;
								border.color: Style.borderColor;

								Text {
									id: rejectText;

									anchors.centerIn: parent;

									text: qsTr("Reject");
									color: Style.textColor;
									font.family: Style.fontFamily;
									font.pixelSize: Style.fontSizeS;
								}

								MouseArea {
									id: rejectArea;

									anchors.fill: parent;

									hoverEnabled: true;
									cursorShape: Qt.PointingHandCursor;

									onClicked: {
										userPanel.profileApiClient.rejectInvitation(model.id);
									}
								}
							}
						}
					}

					Item {
						id: headerRow;

						anchors.fill: parent;

						visible: model.itemType === "header";

						Rectangle {
							id: avatar;

							anchors.left: parent.left;
							anchors.leftMargin: Style.marginM + Style.marginXS;
							anchors.verticalCenter: parent.verticalCenter;

							width: Style.iconSizeXL;
							height: width;
							radius: width / 2;
							color: Style.imagingToolsAccentColor;

							Text {
								anchors.centerIn: parent;

								text: Functions.initialsOf(AuthorizationController.userTokenProvider.login);
								color: Style.baseColor;
								font.family: Style.fontFamilyBold;
								font.pixelSize: Style.fontSizeM;
								font.bold: true;
							}
						}

						Column {
							anchors.left: avatar.right;
							anchors.leftMargin: Style.marginM;
							anchors.right: parent.right;
							anchors.rightMargin: Style.marginM;
							anchors.verticalCenter: parent.verticalCenter;

							Text {
								width: parent.width;

								text: AuthorizationController.userTokenProvider.login;
								textFormat: Text.PlainText;
								color: Style.titleColor;
								font.family: Style.fontFamilyBold;
								font.pixelSize: Style.fontSizeM;
								elide: Text.ElideRight;
							}

							Text {
								width: parent.width;

								text: AuthorizationController.currentTenantName ? AuthorizationController.currentTenantName : qsTr("Personal space");
								textFormat: Text.PlainText;
								color: Style.inactiveTextColor;
								font.family: Style.fontFamily;
								font.pixelSize: Style.fontSizeS;
								elide: Text.ElideRight;
							}
						}
					}

					Rectangle {
						anchors.left: parent.left;
						anchors.leftMargin: Style.marginM;
						anchors.right: parent.right;
						anchors.rightMargin: Style.marginM;
						anchors.verticalCenter: parent.verticalCenter;

						height: 1;
						opacity: 0.5;
						color: Style.borderColor;

						visible: model.itemType === "separator";
					}

					Text {
						anchors.left: parent.left;
						anchors.leftMargin: Style.marginM + Style.marginXS;
						anchors.right: parent.right;
						anchors.rightMargin: Style.marginM;
						anchors.bottom: parent.bottom;

						visible: model.itemType === "caption";

						text: model.name;
						color: Style.inactiveTextColor;
						font.family: Style.fontFamily;
						font.pixelSize: Style.fontSizeS;
						elide: Text.ElideRight;
					}

					Item {
						id: actionRow;

						anchors.fill: parent;

						visible: model.itemType === "action";

						// The row under the pointer or keyboard takes the accent, as in VS Code menus.
						property bool active: model.isEnabled && (actionArea.containsMouse || popupMenuDialog.selectedIndex === model.index);

						Rectangle {
							anchors.fill: parent;
							anchors.leftMargin: Style.marginXXS;
							anchors.rightMargin: Style.marginXXS;

							radius: Style.buttonRadius;
							color: actionRow.active ? Style.buttonPrimaryColor : "transparent";
						}

						Image {
							id: actionIcon;

							anchors.left: parent.left;
							anchors.leftMargin: Style.marginM + Style.marginXS;
							anchors.verticalCenter: parent.verticalCenter;

							width: Style.menuPanelIconSize;
							height: width;
							sourceSize.width: width;
							sourceSize.height: height;

							visible: model.icon !== "";
							source: model.icon === "" ? "" : "qrc:/" + Style.getIconPath(model.icon, Icon.State.On,
																model.isEnabled ? Icon.Mode.Normal : Icon.Mode.Disabled);
							opacity: actionRow.active ? 1.0 : Style.opacityHigh;
						}

						Rectangle {
							anchors.top: actionIcon.top;
							anchors.right: actionIcon.right;

							width: Style.spacingS;
							height: width;
							radius: width / 2;
							color: Style.imagingToolsAccentColor;

							visible: model.badge && actionIcon.visible;
						}

						Text {
							anchors.left: actionIcon.visible ? actionIcon.right : parent.left;
							anchors.leftMargin: actionIcon.visible ? Style.marginM : Style.marginM + Style.marginXS;
							anchors.right: detailText.visible ? detailText.left : checkIcon.visible ? checkIcon.left : parent.right;
							anchors.rightMargin: Style.marginM;
							anchors.verticalCenter: parent.verticalCenter;

							text: model.name;
							textFormat: Text.PlainText;
							color: actionRow.active ? Style.buttonPrimaryTextColor : model.isEnabled ? Style.textColor : Style.inactiveTextColor;
							font.family: Style.fontFamily;
							font.pixelSize: Style.fontSizeM;
							elide: Text.ElideRight;
							verticalAlignment: Text.AlignVCenter;
						}

						// The role in the organization, at the right edge
						Text {
							id: detailText;

							anchors.right: checkIcon.visible ? checkIcon.left : parent.right;
							anchors.rightMargin: Style.marginM;
							anchors.verticalCenter: parent.verticalCenter;

							visible: model.detail !== "";

							text: model.detail;
							textFormat: Text.PlainText;
							color: actionRow.active ? Style.buttonPrimaryTextColor : Style.inactiveTextColor;
							font.family: Style.fontFamily;
							font.pixelSize: Style.fontSizeS;
						}

						// The organisation you are already in reads as picked
						// rather than as unusable, which is what greying it alone
						// used to say.
						Image {
							id: checkIcon;

							anchors.right: parent.right;
							anchors.rightMargin: Style.marginM + Style.marginXS;
							anchors.verticalCenter: parent.verticalCenter;

							width: Style.iconSizeS;
							height: width;
							sourceSize.width: width;
							sourceSize.height: height;

							visible: model.checked;
							source: "qrc:/" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Selected);
						}

						MouseArea {
							id: actionArea;

							anchors.fill: parent;

							enabled: model.isEnabled;
							hoverEnabled: true;
							cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;

							onClicked: {
								popupMenuDialog.finished(model.id, model.index);
							}

							onEntered: {
								popupMenuDialog.selectedIndex = model.index;
							}
						}
					}
				}
			}

			onFinished: {
				if (commandId == "Logout"){
					AuthorizationController.logout();
				}
				else if (commandId == "NoOrganization"){
					if (AuthorizationController.currentTenantId !== "") {
						AuthorizationController.selectTenant("");
					}
				}
				else if (commandId == "Profile"){
					ModalDialogManager.openDialog(profileViewComp, {});
				}
				else if (commandId == "AllOrganizations"){
					Events.sendEvent("ChangePage", "Tenants");
				}
				else if (commandId.indexOf("Organization:") === 0) {
					var tenantId = commandId.substring("Organization:".length);
					if (tenantId !== "" && tenantId !== AuthorizationController.currentTenantId) {
						AuthorizationController.selectTenant(tenantId);
					}
				}
			}
		}
	}

	Component {
		id: profileViewComp;
		
		Dialog {
			id: dialog;
			// A settings-sized window rather than a near full-screen one; it shrinks with a small window.
			width: Math.min(ModalDialogManager.activeView.width - 2 * Style.marginXL, 960);
			height: Math.min(ModalDialogManager.activeView.height - 2 * Style.marginXL, 680);
			title: qsTr("Profile");
			canMove: false;
			backgroundColor: Style.baseColor;
			contentComp: Component {
				ProfileView {
					width: dialog.width;
					height: dialog.height - 40;
					apiClient: userPanel.profileApiClient;
				}
			}
		}
	}
}

