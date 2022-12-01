import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: rolePermissionsContainer;

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
        permissionsTable.height = permissionsTable.headerHeight;

        for (let i = 0; i < permissionsProvider.model.GetItemsCount(); i++){
            let permissionId = permissionsProvider.model.GetData("Id", i);
            let permissionName = permissionsProvider.model.GetData("Name", i);

            let row = {"Id": permissionId, "Name": permissionName, "CheckState": Qt.Unchecked};

            if (selectedPermissionsIds.includes(permissionId)){
                row["CheckState"] = Qt.Checked;
            }

            permissionsTable.insertRow([i], row);
        }

        blockUpdatingModel = false;
    }

    function updateModel(){
        undoRedoManager.beginChanges();

        let permissionsModel = documentModel.AddTreeModel("Permissions");

        for (let i = 0; i < permissionsTable.rowCount; i++){
            let rowObj = permissionsTable.rowModel.get(i);

            let permissionId = rowObj["Id"];
            let permissionName = rowObj["Name"];
            let state = rowObj["CheckState"];

            if (state == Qt.Checked){
                let index = permissionsModel.InsertNewItem();

                permissionsModel.SetData("Id", permissionId, index);
                permissionsModel.SetData("Name", permissionName, index);
            }
        }

        undoRedoManager.endChanges();
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }

    Flickable {
//        width: 400;
//        height: parent.height;

        anchors.fill: parent;
        anchors.leftMargin: 20;

        contentHeight: permissionsTable.height + 50;
        contentWidth: permissionsTable.width;

        boundsBehavior: Flickable.StopAtBounds;

        clip: true;

        BasicTableView {
            id: permissionsTable;

            anchors.top: parent.top;
            anchors.topMargin: 10;

            width: 400;
            height: permissionsTable.headerHeight;

            rowDelegate: TableRolePermissionsDelegate {
                Component.onCompleted: {
                    permissionsTable.height += height;
                }
            }

            Component.onCompleted: {
                permissionsTable.addColumn({"Id": "Name", "Name": "Name"});
            }

            onRowModelDataChanged: {
                if (!blockUpdatingModel){
                    updateModel();
                }
            }
        }//BasicTableView
    }//Flickable
}//Container
