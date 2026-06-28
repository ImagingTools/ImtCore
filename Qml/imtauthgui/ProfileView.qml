import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0
import imtauthgui 1.0
import imtauthProfileSdl 1.0
import imtauthTenantMembershipsSdl 1.0

ViewBase {
	id: container;
	property ProfileData profileData: model
	
	Connections {
		target: container.profileData;
		
		function onModelChanged(){
			saveElementView.buttonEnabled = true;
		}
	}
	
	Connections {
		target: AuthorizationController;
		
		function onTenantSelected(tenantId){
			getProfileRequest.send();
			container.__loadOrganizations();
		}
		
		function onTenantInvitationAccepted(tenantId, membershipId){
			getProfileRequest.send();
			container.__loadOrganizations();
		}
		
		function onTenantInvitationRejected(tenantId, membershipId){
			getProfileRequest.send();
			container.__loadOrganizations();
		}
		
		function onTenantOwnershipTransferred(tenantId){
			getProfileRequest.send();
			container.__loadOrganizations();
		}
		
		function onTenantMembershipRoleChanged(tenantId, userId, role){
			getProfileRequest.send();
			container.__loadOrganizations();
		}
		
		function onTenantMembershipRemoved(tenantId, userId){
			getProfileRequest.send();
			container.__loadOrganizations();
		}
	}
	
	onProfileDataChanged: {
		doUpdateGui();
	}
	
	Component.onCompleted: {
		getProfileRequest.send();
		container.__loadOrganizations();
	}
	
	function updateGui(){
		if (!profileData){
			return
		}

		usernameView.text = profileData.m_username;
		nameInput.text = profileData.m_name;
		mailInput.text = profileData.m_email;
		
		permissionsTable.table.elements = profileData.m_permissions;
		rolesTable.table.elements = profileData.m_roles;
		groupsTable.table.elements = profileData.m_groups;
	}
	
	function updateModel(){
		if (!profileData){
			return
		}

		profileData.m_name = nameInput.text
		profileData.m_email = mailInput.text
	}
	
	
	GqlSdlRequestSender {
		id: getProfileRequest;
		gqlCommandId: ImtauthProfileSdlCommandIds.s_getProfile;
		sdlObjectComp: Component { ProfileData {
				onFinished: {
					container.model = this
				}
		}}
		inputObjectComp: Component { GetProfileInput {
				m_id: AuthorizationController.userTokenProvider.userId;
				m_productId: AuthorizationController.productId;
			}}
	}
	
	GqlSdlRequestSender {
		id: setProfileRequest;
		gqlCommandId: ImtauthProfileSdlCommandIds.s_setProfile;
		sdlObjectComp: Component { SetProfileResponse {}}
		inputObjectComp: Component { SetProfileInput {
				m_id: container.profileData.m_id;
				m_name: container.profileData.m_name;
				m_email: container.profileData.m_email;
			}}
		
		onFinished: {
			saveElementView.buttonEnabled = false;
		}
	}
	
	CustomScrollbar {
		id: scrollbar;
		
		z: parent.z + 1;
		
		anchors.right: parent.right;
		anchors.top: flickable.top;
		anchors.bottom: flickable.bottom;
		
		secondSize: Style.marginM;
		targetItem: flickable;
	}
	
	CustomScrollbar{
		id: scrollHoriz;
		
		z: parent.z + 1;
		
		anchors.left: flickable.left;
		anchors.right: flickable.right;
		anchors.bottom: flickable.bottom;
		
		secondSize: Style.marginM;
		
		vertical: false;
		targetItem: flickable;
	}
	
	Flickable {
		id: flickable;
		anchors.top: parent.top;
		anchors.topMargin: Style.marginXL;
		anchors.bottom: parent.bottom;
		anchors.bottomMargin: Style.marginXL;
		anchors.left: parent.left;
		anchors.leftMargin: Style.marginXL;
		anchors.right: scrollbar.left;
		anchors.rightMargin: Style.marginXL;
		contentWidth: bodyColumn.width;
		contentHeight: bodyColumn.height + 2 * Style.marginXL;
		boundsBehavior: Flickable.StopAtBounds;
		clip: true;
		
		Column {
			id: bodyColumn;
			width: Style.sizeHintXXL;
			spacing: Style.marginXL;
			
			GroupHeaderView {
				width: parent.width;
				title: qsTr("General");
				groupView: generalGroup;
			}
			
			GroupElementView {
				id: generalGroup;
				width: parent.width;
				
				TextInputElementView {
					id: usernameView;
					name: qsTr("Username");
					readOnly: true;
					
					KeyNavigation.tab: nameInput;
					KeyNavigation.backtab: mailInput;
				}
				
				TextInputElementView {
					id: nameInput;
					
					name: qsTr("Name");
					placeHolderText: qsTr("Enter the name");
					readOnly: container.readOnly;
					
					KeyNavigation.tab: mailInput;
					KeyNavigation.backtab: usernameView;
					
					onEditingFinished: {
						container.doUpdateModel();
					}
				}
				
				RegularExpressionValidator {
					id: mailValid;
					
					regularExpression: /\w+([-+.']\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*/;
				}
				
				TextInputElementView {
					id: mailInput;
					
					name: qsTr("Email Address");
					textInputValidator: mailValid;
					
					placeHolderText: qsTr("Enter the email");
					readOnly: container.readOnly;
					KeyNavigation.tab: usernameView;
					KeyNavigation.backtab: nameInput;
					onEditingFinished: {
						container.doUpdateModel();
					}
				}
				
				ButtonElementView {
					id: saveElementView;
					text: qsTr("Save");
					buttonEnabled: false;
					onClicked: {
						setProfileRequest.send();
					}
				}
			}
			
			ElementView {
				id: changePasswordButton;
				name: qsTr("Change password");
				visible: container.profileData ? container.profileData.m_systemId === "" : false;
				
				controlComp: Component {
					Button {
						width: Style.buttonWidthXXL;
						height: Style.controlHeightM;
						text: qsTr("Change");
						onClicked: {
							ModalDialogManager.openDialog(changePasswordComp, {});
						}
					}
				}
				
				Component {
					id: changePasswordComp;
					ChangePasswordDialog {
						title: qsTr("Change Password");
						currentPasswordInputVisible: !AuthorizationController.loggedUserIsSuperuser();
						
						onFinished: {
							if (buttonId == Enums.save){
								AuthorizationController.changePassword(container.profileData.m_username, contentItem.oldPassword, contentItem.newPassword);
							}
						}
					}
				}
			}
			
			Text {
				width: parent.width;
				text: qsTr("My Invitations");
				visible: invitationsGroup.visible;
				color: Style.textColor;
				font.family: Style.fontFamilyBold;
				font.pixelSize: Style.fontSizeXXL;
			}
			
			Rectangle {
				id: invitationsGroup;
				width: parent.width;
				height: invitationsColumn.height + 2 * Style.marginM;
				visible: AuthorizationController.pendingInvitationsCount > 0;
				border.width: 1;
				border.color: Style.borderColor;
				radius: Style.marginXS;
				color: Style.baseColor;

				Column {
					id: invitationsColumn;
					anchors.left: parent.left;
					anchors.right: parent.right;
					anchors.top: parent.top;
					anchors.margins: Style.marginM;
					spacing: Style.spacingM;

					Repeater {
						model: AuthorizationController.pendingInvitations

						delegate: Rectangle {
							width: invitationsColumn.width
							height: Style.controlHeightM
							color: "transparent"

							Text {
								anchors.verticalCenter: parent.verticalCenter
								anchors.left: parent.left
								anchors.leftMargin: Style.marginM
								anchors.right: btnRow.left
								anchors.rightMargin: Style.marginM
								text: modelData.tenantName ? modelData.tenantName : modelData.tenantId
								font.pixelSize: Style.fontSizeM
								font.family: Style.fontFamily
								color: Style.textColor
								elide: Text.ElideRight
							}

							Row {
								id: btnRow
								anchors.verticalCenter: parent.verticalCenter
								anchors.right: parent.right
								anchors.rightMargin: Style.marginM
								spacing: Style.spacingM

								Rectangle {
									width: acceptLabel.contentWidth + 2 * Style.marginL
									height: Style.controlHeightS
									radius: Style.radiusM
									color: Style.imaginToolsAccentColor
									opacity: acceptMA.containsMouse ? 0.8 : 1

									Text {
										id: acceptLabel
										anchors.centerIn: parent
										text: qsTr("Accept")
										font.pixelSize: Style.fontSizeS
										color: Style.baseColor
									}

									MouseArea {
										id: acceptMA
										anchors.fill: parent
										hoverEnabled: true
										cursorShape: Qt.PointingHandCursor
										onClicked: {
											container.acceptInvitation(modelData.id)
										}
									}
								}

								Rectangle {
									width: rejectLabel.contentWidth + 2 * Style.marginL
									height: Style.controlHeightS
									radius: Style.radiusM
									color: Style.negativeAccentColor
									opacity: rejectMA.containsMouse ? 0.8 : 1

									Text {
										id: rejectLabel
										anchors.centerIn: parent
										text: qsTr("Reject")
										font.pixelSize: Style.fontSizeS
										color: Style.baseColor
									}

									MouseArea {
										id: rejectMA
										anchors.fill: parent
										hoverEnabled: true
										cursorShape: Qt.PointingHandCursor
										onClicked: {
											container.rejectInvitation(modelData.id)
										}
									}
								}
							}
						}
					}
				}
			}
			
			Text {
				width: parent.width;
				text: qsTr("My Organizations");
				visible: organizationsGroup.visible;
				color: Style.textColor;
				font.family: Style.fontFamilyBold;
				font.pixelSize: Style.fontSizeXXL;
			}

			Rectangle {
				id: organizationsGroup;
				width: parent.width;
				height: organizationsColumn.height + 2 * Style.marginM;
				visible: container.__organizationsList.length > 1;
				border.width: 1;
				border.color: Style.borderColor;
				radius: Style.marginXS;
				color: Style.baseColor;

				Column {
					id: organizationsColumn;
					anchors.left: parent.left;
					anchors.right: parent.right;
					anchors.top: parent.top;
					anchors.margins: Style.marginM;
					spacing: Style.spacingM;

					Repeater {
						id: organizationsRepeater
						model: container.__organizationsList

						delegate: Rectangle {
							width: organizationsColumn.width
							height: Style.controlHeightL
							color: modelData.id === AuthorizationController.currentTenantId ? Style.buttonHoverColor : "transparent"

							Rectangle {
								visible: modelData.id === AuthorizationController.currentTenantId
								anchors.left: parent.left
								anchors.top: parent.top
								anchors.bottom: parent.bottom
								width: 4
								color: Style.imaginToolsAccentColor
							}

							Column {
								id: orgCol
								anchors.verticalCenter: parent.verticalCenter
								anchors.left: parent.left
								anchors.leftMargin: Style.marginL
								anchors.right: leaveBtn.visible ? leaveBtn.left : (switchBtn.visible ? switchBtn.left : parent.right)
								anchors.rightMargin: Style.marginM
								spacing: Style.marginXS

								Text {
									text: modelData.name ? modelData.name : modelData.id
									font.pixelSize: Style.fontSizeM
									font.bold: true
									font.family: Style.fontFamily
									color: Style.textColor
									elide: Text.ElideRight
									width: parent.width
								}

								Text {
									visible: modelData.description && modelData.description !== ""
									text: modelData.description
									font.pixelSize: Style.fontSizeS
									font.family: Style.fontFamily
									color: Style.inactiveTextColor
									elide: Text.ElideRight
									width: parent.width
								}
							}

							Rectangle {
								id: switchBtn
								visible: modelData.id !== AuthorizationController.currentTenantId
								anchors.right: parent.right
								anchors.rightMargin: Style.marginM
								anchors.verticalCenter: orgCol.verticalCenter
								width: switchLabel.contentWidth + 2 * Style.marginL
								height: Style.controlHeightS
								radius: Style.radiusM
								color: Style.imaginToolsAccentColor
								opacity: switchMA.containsMouse ? 0.8 : 1

								Text {
									id: switchLabel
									anchors.centerIn: parent
									text: qsTr("Switch")
									font.pixelSize: Style.fontSizeS
									color: Style.baseColor
								}

								MouseArea {
									id: switchMA
									anchors.fill: parent
									hoverEnabled: true
									cursorShape: Qt.PointingHandCursor
									onClicked: {
										AuthorizationController.selectTenant(modelData.id)
									}
								}
							}

							Rectangle {
								id: leaveBtn
								visible: !modelData.isDelegated && modelData.id !== ""
								anchors.right: switchBtn.visible ? switchBtn.left : parent.right
								anchors.rightMargin: switchBtn.visible ? Style.marginS : Style.marginM
								anchors.verticalCenter: orgCol.verticalCenter
								width: leaveLabel.contentWidth + 2 * Style.marginL
								height: Style.controlHeightS
								radius: Style.radiusM
								color: Style.negativeAccentColor
								opacity: leaveMA.containsMouse ? 0.8 : 1

								Text {
									id: leaveLabel
									anchors.centerIn: parent
									text: qsTr("Leave")
									font.pixelSize: Style.fontSizeS
									color: Style.baseColor
								}

								MouseArea {
									id: leaveMA
									anchors.fill: parent
									hoverEnabled: true
									cursorShape: Qt.PointingHandCursor
									onClicked: {
										container.__confirmLeaveTenant(modelData.id, modelData.name)
									}
								}
							}
						}
					}
				}
			}

			GroupHeaderView {
				width: parent.width;
				title: qsTr("Roles");
				groupView: rolesGroup;
				visible: rolesGroup.visible;
			}
			
			GroupElementView {
				id: rolesGroup;
				
				width: parent.width;
				visible: rolesTable.table ? rolesTable.table.elementsCount : false;
				
				TableElementView {
					id: rolesTable;
				}
			}
			
			GroupHeaderView {
				width: parent.width;
				title: qsTr("Groups");
				groupView: groupsBlock;
				visible: groupsBlock.visible;
			}
			
			GroupElementView {
				id: groupsBlock;
				width: parent.width;
				visible: groupsTable.table ? groupsTable.table.elementsCount > 0 : false;
				
				TableElementView {
					id: groupsTable;
				}
			}
			
			GroupHeaderView {
				width: parent.width;
				title: qsTr("Permissions");
				groupView: permissionsBlock;
				visible: permissionsBlock.visible;
			}
			
			GroupElementView {
				id: permissionsBlock;
				width: parent.width;
				visible: permissionsTable.table ? permissionsTable.table.elementsCount > 0: false
				
				TableElementView {
					id: permissionsTable;
				}
			}
		}
	}
	
	TreeItemModel {
		id: headersModel;
		
		function updateHeaders(){
			headersModel.clear();
			
			let index = headersModel.insertNewItem();
			
			headersModel.setData("id", "name", index);
			headersModel.setData("name", qsTr("Name"), index);
			
			index = headersModel.insertNewItem();
			
			headersModel.setData("id", "description", index);
			headersModel.setData("name", qsTr("Description"), index);
			
			headersModel.refresh();
			
			if (rolesTable.table){
				rolesTable.table.headers = headersModel;
			}
			
			if (permissionsTable.table){
				permissionsTable.table.headers = headersModel;
			}
			
			if (groupsTable.table){
				groupsTable.table.headers = headersModel;
			}
		}
		
		Component.onCompleted: {
			updateHeaders();
		}
	}
	
	function acceptInvitation(invitationId) {
		__acceptInvitationInput.m_invitationId = invitationId
		__acceptInvitationSender.send(__acceptInvitationInput)
	}

	function rejectInvitation(invitationId) {
		__rejectInvitationInput.m_invitationId = invitationId
		__rejectInvitationSender.send(__rejectInvitationInput)
	}

	property AcceptTenantInvitationInput __acceptInvitationInput: AcceptTenantInvitationInput {}
	property GqlSdlRequestSender __acceptInvitationSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_acceptTenantInvitation
		sdlObjectComp: Component {
			AcceptTenantInvitationPayload {
				onFinished: {
					if (m_success) {
						AuthorizationController.tenantInvitationAccepted("", "")
					} else if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
					}
				}
			}
		}

		function onError(message, type) {
			ModalDialogManager.showInfoDialog(message)
		}
	}

	property RejectTenantInvitationInput __rejectInvitationInput: RejectTenantInvitationInput {}
	property GqlSdlRequestSender __rejectInvitationSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_rejectTenantInvitation
		sdlObjectComp: Component {
			RejectTenantInvitationPayload {
				onFinished: {
					if (m_success) {
						AuthorizationController.tenantInvitationRejected("", "")
					} else if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
					}
				}
			}
		}

		function onError(message, type) {
			ModalDialogManager.showInfoDialog(message)
		}
	}

	// --- Organizations list ---
	property var __organizationsList: []

	function __loadOrganizations() {
		if (!AuthorizationController.userTokenProvider.userId)
			return
		__organizationsRequest.send(__organizationsInput)
	}

	property GetProfileInput __organizationsInput: GetProfileInput {
		m_id: AuthorizationController.userTokenProvider.userId
		m_productId: AuthorizationController.productId
	}

	GqlSdlRequestSender {
		id: __organizationsRequest
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
					// Always provide No Organization entry to allow switching to global (non-tenant) context
					list.push({
						id: "",
						name: qsTr("No organization"),
						description: "",
						isDelegated: false
					})
					container.__organizationsList = list
				}
			}
		}
	}

	// --- Leave organization ---
	function __confirmLeaveTenant(tenantId, tenantName) {
		__leaveConfirmTenantId = tenantId
		__leaveConfirmTenantName = tenantName || tenantId
		ModalDialogManager.openDialog(__leaveConfirmDialogComp, {"tenantId": tenantId, "tenantName": __leaveConfirmTenantName})
	}

	property string __leaveConfirmTenantId: ""
	property string __leaveConfirmTenantName: ""

	Component {
		id: __leaveConfirmDialogComp
		MessageDialog {
			property string tenantId: ""
			property string tenantName: ""
			width: Style.sizeHintM
			title: qsTr("Leave organization")
			message: qsTr("Are you sure you want to leave \"%1\"?").arg(tenantName)
			onFinished: {
				if (buttonId == Enums.yes) {
					container.__leaveTenant(tenantId)
				}
			}
		}
	}

	function __leaveTenant(tenantId) {
		if (!tenantId) return
		__leaveTenantId = tenantId
		__findMembershipForLeaveInput.m_userId = AuthorizationController.userTokenProvider.userId
		__findMembershipForLeaveInput.m_tenantId = tenantId
		__findMembershipForLeaveSender.send(__findMembershipForLeaveInput)
	}

	property string __leaveTenantId: ""

	property FindMembershipInput __findMembershipForLeaveInput: FindMembershipInput {}
	property GqlSdlRequestSender __findMembershipForLeaveSender: GqlSdlRequestSender {
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_findMembership

		sdlObjectComp: Component {
			FindMembershipPayload {
				onFinished: {
					if (m_membership && m_membership.m_id && m_membership.m_id !== "") {
						container.__removeMembershipForLeaveInput.m_membershipId = m_membership.m_id
						container.__removeMembershipForLeaveSender.send(container.__removeMembershipForLeaveInput)
					} else if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
					}
				}
			}
		}

		function onError(message, type) {
			ModalDialogManager.showInfoDialog(message)
		}
	}

	property RemoveMembershipInput __removeMembershipForLeaveInput: RemoveMembershipInput {}
	property GqlSdlRequestSender __removeMembershipForLeaveSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_removeMembership

		sdlObjectComp: Component {
			RemoveMembershipPayload {
				onFinished: {
					if (m_success) {
						if (container.__leaveTenantId === AuthorizationController.currentTenantId) {
							AuthorizationController.selectTenant("")
						}
						container.__loadOrganizations()
					} else if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
					}
				}
			}
		}

		function onError(message, type) {
			ModalDialogManager.showInfoDialog(message)
		}
	}

	Loading {
		id: loading;
		anchors.fill: parent;
		visible: getProfileRequest.state == "Loading" || setProfileRequest.state == "Loading";
		background.color: Style.backgroundColor2;
	}
}
