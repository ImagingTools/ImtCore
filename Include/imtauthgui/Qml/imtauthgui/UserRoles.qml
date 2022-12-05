import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: userRolesContainer;

    property TreeItemModel documentModel: TreeItemModel {}
    property UndoRedoManager undoRedoManager: null;

    property bool blockUpdatingModel: false;

    Component.onCompleted: {
        rolesProvider.updateModel();
    }

    onDocumentModelChanged: {
        console.log("UserRoles onDocumentModelChanged", documentModel);

        updateGui();
    }

    function updateGui(){
        console.log("UserRoles updateGui");
        blockUpdatingModel = true;

        let selectedRolesList = []
        let selectedRoles = documentModel.GetData("Products");
        if (!selectedRoles){
            selectedRoles = documentModel.AddTreeModel("Products");
        }

        for (let i = 0; i < selectedRoles.GetItemsCount(); i++){
            let productId = selectedRoles.GetData("Id", i);
            let rolesModel = selectedRoles.GetData("Roles", i);
            if (rolesModel){
                for (let j = 0; j < rolesModel.GetItemsCount(); j++){
                    let roleId = rolesModel.GetData("Id", j);

                    let value = productId + ';' + roleId;

                    selectedRolesList.push(value)
                }
            }
        }

        rolesTable.rowModel.clear();
        rolesTable.height = 0;

        for (let i = 0; i < rolesProvider.model.GetItemsCount(); i++){
            let productId = rolesProvider.model.GetData("Id", i);

            let row = {"Id": productId, "Name": productId, "Roles": []};
            let rolesModel = rolesProvider.model.GetData("Roles", i);
            if (rolesModel){
                let roles = []
                for (let j = 0; j < rolesModel.GetItemsCount(); j++){

                    let roleId = rolesModel.GetData("Id", j);
                    let roleName = rolesModel.GetData("Name", j);

                    let value = productId + ';' + roleId;

                    let roleRow = {"Id": roleId, "Name": roleName, "CheckState": Qt.Unchecked}

                    if (selectedRolesList.includes(value)){
                        roleRow["CheckState"] = Qt.Checked;
                    }

                    roles.push(roleRow)
                }

                row["Roles"] = roles;
            }

            rolesTable.addRow(row);
        }

        permissionsTable.rowModel.clear();
        permissionsTable.height = permissionsTable.headerHeight;

        //Permissions update...

        blockUpdatingModel = false;
    }

    function updateModel(){
        console.log("UserRoles updateModel");

        undoRedoManager.beginChanges();

        let selectedRoles = documentModel.AddTreeModel("Products");

        for (let i = 0; i < rolesTable.rowCount; i++){
            let rowObj = rolesTable.rowModel.get(i);

            let productId = rowObj["Id"];

            let roles = rowObj["Roles"];

            console.log("roles", roles);

            for (let j = 0; j < roles.count; j++){
                let roleObj = roles.get(j);

                console.log("roleObj", roleObj);

                let roleId = roleObj["Id"];
                let roleName = roleObj["Name"];
                let state = roleObj["CheckState"];

                if (state == Qt.Checked){
                    let productIndex = -1;

                    for (let k = 0; k < selectedRoles.GetItemsCount(); k++){
                        let selectedProductId = selectedRoles.GetData("Id", k);
                        if (selectedProductId == productId){
                            productIndex = k;
                            break;
                        }
                    }

                    let rolesModel;
                    if (productIndex < 0){
                        productIndex = selectedRoles.InsertNewItem();

                        selectedRoles.SetData("Id", productId, productIndex);
                        selectedRoles.SetData("Name", productId, productIndex);

                        rolesModel = selectedRoles.AddTreeModel("Roles", productIndex);
                    }
                    else{
                        rolesModel = selectedRoles.GetData("Roles", productIndex)
                    }

                    let roleIndex = rolesModel.InsertNewItem();

                    rolesModel.SetData("Id", roleId, roleIndex);
                    rolesModel.SetData("Name", roleName, roleIndex);
                }
            }
        }

        undoRedoManager.endChanges();
    }

    Component{
        id: emptyDecorator;
        Item{
            property Item rootItem;
        }
    }

    RolesProvider {
        id: rolesProvider;
    }

    Rectangle {
        id: background;
        anchors.fill: parent;
        color: Style.backgroundColor;
        Loader{
            id: backgroundDecoratorLoader;

            sourceComponent: Style.backGroundDecorator !==undefined ? Style.backGroundDecorator: emptyDecorator;
            onLoaded: {
                if(backgroundDecoratorLoader.item){
                    backgroundDecoratorLoader.item.rootItem = background;
                }
            }
        }

    }

    Flickable {
        anchors.fill: parent;

        anchors.leftMargin: 20;

        contentWidth: bodyColumn.width;
        contentHeight: bodyColumn.height + 50;

        boundsBehavior: Flickable.StopAtBounds;

        Column {
            id: bodyColumn;

            width: 400;

            spacing: 7;

            Text {
                id: titleRoles;

                text: qsTr("Roles");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            BasicTableView {
                id: rolesTable;

                width: bodyColumn.width;

                headerVisible: false;

                rowDelegate: TableUserRolesDelegate {
                    Component.onCompleted: {
                        rolesTable.height += height;
                    }
                }

                Component.onCompleted: {
                    rolesTable.addColumn({"Id": "Name", "Name": "Name"})
                }

                onRowModelDataChanged: {
                    console.error("onRowModelDataChanged", prop)
                    if (!blockUpdatingModel){
                        updateModel();
                    }
                }
            }

            Text {
                id: titlePermissions;

                text: qsTr("Permissions");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            BasicTableView {
                id: permissionsTable;

                width: bodyColumn.width;
                height: permissionsTable.headerHeight;

                rowDelegate: TableViewItemDelegateBase {
                    Component.onCompleted: {
                        permissionsTable.height += height;
                    }
                }

                Component.onCompleted: {
                    permissionsTable.addColumn({"Id": "Name", "Name": "Name"})
                }
            }
        }//Column bodyColumn
    }//Flickable
}//Container
