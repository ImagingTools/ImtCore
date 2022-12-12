import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: rolePermissionsContainer;

//    anchors.fill: parent;

    property TreeItemModel documentModel: TreeItemModel {}
    property UndoRedoManager undoRedoManager: null;

    property bool blockUpdatingModel: false;

    onDocumentModelChanged: {
        console.log("RolePermissions onDocumentModelChanged");
        if (documentModel.ContainsKey("ProductId")){
            let productId = documentModel.GetData("ProductId");

            permissionsProvider.productId = productId;

            permissionsProvider.updateModel();
        }
    }

//    Rectangle {
//        anchors.fill: parent;

//        color: "red";
//    }

    PermissionsProvider {
        id: permissionsProvider;

        onModelChanged: {
            updateGui();
        }
    }

    function updateGui(){
        console.log("RolePermissions updateGui");

        blockUpdatingModel = true;

        let selectedPermissionsIds = [];
        let selectedPermissionsModel = documentModel.GetData("Permissions");
        if (!selectedPermissionsModel){
            selectedPermissionsModel = documentModel.AddTreeModel("Permissions");
        }

        for (let i = 0; i < selectedPermissionsModel.GetItemsCount(); i++){
            let permissionId = selectedPermissionsModel.GetData("Id", i);

            selectedPermissionsIds.push(permissionId);
        }

        console.log("selectedPermissionsIds", selectedPermissionsIds);

        permissionsTable.rowModel.clear();
//        permissionsTable.height = permissionsTable.headerHeight;

        recursiveUpdateGui(permissionsProvider.model, [], selectedPermissionsIds);

        blockUpdatingModel = false;
    }

    function recursiveUpdateGui(model, tableIndexes, selectedPermissions){
        for (let i = 0; i < model.GetItemsCount(); i++){
            let permissionId = model.GetData("Id", i);
            let permissionName = model.GetData("Name", i);

            let row = {"Id": permissionId, "Name": permissionName, "CheckState": Qt.Unchecked};

            if (selectedPermissions.includes(permissionId)){
                row["CheckState"] = Qt.Checked;
            }

            permissionsTable.insertRow(tableIndexes.concat([i]), row);

            let childModel = model.GetData("ChildModel", i);
            if (childModel){
                let childIndexes = [].concat(tableIndexes.concat([i]))
                recursiveUpdateGui(childModel, childIndexes,selectedPermissions);
            }
        }
    }

    function updateModel(){
        undoRedoManager.beginChanges();

        let permissionsModel = documentModel.AddTreeModel("Permissions");

        recursiveUpdateModel(permissionsModel, permissionsTable.rowModel);

        undoRedoManager.endChanges();
    }

    function recursiveUpdateModel(model, guiModel){
        console.log("recursiveUpdateModel", model, guiModel);
        for (let i = 0; i < guiModel.count; i++){
            let rowObj = guiModel.get(i);

            let permissionId = rowObj["Id"];
            let permissionName = rowObj["Name"];
            let state = rowObj["CheckState"];

            if (state == Qt.Checked){
                let index = model.InsertNewItem();

                model.SetData("Id", permissionId, index);
                model.SetData("Name", permissionName, index);
            }

            let rowChildModel = rowObj["ChildModel"]
            if (rowChildModel.count > 0){
                recursiveUpdateModel(model, rowChildModel);
            }
        }
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }

    BasicTreeView {
        id: permissionsTable;

        anchors.top: parent.top;
        anchors.topMargin: 10;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;

        tristate: true;

        width: 400;

        Component.onCompleted: {
            permissionsTable.addColumn({"Id": "Name", "Name": "Name"});
        }

        onRowModelDataChanged: {
            if (!blockUpdatingModel){
                updateModel();
            }
        }
    }//BasicTableView
}//Container
