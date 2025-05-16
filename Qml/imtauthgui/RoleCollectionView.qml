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

RemoteCollectionView {
	id: roleCollectionViewContainer;
	
	collectionId: "Roles";
	visibleMetaInfo: true;

	additionalFieldIds: [RoleItemTypeMetaInfo.s_productId, RoleItemTypeMetaInfo.s_parentRoles]
	
	commandsDelegateComp: Component {RoleCollectionViewCommandsDelegate {
			collectionView: roleCollectionViewContainer;
			documentManager: roleCollectionViewContainer.documentManager;
			
			documentViewsComp: [roleDocumentComp];
			documentDataControllersComp: [dataControllerComp];
			documentValidatorsComp: [];
			
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
	
	Component.onCompleted: {
		if (roleCollectionViewContainer.documentManager){
			documentManager.registerDocumentView("Role", "RoleEditor", roleDocumentComp);
			documentManager.registerDocumentDataController("Role", dataControllerComp);
		}
	}
	
	onProductIdChanged: {
		permissionsProvider.productId = productId;
		permissionsProvider.updateModel();
	}
	
	property TreeItemModel rolesModel;
	property TreeItemModel permissionsModel;
	
	PermissionsProvider {
		id: permissionsProvider;
		productId: roleCollectionViewContainer.productId;
		
		onPermissionsModelChanged: {
			if (permissionsProvider.permissionsModel != null){
				roleCollectionViewContainer.permissionsModel = permissionsProvider.permissionsModel;
			}
		}
		
		function getHeaders(){
			return roleCollectionViewContainer.getHeaders()
		}
	}
	
	Component {
		id: roleDocumentComp;
		
		RoleView {
			id: roleEditor;
			
			permissionsModel: roleCollectionViewContainer.permissionsModel;
			rolesModel: roleCollectionViewContainer.rolesModel;
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
