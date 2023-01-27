import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

DocumentBase {
    id: container;

    property bool blockUpdatingModel: false;

    commandsDelegateSourceComp: Component {
        ProductViewCommandsDelegate {
            tableData: tableView;
        }
    }

    onDocumentModelChanged: {
        let headers = container.documentModel.GetData("Headers");
        tableView.columnModel.clear();
        for (let i = 0; i < headers.GetItemsCount(); i++){
            let headerId = headers.GetData("Id", i);
            let headerName = headers.GetData("Name", i);

            tableView.addColumn({"Id": headerId, "Name": headerName});
        }

        let items = container.documentModel.GetData("Items");
        if (!items){
            items = container.documentModel.AddTreeModel("Items");
        }

        let dependencies = container.documentModel.GetData("Features");
        if (!dependencies){
            dependencies = container.documentModel.AddTreeModel("Features");
        }

        container.updateGui();

        undoRedoManager.registerModel(documentModel);
    }

    onCommandsDelegateLoaded: {
        container.commandsDelegate.tableData = tableView;
    }

    UndoRedoManager {
        id: undoRedoManager;

        commandsId: container.commandsId;
        commandsDelegate: container.commandsDelegate;

        onModelStateChanged: {
            console.log("UndoRedoManager onModelStateChanged");

            container.updateGui();
        }
    }

    function updateModel(){
        console.log("Begin updateModel");
        undoRedoManager.beginChanges();

        let items = container.documentModel.AddTreeModel("Items");
        let featuresModel = container.documentModel.AddTreeModel("Features");

        let categoryIndex = categoryComboBox.currentIndex;
        let categoryId = modelCategogy.GetData("Id", categoryIndex);

        container.documentModel.SetData("CategoryId", categoryId);

        for (let i = 0; i < tableView.rowModel.count; i++){
            let rowObj = tableView.rowModel.get(i);

            let index = items.InsertNewItem();

            items.SetData("Id", rowObj["Id"], index);
            items.SetData("Name", rowObj["Name"], index);
            items.SetData("Description", rowObj["Description"], index);

            let rowChildModel = rowObj["ChildModel"]
            if (rowChildModel.count > 0){
                container.licenseFeaturesUpdateModel(rowObj["Id"], rowChildModel, featuresModel);
            }
        }

        undoRedoManager.endChanges();
        console.log("End updateModel");
    }


    //Обновление Features
    function licenseFeaturesUpdateModel(licenseId, guiModel, featuresModel){
        let featureModel = featuresModel.AddTreeModel(licenseId);

        for (let i = 0; i < guiModel.count; i++){
            let index = featureModel.InsertNewItem();

            let rowObj = guiModel.get(i);

            let rowId = rowObj["Id"];
            let rowName = rowObj["Name"];
            let description = rowObj["Description"];
            let optional = rowObj["Optional"];

            featureModel.SetData("Id", rowId, index);
            featureModel.SetData("Name", rowName, index);
            featureModel.SetData("Description", description, index);
            featureModel.SetData("Optional", optional, index);

            let rowChildModel = rowObj["ChildModel"]
            if (rowChildModel.count > 0){
                container.recursiveLicenseFeaturesUpdateModel(rowChildModel, featureModel);
            }
        }
    }

    //Обновление Subfeatures
    function recursiveLicenseFeaturesUpdateModel(guiModel, featureModel){
        for (let i = 0; i < guiModel.count; i++){
            let rowObj = guiModel.get(i);

            let rowId = rowObj["Id"];
            let rowName = rowObj["Name"];
            let description = rowObj["Description"];
            let optional = rowObj["Optional"];
            let state = rowObj["State"];

            if (state == Qt.Checked){
                let index = featureModel.InsertNewItem();

                featureModel.SetData("Id", rowId, index);
                featureModel.SetData("Name", rowName, index);
                featureModel.SetData("Description", description, index);
                featureModel.SetData("Optional", optional, index);
            }

            let rowChildModel = rowObj["ChildModel"]
            if (rowChildModel.count > 0){
                container.recursiveLicenseFeaturesUpdateModel(rowChildModel, featureModel);
            }
        }
    }

    function updateGui(){
        console.log("begin updateGui");
        container.blockUpdatingModel = true;

        tableView.rowModel.clear();

        let items = container.documentModel.GetData("Items");
        let featuresModel = container.documentModel.GetData("Features");

        let categoryId = container.documentModel.GetData("CategoryId");
        for (let i = 0; i < modelCategogy.GetItemsCount(); i++){
            if (categoryId == modelCategogy.GetData("Id", i)){
                categoryComboBox.currentIndex = i;

                break;
            }
        }

        for (let i = 0; i < items.GetItemsCount(); i++){
            let licenseId = items.GetData("Id", i);
            let licenseName = items.GetData("Name", i);
            let description = items.GetData("Description", i);

            tableView.insertRow([i], {"Id": licenseId, "Name": licenseName, "Description": description});

            if (featuresModel.ContainsKey(licenseId)){
                let featureModel = featuresModel.GetData(licenseId);
                let optionalFeatureIds = container.getOptionalFeatures(featureModel);
                let notOptionalFeatureIds = container.getNotOptionalFeatures(featureModel);

                console.log("optionalFeatureIds", optionalFeatureIds);
                console.log("notOptionalFeatureIds", notOptionalFeatureIds);

                for (let j = 0; j < notOptionalFeatureIds.length; j++){
                    let featureId = notOptionalFeatureIds[j];

                    container.licenseFeaturesUpdateGui(licenseId, featureId, i, optionalFeatureIds);
                }
            }
        }

        container.blockUpdatingModel = false;
        console.log("End updateGui");
    }

    function licenseFeaturesUpdateGui(licenseId, featureId, index, optionalFeatureIds){
        let model = featuresProvider.model;
        for (let i = 0; i < model.GetItemsCount(); i++){
            let childModel = model.GetData("ChildModel", i);
            if (childModel){
                for (let j = 0; j < childModel.GetItemsCount(); j++){
                    let childFeatureId = childModel.GetData("Id", j);
                    let childFeatureName = childModel.GetData("Name", j);
                    let childFeatureDescription = childModel.GetData("Description", j);
                    let optional = childModel.GetData("Optional", j);

                    if (childFeatureId == featureId){
                        tableView.insertRow([index, 0], {"LicenseId": licenseId, "Id": childFeatureId, "Name": childFeatureName, "Description": childFeatureDescription, "Optional": optional, "State": Qt.Unchecked});

                        let subFeaturesModel = childModel.GetData("ChildModel", j);
                        if (subFeaturesModel){
                            container.licenseFeaturesRecursiveUpdateGui(subFeaturesModel, [index, 0], optionalFeatureIds, licenseId);
                        }
                    }
                }
            }
        }
    }

    function licenseFeaturesRecursiveUpdateGui(model, indexes, optionalFeatureIds, licenseId){
        console.log("recursiveUpdateGui", licenseId);
        for (let i = 0; i < model.GetItemsCount(); i++){
            let rowId = model.GetData("Id", i);
            let rowName = model.GetData("Name", i);
            let description = model.GetData("Description", i);
            let optional = model.GetData("Optional", i);

            let row = {"LicenseId": licenseId, "Id": rowId, "Name": rowName, "Description": description, "Optional": optional, "State": Qt.Unchecked};
            if (optionalFeatureIds.includes(rowId)){
                console.log("optionalFeatureIds.includes(rowId)");
                row["State"] = Qt.Checked;
            }

            tableView.insertRow(indexes.concat([i]), row);

            let childModel = model.GetData("ChildModel", i);
            if (childModel){
                let childIndexes = []
                childIndexes = childIndexes.concat(indexes.concat([i]))
                container.licenseFeaturesRecursiveUpdateGui(childModel, childIndexes, optionalFeatureIds, licenseId);
            }
        }
    }

    function getNotOptionalFeatures(model){
        let retVal = []

        for (let i = 0; i < model.GetItemsCount(); i++){
            let featureId = model.GetData("Id", i);
            let isOptional = model.GetData("Optional", i);

            if (!isOptional){
                retVal.push(featureId);
            }
        }

        return retVal;
    }

    function getOptionalFeatures(model){
        let retVal = []

        for (let i = 0; i < model.GetItemsCount(); i++){
            let featureId = model.GetData("Id", i);
            let isOptional = model.GetData("Optional", i);

            if (isOptional){
                retVal.push(featureId);
            }
        }

        return retVal;
    }

    Component {
        id: featuresDialog;

        FeaturesDialog {
            onFinished: {
                if (buttonId == "Add"){
                    undoRedoManager.beginChanges();

                    let featuresModel = documentModel.GetData("Features");
                    if (!featuresModel){
                        featuresModel = documentModel.AddTreeModel("Features");
                    }

                    let licenseId = tableView.selectedIndex.itemData.Id;

                    let featureId = selectedIndex.itemData.Id;
                    let featureName = selectedIndex.itemData.Name;
                    let isOptional = selectedIndex.itemData.Optional;

                    if (featuresModel.ContainsKey(licenseId)){
                        let featureModel = featuresModel.GetData(licenseId);

                        let index = -1;
                        for (let i = 0; i < featureModel.GetItemsCount(); i++){
                            let productFeatureId = featureModel.GetData("Id", i);
                            if (productFeatureId == featureId){
                                index = i;
                                break;
                            }
                        }

                        if (index < 0){
                            let itemIndex = featureModel.InsertNewItem();

                            featureModel.SetData("Id", featureId, itemIndex);
                            featureModel.SetData("Name", featureName, itemIndex);
                            featureModel.SetData("Optional", isOptional, itemIndex);
                        }
                    }
                    else{
                        let featureModel = featuresModel.AddTreeModel(licenseId);

                        featureModel.SetData("Id", featureId);
                        featureModel.SetData("Name", featureName);
                        featureModel.SetData("Optional", isOptional);
                    }

                    undoRedoManager.endChanges();

                    container.updateGui();
                }
            }
        }
    }

    TreeItemModel {
        id: modelCategogy;

        Component.onCompleted: {
            let index = modelCategogy.InsertNewItem();

            modelCategogy.SetData("Id", "Software", index);
            modelCategogy.SetData("Name", "Software", index);

            index = modelCategogy.InsertNewItem();

            modelCategogy.SetData("Id", "Hardware", index);
            modelCategogy.SetData("Name", "Hardware", index);

            categoryComboBox.model = modelCategogy;
        }
    }

    Item {
        id: headerPanel;

        width: parent.width;

        height: 50;

        Text {
            id: categoryLable;

            anchors.verticalCenter: headerPanel.verticalCenter;

            anchors.left: parent.left;
            anchors.leftMargin: 10;

            color: Style.buttonText;
            font.family: Style.fontFamilyBold;

            text: qsTr("Category");
        }

        ComboBox {
            id: categoryComboBox;

            anchors.verticalCenter: headerPanel.verticalCenter;
            anchors.left: categoryLable.right;
            anchors.leftMargin: 10

            height: 25;
            width: 140;

            backgroundColor: Style.baseColor;
            currentIndex: 0;

            radius: 0;

            onCurrentIndexChanged: {
                if (!container.blockUpdatingModel){
                    updateModel();
                }
            }
        }
    }

    BasicTreeView {
        id: tableView;

        anchors.top: headerPanel.bottom
        anchors.left: parent.left;
        anchors.right: parent.right;

        height: parent.height;

        readOnly: false;

        rowDelegate: ProductViewItemDelegate {
            onAddButtonClicked: {
                tableView.selectedIndex = modelIndex;
                console.log("ProductViewItemDelegate onButtonClicked");
                modalDialogManager.openDialog(featuresDialog, {});
            }

            onRemoveButtonClicked: {
                tableView.removeRow(modelIndex.getIndexes());
            }
        }

        onRowAdded: {
            console.log("onRowAdded");
            if (!container.blockUpdatingModel){
                container.updateModel();
            }
        }

        onRowRemoved: {
            console.log("onRowRemoved");
            if (!container.blockUpdatingModel){
                container.updateModel();
            }
        }

        onRowModelDataChanged: {
            console.log("onRowModelDataChanged", prop);
            if (!container.blockUpdatingModel){
                container.updateModel();
            }
        }
    }
}
