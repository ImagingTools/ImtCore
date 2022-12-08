import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

DocumentBase {
    id: packageViewContainer;

    commandsDelegateSourceComp: PackageViewCommandsDelegate {}

    property bool blockUpdatingModel: false;

    property TreeItemModel treeViewModel: TreeItemModel {}

    Component.onCompleted: {
        console.log("PackageView onCompleted");
        updateTreeViewModel();

        featuresProvider.modelChanged.connect(updateTreeViewModel);

        commandsDelegate.tableTreeViewEditor = tableView;
    }

    Component.onDestruction: {
        featuresProvider.modelChanged.disconnect(updateTreeViewModel);
    }

    onDocumentModelChanged: {
        console.log("documentBase onDocumentModelChanged");

        let items = documentModel.GetData("Items");
        if (!items){
            documentModel.AddTreeModel("Items");
        }

        let dependenciesModel = documentModel.GetData("DependenciesModel");
        if (!dependenciesModel){
            documentModel.AddTreeModel("DependenciesModel");
        }

        tableView.columnModel.clear();
        let headers = documentModel.GetData("Headers");

        for (let i = 0; i < headers.GetItemsCount(); i++){
            let headerId = headers.GetData("Id", i);
            let headerName = headers.GetData("Name", i);

            tableView.addColumn({"Id": headerId, "Name": headerName});
        }

        syncronise();

        updateGui();

        undoRedoManager.registerModel(documentModel);
    }

    onWidthChanged: {
        if (packageViewContainer.width > 0 && packageViewContainer.width - rightPanel.width > 250){
            splitter.x = packageViewContainer.width - rightPanel.width;
        }
    }

    UndoRedoManager {
        id: undoRedoManager;

        commandsId: packageViewContainer.commandsId;

        onModelStateChanged: {
            console.log("UndoRedoManager onModelStateChanged");
            syncronise();

            updateGui();
        }
    }

    //Обновить модель для TreeView
    function updateTreeViewModel(){
        let treeViewModelJson = featuresProvider.model.toJSON();
        treeViewModel.CreateFromJson(treeViewModelJson);
    }

    //Синхронизация фич в package и treeView
    function syncronise(){
        console.log("syncronise");
        let items = documentModel.GetData("Items");

        let packageIndex = -1;
        for (let i = 0; i < packageViewContainer.treeViewModel.GetItemsCount(); i++){
            let id = packageViewContainer.treeViewModel.GetData("Id", i);
            if (packageViewContainer.itemId == id){
                packageIndex = i;
                break;
            }
        }

        if (packageIndex >= 0){
            packageViewContainer.treeViewModel.SetData("ChildModel", items, packageIndex);
        }
    }

    function updateGui(){
        console.log("Begin updateGui");
        blockUpdatingModel = true;

        tableView.rowModel.clear();

        let items = documentModel.GetData("Items");
        if (!items){
            items = documentModel.AddTreeModel("Items");
        }

        if (items.GetItemsCount() > 0){
            recursiveUpdateGui(items, []);
        }

        blockUpdatingModel = false;
        console.log("End updateGui");
    }

    function recursiveUpdateGui(documentModel, indexes){
        console.log("recursiveUpdateGui");
        for (let i = 0; i < documentModel.GetItemsCount(); i++){
            let rowId = documentModel.GetData("Id", i);
            let rowName = documentModel.GetData("Name", i);
            let description = documentModel.GetData("Description", i);
            let optional = documentModel.GetData("Optional", i);

            tableView.insertRow(indexes.concat([i]), {"Id": rowId, "Name": rowName, "Description": description, "Optional": optional});

            let childModel = documentModel.GetData("ChildModel", i);
            if (childModel){
                let childIndexes = []
                childIndexes = childIndexes.concat(indexes.concat([i]))
                recursiveUpdateGui(childModel, childIndexes);
            }
        }
    }

    function updateModel(){
        console.log("updateModel");
        undoRedoManager.beginChanges();

        let items = documentModel.AddTreeModel("Items")
        recursiveUpdateModel(tableView.rowModel, items);

        syncronise();

        console.log("documentModel", documentModel.toJSON());

        undoRedoManager.endChanges();
    }

    function recursiveUpdateModel(guiModel, documentModel){
        console.log("recursiveUpdateModel");
        for (let i = 0; i < guiModel.count; i++){

            let index = documentModel.InsertNewItem();

            let rowObj = guiModel.get(i);

            let rowId = rowObj["Id"];
            let rowName = rowObj["Name"];
            let description = rowObj["Description"];
            let optional = rowObj["Optional"];

            documentModel.SetData("Id", rowId, index);
            documentModel.SetData("Name", rowName, index);
            documentModel.SetData("Description", description, index);
            documentModel.SetData("Optional", optional, index);

            let rowChildModel = rowObj["ChildModel"]

            if (rowChildModel.count > 0){
                let documentChildModel = documentModel.AddTreeModel("ChildModel", index);
                recursiveUpdateModel(rowChildModel, documentChildModel);
            }
        }
    }

    function updateDependenciesModel(featureId, checkState){
        console.log("updateDependenciesModel");
        undoRedoManager.beginChanges();

        let dependenciesModel = documentModel.GetData("DependenciesModel");

        if (!dependenciesModel){
            dependenciesModel = documentModel.AddTreeModel("DependenciesModel");
        }

        let selectedId = tableView.selectedIndex.itemData.Id;
        let dependencyId = featureId;

        if (dependenciesModel.ContainsKey(selectedId)){
            let dependencies = dependenciesModel.GetData(selectedId);

            if (dependencies != ""){
                let dependenciesList = dependencies.split(';')

                let pos = dependenciesList.indexOf(dependencyId);
                if (checkState == Qt.Checked){
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

                if (newDependencies.length == 0){
                    dependenciesModel.RemoveData(selectedId)
                }
                else{
                    dependenciesModel.SetData(selectedId, newDependencies);
                }
            }
        }
        else{
            if (checkState == Qt.Checked){
                dependenciesModel.SetData(selectedId, dependencyId);
            }
        }

        undoRedoManager.endChanges();
    }

    BasicTreeView {
        id: tableView;

        anchors.left: parent.left;
        anchors.right: splitter.left;

        height: parent.height;

        readOnly: false;

        rowDelegate: PackageViewItemDelegate {}

        onSelectedIndexChanged: {
            console.log("TableView Begin onSelectedIndexChanged");
            let packageId = documentModel.GetData("Id");
            console.log("packageId", packageId);
            if (selectedIndex != null && selectedIndex.itemData.Id != "" && packageId != ""){
                treeView.visible = true;
                rightPanel.updateTreeViewGui();
            }
            else{
                treeView.visible = false;
            }

            console.log("TableView End onSelectedIndexChanged");
        }

        onRowModelDataChanged: {
            console.log("TableView onRowModelDataChanged", delegate, prop);

            if (!blockUpdatingModel){
                updateModel();
            }
        }

        onRowAdded: {
            console.log("onRowAdded");
            if (!blockUpdatingModel){
                updateModel();
            }
        }

        onRowRemoved: {
            console.log("onRowRemoved");
            if (!blockUpdatingModel){
                updateModel();
            }
        }
    }

    Splitter {
        id: splitter;
        x: packageViewContainer.width - 250;

        height: parent.height;
        width: 4;

        onXChanged: {
            if (!packageViewContainer.visible){
                return;
            }

            if (splitter.x > packageViewContainer.width - titleHeader.width){
                splitter.x = packageViewContainer.width - splitter.width;
            }

            if (splitter.x < 250){
                splitter.x = 250;
            }
        }
    }

    Column {
        id: rightPanel;

        anchors.left: splitter.right;

        width: packageViewContainer.width > 0 ? packageViewContainer.width - tableView.width : 250;
        height: parent.height;

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

                text: qsTr("Dependencies");
                color: Style.textColor;
                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamilyBold;
                font.bold: true;
            }
        }

        function updateTreeViewGui(){
            console.log("Begin updateTreeViewGui", tableView.selectedIndex);
            if (tableView.selectedIndex == null){
                return;
            }

            console.log("rowModel begin clear");
            treeView.rowModel.clear();

            console.log("rowModel end clear");

            let selectedId = tableView.selectedIndex.itemData.Id;

            //Список всех зависящих фич для selectedId
            let childrenFeatureList = []
            findChildrenFeatureDependencies(selectedId, childrenFeatureList);

            //Список всех зависящих фич от selectedId
            let inactiveElements = []
            findParentFeatureDependencies(selectedId, inactiveElements)

            //Запрещаем зависимость от всех родителей
            let parentIds = getAllParents(tableView.selectedIndex);
            inactiveElements = inactiveElements.concat(parentIds)


            for (let i = 0; i < parentIds.length; i++){
                let parentId = parentIds[i];

                //Запрещаем зависимость для всех фич которые зависят от родителей
                findParentFeatureDependencies(parentId, inactiveElements)

                //Автоматом выбираем фичи от которых зависят родители
                findChildrenFeatureDependencies(parentId, childrenFeatureList);
            }

            for (let i = 0; i < treeViewModel.GetItemsCount(); i++){
                let id = treeViewModel.GetData("Id", i);
                let name = treeViewModel.GetData("Name", i);

                let childModel = treeViewModel.GetData("ChildModel", i);

                treeView.insertRow([i], {"Name": name, "Id": id, "CheckBoxVisible": false})
                console.log("insertRow");

                if (childModel){
                    rightPanel.recursiveUpdateGui(childModel, [i], inactiveElements, childrenFeatureList)
                }
            }
            console.log("End updateTreeViewGui");
        }

        function recursiveUpdateGui(model, indexes, inactiveElements, childrenFeatureList){
            console.log("recursiveUpdateTreeGui");
            for (let i = 0; i < model.GetItemsCount(); i++){
                let name = model.GetData("Name", i);
                let id = model.GetData("Id", i);

                let rowObj = {"Name": name, "Id": id}

                if (tableView.selectedIndex != null){

                    if (id == ""){
                        rowObj["Active"] = false;
                    }

                    let selectedId = tableView.selectedIndex.itemData.Id;

                    if (selectedId == id){
                        rowObj["Visible"] = false;
                    }

                    //Список основных зависящих фич для selectedId
                    let dependenciesList = []

                    let dependenciesModel = documentModel.GetData("DependenciesModel");
                    if (dependenciesModel && dependenciesModel.ContainsKey(selectedId)){
                        let dependencies = dependenciesModel.GetData(selectedId);
                        if (dependencies != ""){
                            dependenciesList = dependencies.split(';');
                        }
                    }

                    if (inactiveElements.includes(id)){
                        rowObj["Active"] = false;
                    }

                    //Если содержится во всех фичах и не содержится в основном
                    if (childrenFeatureList.includes(id) && !dependenciesList.includes(id)){
                        rowObj["Active"] = false;
                        rowObj["CheckState"] = Qt.Checked;
                    }
                    //Если содержится во всех фичах и содержится в основном
                    else if (childrenFeatureList.includes(id) && dependenciesList.includes(id)){
                        rowObj["Active"] = true;
                        rowObj["CheckState"] = Qt.Checked;
                    }
                }

                treeView.insertRow(indexes.concat([i]), rowObj)

                console.log("insertRow");

                let childModel = model.GetData("ChildModel", i);
                if (childModel){
                    let childIndexes = []
                    childIndexes = childIndexes.concat(indexes.concat([i]))
                    rightPanel.recursiveUpdateGui(childModel, childIndexes, inactiveElements, childrenFeatureList)
                }
            }
        }

        function getAllParents(selectedIndex){
            console.log("getAllParents", selectedIndex);
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
            console.log("findParentFeatureDependencies");
            let dependenciesModel = documentModel.GetData("DependenciesModel");

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
                        findParentFeatureDependencies(key, retVal)
                    }
                }
            }
        }

        function findChildrenFeatureDependencies(featureId, retVal){
            console.log("findChildrenFeatureDependencies");
            let dependenciesModel = documentModel.GetData("DependenciesModel");

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

                        findChildrenFeatureDependencies(value, retVal);
                    }
                }
            }
        }

        BasicTreeView {
            id: treeView;

            height: packageViewContainer.height;
            width: parent.width;

            clip: true;

            headerVisible: false;

            tristate: true;

            visible: false;

            Component.onCompleted: {
                addColumn({"Id": "Name", "Name": "Name"});

                rightPanel.updateTreeViewGui();
            }

            onRowModelDataChanged: {
                console.log("onRowModelDataChanged", delegate, prop);

                if (prop == "CheckState"){
                    updateDependenciesModel(delegate.itemData.Id, delegate.itemData.CheckState);

                    rightPanel.updateTreeViewGui();
                }
            }
        }
    }
}
