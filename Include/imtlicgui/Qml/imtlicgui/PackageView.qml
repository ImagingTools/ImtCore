import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

//DocumentBase {
//    id: packageViewContainer;

//    TreeItemModel {
//        id: testModel;

//        Component.onCompleted: {
//            for (let i = 0; i < 1000; i++){
//                testModel.InsertNewItem();

//                testModel.SetData("Id", "Test" + i, i);
//                testModel.SetData("Name", "Test" + i, i);
//                testModel.SetData("Description", "Test" + i, i);
//            }

////            listView.model = testModel;
//            table.elements = testModel;
//        }
//    }
////    ListModel {
////        id: listModel;

////        Component.onCompleted: {
////            for (let i = 0; i < 1000; i++){
////                listModel.append({"Id": "Test" + i, "Name": "Test" + i, "Description": "Test" + i})
////            }

////            table.elements = listModel;
////        }
////    }

//    TreeItemModel {
//        id: headerModel;

//        Component.onCompleted: {
//            let index = headerModel.InsertNewItem();

//            headerModel.SetData("Id", "Id", index);
//            headerModel.SetData("Name", "Id", index);

//            index = headerModel.InsertNewItem();

//            headerModel.SetData("Id", "Name", index);
//            headerModel.SetData("Name", "Name", index);

//            index = headerModel.InsertNewItem();

//            headerModel.SetData("Id", "Description", index);
//            headerModel.SetData("Name", "Description", index);

//            table.headers = headerModel;
//        }
//    }

//    AuxTable {
//        id: table;

//        anchors.fill: parent;

////        delegate: delegateRect;

////        cellDelegateComp: Rectangle {
////            width: 50;
////            height: 20;

////            color: "red";

////            border.color: "black";
////        }

////        delegate: Item {
////            width: parent.width;
////            height: 20;

////            Row {
////                height: 20;

////                Text {
////                    id: text1;
////                    text: model.Id;
////                }
////                Text {
////                    id: text2;
////                    text: model.Name;
////                }

////                Text {
////                    id: text3;
////                    text: model.Description;
////                }
////            }
////        }
//    }

//    Rectangle {
//        id: delegateRect;

//        width: table.width;
//        height: 35;

//        color: "transparent";

//        ListView {
//            id: dataList;

//            anchors.left: checkBox.visible ? checkBox.right : parent.left;
//            anchors.right: parent.right;
//            anchors.top: parent.top;
//            anchors.bottom: parent.bottom;

//            clip: true;
//            orientation: ListView.Horizontal;
//            spacing: 0;
//            property bool compl: false;

//            model: 3;

//            delegate: TableCellDelegate {
//                id: tableCellDelegate
//                pTableDelegateContainer: delegateRect;
//                contentComp: table.columnContentComps[model.index] !== null ? table.columnContentComps[model.index] : tableCellDelegate.defaultContentComp;
////                rowIndex: tableDelegateContainer.rowIndex;
//            }//delegate
//        }//dataList
//    }

////    ListView {
////        id: listView;

////        anchors.fill: parent;

////        delegate: Item {
////            width: parent.width;
////            height: 20;

////            Row {
////                height: 20;

////                Text {
////                    id: text1;
////                    text: model.Id;
////                }
////                Text {
////                    id: text2;
////                    text: model.Name;
////                }

////                Text {
////                    id: text3;
////                    text: model.Description;
////                }
////            }
////        }
////    }
//}

