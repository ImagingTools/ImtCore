import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtauthRolesSdl 1.0
import imtauthPermissionsSdl 1.0

RemoteCollectionView {
	id: roleCollectionViewContainer;
	
	collectionId: "Roles";
	visibleMetaInfo: true;
	documentCollectionFilter: null

	additionalFieldIds: [RoleItemDataTypeMetaInfo.s_productId, RoleItemDataTypeMetaInfo.s_parentRoles]
	
	commandsDelegateComp: Component {RoleCollectionViewCommandsDelegate {
			collectionView: roleCollectionViewContainer;
			documentManagerId: "Administration/Roles"
			documentTypeIds: ["Role"]
			documentViewsComp: [roleDocumentComp];
			documentDataControllersComp: [dataControllerComp];
			isSingleDocumentMode: true

			function getHeaders(){
				return roleCollectionViewContainer.getHeaders()
			}
		}
	}
	
	property string productId;
	property var documentManager: null;
	
	function handleSubscription(dataModel){
		roleCollectionViewContainer.doUpdateGui();
	}

	onProductIdChanged: {
		roleCollectionViewContainer.__fetchAllPermissions()
	}
	
	property var flatPermissions: [];
	
	property GetProductPermissionsInput __getPermsInput: GetProductPermissionsInput {}
	property GqlSdlRequestSender __getPermsSender: GqlSdlRequestSender {
		gqlCommandId: ImtauthPermissionsSdlCommandIds.s_getProductPermissions

		sdlObjectComp: Component {
			GetProductPermissionsPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
					} else {
						roleCollectionViewContainer.__onPermissionsReceived(m_groups)
					}
				}
			}
		}
	}

	function __fetchAllPermissions() {
		__getPermsInput.m_productId = roleCollectionViewContainer.productId || ""
		__getPermsSender.send(__getPermsInput)
	}

	function __onPermissionsReceived(groupsList) {
		var result = []
		if (groupsList) {
			for (var gi = 0; gi < groupsList.count; ++gi) {
				var group = groupsList.get(gi).item
				if (!group) continue
				var groupObj = {
					"groupId": group.m_groupId || "",
					"groupName": group.m_groupName || "",
					"entries": []
				}
				var entries = group.m_entries
				if (entries) {
					for (var ei = 0; ei < entries.count; ++ei) {
						var entry = entries.get(ei).item
						if (!entry) continue
						groupObj.entries.push({
							"permissionId": entry.m_permissionId || "",
							"displayName": entry.m_displayName || "",
							"description": entry.m_description || ""
						})
					}
				}
				result.push(groupObj)
			}
		}
		roleCollectionViewContainer.flatPermissions = result
	}
	
	Component {
		id: roleDocumentComp;
		
		RoleView {
			id: roleEditor;
			
			flatPermissions: roleCollectionViewContainer.flatPermissions;
			productId: roleCollectionViewContainer.productId;
			
			commandsControllerComp: Component {GqlBasedCommandsController {
					typeId: "Role";
				}
			}
			
			function getHeaders(){
				return roleCollectionViewContainer.getHeaders()
			}
		}
	}
	Component {
		id: dataControllerComp;
		
		GqlRequestDocumentDataController {
			id: requestDocumentDataController
			
			property RoleData roleData: documentModel;
			
			typeId: "Role";
			documentName: roleData ? roleData.m_name: "";
			documentDescription: roleData ? roleData.m_description: "";
			
			gqlGetCommandId: ImtauthRolesSdlCommandIds.s_roleItem;
			gqlUpdateCommandId: ImtauthRolesSdlCommandIds.s_roleUpdate;
			gqlAddCommandId: ImtauthRolesSdlCommandIds.s_roleAdd;
			
			Component.onCompleted: {
				if (roleCollectionViewContainer.productId === ""){
					console.error("Unable to create an additional GraphQL input parameters. Product-ID is empty:", gqlGetCommandId);
					return null;
				}
				
				getRequestInputParam.InsertField(RoleItemInputTypeMetaInfo.s_productId, roleCollectionViewContainer.productId);
				addRequestInputParam.InsertField(RoleItemInputTypeMetaInfo.s_productId, roleCollectionViewContainer.productId);
				updateRequestInputParam.InsertField(RoleItemInputTypeMetaInfo.s_productId, roleCollectionViewContainer.productId);
			}
			
			documentModelComp: Component {
				RoleData {}
			}
			
			function getHeaders(){
				return roleCollectionViewContainer.getHeaders()
			}
		}
	}
}
