import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtlicgui 1.0
import imtcontrols 1.0

ViewBase {
    id: rolePermissionsContainer;

    height: permissionsGroup.height;

    property int mainMargin: 0;

    property string productId: "";
    property TreeItemModel permissionsModel: TreeItemModel {};

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", rolePermissionsContainer.onLocalizationChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", rolePermissionsContainer.onLocalizationChanged);
    }

    function onLocalizationChanged(language){
        permissionHeaders.updateHeaders();
    }

    FeaturesDependenciesProvider {
        id: dependenciesProvider;
    }

    function updateGui(){
        let selectedPermissionsIds = [];
        if (rolePermissionsContainer.model.ContainsKey("Permissions")){
            let selectedPermissions = rolePermissionsContainer.model.GetData("Permissions");
            if (selectedPermissions !== ""){
                selectedPermissionsIds = selectedPermissions.split(';');
            }
        }

        selectedPermissionsIds.sort();

        permissionsGroup.treeView.uncheckAll();

        let itemsList = permissionsGroup.treeView.getItemsDataAsList();
        for (let i = 0; i < itemsList.length; i++){
            let delegateItem = itemsList[i];
            if (!delegateItem.hasChild){
                let itemData = delegateItem.getItemData();
                let id = itemData.FeatureId;

                if (selectedPermissionsIds.includes(id)){
                    delegateItem.isOpened = true;
                    permissionsGroup.treeView.checkItem(delegateItem);
                }
                else{
                    delegateItem.isOpened = false;
                }
            }
        }
    }

    function updateModel(){
        let selectedPermissionIds = []
        let itemsList = permissionsGroup.treeView.getCheckedItems();
        for (let delegate of itemsList){
            if (!delegate.hasChild){
                let itemData = delegate.getItemData();
                let id = itemData.FeatureId;
                selectedPermissionIds.push(id)
            }
        }

        selectedPermissionIds.sort();

        let permissions = rolePermissionsContainer.model.GetData("Permissions");
        rolePermissionsContainer.model.SetData("Permissions", selectedPermissionIds.join(';'));
    }

    TreeViewElementView {
        id: permissionsGroup;

        width: parent.width;

        Component.onCompleted: {
            permissionsGroup.treeView.tristate = true;
            permissionsGroup.treeView.rowModel = rolePermissionsContainer.permissionsModel;
        }

        Connections {
            target: permissionsGroup.treeView;

            function onSelectedIndexChanged() {
                let selectedIndex = permissionsGroup.treeView.selectedIndex;
                if (selectedIndex !== null && selectedIndex.itemData){
                    let selectedFeatureId = selectedIndex.itemData.FeatureId;

                    let dependencies = selectedIndex.itemData.Dependencies;

                    let featureDependencies = []

                    if (dependencies !== ""){
                        featureDependencies = dependencies.split(';');
                    }

                    if (featureDependencies.length === 0){
                        featureDependencies.push(qsTr("No dependencies"))
                    }
                }
            }

            function onCheckedItemsChanged(){
                rolePermissionsContainer.doUpdateModel();
            }
        }

        TreeItemModel {
            id: permissionHeaders;

            function updateHeaders(){
                permissionHeaders.Clear();

                let index = permissionHeaders.InsertNewItem();
                permissionHeaders.SetData("Id", "FeatureName", index)
                permissionHeaders.SetData("Name", qsTr("Permission"), index)

                permissionHeaders.Refresh();

                permissionsGroup.treeView.columnModel = permissionHeaders;
            }
        }
    }

    function findChildrenFeatureDependencies(featureId, retVal){
        let itemsDataList = permissionsGroup.treeView.getItemsDataAsList();
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
