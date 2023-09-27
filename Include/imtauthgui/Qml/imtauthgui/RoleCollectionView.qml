import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

CollectionView {
    id: roleCollectionViewContainer;

    contentMargins: 10;

    hasFilter: false;

    property bool newCommandIsEnabled: true;

    tableElementsDelegate: Component { TableProductRolesDelegate {
        width: baseCollectionView.table.width;

        selectedIndex: baseCollectionView.table.selectedIndex;
        commandsId: roleCollectionViewContainer.commandsId;
        baseCollectionView: roleCollectionViewContainer.baseCollectionView;
        newIsEnabled: roleCollectionViewContainer.newCommandIsEnabled;

        onDoubleClicked: {
            baseCollectionView.table.tableSelection.singleSelect(index);

            baseCollectionView.table.selectItem(id, name);
        }

        onClicked: {
            baseCollectionView.table.tableSelection.singleSelect(index);
            roleCollectionViewContainer.baseCollectionView.table.elementsList.forceActiveFocus();
        }
    } }

    Component.onCompleted: {
        roleCollectionViewContainer.commandsDelegatePath = "../../imtauthgui/RoleCollectionViewCommandsDelegate.qml";

        elementsList.spacing = 10;
        baseCollectionView.table.showHeaders = false;
        baseCollectionView.hasPagination = false;
    }

    onElementsChanged: {
        if (elementsList.model.ContainsKey("Roles")){
            let elementsModel = elementsList.model.GetData("Roles");
            baseCollectionView.table.tableSelection.countElements = elementsModel.GetItemsCount();
        }
    }

    function getSelectedIds(){
        let retVal = []

        let elementsModel = roleCollectionViewContainer.baseCollectionView.table.elements;
        let model = elementsModel.GetData("Roles");
        let indexes = roleCollectionViewContainer.baseCollectionView.table.getSelectedIndexes();
        for (let i = 0; i < indexes.length; i++){
            if (model.ContainsKey("Id", indexes[i])){
                let id = model.GetData("Id", indexes[i]);
                retVal.push(id);
            }
        }

        return retVal;
    }

    function getSelectedNames(){
        let retVal = []

        let elementsModel = roleCollectionViewContainer.baseCollectionView.table.elements;
        let model = elementsModel.GetData("Roles");
        let indexes = roleCollectionViewContainer.baseCollectionView.table.getSelectedIndexes();
        for (let i = 0; i < indexes.length; i++){
            if (model.ContainsKey("Name", indexes[i])){
                let id = model.GetData("Name", indexes[i]);
                retVal.push(id);
            }
        }

        return retVal;
    }

    function selectItem(id, name){
        Events.sendEvent("CommandsClearModel");

        let editorPath = baseCollectionView.commands.objectViewEditorPath;
        let commandsId = baseCollectionView.commands.objectViewEditorCommandsId;

        let elements = baseCollectionView.table.elements;

        let productId = elements.GetData("Id");
        documentManager.openDocument(id, {"Id": id, "ProductId": productId, "Name": name, "Source": editorPath, "CommandsId": commandsId});
    }

    function onCommandsModelChanged(){
        roleCollectionViewContainer.newCommandIsEnabled = commandsProvider.commandExists("New");
    }
}
