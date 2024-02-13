import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0

CollectionView {
    id: roleCollectionViewContainer;

    hasFilter: false;

    property bool newCommandIsEnabled: true;

    table.showHeaders: false;
    filterMenuVisible: false;

    Component {
        id: roleTableDelegateComp;

        TableProductRolesDelegate {
            width: roleCollectionViewContainer.table.width;

            newIsEnabled: roleCollectionViewContainer.newCommandIsEnabled;

            Component.onCompleted: {
                roleCollectionViewContainer.selectionChanged.connect(this.selectionChanged);
            }

            Component.onDestruction: {
                roleCollectionViewContainer.selectionChanged.disconnect(this.selectionChanged);
            }

            onDoubleClicked: {
                roleCollectionViewContainer.table.select(index);

                roleCollectionViewContainer.doubleClicked(id, index)
            }

            onClicked: {
                roleCollectionViewContainer.table.select(index)
                roleCollectionViewContainer.table.elementsList.forceActiveFocus();
            }

            onNewClicked: {
                if (roleCollectionViewContainer.commandsDelegate){
                    roleCollectionViewContainer.commandsDelegate.commandHandle("New");
                }
            }

            function selectionChanged(selection){
                selectedIndex = selection[0];
            }
        }
    }

    dataController: CollectionRepresentation {
        collectionId: "Roles";
    }

    commandsController: CommandsRepresentationProvider {
        commandId: "Roles";
        uuid: roleCollectionViewContainer.viewId;
    }

    commandsDelegate: RoleCollectionViewCommandsDelegate {
        collectionView: roleCollectionViewContainer;

        documentTypeId: "Role";
        viewTypeId: "RoleEditor";
    }

    Component.onCompleted: {
        let documentManager = MainDocumentManager.getDocumentManager("Administration");
        if (documentManager){
            roleCollectionViewContainer.commandsDelegate.documentManager = documentManager;

            documentManager.registerDocumentView("Role", "RoleEditor", roleDocumentComp);
            documentManager.registerDocumentDataController("Role", dataControllerComp);
        }

        table.delegate = roleTableDelegateComp;
    }

    onElementsChanged: {
        if (table.elements.ContainsKey("Roles")){
            let elementsModel = table.elements.GetData("Roles");
            table.tableSelection.countElements = elementsModel.GetItemsCount();
        }
    }

    function onCommandsModelChanged(){
        roleCollectionViewContainer.newCommandIsEnabled = commandsController.commandExists("New");
    }

    Component {
        id: roleDocumentComp;

        RoleView {
            id: roleEditor;

            commandsController: CommandsRepresentationProvider {
                commandId: "Role";
                uuid: roleEditor.viewId;
            }

            Component.onCompleted: {
                let elements = roleCollectionViewContainer.table.elements;
                productId = elements.GetData("Id")
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
                let productId = elements.GetData("Id")

                let obj = {}
                obj["ProductId"] = productId;

                return obj;
            }

            function getDocumentName(){
                let prefixName = qsTr("Roles");

                if (documentModel.ContainsKey("Name")){
                    return prefixName + " / " + documentModel.GetData("Name")
                }

                let newRoleName = qsTr("New Role");
                return newRoleName;
            }
        }
    }
}
