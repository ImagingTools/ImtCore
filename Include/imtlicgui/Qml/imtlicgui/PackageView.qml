import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

DocumentBase {
    id: container;

    commandsDelegateSourceComp: packageViewCommandsDelegate;
    Component {
        id: packageViewCommandsDelegate;
        PackageViewCommandsDelegate {}
    }

    nameOutsideEditor: true;

    property int packageIndex: -1;
    property TreeItemModel featuresModel: TreeItemModel {
        Component.onCompleted: {
            container.featuresModel.dataChanged.connect(container.syncronise);
            container.featuresModel.dataChanged.connect(container.onFeatureModelChanged);
        }
    }

    property TreeItemModel treeViewModel: TreeItemModel {}
    property TreeItemModel headersModel: TreeItemModel {
        Component.onCompleted: {
            let index = container.headersModel.InsertNewItem();
            container.headersModel.SetData("Id", "Name", index);
            container.headersModel.SetData("Name", "Feature Name", index);

            index = container.headersModel.InsertNewItem();
            container.headersModel.SetData("Id", "Id", index);
            container.headersModel.SetData("Name", "Feature-ID", index);

            index = container.headersModel.InsertNewItem();
            container.headersModel.SetData("Id", "Description", index);
            container.headersModel.SetData("Name", "Description", index);

            index = container.headersModel.InsertNewItem();
            container.headersModel.SetData("Id", "Optional", index);
            container.headersModel.SetData("Name", "Optional", index);
        }
    }

    Component.onCompleted: {
        commandsDelegate.tableTreeViewEditor = tableView;
        featuresProvider.onModelChanged.connect(container.updateTreeViewModel);
    }

    Component.onDestruction: {
        featuresProvider.onModelChanged.disconnect(container.updateTreeViewModel);
    }

    onCommandsIdChanged: {
        if (container.itemId === ""){
            container.commandsProvider.setCommandIsEnabled("Save", true);
        }
    }

    onSaved: {
        featuresProvider.updateModel();
    }

    onFeaturesModelChanged: {
        container.featuresModel.dataChanged.connect(container.syncronise);
        container.featuresModel.dataChanged.connect(container.onFeatureModelChanged);
    }

    onModelIsReadyChanged: {
        if (container.modelIsReady){
            container.blockUpdatingModel = true;

            if (!container.documentModel.ContainsKey("Items")){
                container.documentModel.AddTreeModel("Items")
            }

            if (!container.documentModel.ContainsKey("DependenciesModel")){
                container.documentModel.AddTreeModel("DependenciesModel")
            }

            tableView.columnModel = container.headersModel;

            tableView.rowModel = container.featuresModel;
            tableView.rowModel.dataChanged.connect(container.syncronise);
            container.updateTreeViewModel();
            container.updateGui();

            container.blockUpdatingModel = false;
            undoRedoManager.registerModel(container.documentModel);
        }
    }

    onWidthChanged: {
        if (container.width > 0 && container.width - rightPanel.width > 250){
            splitter.x = container.width - rightPanel.width;
        }
    }

    UndoRedoManager {
        id: undoRedoManager;
        documentBase: container;
        onModelStateChanged: {
            console.log("onModelStateChanged");

            undoRedoManager.modelIsRegistered = false;
            container.updateGui();
            container.syncronise();
            undoRedoManager.modelIsRegistered = true;
        }
    }

    function onFeatureModelChanged(){
        console.log("onFeatureModelChanged");
        container.updateModel();
    }

    //Обновить модель для TreeView
    function updateTreeViewModel(){
        console.log("updateTreeViewModel");

        container.treeViewModel.Copy(featuresProvider.model);
        treeView.rowModel = container.treeViewModel;
        container.treeViewModel.Refresh();

        for (let i = 0; i < treeView.rowModel.GetItemsCount(); i++){
            let id =  treeView.rowModel.GetData("Id", i);
            if (container.itemId === id){
                container.packageIndex = i;
                break;
            }
        }

        rightPanel.updateTreeViewGui();

        console.log("end updateTreeViewModel");
    }

    function copyFrom(model1, model2){
        if (!model1 || !model2){
            return;
        }

        for (let i = 0; i < model2.GetItemsCount(); i++){
            model1.InsertNewItem();
            let keys = model2.GetKeys(i);
            for (let key of keys){
                let data = model2.GetData(key, i);
                if (typeof data  == 'object'){
                    let childModel = model1.AddTreeModel(key, i);
                    container.copyFrom(childModel, data);
                }
                else{
                    model1.SetData(key, data, i);
                }
            }
        }
    }

    //Синхронизация фич в package и treeView
    function syncronise(){
        console.log("syncronise");

        let items = container.documentModel.GetData("Items");
        if (container.packageIndex >= 0){
            let childModel = treeView.rowModel.AddTreeModel("ChildModel", container.packageIndex);
            container.copyFrom(childModel, items)
            childModel.Refresh();
        }

        rightPanel.updateTreeViewGui();

        container.updateModel();

        console.log("end syncronise");
    }

    function updateGui(){
        console.log("updateGui");
//        container.blockUpdatingModel = true;

        if (container.documentModel.ContainsKey("Items")){
            container.featuresModel = container.documentModel.GetData("Items");
        }

        tableView.rowModel = container.featuresModel;

        tableView.rowModel.Refresh();

        tableView.resetSelection();

        rightPanel.updateTreeViewGui();

//        container.blockUpdatingModel = false;
        console.log("end updateGui");

    }

    function updateModel(){
        console.log(" updateModel");

        undoRedoManager.beginChanges();

        container.documentModel.SetData("Id", container.itemId);
        container.documentModel.SetData("Name", container.itemName);

        if (!container.documentModel.ContainsKey("Items")){
            container.featuresModel = container.documentModel.AddTreeModel("Items")
        }

        if (!container.documentModel.ContainsKey("DependenciesModel")){
            container.documentModel.AddTreeModel("DependenciesModel")
        }

        undoRedoManager.endChanges();

        console.log("end updateModel");
    }

    function updateDependenciesModel(featureId, checkState){
        console.log(" updateDependenciesModel");

        undoRedoManager.beginChanges();

        let dependenciesModel = container.documentModel.GetData("DependenciesModel");

        if (!dependenciesModel){
            dependenciesModel = container.documentModel.AddTreeModel("DependenciesModel");
        }

        let selectedId = tableView.selectedIndex.itemData.Id;
        let dependencyId = featureId;

        if (dependenciesModel.ContainsKey(selectedId)){
            let dependencies = dependenciesModel.GetData(selectedId);

            if (dependencies !== ""){
                let dependenciesList = dependencies.split(';')

                let pos = dependenciesList.indexOf(dependencyId);
                if (checkState === Qt.Checked){
                    if (pos < 0){
                        dependenciesList.push(dependencyId)
                    }
                }
                else{
                    if (pos >= 0){
                        dependenciesList.splice(pos, 1)
                    }
                }

                let newDependencies = dependenciesList.join(';')

                if (newDependencies.length === 0){
                    dependenciesModel.RemoveData(selectedId)
                }
                else{
                    dependenciesModel.SetData(selectedId, newDependencies);
                }
            }
        }
        else{
            if (checkState === Qt.Checked){
                dependenciesModel.SetData(selectedId, dependencyId);
            }
        }

        undoRedoManager.endChanges();
        console.log("end updateDependenciesModel");
    }

    function isPackage(itemId){
        if (featuresProvider && featuresProvider.model){
            for (let i = 0; i < featuresProvider.model.GetItemsCount(); i++){
                let packageId = featuresProvider.model.GetData("Id", i);
                if (packageId === itemId){
                    return true;
                }
            }
        }

        return false;
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }

    BasicTreeView {
        id: tableView;

        anchors.top: parent.top;
        anchors.bottom: parent.bottom;

        anchors.left: parent.left;
        anchors.right: splitter.left;

        readOnly: false;

        rowDelegate: Component { PackageViewItemDelegate { root: tableView; } }

        function openFeatureErrorDialog(message){
            container.documentManager.openErrorDialog(message);
        }

        function canRename(id){
            if (container.featureHasDependencies(id)){
                return false;
            }

            return true;
        }

        function featureIdExists(featureId){
            let delegateItems = tableView.getItemsDataAsList();
            for (let item of delegateItems){
                let itemData = item.getItemData();
                if (itemData.Id === featureId){
                    return true;
                }
            }

            let globalExists = featuresProvider.featureIsExists(featureId);

            return globalExists;
        }
    }

    function featureHasDependencies(featureId){
        console.log("featureHasDependencies", featureId);

        let dependenciesModel = container.documentModel.GetData("DependenciesModel");
        if (dependenciesModel){
            if (dependenciesModel.ContainsKey(featureId)){
                return true;
            }

            let keys = dependenciesModel.GetKeys();

            for (let i = 0; i < keys.length; i++){
                let key = keys[i];
                let values = dependenciesModel.GetData(key);

                if (values !== ""){
                    let dependenciesList = values.split(';');

                    if (dependenciesList.includes(featureId)){
                        return true;
                    }
                }
            }
        }

        return false;
    }

    Splitter {
        id: splitter;
        x: container.width - 250;

        height: parent.height;
        width: 4;

        onXChanged: {
            if (!container.visible){
                return;
            }

            if (splitter.x <= 250){
                splitter.x = 250;
            } else if (splitter.x > container.width - titleHeader.width){
                splitter.x = container.width - splitter.width;
            }
        }
    }

    Column {
        id: rightPanel;

        anchors.left: splitter.right;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;

        width: container.width > 0 ? container.width - tableView.width : 250;

        Rectangle {
            id: headerTreeView;

            width: parent.width;
            height: 35;

            color: Style.alternateBaseColor;

            border.width: 1;
            border.color: Style.imagingToolsGradient2;

            Text {
                id: titleHeader;

                anchors.verticalCenter: headerTreeView.verticalCenter;
                anchors.left: headerTreeView.left;
                anchors.leftMargin: 10;

                color: Style.textColor;
                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamilyBold;
                font.bold: true;

                text: qsTr("Dependencies");
            }
        }

        function updateTreeViewGui(){
            console.log("updateTreeViewGui");
            treeView.delegateUpdatingBlock = true;

            let selectedId = "";

            if (tableView.selectedIndex != null && tableView.selectedIndex.itemData){
                selectedId = tableView.selectedIndex.itemData.Id;
            }

            //Список всех зависящих фич для selectedId
            let childrenFeatureList = [];
            rightPanel.findChildrenFeatureDependencies(selectedId, childrenFeatureList);

            //Список всех зависящих фич от selectedId
            let inactiveElements = [];
            rightPanel.findParentFeatureDependencies(selectedId, inactiveElements);

            //Запрещаем зависимость от всех родителей
            let parentIds = rightPanel.getAllParents(tableView.selectedIndex);
            inactiveElements = inactiveElements.concat(parentIds);

            for (let i = 0; i < parentIds.length; i++){
                let parentId = parentIds[i];
                //Запрещаем зависимость для всех фич которые зависят от родителей
                rightPanel.findParentFeatureDependencies(parentId, inactiveElements);
                //Автоматом выбираем фичи от которых зависят родители
                rightPanel.findChildrenFeatureDependencies(parentId, childrenFeatureList);
            }

            //Список основных зависящих фич для selectedId
            let dependenciesList = []

            let dependenciesModel = container.documentModel.GetData("DependenciesModel");
            if (dependenciesModel && dependenciesModel.ContainsKey(selectedId)){
                let dependencies = dependenciesModel.GetData(selectedId);
                if (dependencies !== ""){
                    dependenciesList = dependencies.split(';');
                }
            }

            let itemsDataList = treeView.getItemsDataAsList();
            for (let i = 0; i < itemsDataList.length; i++){
                let delegateItem = itemsDataList[i]
                let itemData = delegateItem.getItemData();
                let itemId = itemData.Id;

                if (container.isPackage(itemId) && delegateItem.level === 0){
                    delegateItem.isCheckable = false;

                    continue
                }

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

            treeView.delegateUpdatingBlock = false;
        }

        function getAllParents(selectedIndex){
            let retVal = []

            if (selectedIndex != null){
                let parent = selectedIndex.parentIndex;

                while (parent){
                    let parentId = parent.itemData.Id
                    retVal.push(parentId);
                    parent = parent.parentIndex;
                }
            }

            return retVal;
        }

        function findParentFeatureDependencies(featureId, retVal){
            let dependenciesModel = container.documentModel.GetData("DependenciesModel");

            if (!dependenciesModel){
                return;
            }

            let keys = dependenciesModel.GetKeys();

            for (let i = 0; i < keys.length; i++){
                let key = keys[i];

                let values = dependenciesModel.GetData(key);
                if (values != ""){
                    let dependenciesList = values.split(';')
                    if (dependenciesList.includes(featureId)){
                        retVal.push(key)
                        rightPanel.findParentFeatureDependencies(key, retVal)
                    }
                }
            }
        }

        function findChildrenFeatureDependencies(featureId, retVal){
            let dependenciesModel = container.documentModel.GetData("DependenciesModel");

            if (!dependenciesModel){
                return;
            }

            let keys = dependenciesModel.GetKeys();

            if (keys.includes(featureId)){
                let values = dependenciesModel.GetData(featureId);

                if (values != ""){
                    let dependenciesList = values.split(';')

                    for (let i = 0; i < dependenciesList.length; i++){
                        let value = dependenciesList[i];

                        retVal.push(value)

                        rightPanel.findChildrenFeatureDependencies(value, retVal);
                    }
                }
            }
        }

        Text {
            id: saveText;

            anchors.top: headerTreeView.bottom;
            anchors.left: headerTreeView.left;
            anchors.leftMargin: 10;
            anchors.topMargin: 10;

            color: Style.textColor;
            font.pixelSize: Style.fontSize_common;
            font.family: Style.fontFamily;

            text: qsTr("Please save package first");

            visible: container.itemId === "";
        }

        BasicTreeView {
            id: treeView;

            anchors.top: headerTreeView.bottom;
            anchors.bottom: parent.bottom;
            anchors.left: parent.left;
            anchors.right: parent.right;
            anchors.rightMargin: 5;
//            width: parent.width;

            clip: true;

            headerVisible: false;

            tristate: true;

            visible: false;

            property bool delegateUpdatingBlock: false;

            Component.onCompleted: {
                treeView.addColumn({"Id": "Name", "Name": "Name"});
                tableView.selectionChanged.connect(rightPanel.selectedIndexChanged)
            }

            rowDelegate: Component {TreeViewItemDelegateBase {
                id: delegate;

                root: treeView;

                onCheckStateChanged: {
                    console.log("onCheckStateChanged");
                    if (tableView.selectedIndex != null){
                        if (!treeView.delegateUpdatingBlock){

                            if (delegate.itemData.Id !== ""){
                                container.updateDependenciesModel(delegate.itemData.Id, delegate.checkState);
                                rightPanel.updateTreeViewGui();
                            }
                        }
                    }
                }
            }
        } }

        function selectedIndexChanged(){
            console.log("selectedIndexChanged", tableView.selectedIndex);

            let result = tableView.selectedIndex != null && tableView.selectedIndex.itemData.Id !== "" && container.itemId !== "";
            treeView.visible = result;
            if (result){
                rightPanel.updateTreeViewGui();
            }
        }
    }
}
