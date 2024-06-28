import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0

RemoteCollectionView {
    id: roleCollectionViewContainer;

    anchors.left: parent.left;
    anchors.leftMargin: Style.size_mainMargin;
    anchors.right: parent.right;
    anchors.rightMargin: Style.size_mainMargin;
    anchors.top: parent.top;
    anchors.topMargin: Style.size_mainMargin;

    hasFilter: false;
    hasPagination: false;

    collectionId: "Roles";

    property bool newCommandIsEnabled: true;

    table.showHeaders: false;

    table.backgroundElementsColor: Style.backgroundColor2;
    table.enableAlternating: false;
    filterMenuVisible: false;

    dataControllerComp: Component {CollectionRepresentation {
            collectionId: roleCollectionViewContainer.collectionId;
            additionalFieldIds: roleCollectionViewContainer.additionalFieldIds;

            function removeElement(elementIndex){
                if (elementIndex < 0){
                    console.error();

                    return;
                }

                let itemId = elementsModel.getData("Roles").getData("Id", elementIndex);
                removeGqlModel.remove(itemId);
            }
        }
    }

    Component {
        id: roleTableDelegateComp;

        TableProductRolesDelegate {
            tableItem: roleCollectionViewContainer.table;
            width: roleCollectionViewContainer.table.width;

            newIsEnabled: roleCollectionViewContainer.newCommandIsEnabled;

            onRoleDoubleClicked: {
                roleCollectionViewContainer.table.select(index);
                roleCollectionViewContainer.doubleClicked(id, index)
            }

            onRoleClicked: {
                roleCollectionViewContainer.table.select(index)
                roleCollectionViewContainer.table.elementsList.forceActiveFocus();
            }

            onNewClicked: {
                if (roleCollectionViewContainer.commandsDelegate){
                    roleCollectionViewContainer.commandsDelegate.onNew();
                }
            }
        }
    }

    commandsDelegateComp: Component {RoleCollectionViewCommandsDelegate {
            collectionView: roleCollectionViewContainer;

            documentTypeId: "Role";
            viewTypeId: "RoleEditor";
        }
    }

    function onLocalizationChanged(language){
        roleCollectionViewContainer.dataController.updateHeaders();
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

        table.rowDelegate = roleTableDelegateComp;
    }

    onElementsChanged: {
        if(!permissionsProvider) return

        let productId = ""
        if (table.elements.containsKey("Id")){
            productId = table.elements.getData("Id");
        }

        if (productId === ""){
            return
        }

        permissionsProvider.productId = table.elements.getData("Id");

        if (table.elements.containsKey("Roles")){
            let elementsModel = table.elements.getData("Roles");
            table.tableSelection.countElements = elementsModel.getItemsCount();
        }

        permissionsProvider.updateModel();
    }

    function onCommandsModelChanged(){
        roleCollectionViewContainer.newCommandIsEnabled = commandsController.commandExists("New");
    }

    property TreeItemModel rolesModel: CachedRoleCollection.rolesModel;
    property TreeItemModel permissionsModel: TreeItemModel {};

    PermissionsProvider {
        id: permissionsProvider;

        property bool compl: false;

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

            commandsDelegateComp: Component {ViewCommandsDelegateBase {
                    view: roleEditor;
                }
            }

            commandsControllerComp: Component {CommandsRepresentationProvider {
                    commandId: "Role";
                    uuid: roleEditor.viewId;
                }
            }

            Component.onCompleted: {
                let elements = roleCollectionViewContainer.table.elements;
                productId = elements.getData("Id")
            }
        }
    }

    Component {
        id: dataControllerComp;

        GqlDocumentDataController {
            gqlGetCommandId: "RoleItem";
            gqlUpdateCommandId: "RoleUpdate";
            gqlAddCommandId: "RoleAdd";

            function getAdditionalInputParams(){
                let elements = roleCollectionViewContainer.table.elements;
                let productId = elements.getData("Id")

                let obj = {}
                obj["ProductId"] = productId;

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