DocumentBase {
    id: packageViewContainer;

    commandsDelegateSourceComp: packageViewCommandsDelegate;

    nameOutsideEditor: true;

    property TreeItemModel treeViewModel: TreeItemModel {}

    Component {
        id: packageViewCommandsDelegate;
        PackageViewCommandsDelegate {}
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }

    Component.onCompleted: {
        packageViewContainer.updateTreeViewModel();

//        featuresProvider.modelChanged.connect(packageViewContainer.updateTreeViewModel);

        commandsDelegate.tableTreeViewEditor = tableView;
    }

    Component.onDestruction: {
//        featuresProvider.modelChanged.disconnect(packageViewContainer.updateTreeViewModel);
    }

    onDocumentModelChanged: {
        tableView.columnModel.clear();
        let headers = packageViewContainer.documentModel.GetData("Headers");
        for (let i = 0; i < headers.GetItemsCount(); i++){
            let headerId = headers.GetData("Id", i);
            let headerName = headers.GetData("Name", i);
            tableView.addColumn({"Id": headerId, "Name": headerName});
        }

        packageViewContainer.syncronise();

        packageViewContainer.updateGui();

        undoRedoManager.registerModel(packageViewContainer.documentModel);
    }

    onWidthChanged: {
        if (packageViewContainer.width > 0 && packageViewContainer.width - rightPanel.width > 250){
            splitter.x = packageViewContainer.width - rightPanel.width;
        }
    }

    UndoRedoManager {
        id: undoRedoManager;

        commandsId: packageViewContainer.documentUuid;
        commandsDelegate: packageViewContainer.commandsDelegate;
        documentBase: packageViewContainer;
        onModelStateChanged: {
            packageViewContainer.syncronise();

            packageViewContainer.updateGui();
        }
    }

    TreeItemModelConverter {
        id: converter;
    }

    //Обновить модель для TreeView
    function updateTreeViewModel(){
        console.log("updateTreeViewModel", featuresProvider.model);
//        let treeViewModelJson = featuresProvider.model.toJSON();

//        console.log("treeViewModelJson", treeViewModelJson);
//        packageViewContainer.treeViewModel.CreateFromJson(treeViewModelJson);
        let listModel = converter.convertToListModel(featuresProvider.model);

        treeView.rowModel = listModel;
    }

    //Синхронизация фич в package и treeView
    function syncronise(){
        let items = packageViewContainer.documentModel.GetData("Items");

        let itemsListModel = converter.convertToListModel(items);

        let dataList = treeView.getItemsDataAsList();

        let packageIndex = -1;
        for (let i = 0; i < treeView.rowModel.count; i++){
            let id =  treeView.rowModel.get(i).Id;
            if (packageViewContainer.itemId === id){
                packageIndex = i;
                break;
            }
        }

        if (packageIndex >= 0){
            treeView.rowModel.remove(packageIndex);
            let rowObj = {"Name": packageViewContainer.itemName,
                        "Id": packageViewContainer.itemId,
                        "ChildModel": itemsListModel};
            treeView.rowModel.insert(packageIndex, rowObj)
        }

//        let items = packageViewContainer.documentModel.GetData("Items");

//        let packageIndex = -1;
//        for (let i = 0; i < packageViewContainer.treeViewModel.GetItemsCount(); i++){
//            let id = packageViewContainer.treeViewModel.GetData("Id", i);
//            if (packageViewContainer.itemId == id){
//                packageIndex = i;
//                break;
//            }
//        }

//        if (packageIndex >= 0){
//            packageViewContainer.treeViewModel.SetData("ChildModel", items, packageIndex);
//        }
    }

    function updateGui(){
        packageViewContainer.blockUpdatingModel = true;

        tableView.rowModel.clear();

        if (packageViewContainer.documentModel.ContainsKey("Items")){
            let items = packageViewContainer.documentModel.GetData("Items");

            if (items.GetItemsCount() > 0){
                packageViewContainer.recursiveUpdateGui(items, []);
            }
        }

        packageViewContainer.blockUpdatingModel = false;
    }

    function recursiveUpdateGui(documentModel, indexes){
        for (let i = 0; i < documentModel.GetItemsCount(); i++){
            let rowId = documentModel.GetData("Id", i);
            let rowName = documentModel.GetData("Name", i);
            let description = documentModel.GetData("Description", i);
            let optional = documentModel.GetData("Optional", i);

            tableView.insertRow(indexes.concat([i]), {"Id": rowId, "Name": rowName, "Description": description, "Optional": optional});

            let childModel = documentModel.GetData("ChildModel", i);

            if (childModel){
                let childIndexes = [].concat(indexes.concat([i]))
                packageViewContainer.recursiveUpdateGui(childModel, childIndexes);
            }
        }
    }

    function updateModel(){
        undoRedoManager.beginChanges();

        let items = packageViewContainer.documentModel.AddTreeModel("Items")
        packageViewContainer.recursiveUpdateModel(tableView.rowModel, items);

        packageViewContainer.syncronise();

        undoRedoManager.endChanges();
    }

    function recursiveUpdateModel(guiModel, documentModel){
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
                packageViewContainer.recursiveUpdateModel(rowChildModel, documentChildModel);
            }
        }
    }

    function updateDependenciesModel(featureId, checkState){
        undoRedoManager.beginChanges();

        let dependenciesModel = packageViewContainer.documentModel.GetData("DependenciesModel");

        if (!dependenciesModel){
            dependenciesModel = packageViewContainer.documentModel.AddTreeModel("DependenciesModel");
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

    CustomScrollbar {
        id: scrollbar;

        z: 100;

        anchors.right: tableView.right;
        anchors.bottom: tableView.bottom;

        backgroundColor: Style.baseColor;

        secondSize: 10;
        targetItem: tableView.tableListView;
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

    BasicTreeView {
        id: tableView;

        anchors.top: parent.top;
        anchors.bottom: parent.bottom;

        anchors.left: parent.left;
        anchors.right: splitter.left;

        readOnly: false;

        commandId: packageViewContainer.commandsId;

        rowDelegate: Component { PackageViewItemDelegate { root: tableView; } }

        onSelectedIndexChanged: {
        }

        onRowModelDataChanged: {

            if (!packageViewContainer.blockUpdatingModel){
                packageViewContainer.updateModel();
            }
        }

        onRowAdded: {
            if (!packageViewContainer.blockUpdatingModel){
                packageViewContainer.updateModel();
            }
        }

        onRowRemoved: {
            if (!packageViewContainer.blockUpdatingModel){
                packageViewContainer.updateModel();
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

            if (splitter.x <= 250){
                splitter.x = 250;
            } else if (splitter.x > packageViewContainer.width - titleHeader.width){
                splitter.x = packageViewContainer.width - splitter.width;
            }
        }
    }

    Column {
        id: rightPanel;

        anchors.left: splitter.right;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;

        width: packageViewContainer.width > 0 ? packageViewContainer.width - tableView.width : 250;

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
            if (tableView.selectedIndex == null){
                return;
            }

            let selectedId = tableView.selectedIndex.itemData.Id;

            //Список всех зависящих фич для selectedId
            let childrenFeatureList = [];
            rightPanel.findChildrenFeatureDependencies(selectedId, childrenFeatureList);

            //Список всех зависящих фич от selectedId
            let inactiveElements = [];
            rightPanel.findParentFeatureDependencies(selectedId, inactiveElements);

            //Запрещаем зависимость от всех родителей
            let parentIds = getAllParents(tableView.selectedIndex);
            inactiveElements = inactiveElements.concat(parentIds)

            for (let i = 0; i < parentIds.length; i++){
                let parentId = parentIds[i];

                //Запрещаем зависимость для всех фич которые зависят от родителей
                rightPanel.findParentFeatureDependencies(parentId, inactiveElements);

                //Автоматом выбираем фичи от которых зависят родители
                rightPanel.findChildrenFeatureDependencies(parentId, childrenFeatureList);
            }


            //Список основных зависящих фич для selectedId
            let dependenciesList = []

            let dependenciesModel = packageViewContainer.documentModel.GetData("DependenciesModel");
            if (dependenciesModel && dependenciesModel.ContainsKey(selectedId)){
                let dependencies = dependenciesModel.GetData(selectedId);
                if (dependencies != ""){
                    dependenciesList = dependencies.split(';');
                }
            }

            let itemsDataList = treeView.getItemsDataAsList();
            for (let i = 0; i < itemsDataList.length; i++){
                let itemData = itemsDataList[i];
                let itemId = itemData.Id;

                itemData.Visible = itemId !== selectedId;
                itemData.Active = !inactiveElements.includes(itemId);
                itemData.CheckBoxVisible = !packageViewContainer.isPackage(itemId);
                itemData.CheckState = Qt.Unchecked;

                if (childrenFeatureList.includes(itemId) && !dependenciesList.includes(itemId)){
                    itemData.Active = false;
                    itemData.CheckState = Qt.Checked;
                }
                //Если содержится во всех фичах и содержится в основном
                else if (childrenFeatureList.includes(itemId) && dependenciesList.includes(itemId)){
                    itemData.Active = true;
                    itemData.CheckState = Qt.Checked;
                }
            }
        }

        function updateTreeViewGui2(){
            if (tableView.selectedIndex == null){
                return;
            }

            treeView.rowModel.clear();

            let selectedId = tableView.selectedIndex.itemData.Id;

            //Список всех зависящих фич для selectedId
            let childrenFeatureList = [];
            rightPanel.findChildrenFeatureDependencies(selectedId, childrenFeatureList);

            //Список всех зависящих фич от selectedId
            let inactiveElements = [];
            rightPanel.findParentFeatureDependencies(selectedId, inactiveElements);

            //Запрещаем зависимость от всех родителей
            let parentIds = getAllParents(tableView.selectedIndex);
            inactiveElements = inactiveElements.concat(parentIds)

            for (let i = 0; i < parentIds.length; i++){
                let parentId = parentIds[i];

                //Запрещаем зависимость для всех фич которые зависят от родителей
                rightPanel.findParentFeatureDependencies(parentId, inactiveElements);

                //Автоматом выбираем фичи от которых зависят родители
                rightPanel.findChildrenFeatureDependencies(parentId, childrenFeatureList);
            }

            for (let i = 0; i < packageViewContainer.treeViewModel.GetItemsCount(); i++){
                let id = packageViewContainer.treeViewModel.GetData("Id", i);
                let name = packageViewContainer.treeViewModel.GetData("Name", i);

                let childModel = packageViewContainer.treeViewModel.GetData("ChildModel", i);

                treeView.insertRow([i], {"Name": name, "Id": id, "CheckBoxVisible": false})

                if (childModel){
                    rightPanel.recursiveUpdateGui(childModel, [i], inactiveElements, childrenFeatureList);
                }
            }
        }

        function recursiveUpdateGui(model, indexes, inactiveElements, childrenFeatureList){
            for (let i = 0; i < model.GetItemsCount(); i++){
                let name = model.GetData("Name", i);
                let id = model.GetData("Id", i);

                let rowObj = {"Name": name, "Id": id};

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

                    let dependenciesModel = packageViewContainer.documentModel.GetData("DependenciesModel");
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

                let childModel = model.GetData("ChildModel", i);
                if (childModel){
                    let childIndexes = []
                    childIndexes = childIndexes.concat(indexes.concat([i]))
                    rightPanel.recursiveUpdateGui(childModel, childIndexes, inactiveElements, childrenFeatureList)
                }
            }
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
            let dependenciesModel = packageViewContainer.documentModel.GetData("DependenciesModel");

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
            let dependenciesModel = packageViewContainer.documentModel.GetData("DependenciesModel");

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

        //        TreeViewDependencies {
        //            id: treeView;

        //            height: packageViewContainer.height;
        //            width: parent.width;

        //            documentModel: packageViewContainer.documentModel;

        //            featuresTableView: tableView;
        //        }

//        CustomScrollbar {
//            z: 100;

//            anchors.right: treeView.right;
//            anchors.bottom: treeView.bottom;

//            backgroundColor: Style.baseColor;

//            secondSize: 10;
//            targetItem: treeView.tableListView;
//        }

        BasicTreeView {
            id: treeView;

            anchors.top: headerTreeView.bottom;
            anchors.bottom: parent.bottom;

            width: parent.width;

            clip: true;

            headerVisible: false;

            tristate: true;

            visible: false;

            Component.onCompleted: {
                treeView.addColumn({"Id": "Name", "Name": "Name"});

                rightPanel.updateTreeViewGui();

                tableView.selectedIndexChanged.connect(rightPanel.selectedIndexChanged);
            }

            onRowModelDataChanged: {
                if (prop == "CheckState"){
                    packageViewContainer.updateDependenciesModel(delegate.itemData.Id, delegate.itemData.CheckState);

                    rightPanel.updateTreeViewGui();
                }
            }
        }

        function selectedIndexChanged(){
            let result = tableView.selectedIndex != null && tableView.selectedIndex.itemData && tableView.selectedIndex.itemData.Id != "" && packageViewContainer.itemId != "";
            treeView.visible = result;

            if (result){
                rightPanel.updateTreeViewGui();
            }
        }
    }
}
