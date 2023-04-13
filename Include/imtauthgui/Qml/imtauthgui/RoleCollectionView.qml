import QtQuick 2.15
import Acf 1.0
import imtgui 1.0

CollectionView {
    id: roleCollectionViewContainer;

    contentMargins: 10;

    tableElementsDelegate: Component { TableProductRolesDelegate {
        width: baseCollectionView.table.width;

        selectedIndex: baseCollectionView.table.selectedIndex;

        commandsId: roleCollectionViewContainer.commandsId;

        baseCollectionView: roleCollectionViewContainer.baseCollectionView;

        onDoubleClicked: {
            baseCollectionView.table.tableSelection.singleSelect(index);

            baseCollectionView.table.selectItem(id, name);
        }

        onClicked: {
            console.log('roleCollectionViewContainer onClicked', index)
            baseCollectionView.table.tableSelection.singleSelect(index);
            roleCollectionViewContainer.baseCollectionView.table.elementsList.forceActiveFocus();
        }
    } }

    Component.onCompleted: {
        console.log("RoleCollection onCompleted", model.index);
        roleCollectionViewContainer.commandsDelegatePath = "../../imtauthgui/RoleCollectionViewCommandsDelegate.qml";

        elementsList.spacing = 10;
        baseCollectionView.table.showHeaders = false;
        baseCollectionView.hasPagination = false;
    }

    onElementsChanged: {
        let elementsModel = elementsList.model.GetData("Roles");
        console.log('elementsModel', elementsModel.GetItemsCount())
        baseCollectionView.table.tableSelection.countElements = elementsModel.GetItemsCount();
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
        console.log("new getSelectedNames");
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
        console.log("RoleCollectionView selectItem", id, name);
        let editorPath = baseCollectionView.commands.objectViewEditorPath;
        let commandsId = baseCollectionView.commands.objectViewEditorCommandsId;

        let elements = baseCollectionView.table.elements;

        let productId = elements.GetData("Id");
        documentManager.openDocument(id, {"Id": id, "ProductId": productId, "Name": name, "Source": editorPath, "CommandsId": commandsId});
    }
}
