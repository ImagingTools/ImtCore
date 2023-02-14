import QtQuick 2.15
import Acf 1.0
import imtgui 1.0

CollectionView {
    id: roleCollectionViewContainer;

    contentMargins: 10;

    tableElementsDelegate: Component { TableProductRolesDelegate {
        width: parent.width;

        selectedIndex: baseCollectionView.table.selectedIndex;

        commandsId: roleCollectionViewContainer.commandsId;

        baseCollectionView: roleCollectionViewContainer.baseCollectionView;

        onDoubleClicked: {
            baseCollectionView.table.selectItem(id, name);
        }

        onClicked: {
            console.log("onClicked", model.index);
            baseCollectionView.table.selectedIndex = index;
        }
    } }

    Component.onCompleted: {
        console.log("RoleCollection onCompleted", model.index);
        roleCollectionViewContainer.commandUpdateGui = "RoleCollectionUpdateGui";
        roleCollectionViewContainer.commandsDelegatePath = "../../imtauthgui/RoleCollectionViewCommandsDelegate.qml";

        elementsList.spacing = 10;
        baseCollectionView.table.showHeaders = false;
        baseCollectionView.hasPagination = false;
    }

    function selectItem(id, name){
        console.log("RoleCollectionView selectItem", id, name);
        let editorPath = baseCollectionView.commands.objectViewEditorPath;
        let commandsId = baseCollectionView.commands.objectViewEditorCommandsId;

        let elements = baseCollectionView.table.elements;

        let productId = elements.GetData("Id", baseCollectionView.table.selectedIndex);

        console.log("selectItem", id, name, productId)

        documentManager.addDocument({"Id": id, "ProductId": productId, "Name": name, "Source": editorPath, "CommandsId": commandsId});
    }
}
