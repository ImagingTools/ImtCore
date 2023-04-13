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
        if (rolePermissionsContainer.documentModel.ContainsKey("ProductId")){
            let productId = rolePermissionsContainer.documentModel.GetData("ProductId");

            permissionsProvider.productId = productId;

            permissionsProvider.updateModel();
        }
    }

    PermissionsProvider {
        id: permissionsProvider;

        Component.onDestruction: {
            permissionsTable.rowModelDataChanged.disconnect(rolePermissionsContainer.updateModel);
        }

        onDataModelChanged: {
            if (permissionsProvider.dataModel != null){
                let listModel = converter.convertToListModel(permissionsProvider.dataModel);
                permissionsTable.rowModel = listModel;
                rolePermissionsContainer.updateGui();

                permissionsTable.rowModelDataChanged.connect(rolePermissionsContainer.updateModel);
            }
        }

        onDependenciesModelChanged: {
            dependenciesProvider.model = dependenciesModel;
        }
    }

    FeaturesDependenciesProvider {
        id: dependenciesProvider;
    }

    TreeItemModelConverter {
        id: converter;
    }

    function updateGui(){
        console.log("RolePermissions updateGui");
        rolePermissionsContainer.blockUpdatingModel = true;

        let selectedPermissionsIds = [];
        if (rolePermissionsContainer.documentModel.ContainsKey("Permissions")){
            let selectedPermissions = rolePermissionsContainer.documentModel.GetData("Permissions");
            if (selectedPermissions !== ""){
                selectedPermissionsIds = selectedPermissions.split(';');
            }
        }

        let itemsList = permissionsTable.getItemsDataAsList();
        for (let i = 0; i < itemsList.length; i++){
            let id = itemsList[i].Id;
            if (selectedPermissionsIds.includes(id)){
                itemsList[i].CheckState = Qt.Checked;
            }
            else{
                itemsList[i].CheckState = Qt.Unchecked;
            }
        }

        rolePermissionsContainer.blockUpdatingModel = false;
    }

    function recursiveUpdateGui(dataModel, tableIndexes, selectedPermissions){
        for (let i = 0; i < dataModel.GetItemsCount(); i++){
            let permissionId = dataModel.GetData("Id", i);
            let permissionName = dataModel.GetData("Name", i);

            let row = {"Id": permissionId, "Name": permissionName, "CheckState": Qt.Unchecked};

            if (selectedPermissions.includes(permissionId)){
                row["CheckState"] = Qt.Checked;
            }

            permissionsTable.insertRow(tableIndexes.concat([i]), row);

            let childModel = dataModel.GetData("ChildModel", i);
            if (childModel){
                let childIndexes = [].concat(tableIndexes.concat([i]))
                rolePermissionsContainer.recursiveUpdateGui(childModel, childIndexes,selectedPermissions);
            }
        }
    }

    function updateModel(){
        console.log("documentModel1", rolePermissionsContainer.documentModel.toJSON());
        rolePermissionsContainer.undoRedoManager.beginChanges();

        let selectedPermissionIds = []
        let itemsList = permissionsTable.getItemsDataAsList();
        for (let i = 0; i < itemsList.length; i++){
            let id = itemsList[i].Id;
            let state = itemsList[i].CheckState;
            if (state === Qt.Checked){
                selectedPermissionIds.push(id)
            }
        }
        rolePermissionsContainer.documentModel.SetData("Permissions", selectedPermissionIds.join(';'));

        rolePermissionsContainer.undoRedoManager.endChanges();
    }

    function recursiveUpdateModel(dataModel, guiModel){
        console.log("recursiveUpdateModel", dataModel, guiModel);
        for (let i = 0; i < guiModel.count; i++){
            let rowObj = guiModel.get(i);

            let permissionId = rowObj["Id"];
            let permissionName = rowObj["Name"];
            let state = rowObj["CheckState"];

            if (state == Qt.Checked){
                let index = dataModel.InsertNewItem();

                dataModel.SetData("Id", permissionId, index);
                dataModel.SetData("Name", permissionName, index);
            }

            let rowChildModel = rowObj["ChildModel"]
            if (rowChildModel.count > 0){
                rolePermissionsContainer.recursiveUpdateModel(dataModel, rowChildModel);
            }
        }
    }

    Component{
        id: emptyDecorator;
        Item{
            property Item rootItem: null;
            property int mainMargin: 0;
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

    CustomScrollbar {
        id: scrollbar;

        z: 100;

        anchors.right: permissionsTable.right;
        anchors.bottom: permissionsTable.bottom;

        backgroundColor: Style.baseColor;

        secondSize: 10;
        targetItem: permissionsTable.tableListView;
    }

    BasicTreeView {
        id: permissionsTable;

        anchors.left: parent.left;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.topMargin: Math.max(10,rolePermissionsContainer.mainMargin);
        anchors.bottomMargin: 10 + rolePermissionsContainer.mainMargin;
        anchors.leftMargin: rolePermissionsContainer.mainMargin;
        anchors.right: informationBlock.left;

        tristate: true;

        Component.onCompleted: {
            permissionsTable.addColumn({"Id": "Name", "Name": "Permission Name"});
        }

        onRowModelDataChanged: {
            if (!rolePermissionsContainer.blockUpdatingModel){
                rolePermissionsContainer.updateModel();
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

        anchors.right: parent.right;
        anchors.rightMargin: 10;
        anchors.top: parent.top;
        anchors.topMargin: Math.max(10, rolePermissionsContainer.mainMargin);
        anchors.bottom: parent.bottom;

        width: 200;

        Item {
            anchors.fill: parent;

            anchors.rightMargin: 10;
            anchors.leftMargin: 10;
            anchors.bottomMargin: 10;

            Text {
                id: title;

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
