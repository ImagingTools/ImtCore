import QtQuick 2.12
import Acf 1.0
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

    dataControllerComp:
        Component {
        CollectionRepresentation {
            collectionId: roleCollectionViewContainer.collectionId;

            Component.onCompleted: {
                additionalFieldIds.push("ProductId");
                additionalFieldIds.push("ParentRoles");
            }

            function getAdditionalInputParams(){
                let obj = {}
                obj["ProductId"] = roleCollectionViewContainer.productId;
                return obj;
            }
        }
    }

    commandsDelegateComp: Component {RoleCollectionViewCommandsDelegate {
            collectionView: roleCollectionViewContainer;

            documentTypeId: "Role";
            viewTypeId: "RoleEditor";
        }
    }

    property string productId: context.appName;

    function onLocalizationChanged(language){
        roleCollectionViewContainer.dataController.updateHeaders();
    }

    function handleSubscription(dataModel){
        roleCollectionViewContainer.doUpdateGui();
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", roleCollectionViewContainer.onLocalizationChanged);
    }

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", roleCollectionViewContainer.onLocalizationChanged);
        CachedRoleCollection.updateModel();

        let documentManager = MainDocumentManager.getDocumentManager("Administration");
        if (documentManager){
            roleCollectionViewContainer.commandsDelegate.documentManager = documentManager;

            documentManager.registerDocumentView("Role", "RoleEditor", roleDocumentComp);
            documentManager.registerDocumentDataController("Role", dataControllerComp);
        }
    }

    onElementsChanged: {
        if(!permissionsProvider) return

        permissionsProvider.updateModel();
    }

    property TreeItemModel rolesModel: CachedRoleCollection.collectionModel;
    property TreeItemModel permissionsModel: TreeItemModel {};

    PermissionsProvider {
        id: permissionsProvider;

        property bool compl: false;
        productId: roleCollectionViewContainer.productId;

        onDataModelChanged: {
            if (permissionsProvider.dataModel != null){
                roleCollectionViewContainer.permissionsModel = permissionsProvider.dataModel;
            }
        }
    }

    Component {
        id: roleDocumentComp;

        RoleView {
            id: roleEditor;

            permissionsModel: roleCollectionViewContainer.permissionsModel;
            rolesModel: roleCollectionViewContainer.rolesModel;
            productId: roleCollectionViewContainer.productId;

            commandsDelegateComp: Component {ViewCommandsDelegateBase {
                    view: roleEditor;
                }
            }

            commandsControllerComp: Component {CommandsPanelController {
                    commandId: "Role";
                    uuid: roleEditor.viewId;
                    commandsView: roleEditor.commandsView;
                }
            }
        }
    }

    Component {
        id: dataControllerComp;

        GqlRequestDocumentDataController {
            id: requestDocumentDataController

            gqlGetCommandId: ImtauthRolesSdlCommandIds.s_roleItem;
            gqlUpdateCommandId: ImtauthRolesSdlCommandIds.s_roleUpdate;
            gqlAddCommandId: ImtauthRolesSdlCommandIds.s_roleAdd;

            Component.onCompleted: {
                getRequestInputParam.InsertField(RoleItemInputTypeMetaInfo.s_productId, roleCollectionViewContainer.productId);
                addRequestInputParam.InsertField(RoleItemInputTypeMetaInfo.s_productId, roleCollectionViewContainer.productId);
                updateRequestInputParam.InsertField(RoleItemInputTypeMetaInfo.s_productId, roleCollectionViewContainer.productId);
            }

            documentModelComp: Component {
                RoleData {}
            }

            payloadModel: RoleDataPayload {
                onFinished: {
                    requestDocumentDataController.documentModel = m_roleData;
                }
            }

            function getAdditionalInputParams(){
                let obj = {}
                obj["ProductId"] = roleCollectionViewContainer.productId;

                return obj;
            }

            function getDocumentName(){
                let prefixName = qsTr("Roles");

                if (documentName !== ""){
                    return prefixName + " / " + documentName;
                }

                return prefixName + " / " + qsTr("New Role");
            }
        }
    }
}
