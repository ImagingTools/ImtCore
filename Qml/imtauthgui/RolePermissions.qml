import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtqml 1.0
import imtlicgui 1.0

Item {
    id: rolePermissionsContainer;

//    anchors.fill: parent;

    property TreeItemModel documentModel: TreeItemModel {}
    property Item documentPtr: null;

    property int mainMargin: 0;
    property int panelWidth: 400;

    property bool completed: permissionsProvider.compl;

    property string productId: documentPtr ? documentPtr.productId : "";

    onProductIdChanged: {
        console.log("onProductIdChanged", productId);
        if (productId !== ""){
            permissionsProvider.productId = productId;

            permissionsProvider.updateModel();
        }
    }

    PermissionsProvider {
        id: permissionsProvider;

        property bool compl: false;

        onDataModelChanged: {
            if (permissionsProvider.dataModel != null){
                permissionsTable.rowModel = permissionsProvider.dataModel;

                compl = true;
            }
        }

        onDependenciesModelChanged: {
            dependenciesProvider.model = dependenciesModel;
        }
    }

    FeaturesDependenciesProvider {
        id: dependenciesProvider;
    }

    function blockEditing(){
        permissionsTable.readOnly = true;
    }

    function updateGui(){
        console.log("RolePermissions updateGui");

        let selectedPermissionsIds = [];
        if (rolePermissionsContainer.documentModel.ContainsKey("Permissions")){
            let selectedPermissions = rolePermissionsContainer.documentModel.GetData("Permissions");
            if (selectedPermissions !== ""){
                selectedPermissionsIds = selectedPermissions.split(';');
            }
        }

        permissionsTable.uncheckAll();

        let itemsList = permissionsTable.getItemsDataAsList();
        for (let i = 0; i < itemsList.length; i++){
            let delegateItem = itemsList[i];
            if (!delegateItem.hasChild){
                let itemData = delegateItem.getItemData();
                let id = itemData.FeatureId;

                if (selectedPermissionsIds.includes(id)){
                    delegateItem.isOpened = true;
                    permissionsTable.checkItem(delegateItem);
                }
                else{
                    delegateItem.isOpened = false;
                }
            }
        }
    }

    function updateModel(){
        let selectedPermissionIds = []
        let itemsList = permissionsTable.getCheckedItems();
        for (let delegate of itemsList){
            if (!delegate.hasChild){
                let itemData = delegate.getItemData();
                let id = itemData.FeatureId;
                selectedPermissionIds.push(id)
            }
        }

        console.log("selectedPermissionIds", selectedPermissionIds.join(';'));
        let permissions = rolePermissionsContainer.documentModel.GetData("Permissions");
        console.log("permissions", permissions);

        rolePermissionsContainer.documentModel.SetData("Permissions", selectedPermissionIds.join(';'));
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
            permissionsTable.addColumn({"Id": "FeatureName", "Name": "Permission"});
        }

        onSelectedIndexChanged: {
            if (selectedIndex != null && selectedIndex.itemData){
                let selectedFeatureId = selectedIndex.itemData.FeatureId;

                let dependencies = selectedIndex.itemData.Dependencies;

                let featureDependencies = []

                if (dependencies !== ""){
                    featureDependencies = dependencies.split(';');
                }

                if (featureDependencies.length === 0){
                    featureDependencies.push(qsTr("No dependencies"))
                }

                repeater.model = featureDependencies;
            }
        }

        onCheckedItemsChanged: {
            if (rolePermissionsContainer.documentPtr){
                rolePermissionsContainer.documentPtr.doUpdateModel();
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

        visible: permissionsTable.selectedIndex != null;

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

                    visible: false;

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

    function findChildrenFeatureDependencies(featureId, retVal){
        let itemsDataList = permissionsTable.getItemsDataAsList();
        for (let i = 0; i < itemsDataList.length; i++){
            let delegateItem = itemsDataList[i]
            let itemData = delegateItem.getItemData();
            let id = itemData.FeatureId;

            if (featureId === id){
                let dependencies = itemData.Dependencies;
                if (dependencies && dependencies !== ""){
                    let dependencyList = dependencies.split(';');

                    for (let dependencyId of dependencyList){
                        if (!retVal.includes(dependencyId)){
                            retVal.push(dependencyId)

                            findChildrenFeatureDependencies(dependencyId, retVal);
                        }
                    }
                }
            }
        }
    }
}//Container
