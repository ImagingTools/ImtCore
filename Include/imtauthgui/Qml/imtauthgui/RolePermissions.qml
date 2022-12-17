import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import imtlicgui 1.0
import Acf 1.0

Item {
    id: rolePermissionsContainer;

//    anchors.fill: parent;

    property TreeItemModel documentModel: TreeItemModel {}
    property UndoRedoManager undoRedoManager: null;

    property bool blockUpdatingModel: false;

    property int mainMargin: 0;
    property int panelWidth: 400;

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

    FeaturesDependenciesProvider {
        id: dependenciesProvider;

        Component.onCompleted: {
            dependenciesProvider.updateModel();
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

        permissionsTable.rowModel.clear();

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

    Component{
        id: emptyDecorator;
        Item{
            property Item rootItem;
        }
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

    Loader{
        id: mainPanelFrameLoader;

        anchors.top: parent.top;
        anchors.topMargin: 2;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: rolePermissionsContainer.mainMargin;

        width: 400 + 2*rolePermissionsContainer.mainMargin;

        sourceComponent: Style.frame !==undefined ? Style.frame: emptyDecorator;

        onLoaded: {
            if(mainPanelFrameLoader.item){
                rolePermissionsContainer.mainMargin = mainPanelFrameLoader.item.mainMargin;
            }
        }
    }//Loader

    BasicTreeView {
        id: permissionsTable;

        anchors.left: parent.left;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.topMargin: Math.max(10,rolePermissionsContainer.mainMargin);
        anchors.bottomMargin: 10 + rolePermissionsContainer.mainMargin;
        anchors.leftMargin: rolePermissionsContainer.mainMargin;

        width: 400;

        tristate: true;

        Component.onCompleted: {
            permissionsTable.addColumn({"Id": "Name", "Name": "Name"});
        }

        onRowModelDataChanged: {
            if (!blockUpdatingModel){
                updateModel();
            }
        }

        onSelectedIndexChanged: {
            if (selectedIndex != null){
                let selectedFeatureId = selectedIndex.itemData.Id;

                let featureDependencies = dependenciesProvider.getAllDependencies(selectedFeatureId);

                repeater.model = featureDependencies;
            }
        }
    }//BasicTableView

    Item {
        id: informationBlock;

        anchors.left: permissionsTable.right;
        anchors.leftMargin: 10;
        anchors.top: parent.top;
        anchors.topMargin: Math.max(10, rolePermissionsContainer.mainMargin);
        anchors.bottom: parent.bottom;

        width: 200;

        Column {
            id: informationColumn;

            width: parent.width;

            Text {
                id: title;

                width: informationColumn.width;

                text: qsTr("Dependencies");

                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamilyBold;
                font.bold: true;

                color: Style.lightBlueColor;
                elide: Text.ElideRight;
                wrapMode: Text.WrapAnywhere ;
            }

            Column {
                id: childColumn;

                anchors.top: title.bottom;
                anchors.topMargin: 5;

                width: parent.width;

                Repeater {
                    id: repeater;

                    delegate: Text {
                        width: 200;
                        height: 20;

                        text: modelData;

                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_small;

                        color: Style.textColor;
                        elide: Text.ElideRight;
                    }
                }
            }
        }
    }
}//Container
