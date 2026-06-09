import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtguigql 1.0
import imtauthProfileSdl 1.0

Item {
	id: root;
	
	width: 50;
	height: Style.controlHeightM;
	
	property bool enabled: false;
	
	property alias iconSource: loginButton.iconSource;
	property bool isExitButton: false;
	
	Component.onCompleted: {
		Events.subscribeEvent("SetUserPanelEnabled", root.setUserPanelEnabled);
	}
	
	Component.onDestruction: {
		Events.unSubscribeEvent("SetUserPanelEnabled", root.setUserPanelEnabled);
	}

	Connections {
		target: AuthorizationController;
		
		function onLoggedIn(){
			root.enabled = true;
		}
		
		function onLoggedOut(){
			root.enabled = false;
		}
	}
	
	function setUserPanelEnabled(enabled){
		root.enabled = enabled;
	}
	
	function setVisible(visible){
		root.visible = visible;
	}

	// --- Organizations list for submenu ---
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
					root.__organizationsList = list
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
		anchors.verticalCenter: root.verticalCenter;
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
		anchors.verticalCenter: root.verticalCenter;
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
		anchors.verticalCenter: root.verticalCenter;
		
		width: Style.buttonWidthM;
		height: width;
		
		iconSource: "../../../" + Style.getIconPath("Icons/Account", Icon.State.On, Icon.Mode.Normal);
		
		enabled: root.enabled;
		
		onClicked: {
			if(root.isExitButton){
				AuthorizationController.logout();
			}
			else{
				menu.open()
			}
		}
	}
	
	Component {
		id: popupMenu;
		
		PopupMenuDialog {
			id: popupMenuDialog;
			
			onFinished: {
				if (commandId == "Logout"){
					AuthorizationController.logout();
				}
				else if (commandId == "Profile"){
					ModalDialogManager.openDialog(profileViewComp, {});
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
				model: root.__organizationsList.length

				delegate: MenuItem {
					property var orgData: root.__organizationsList[index]
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
				visible: root.__organizationsList.length > 0
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
			root.__loadOrganizations()
		}
	}

	Component {
		id: profileViewComp;
		
		Dialog {
			id: dialog;
			width: 760;
			height: ModalDialogManager.activeView.height - 100;
			title: qsTr("Profile");
			canMove: false;
			contentComp: Component {
				ProfileView {
					width: dialog.width;
					height: dialog.height - 40;
				}
			}
		}
	}
}

