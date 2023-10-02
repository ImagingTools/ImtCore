import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

DocumentBase {
    id: packageViewRoot;

    commandsDelegateSourceComp: packageViewCommandsDelegate;
    Component {
        id: packageViewCommandsDelegate;
        PackageViewCommandsDelegate {}
    }

    nameOutsideEditor: true;

    property int packageIndex: -1;
    property TreeItemModel featuresModel: TreeItemModel {
        Component.onCompleted: {
            packageViewRoot.featuresModel.dataChanged.connect(packageViewRoot.syncronise);
            packageViewRoot.featuresModel.dataChanged.connect(packageViewRoot.onFeatureModelChanged);
        }
    }

    property TreeItemModel treeViewModel: TreeItemModel {}
    property TreeItemModel headersModel: TreeItemModel {
        Component.onCompleted: {
            let index = packageViewRoot.headersModel.InsertNewItem();
            packageViewRoot.headersModel.SetData("Id", "Name", index);
            packageViewRoot.headersModel.SetData("Name", qsTr("Feature Name"), index);

            index = packageViewRoot.headersModel.InsertNewItem();
            packageViewRoot.headersModel.SetData("Id", "Id", index);
            packageViewRoot.headersModel.SetData("Name", qsTr("Feature-ID"), index);

            index = packageViewRoot.headersModel.InsertNewItem();
            packageViewRoot.headersModel.SetData("Id", "Description", index);
            packageViewRoot.headersModel.SetData("Name", qsTr("Description"), index);

            index = packageViewRoot.headersModel.InsertNewItem();
            packageViewRoot.headersModel.SetData("Id", "Optional", index);
            packageViewRoot.headersModel.SetData("Name", qsTr("Optional"), index);
        }
    }

    Component.onCompleted: {
        commandsDelegate.tableTreeViewEditor = tableView;
        if (featuresProvider && featuresProvider.onModelChanged){
            featuresProvider.onModelChanged.connect(packageViewRoot.updateTreeViewModel);
        }
    }

    Component.onDestruction: {
        if (featuresProvider && featuresProvider.onModelChanged){
            featuresProvider.onModelChanged.disconnect(packageViewRoot.updateTreeViewModel);
        }
    }

    onLocalizationChanged: {
        console.log("Package view onLocalizationChanged", language);
        for (let i = 0; i < packageViewRoot.headersModel.GetItemsCount(); i++){
            let headerId = packageViewRoot.headersModel.GetData("Id", i);
            if (headerId === "Name"){
                packageViewRoot.headersModel.SetData("Name", qsTr("Feature Name"), i);
            }
            else if (headerId === "Id"){
                packageViewRoot.headersModel.SetData("Name", qsTr("Feature-ID"), i);
            }
            else if (headerId === "Description"){
                packageViewRoot.headersModel.SetData("Name", qsTr("Description"), i);
            }
            else if (headerId === "Optional"){
                packageViewRoot.headersModel.SetData("Name", qsTr("Optional"), i);
            }
        }

        packageViewRoot.headersModel.Refresh();
    }

    onCommandsIdChanged: {
        if (packageViewRoot.itemId === ""){
            packageViewRoot.commandsProvider.setCommandIsEnabled("Save", true);
        }
    }

    onSaved: {
        featuresProvider.updateModel();
    }

    onFeaturesModelChanged: {
        packageViewRoot.featuresModel.dataChanged.connect(packageViewRoot.syncronise);
        packageViewRoot.featuresModel.dataChanged.connect(packageViewRoot.onFeatureModelChanged);
    }

    onModelIsReadyChanged: {
        if (packageViewRoot.modelIsReady){
            packageViewRoot.blockUpdatingModel = true;

            if (!packageViewRoot.documentModel.ContainsKey("Items")){
                packageViewRoot.documentModel.AddTreeModel("Items")
            }

            if (!packageViewRoot.documentModel.ContainsKey("DependenciesModel")){
                packageViewRoot.documentModel.AddTreeModel("DependenciesModel")
            }

            tableView.columnModel = packageViewRoot.headersModel;

            tableView.rowModel = packageViewRoot.featuresModel;
            tableView.rowModel.dataChanged.connect(packageViewRoot.syncronise);
            packageViewRoot.updateTreeViewModel();
            packageViewRoot.updateGui();

            packageViewRoot.blockUpdatingModel = false;
            undoRedoManager.registerModel(packageViewRoot.documentModel);
        }
    }

    onWidthChanged: {
        if (packageViewRoot.width > 0 && packageViewRoot.width - rightPanel.width > 250){
            splitter.x = packageViewRoot.width - rightPanel.width;
        }
    }

    UndoRedoManager {
        id: undoRedoManager;
        documentBase: packageViewRoot;
        onModelStateChanged: {
            console.log("onModelStateChanged");

            undoRedoManager.modelIsRegistered = false;
            packageViewRoot.updateGui();
            packageViewRoot.syncronise();
            undoRedoManager.modelIsRegistered = true;
        }
    }

    function onEntered(value){
        packageViewRoot.blockUpdatingModel = true;
        packageViewRoot.documentModel.SetData("Name", value);
        packageViewRoot.blockUpdatingModel = false;
    }

    function blockEditing(){
        treeView.readOnly = true;
    }

    function onFeatureModelChanged(){
        console.log("onFeatureModelChanged");
        packageViewRoot.updateModel();
    }

    //Обновить модель для TreeView
    function updateTreeViewModel(){
        console.log("updateTreeViewModel");
        if (!featuresProvider){
            return;
        }

        packageViewRoot.treeViewModel.Copy(featuresProvider.model);
        treeView.rowModel = packageViewRoot.treeViewModel;
        packageViewRoot.treeViewModel.Refresh();

        for (let i = 0; i < treeView.rowModel.GetItemsCount(); i++){
            let id =  treeView.rowModel.GetData("Id", i);
            if (packageViewRoot.itemId === id){
                packageViewRoot.packageIndex = i;
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
                    packageViewRoot.copyFrom(childModel, data);
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

        let items = packageViewRoot.documentModel.GetData("Items");
        if (packageViewRoot.packageIndex >= 0){
            let childModel = treeView.rowModel.AddTreeModel("ChildModel", packageViewRoot.packageIndex);
            packageViewRoot.copyFrom(childModel, items)
            childModel.Refresh();
        }

        rightPanel.updateTreeViewGui();

        packageViewRoot.updateModel();

        console.log("end syncronise");
    }

    function updateGui(){
        console.log("updateGui");
//        packageViewRoot.blockUpdatingModel = true;

        if (packageViewRoot.documentModel.ContainsKey("Items")){
            packageViewRoot.featuresModel = packageViewRoot.documentModel.GetData("Items");
        }

        tableView.rowModel = packageViewRoot.featuresModel;

        tableView.rowModel.Refresh();

        tableView.resetSelection();

        rightPanel.updateTreeViewGui();

//        packageViewRoot.blockUpdatingModel = false;
        console.log("end updateGui");

    }

    function updateModel(){
        console.log(" updateModel");

        undoRedoManager.beginChanges();

//        packageViewRoot.documentModel.SetData("Id", packageViewRoot.itemId);
        packageViewRoot.documentModel.SetData("Name", packageViewRoot.itemName);

        if (!packageViewRoot.documentModel.ContainsKey("Items")){
            packageViewRoot.featuresModel = packageViewRoot.documentModel.AddTreeModel("Items")
        }

        if (!packageViewRoot.documentModel.ContainsKey("DependenciesModel")){
            packageViewRoot.documentModel.AddTreeModel("DependenciesModel")
        }

        undoRedoManager.endChanges();

        console.log("end updateModel");
    }

    function updateDependenciesModel(featureId, checkState){
        console.log(" updateDependenciesModel");

        undoRedoManager.beginChanges();

        let dependenciesModel = packageViewRoot.documentModel.GetData("DependenciesModel");

        if (!dependenciesModel){
            dependenciesModel = packageViewRoot.documentModel.AddTreeModel("DependenciesModel");
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
            packageViewRoot.documentManager.openErrorDialog(message);
        }

        function canRename(id){
            if (packageViewRoot.featureHasDependencies(id)){
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

            if (!featuresProvider){
                return false;
            }

            let globalExists = featuresProvider.featureIsExists(featureId);

            return globalExists;
        }
    }

    function featureHasDependencies(featureId){
        console.log("featureHasDependencies", featureId);

        let dependenciesModel = packageViewRoot.documentModel.GetData("DependenciesModel");
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
        x: packageViewRoot.width - 250;

        height: parent.height;
        width: 4;

        onXChanged: {
            if (!packageViewRoot.visible){
                return;
            }

            if (splitter.x <= 250){
                splitter.x = 250;
            } else if (splitter.x > packageViewRoot.width - titleHeader.width){
                splitter.x = packageViewRoot.width - splitter.width;
            }
        }
    }

    Column {
        id: rightPanel;

        anchors.left: splitter.right;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;

        width: packageViewRoot.width > 0 ? packageViewRoot.width - tableView.width : 250;

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

            let dependenciesModel = packageViewRoot.documentModel.GetData("DependenciesModel");
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

                if (packageViewRoot.isPackage(itemId) && delegateItem.level === 0){
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
            let dependenciesModel = packageViewRoot.documentModel.GetData("DependenciesModel");

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
            let dependenciesModel = packageViewRoot.documentModel.GetData("DependenciesModel");

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

            visible: packageViewRoot.itemId === "";
        }

        BasicTreeView {
            id: treeView;

            anchors.top: headerTreeView.bottom;
            anchors.bottom: parent.bottom;
            anchors.left: parent.left;
            anchors.right: parent.right;
            anchors.rightMargin: 5;

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
                    console.log("onCheckStateChanged", packageViewRoot);
                    if (tableView.selectedIndex != null){
                        if (!treeView.delegateUpdatingBlock){

                            if (delegate.itemData.Id !== ""){
                                packageViewRoot.updateDependenciesModel(delegate.itemData.Id, delegate.checkState);
                                rightPanel.updateTreeViewGui();
                            }
                        }
                    }
                }
            }
        } }

        function selectedIndexChanged(){
            console.log("selectedIndexChanged", tableView.selectedIndex);

            let result = tableView.selectedIndex != null && tableView.selectedIndex.itemData.Id !== "" && packageViewRoot.itemId !== "";
            treeView.visible = result;
            if (result){
                rightPanel.updateTreeViewGui();
            }
        }
    }
}
