import QtQuick 2.12
import Acf 1.0
import imtauthgui 1.0
import imtdocgui 1.0
import imtgui 1.0

DocumentData {
    id: featureEditor;

    property TreeItemModel dependenciewViewModel: TreeItemModel {}

    property string featureId: "";

    property Component treeItemModelComp: Component {
        TreeItemModel {}
    }

    commandsDelegate: DocumentWorkspaceCommandsDelegateBase {
        documentPtr: featureEditor;

        onCommandActivated: {
            console.log("FeatureViewCommands onCommandActivated", commandId);
            let selectedIndex = null;
            if (tableView.tableSelection.items.length > 0){
                selectedIndex = tableView.tableSelection.items[0];
            }

            if (commandId === "New"){
                if (selectedIndex != null){
                    tableView.addChildItem(selectedIndex, {"FeatureId":"", "FeatureName":"Feature Name", "FeatureDescription":"", "Dependencies":"", "Optional":false, "ChildModel":0})
                }
            }
            else if (commandId === "Remove"){
                if (selectedIndex != null){
                    tableView.removeChildItem(selectedIndex);
                }
            }
        }
    }

    function getAllParents(selectedIndex){
        let retVal = []

        if (selectedIndex != null){
            let parent = selectedIndex.parentIndex;

            while (parent && parent.itemData){
                let parentId = parent.itemData.FeatureId
                retVal.push(parentId);
                parent = parent.parentIndex;
            }
        }

        return retVal;
    }

    function updateTreeViewGui(){
        console.log("updateTreeViewGui");

        let selectedIndex = null;
        if (tableView.tableSelection.items.length > 0){
            selectedIndex = tableView.tableSelection.items[0];
        }

        if (selectedIndex == null || !selectedIndex.itemData){
            return;
        }

        featureDependenciesView.delegateUpdatingBlock = true;

        let selectedId = "";

        if (selectedIndex != null && selectedIndex.itemData){
            selectedId = selectedIndex.itemData.FeatureId;
        }

        //Список всех зависящих фич для selectedId
        let childrenFeatureList = [];
        tableView.findChildrenFeatureDependencies(selectedId, childrenFeatureList);

        console.log("childrenFeatureList", childrenFeatureList);

        //Список всех зависящих фич от selectedId
        let inactiveElements = [];
        tableView.findParentFeatureDependencies(selectedId, inactiveElements);

        //Запрещаем зависимость от всех родителей
        let parentIds = featureEditor.getAllParents(selectedIndex);
        inactiveElements = inactiveElements.concat(parentIds);

        for (let i = 0; i < parentIds.length; i++){
            let parentId = parentIds[i];
            //Запрещаем зависимость для всех фич которые зависят от родителей
            tableView.findParentFeatureDependencies(parentId, inactiveElements);

            //Автоматом выбираем фичи от которых зависят родители
            tableView.findChildrenFeatureDependencies(parentId, childrenFeatureList);
        }

        console.log("inactiveElements", inactiveElements);

        //Список основных зависящих фич для selectedId
        let dependenciesList = []

        let dependencies = selectedIndex.itemData.Dependencies;
        if (dependencies && dependencies !== ""){
            dependenciesList = dependencies.split(';');
        }

        console.log("dependenciesList", dependenciesList);

        let itemsDataList = featureDependenciesView.getItemsDataAsList();
        for (let i = 0; i < itemsDataList.length; i++){
            let delegateItem = itemsDataList[i]
            let itemData = delegateItem.getItemData();
            let itemId = itemData.FeatureId;

            delegateItem.isVisible = itemId !== selectedId;
            delegateItem.isActive = !inactiveElements.includes(itemId);
            delegateItem.isCheckable = itemId !== "";
            delegateItem.checkState = Qt.Unchecked;

            if (childrenFeatureList.includes(itemId) && !dependenciesList.includes(itemId)){
                delegateItem.isActive = false;
                delegateItem.checkState = Qt.Checked;
            }
            //Если содержится во всех фичах и содержится в основном
            else if (childrenFeatureList.includes(itemId) && dependenciesList.includes(itemId)){
                delegateItem.isActive = true;
                delegateItem.checkState = Qt.Checked;
            }
        }

        console.log("end updateTreeViewGui");

        featureDependenciesView.delegateUpdatingBlock = false;
    }

    function updateGui(){
        console.log("updateGui", documentModel.toJSON());

//        if (tableView.rowModel){
//            tableView.rowModel = documentModel;

//            tableView.rowModel.Refresh();
//        }

        featureEditor.updateTreeViewGui();
    }

    function updateModel(){
        console.log("updateModel");

        if (documentModel.GetItemsCount() !== 1){
            let emptyModel = featureEditor.treeItemModelComp.createObject(documentModel);
            documentModel.InsertNewItemWithParameters(0, {"FeatureId":"", "FeatureName":"Feature Name", "FeatureDescription":"", "Dependencies":"", "Optional":false, "ChildModel": emptyModel});
        }
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }

    Item {
        id: centerPanel;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.right: rightBlock.left;
        anchors.rightMargin: 20;

        height: parent.height;

        Item {
            id: headerBlock;

            anchors.top: parent.top;
            anchors.left: parent.left;
            anchors.leftMargin: 10;

            width: parent.width;
            height: 0;

            visible: false;

            Text {
                id: titleTableView;

                anchors.left: parent.left;
                anchors.verticalCenter: parent.verticalCenter;

                color: Style.textColor;
                font.family: Style.fontFamilyBold;
                font.pixelSize: Style.fontSize_common;

                text: qsTr("Subfeatures");
            }
        }

        BasicTreeView {
            id: tableView;

            anchors.top: headerBlock.bottom;
            anchors.left: parent.left;
            anchors.leftMargin: 10;

            width: parent.width;
            height: parent.height;

            readOnly: false;

            rowModel: featureEditor.documentModel;

            rowDelegate: Component { PackageViewItemDelegate { root: tableView; } }

            Component.onCompleted: {
                let ok = PermissionsController.checkPermission("ChangeFeature");
                tableView.readOnly = !ok;
            }

            function canRename(id){
                return true;
            }

            function featureIdExists(featureId){
                console.log("featureIdExists", featureId);
                let delegates = tableView.getItemsDataAsList();
                for (let delegate of delegates){
                    if (delegate.itemData.FeatureId === featureId){
                        return true;
                    }
                }

                return false;
            }

            onSelectionChanged: {
                console.log("onSelectionChanged", tableSelection.items);

                let featureId = "";

                let selectedIndex =  null;
                if (tableSelection.items.length > 0){
                    selectedIndex = tableSelection.items[0];
                }

                if (selectedIndex != null){
                    featureId = selectedIndex.getData("FeatureId");
                    if (featureId !== ""){
                        updateTreeViewGui();
                    }
                }

                featureDependenciesView.contentVisible = featureId !== "" && selectedIndex != null;

                let newIsEnabled = selectedIndex != null;
                let removeIsEnabled = selectedIndex != null;

                if (selectedIndex != null){
                    removeIsEnabled = removeIsEnabled && selectedIndex.depth > 0
                }

                featureEditor.commandsProvider.setCommandIsEnabled("New", newIsEnabled)
                featureEditor.commandsProvider.setCommandIsEnabled("Remove", removeIsEnabled)
            }

            function findParentFeatureDependencies(featureId, retVal){
                let itemsDataList = tableView.getItemsDataAsList();
                for (let i = 0; i < itemsDataList.length; i++){
                    let delegateItem = itemsDataList[i]
                    let itemData = delegateItem.getItemData();
                    let id = itemData.FeatureId;
                    let dependencies = itemData.Dependencies;
                    if (dependencies && dependencies !== ""){
                        let dependencyList = dependencies.split(';');

                        if (dependencyList.includes(featureId)){
                            if (!retVal.includes(id)){
                                retVal.push(id);

                                tableView.findParentFeatureDependencies(id, retVal);
                                featureDependenciesView.findParentFeatureDependencies(id, retVal);
                            }
                        }
                    }
                }

                featureDependenciesView.findParentFeatureDependencies(featureId, retVal);
            }

            function findChildrenFeatureDependencies(featureId, retVal){
                let itemsDataList = tableView.getItemsDataAsList();
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

                                    tableView.findChildrenFeatureDependencies(dependencyId, retVal);
                                    featureDependenciesView.findChildrenFeatureDependencies(dependencyId, retVal);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Item {
        id: rightBlock;

        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.right: parent.right;
        anchors.rightMargin: 5;

        width: 300;

        Item {
            id: titleBlock;

            anchors.top: parent.top;
            anchors.left: parent.left;

            width: parent.width;
            height: 0;

            visible: false;

            Text {
                id: titleDependenciesView;

                anchors.left: parent.left;
                anchors.verticalCenter: parent.verticalCenter;

                color: Style.textColor;
                font.family: Style.fontFamilyBold;
                font.pixelSize: Style.fontSize_common;

                text: qsTr("Dependencies");
            }
        }

        BasicTreeView {
            id: featureDependenciesView;

            anchors.top: titleBlock.bottom;
            anchors.bottom: parent.bottom;
            anchors.right: parent.right;
            anchors.left: parent.left;

            clip: true;

            tristate: true;

            contentVisible: false;

            property bool delegateUpdatingBlock: false;

            Component.onCompleted: {
                let ok = PermissionsController.checkPermission("ChangeFeature");
                featureDependenciesView.readOnly = !ok;

                featureDependenciesView.addColumn({"Id": "FeatureName", "Name": "Dependencies"});
                console.log("FeaturesProvider.model", FeaturesProvider.model);

                featureEditor.dependenciewViewModel.Copy(FeaturesProvider.model);
                featureDependenciesView.rowModel = featureEditor.dependenciewViewModel;

                FeaturesProvider.onModelChanged.connect(featureDependenciesView.onFeaturesProviderModelChanged);
            }

            Component.onDestruction: {
                FeaturesProvider.onModelChanged.disconnect(featureDependenciesView.onFeaturesProviderModelChanged);
            }

            function onFeaturesProviderModelChanged(){
                console.log("onFeaturesProviderModelChanged", FeaturesProvider.model);
                console.log("FeaturesProvider.model", FeaturesProvider.model.toJSON());

                featureEditor.dependenciewViewModel.Copy(FeaturesProvider.model)

                console.log("FeaturesProvider.model", featureEditor.dependenciewViewModel.toJSON());

                featureEditor.dependenciewViewModel.Refresh();

                featureEditor.updateTreeViewGui();
            }

            rowDelegate: Component {TreeViewItemDelegateBase {
                    id: delegate;

                    root: featureDependenciesView;

                    onCheckStateChanged: {
                        console.log("onCheckStateChanged", delegate.checkState);

                        let selectedIndex =  null;
                        if (tableView.tableSelection.items.length > 0){
                            selectedIndex = tableView.tableSelection.items[0];
                        }

                        if (selectedIndex != null){
                            if (!featureDependenciesView.delegateUpdatingBlock){
                                if (delegate.itemData.FeatureId !== ""){
                                    let featureId = delegate.itemData.FeatureId;
                                    let state = delegate.checkState;

                                    console.log("onCheckStateChanged", featureId, state);


                                    let selectedId = selectedIndex.itemData.FeatureId;
                                    let dependencies = selectedIndex.itemData.Dependencies;

                                    console.log("selectedId", selectedId);
                                    console.log("dependencies1", dependencies);

                                    let dependencyList = []
                                    if (dependencies != ""){
                                        dependencyList = dependencies.split(';')
                                    }

                                    if (state == Qt.Checked){
                                        if (!dependencyList.includes(featureId)){
                                            dependencyList.push(featureId);
                                        }
                                    }
                                    else{
                                        let pos = dependencyList.indexOf(featureId);
                                        if (pos >= 0){
                                            dependencyList.splice(pos, 1)
                                        }
                                    }

                                    if (dependencyList.length > 0){
                                        selectedIndex.itemData.Dependencies = dependencyList.join(';')
                                    }
                                    else{
                                        selectedIndex.itemData.Dependencies = "";
                                    }

                                    console.log("dependencies2", selectedIndex.itemData.Dependencies);

                                    featureEditor.updateTreeViewGui();
                                }
                            }
                        }
                    }
                }
            }

            function findParentFeatureDependencies(featureId, retVal){
                console.log("findParentFeatureDependencies", featureId, retVal);

                let itemsDataList = featureDependenciesView.getItemsDataAsList();
                for (let i = 0; i < itemsDataList.length; i++){
                    let delegateItem = itemsDataList[i]
                    let itemData = delegateItem.getItemData();
                    let id = itemData.FeatureId;
                    let dependencies = itemData.Dependencies;

                    let rootId = itemData.RootFeatureId;

                    if (rootId === featureEditor.featureId){
                        continue;
                    }

                    console.log("FeatureId",id);
                    console.log("Dependencies", dependencies.split(';'));
                    console.log();

                    if (dependencies && dependencies !== ""){
                        let dependencyList = dependencies.split(';');

                        if (dependencyList.includes(featureId)){
                            if (!retVal.includes(id)){
                                retVal.push(id);

                                featureDependenciesView.findParentFeatureDependencies(id, retVal);
                            }
                        }
                    }
                }
            }

            function findChildrenFeatureDependencies(featureId, retVal){
                let itemsDataList = featureDependenciesView.getItemsDataAsList();
                for (let i = 0; i < itemsDataList.length; i++){
                    let delegateItem = itemsDataList[i]
                    let itemData = delegateItem.getItemData();
                    let rootId = itemData.RootFeatureId;

                    if (rootId === featureEditor.featureId){
                        continue;
                    }

                    let id = itemData.FeatureId;

                    if (featureId === id){
                        let dependencies = itemData.Dependencies;
                        if (dependencies && dependencies !== ""){
                            let dependencyList = dependencies.split(';');

                            for (let dependencyId of dependencyList){
                                if (!retVal.includes(dependencyId)){
                                    retVal.push(dependencyId)

                                    featureDependenciesView.findChildrenFeatureDependencies(dependencyId, retVal);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    TreeItemModel {
        id: headersModel;

        Component.onCompleted: {
            let index = headersModel.InsertNewItem();
            headersModel.SetData("Id", "FeatureName", index);
            headersModel.SetData("Name", qsTr("Feature Name"), index);

            index = headersModel.InsertNewItem();
            headersModel.SetData("Id", "FeatureId", index);
            headersModel.SetData("Name", qsTr("Feature-ID"), index);

            index = headersModel.InsertNewItem();
            headersModel.SetData("Id", "FeatureDescription", index);
            headersModel.SetData("Name", qsTr("Feature Description"), index);

            index = headersModel.InsertNewItem();
            headersModel.SetData("Id", "Optional", index);
            headersModel.SetData("Name", qsTr("Optional"), index);

            tableView.columnModel = headersModel;
        }
    }
}
