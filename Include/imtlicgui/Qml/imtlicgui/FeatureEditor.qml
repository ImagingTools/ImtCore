import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

DocumentBase {
    id: featureEditor;

    undoManager: undoRedoManager;

    property TreeItemModel dependenciewViewModel: TreeItemModel {}

    property Component treeItemModelComp: Component {
        TreeItemModel {}
    }

    onModelIsReadyChanged: {
        updateGui();
        undoRedoManager.registerModel(documentModel);
    }

    UndoRedoManager {
        id: undoRedoManager;
        documentBase: featureEditor;
        onModelStateChanged: {
            featureEditor.updateGui();
        }
    }

    onSaved: {
        FeaturesProvider.updateModel();
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
                    retVal.push(id);

                    findParentFeatureDependencies(id, retVal);
                }
            }
        }
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
                        retVal.push(dependencyId)

                        findChildrenFeatureDependencies(dependencyId, retVal);
                    }
                }
            }
        }
    }

    function getAllParents(selectedIndex){
        let retVal = []

        if (selectedIndex != null){
            let parent = selectedIndex.parentIndex;

            while (parent){
                let parentId = parent.itemData.FeatureId
                retVal.push(parentId);
                parent = parent.parentIndex;
            }
        }

        return retVal;
    }

    function updateTreeViewGui(){
        console.log("updateTreeViewGui");
        if (tableView.selectedIndex == null || !tableView.selectedIndex.itemData){
            return;
        }

        featureDependenciesView.delegateUpdatingBlock = true;
        let selectedId = "";

        if (tableView.selectedIndex != null && tableView.selectedIndex.itemData){
            selectedId = tableView.selectedIndex.itemData.FeatureId;
        }

        //Список всех зависящих фич для selectedId
        let childrenFeatureList = [];
        featureEditor.findChildrenFeatureDependencies(selectedId, childrenFeatureList);

        //Список всех зависящих фич от selectedId
        let inactiveElements = [];
        featureEditor.findParentFeatureDependencies(selectedId, inactiveElements);

        //Запрещаем зависимость от всех родителей
        let parentIds = featureEditor.getAllParents(tableView.selectedIndex);
        inactiveElements = inactiveElements.concat(parentIds);

        for (let i = 0; i < parentIds.length; i++){
            let parentId = parentIds[i];
            //Запрещаем зависимость для всех фич которые зависят от родителей
            featureEditor.findParentFeatureDependencies(parentId, inactiveElements);
            //Автоматом выбираем фичи от которых зависят родители
            featureEditor.findChildrenFeatureDependencies(parentId, childrenFeatureList);
        }

        //Список основных зависящих фич для selectedId
        let dependenciesList = []

        let dependencies = tableView.selectedIndex.itemData.Dependencies;
        if (dependencies && dependencies !== ""){
            dependenciesList = dependencies.split(';');
        }

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
        console.log("updateGui");

        blockUpdatingModel = true;

        if (documentModel.ContainsKey("FeatureId")){
            featureIdInput.text = documentModel.GetData("FeatureId");
        }

        if (documentModel.ContainsKey("Name")){
            featureNameInput.text = documentModel.GetData("Name");
        }

        if (documentModel.ContainsKey("Description")){
            descriptionInput.text = documentModel.GetData("Description");
        }

        if (documentModel.ContainsKey("ChildModel")){
            tableView.rowModel = documentModel.GetData("ChildModel");
        }

        featureEditor.updateTreeViewGui();

        blockUpdatingModel = false;
    }

    function updateModel(){
        console.log("updateModel");

        if (blockUpdatingModel){
            return;
        }

        undoRedoManager.beginChanges();

        documentModel.SetData("FeatureId", featureIdInput.text);
        documentModel.SetData("Name", featureNameInput.text);
        documentModel.SetData("Description", descriptionInput.text);

        undoRedoManager.endChanges();
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }

    Item {
        id: mainHeader;

        anchors.top: parent.top;

        width: column.width;
        height: 30;

        Text {
            id: title;

            anchors.left: parent.left;
            anchors.verticalCenter: parent.verticalCenter;

            color: Style.textColor;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSize_common;

            text: qsTr("Feature Info");
        }
    }

    Rectangle {
        id: column;

        anchors.top: mainHeader.bottom;

        width: 300;
        height: featureInformationBlock.height + 25;

        color: "transparent";

        border.width: 1;
        border.color: Style.borderColor;

        Column {
            id: featureInformationBlock;

            anchors.centerIn: column;

            width: parent.width - 20;

            spacing: 7;

            Text {
                id: titleFeatureName;

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;

                text: qsTr("Feature Name");
            }

            CustomTextField {
                id: featureNameInput;

                width: parent.width;
                height: 30;

                placeHolderText: qsTr("Enter the feature name");

                onEditingFinished: {
                    let old = documentModel.GetData("Name");
                    if (old !== featureNameInput.text){
                        featureEditor.updateModel();
                    }

                    if (featureIdInput.text === ""){
                        let id = featureNameInput.text.replace(/\s+/g, '');

                        featureIdInput.text = id;
                    }
                }

                KeyNavigation.tab: featureIdInput;

            }

            Text {
                id: titleFeatureId;

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;

                text: qsTr("Feature-ID");
            }

            CustomTextField {
                id: featureIdInput;

                width: parent.width;
                height: 30;

                placeHolderText: qsTr("Enter the feature ID");

                onEditingFinished: {
                    let oldFeatureId = documentModel.GetData("FeatureId");
                    if (oldFeatureId !== featureIdInput.text){
                        featureEditor.updateModel();
                    }
                }

                KeyNavigation.tab: descriptionInput;
            }

            Text {
                id: titleDescription;

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;

                text: qsTr("Description");
            }

            CustomTextField {
                id: descriptionInput;

                width: parent.width;
                height: 30;

                placeHolderText: qsTr("Enter the description");

                onEditingFinished: {
                    let old = documentModel.GetData("Description");
                    if (old !== descriptionInput.text){
                        featureEditor.updateModel();
                    }
                }

                KeyNavigation.tab: featureNameInput;
            }
        }
    }

    Item {
        id: centerPanel;

        anchors.top: parent.top;
        anchors.left: column.right;
        anchors.right: rightBlock.left;
        anchors.rightMargin: 20;

        height: parent.height;

        Item {
            id: headerBlock;

            anchors.top: parent.top;
            anchors.left: parent.left;
            anchors.leftMargin: 10;

            width: parent.width;
            height: 30;

            Text {
                id: titleTableView;

                anchors.left: parent.left;
                anchors.verticalCenter: parent.verticalCenter;

                color: Style.textColor;
                font.family: Style.fontFamilyBold;
                font.pixelSize: Style.fontSize_common;

                text: qsTr("Subfeatures");
            }

            SimpleCommandsDecorator{
                id: commands;

                anchors.left: titleTableView.right;
                anchors.leftMargin: 10;
                anchors.verticalCenter: parent.verticalCenter;

                onCommandActivated: {
                    console.log("Feature commands", commandId);
                    if (commandId === "New"){
                        let selectedIndex = tableView.selectedIndex;

                        undoRedoManager.beginChanges();

                        if (selectedIndex == null){
                            console.log("featureEditor.documentModel", JSON.stringify(featureEditor.documentModel));

                            let childModel = featureEditor.documentModel.GetData("ChildModel");
                            console.log("childModel", childModel);
                            if (!childModel){
                                console.log("AddTreeModel");
                                childModel = featureEditor.documentModel.AddTreeModel("ChildModel");
                            }

                            let emptyModel = featureEditor.treeItemModelComp.createObject(childModel);
                            childModel.InsertNewItemWithParameters(0, {"FeatureId":"", "Name":"Feature Name", "Description":"", "Dependencies":"", "Optional":false, "ChildModel": emptyModel});

                            tableView.rowModel = childModel;
                        }
                        else{
                            let subfeaturesModel = featureEditor.documentModel.GetData("ChildModel");
                            if (!subfeaturesModel){
                                subfeaturesModel = featureEditor.documentModel.AddTreeModel("ChildModel");
                            }

                            let index = selectedIndex.index;

                            let parentModel = tableView.selectedIndex.getParentModel();
                            let childModel = selectedIndex.getData("ChildModel");

                            console.log("childModel", childModel);
                            if (!childModel){
                                if (parentModel){
                                    childModel = parentModel.AddTreeModel("ChildModel", index);
                                }
                                else{
                                    childModel = subfeaturesModel.AddTreeModel("ChildModel", index);
                                }
                            }

                            let emptyModel = featureEditor.treeItemModelComp.createObject(childModel);
                            childModel.InsertNewItemWithParameters(0, {"FeatureId":"", "Name":"Feature Name", "Description":"","Dependencies":"", "Optional":false, "ChildModel": emptyModel});

                            //subfeaturesModel.Refresh();

                            tableView.rowModel = 0;
                            tableView.rowModel = subfeaturesModel;
                        }

                        undoRedoManager.endChanges();
                    }
                    else if (commandId === "Remove"){
                        if (tableView.selectedIndex != null){

                            undoRedoManager.beginChanges();

                            let index = tableView.selectedIndex.index;

                            let parentModel = tableView.selectedIndex.getParentModel();
                            if (parentModel){
                                parentModel.RemoveItem(index);
                            }
                            else{
                                let subfeaturesModel = featureEditor.documentModel.GetData("ChildModel");
                                if (subfeaturesModel){
                                    subfeaturesModel.RemoveItem(index);
                                }
                            }

                            undoRedoManager.endChanges();
                        }
                    }
                }
            }

            TreeItemModel {
                id: commandsModel;

                Component.onCompleted: {
                    let index = commandsModel.InsertNewItem();

                    commandsModel.SetData("Id", "New", index);
                    commandsModel.SetData("Name", "New", index);
                    commandsModel.SetData("Icon", "Icons/Add", index);
                    commandsModel.SetData("IsEnabled", true, index);
                    commandsModel.SetData("Visible", true, index);

                    index = commandsModel.InsertNewItem();

                    commandsModel.SetData("Id", "Remove", index);
                    commandsModel.SetData("Name", "Remove", index);
                    commandsModel.SetData("Icon", "Icons/Delete", index);
                    commandsModel.SetData("IsEnabled", false, index);
                    commandsModel.SetData("Visible", true, index);

                    commands.commandModel = commandsModel;
                }
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

            rowDelegate: Component { PackageViewItemDelegate { root: tableView; } }

            function canRename(id){
                return true;
            }

            function featureIdExists(featureId){
                return false;
            }

            onSelectedIndexChanged: {
                // Commands
                let isEnabled = selectedIndex == null;
                commandsModel.SetData("IsEnabled", !isEnabled, 1);

                if (selectedIndex != null){
                    featureDependenciesView.contentVisible = true;
                    updateTreeViewGui();
                }
                else{
                    featureDependenciesView.contentVisible = false;
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
            height: 30;

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
                featureDependenciesView.addColumn({"Id": "Name", "Name": "Feature Name"});
                console.log("FeaturesProvider.model", FeaturesProvider.model);

                featureEditor.dependenciewViewModel.Copy(FeaturesProvider.model);
                featureDependenciesView.rowModel = featureEditor.dependenciewViewModel;

                FeaturesProvider.onModelChanged.connect(featureDependenciesView.onFeaturesProviderModelChanged);
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
                    if (tableView.selectedIndex != null){
                        if (!featureDependenciesView.delegateUpdatingBlock){
                            if (delegate.itemData.FeatureId !== ""){
                                undoRedoManager.beginChanges();

                                let featureId = delegate.itemData.FeatureId;
                                let state = delegate.checkState;

                                let selectedId = tableView.selectedIndex.itemData.FeatureId;
                                let dependencies = tableView.selectedIndex.itemData.Dependencies;

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
                                    tableView.selectedIndex.itemData.Dependencies = dependencyList.join(';')
                                }
                                else{
                                    tableView.selectedIndex.itemData.Dependencies = "";
                                }

                                undoRedoManager.endChanges();

                                featureEditor.updateTreeViewGui();
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
            headersModel.SetData("Id", "Name", index);
            headersModel.SetData("Name", qsTr("Feature Name"), index);

            index = headersModel.InsertNewItem();
            headersModel.SetData("Id", "Id", index);
            headersModel.SetData("Name", qsTr("Feature-ID"), index);

            index = headersModel.InsertNewItem();
            headersModel.SetData("Id", "Description", index);
            headersModel.SetData("Name", qsTr("Description"), index);

            index = headersModel.InsertNewItem();
            headersModel.SetData("Id", "Optional", index);
            headersModel.SetData("Name", qsTr("Optional"), index);

            tableView.columnModel = headersModel;
        }
    }
}
