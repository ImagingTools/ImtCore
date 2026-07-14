import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtguigql 1.0
import imtauthProfileSdl 1.0

Item {
	id: userPanel;
	
	width: 50;
	height: Style.controlHeightM;
	
	property bool enabled: false;
	
	property alias iconSource: loginButton.iconSource;
	property bool isExitButton: false;

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
	}
	
	function setUserPanelEnabled(enabled){
		userPanel.enabled = enabled;
	}
	
	function setVisible(visible){
		userPanel.visible = visible;
	}

	// --- Organizations list for context menu ---
	property var __organizationsList: []

	function __loadOrganizations() {
		if (!AuthorizationController.userTokenProvider.userId)
			return
		organizationsRequest.send(organizationsInput)
	}

	GqlSdlRequestSender {
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
								var displayName = org.m_name || org.m_id || ""
								var isDelegated = org.m_isDelegated || false
								if (isDelegated) {
									displayName = displayName + " " + qsTr("(delegated)")
								}
								list.push({
									id: org.m_id || "",
									name: displayName,
									isDelegated: isDelegated,
									delegatedRoles: org.m_delegatedRoles || []
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

	Text {
		id: tenantText;
		anchors.verticalCenter: userPanel.verticalCenter;
		anchors.right: usernameText.left;
		anchors.rightMargin: Style.marginM;
		color: Style.inactiveTextColor;
		font.family: Style.fontFamily;
		font.pixelSize: Style.fontSizeM;
		text: AuthorizationController.currentTenantName ? AuthorizationController.currentTenantName : "";
		visible: text !== "";
	}

	Text {
		id: usernameText;
		anchors.verticalCenter: userPanel.verticalCenter;
		anchors.right: loginButton.left;
		anchors.rightMargin: Style.marginXS;
		color: Style.textColor;
		font.family: Style.fontFamily;
		font.pixelSize: Style.fontSizeM;
		text: AuthorizationController.userTokenProvider.login;
	}
	
	ToolButton {
		id: loginButton;
		
		anchors.right: parent ? parent.right : undefined;
		anchors.verticalCenter: userPanel.verticalCenter;
		
		width: Style.buttonWidthM;
		height: width;
		
		iconSource: "../../../" + Style.getIconPath("Icons/Account", Icon.State.On, Icon.Mode.Normal);
		
		enabled: userPanel.enabled;
		
		onClicked: {
			if(userPanel.isExitButton){
				AuthorizationController.logout();
			}
			else{
				contextMenuModel.fillModel();
				
				var point = mapToItem(null, x - width, y + height);
				point.x = point.x - 200;
				
				// menu.open()
				ModalDialogManager.openDialog(popupMenu, {"x": point.x, "y": point.y, "model": contextMenuModel});
			}
		}
	}

	Rectangle {
		id: invitationBadge
		anchors.top: loginButton.top
		anchors.right: loginButton.right
		width: Style.spacingM
		height: width
		visible: AuthorizationController.pendingInvitationsCount > 0
		radius: width/2
		color: Style.imaginToolsAccentColor
		z: parent.z + 1
	}

	ListModel {
		id: contextMenuModel;
		
		Component.onCompleted: {
			fillModel();
		}
		
		function fillModel(){
			var currentTenantId = AuthorizationController.currentTenantId || "";
			var hasInvitations = AuthorizationController.pendingInvitationsCount > 0;
			var profileName = hasInvitations ? qsTr("Profile") + " ●" : qsTr("Profile");
			contextMenuModel.clear();
			contextMenuModel.append({"id": "Profile", "name": profileName, "icon": "Icons/Account", "isEnabled": true});
			contextMenuModel.append({"id": "", "name": "", "Icon": ""});

			for (var i = 0; i < userPanel.__organizationsList.length; i++) {
				var orgData = userPanel.__organizationsList[i];
				if (!orgData || !orgData.id)
					continue;

				var isCurrent = orgData.id === currentTenantId;
				var orgName = orgData.name || "";
				if (isCurrent)
					orgName = orgName + " " + qsTr("(current)");

				contextMenuModel.append({
					"id": "Organization:" + orgData.id,
					"name": orgName,
					"icon": "",
					"isEnabled": !isCurrent
				});
			}

			contextMenuModel.append({"id": "NoOrganization", "name": qsTr("No organization"), "icon": "", "isEnabled": currentTenantId !== ""});
			contextMenuModel.append({"id": "", "name": "", "Icon": ""});
			contextMenuModel.append({"id": "Logout", "name": qsTr("Logout"), "icon": "Icons/Exit", "isEnabled": true});
		}
	}

	Component {
		id: popupMenu;
		
		PopupMenuDialog {
			id: popupMenuDialog;
			shownItemsCount: 10
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
				else if (commandId.indexOf("Organization:") === 0) {
					var tenantId = commandId.substring("Organization:".length);
					if (tenantId !== "" && tenantId !== AuthorizationController.currentTenantId) {
						AuthorizationController.selectTenant(tenantId);
					}
				}
			}
		}
	}

	Menu {
		id: menu
		x: loginButton.x - menu.width
		y: loginButton.y + loginButton.height + Style.spacingM
		closePolicy: Enums.popupCloseOnEscape | Enums.popupCloseOnPressOutsideParent

		MenuItem {
			id: profileItem
			text: qsTr("Profile")
			iconSource: "qrc:/" + Style.getIconPath("Icons/Account", Icon.State.On, Icon.Mode.Normal)
			onTriggered: {
				ModalDialogManager.openDialog(profileViewComp, {})
			}
		}
		Menu {
			id: organizationsSubmenu
			title: qsTr("Organization")
			Instantiator {
				model: userPanel.__organizationsList.length

				delegate: MenuItem {
					property var orgData: userPanel.__organizationsList[index]
					text: orgData ? orgData.name : ""
					checked: orgData && orgData.id === AuthorizationController.currentTenantId
					onTriggered: {
						if (orgData && orgData.id !== AuthorizationController.currentTenantId) {
							AuthorizationController.selectTenant(orgData.id)
						}
					}
				}

				onObjectAdded:{ organizationsSubmenu.insertItem(index, object) }
				onObjectRemoved: { organizationsSubmenu.removeItem(object) }
			}

			MenuSeparator {
				visible: userPanel.__organizationsList.length > 0
			}

			MenuItem {
				text: qsTr("No organization")
				checked: AuthorizationController.currentTenantId === ""
				onTriggered: {
					if (AuthorizationController.currentTenantId !== "") {
						AuthorizationController.selectTenant("")
					}
				}
			}
		}

		MenuSeparator {}
		MenuItem {
			text: qsTr("Logout")
			iconSource: "qrc:/" + Style.getIconPath("Icons/Exit", Icon.State.On, Icon.Mode.Normal)
			onTriggered: {
				AuthorizationController.logout()
			}
		}

		onAboutToShow: {
			userPanel.__loadOrganizations()
		}
	}

	Component {
		id: profileViewComp;
		
		Dialog {
			id: dialog;
			width: Math.max(Style.sizeHintXXL, Math.min(ModalDialogManager.activeView.width - 160, 1120));
			height: ModalDialogManager.activeView.height - 100;
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

