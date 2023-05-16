import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

DocumentBase {
    id: productViewContainer;

    nameOutsideEditor: true;

    commandsDelegateSourceComp: Component {
        ProductViewCommandsDelegate {
            tableData: tableView;
        }
    }

    property bool featuresUpdated: false;

    property TreeItemModel licensesModel: TreeItemModel {}
    property TreeItemModel featuresModel: TreeItemModel {}

    property TreeItemModel softwareHeadersModel: TreeItemModel {
        Component.onCompleted: {
            let index = productViewContainer.softwareHeadersModel.InsertNewItem();
            productViewContainer.softwareHeadersModel.SetData("Id", "Name", index);
            productViewContainer.softwareHeadersModel.SetData("Name", "License Name", index);

            index = productViewContainer.softwareHeadersModel.InsertNewItem();
            productViewContainer.softwareHeadersModel.SetData("Id", "Id", index);
            productViewContainer.softwareHeadersModel.SetData("Name", "License-ID", index);

            index = productViewContainer.softwareHeadersModel.InsertNewItem();
            productViewContainer.softwareHeadersModel.SetData("Id", "Description", index);
            productViewContainer.softwareHeadersModel.SetData("Name", "Description", index);
        }
    }

    property TreeItemModel hardwareHeadersModel: TreeItemModel {
        Component.onCompleted: {
            let index = productViewContainer.hardwareHeadersModel.InsertNewItem();
            productViewContainer.hardwareHeadersModel.SetData("Id", "Name", index);
            productViewContainer.hardwareHeadersModel.SetData("Name", "Model Name", index);

            index = productViewContainer.hardwareHeadersModel.InsertNewItem();
            productViewContainer.hardwareHeadersModel.SetData("Id", "Id", index);
            productViewContainer.hardwareHeadersModel.SetData("Name", "Model-ID", index);

            index = productViewContainer.hardwareHeadersModel.InsertNewItem();
            productViewContainer.hardwareHeadersModel.SetData("Id", "Description", index);
            productViewContainer.hardwareHeadersModel.SetData("Name", "Description", index);
        }
    }

    Component.onCompleted: {
        Events.subscribeEvent("FeaturesUpdated", productViewContainer.onFeaturesUpdated);

        productViewContainer.commandsDelegate.tableData = tableView;

        productViewContainer.updateFeaturesModel();
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("FeaturesUpdated", productViewContainer.onFeaturesUpdated);
    }


    Component {
        id: messageDialog;
        MessageDialog {
            onFinished: {
                if (buttonId == "Yes"){
                    productViewContainer.updateGui();
                }
            }
        }
    }

    onModelIsReadyChanged: {
        let headers = productViewContainer.softwareHeadersModel;
        for (let i = 0; i < headers.GetItemsCount(); i++){
            let headerId = headers.GetData("Id", i);
            let headerName = headers.GetData("Name", i);

            tableView.addColumn({"Id": headerId, "Name": headerName});
        }

        productViewContainer.licensesModel.Copy(productViewContainer.documentModel.GetData("Items"))

        productViewContainer.updateGui();

        undoRedoManager.registerModel(documentModel);
    }

    onCommandsDelegateLoaded: {
        productViewContainer.commandsDelegate.tableData = tableView;
    }

    function onFeaturesUpdated(){
        console.log( "ProductView onFeaturesUpdated", productViewContainer.featuresUpdated);

        productViewContainer.updateFeaturesModel();
    }

    function updateFeaturesModel(){
        productViewContainer.featuresModel.Clear();

        if (featuresProvider.model){
            for (let i = 0; i < featuresProvider.model.GetItemsCount(); i++){
                let childModel = featuresProvider.model.GetData("ChildModel", i);
                if (childModel){
                    for (let j = 0; j < childModel.GetItemsCount(); j++){
                        let index = productViewContainer.featuresModel.InsertNewItem();
                        productViewContainer.featuresModel.CopyItemDataFromModel(index, childModel, j)
                    }
                }
            }
        }
    }

    function onLicensesModelDataChanged(){
        if (productViewContainer.blockUpdatingModel){
            return;
        }
        console.log("onLicensesModelDataChanged");

        productViewContainer.updateModel();
    }

    UndoRedoManager {
        id: undoRedoManager;

        documentBase: productViewContainer;
        onModelStateChanged: {
            console.log("UndoRedoManager onModelStateChanged");

            productViewContainer.updateGui();
        }
    }

    function updateModel(){
        console.log("updateModel");

        if (productViewContainer.blockUpdatingModel){
            return;
        }

        undoRedoManager.beginChanges();

        if (!productViewContainer.documentModel.ContainsKey("Items")){
            productViewContainer.documentModel.AddTreeModel("Items")
        }

        if (!productViewContainer.documentModel.ContainsKey("Features")){
            productViewContainer.documentModel.AddTreeModel("Features")
        }

        undoRedoManager.endChanges();
    }

    function updateGui(){
        productViewContainer.blockUpdatingModel = true;

        let categoryId = productViewContainer.documentModel.GetData("CategoryId");
        if (categoryId === "Software"){
            categoryComboBox.currentIndex = 0;
        }
        else{
            categoryComboBox.currentIndex = 1;
        }

        tableView.rowModel = 0;
        productViewContainer.licensesModel.Clear();

        if (productViewContainer.documentModel.ContainsKey("Items")){
            let itemsModel = productViewContainer.documentModel.GetData("Items");

            productViewContainer.licensesModel.Copy(itemsModel);
        }

        productViewContainer.updateLicenseFeaturesModel();
        tableView.rowModel = productViewContainer.licensesModel;

        productViewContainer.blockUpdatingModel = false;
    }

    function updateLicenseFeaturesModel(){
        if (productViewContainer.documentModel.ContainsKey("Features")){
            let itemsModel = productViewContainer.licensesModel;
            let featuresModel = productViewContainer.documentModel.GetData("Features");

            for (let i = 0; i < itemsModel.GetItemsCount(); i++){
                let licenseId = itemsModel.GetData("Id", i);

                if (featuresModel.ContainsKey(licenseId)){
                    let childModel = itemsModel.GetData("ChildModel", i)
                    if (!childModel){
                        childModel = itemsModel.AddTreeModel("ChildModel", i);
                    }

                    //childModel.Clear();

                    let licenseFeaturesModel = featuresModel.GetData(licenseId);

                    let optionalFeatureIds = productViewContainer.getOptionalFeatures(licenseFeaturesModel);
                    for (let j = 0; j < licenseFeaturesModel.GetItemsCount(); j++){
                        let featureId = licenseFeaturesModel.GetData("Id", j);
                        let featureName = licenseFeaturesModel.GetData("Name", j);
                        let optional = licenseFeaturesModel.GetData("Optional", j);
                        if (!optional){
                            let count = childModel.GetItemsCount();
                            let index = childModel.InsertNewItem();

                            childModel.SetData("Id", featureId, index);
                            childModel.SetData("Name", featureName, index);
                            childModel.SetData("Optional", optional, index);
                            childModel.SetData("LicenseId", licenseId, index);
                            let subModel = childModel.AddTreeModel("ChildModel", index);

                            let subFeaturesModel = productViewContainer.getSubFeatures(featureId);
                            if (subFeaturesModel){
                                subModel.Copy(subFeaturesModel);
                                productViewContainer.checkSubFeatures(subModel, optionalFeatureIds, licenseId);
                            }
                        }
                    }
                }
            }
        }
    }

    function checkSubFeatures(model, optionalFeatureIds, licenseId){
        console.log("productViewContainer.licensesModel", productViewContainer.licensesModel.toJSON());

        for (let j = 0; j < model.GetItemsCount(); j++){
            let featureId = model.GetData("Id", j);
            if (optionalFeatureIds.includes(featureId)){
                model.SetData("CheckState", Qt.Checked, j);
            }
            else{
                model.SetData("CheckState", Qt.Unchecked, j);
            }

            model.SetData("LicenseId", licenseId, j);
        }
    }

    function getSubFeatures(featureId){
        let model = featuresProvider.model;
        for (let i = 0; i < model.GetItemsCount(); i++){
            let childModel = model.GetData("ChildModel", i);
            if (childModel){
                for (let j = 0; j < childModel.GetItemsCount(); j++){
                    let id = childModel.GetData("Id", j);
                    if (id === featureId){
                        let subFeaturesModel = childModel.GetData("ChildModel", j);
                        return subFeaturesModel;
                    }
                }
            }
        }

        return null;
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
//            featuresModel: productViewContainer.featuresModel;

            onStarted: {
                updateGui();
            }

            onFinished: {
                if (buttonId == "Add"){
                    let features = productViewContainer.documentModel.GetData("Features");
                    if (!features){
                        features = productViewContainer.documentModel.AddTreeModel("Features");
                    }

                    let licenseId = tableView.selectedIndex.itemData.Id;
                    for (let selectedIndex of this.selectedIndexes){
                        let featureId = this.featuresModel.GetData("Id", selectedIndex);
                        let featureName = this.featuresModel.GetData("Name", selectedIndex);
                        let isOptional = this.featuresModel.GetData("Optional ", selectedIndex);

                        let featureModel = features.GetData(licenseId)
                        if (!featureModel){
                            featureModel = features.AddTreeModel(licenseId);
                        }

                        let itemIndex = featureModel.InsertNewItem();

                        featureModel.SetData("Id", featureId, itemIndex);
                        featureModel.SetData("Name", featureName, itemIndex);
                        featureModel.SetData("Optional", isOptional, itemIndex);
                    }

                    productViewContainer.updateGui();

                    productViewContainer.updateModel();
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

    Rectangle {
        id: headerPanel;

        width: parent.width;

        color: Style.backgroundColor;

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
                let category = modelCategogy.GetData("Id", categoryComboBox.currentIndex);
                productViewContainer.documentModel.SetData("CategoryId", category);

                if (category === "Software"){
                    tableView.columnModel = productViewContainer.softwareHeadersModel;
                }
                else if (category === "Hardware"){
                    tableView.columnModel = productViewContainer.hardwareHeadersModel;
                }

                productViewContainer.updateModel();
            }
        }
    }

    CustomScrollbar {
        id: scrollbar;

        z: 100;

        anchors.right: parent.right;
        anchors.bottom: parent.bottom;

        backgroundColor: Style.baseColor;

        secondSize: 10;
        targetItem: tableView;
    }

    TreeItemModel {
        id: filterFeatureModel;
    }

    BasicTreeView {
        id: tableView;

        anchors.top: headerPanel.bottom
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom

        readOnly: false;

        rowDelegate: Component { ProductViewItemDelegate {
            root: tableView;

            onDataChanged: {
                let itemsModel = productViewContainer.documentModel.GetData("Items");

                let licenseId = tableView.rowModel.GetData("Id", model.index);
                let licenseName = tableView.rowModel.GetData("Name", model.index);
                let licenseDescription = tableView.rowModel.GetData("Description", model.index);

                itemsModel.SetData("Id", licenseId, model.index);
                itemsModel.SetData("Name", licenseName, model.index);
                itemsModel.SetData("Description", licenseDescription, model.index);

                productViewContainer.updateModel();
            }

            onCheckStateChanged: {
                console.log("onCheckStateChanged", model.CheckState);

                if (productViewContainer.blockUpdatingModel){
                    return;
                }

                let licenseId = model.LicenseId;
                let featureId = model.Id;
                if (model.CheckState === Qt.Checked){
                    let featureName = model.Name;
                    let optional = model.Optional;

                    let featuresModel = productViewContainer.documentModel.GetData("Features");
                    let licenseFeaturesModel = featuresModel.GetData(licenseId);

                    let index = licenseFeaturesModel.InsertNewItem();
                    licenseFeaturesModel.SetData("Id", featureId, index);
                    licenseFeaturesModel.SetData("Name", featureName, index);
                    licenseFeaturesModel.SetData("Optional", optional, index);
                }
                else if (model.CheckState === Qt.Unchecked){
                    let featuresModel = productViewContainer.documentModel.GetData("Features");
                    if (featuresModel.ContainsKey(licenseId)){
                        let licenseFeaturesModel = featuresModel.GetData(licenseId);
                        for (let i = 0; i < licenseFeaturesModel.GetItemsCount(); i++){
                            let id = licenseFeaturesModel.GetData("Id", i);
                            if (id === featureId){
                                licenseFeaturesModel.RemoveItem(i);
                                break;
                            }
                        }

                        productViewContainer.modelChanged();
                    }
                }

                productViewContainer.updateModel();
            }

            onAddButtonClicked: {
                tableView.selectedIndex = modelIndex;
                console.log("ProductViewItemDelegate onButtonClicked");

                let featureIDs = []
                let licenseId = tableView.selectedIndex.itemData.Id;
                if (productViewContainer.documentModel.ContainsKey("Features")){
                    let featuresModel = productViewContainer.documentModel.GetData("Features");
                    if (featuresModel.ContainsKey(licenseId)){
                        let featureModel = featuresModel.GetData(licenseId);

                        for (let i = 0; i < featureModel.GetItemsCount(); i++){
                            let featureID = featureModel.GetData("Id", i);
                            featureIDs.push(featureID);
                        }
                    }
                }

                filterFeatureModel.Clear();

                for (let i = 0; i < productViewContainer.featuresModel.GetItemsCount(); i++){
                    let featureId = productViewContainer.featuresModel.GetData("Id", i);
                    if (!featureIDs.includes(featureId)){
                        let index = filterFeatureModel.InsertNewItem();
                        filterFeatureModel.CopyItemDataFromModel(index, productViewContainer.featuresModel, i)
                    }
                }

                modalDialogManager.openDialog(featuresDialog, {"featuresModel": filterFeatureModel});
            }

            onRemoveButtonClicked: {
                if (tableView.selectedIndex != null){
                    let selectedlFeatureId = tableView.selectedIndex.itemData.Id;
                    let licenseId = tableView.selectedIndex.itemData.LicenseId;
                    if (productViewContainer.documentModel.ContainsKey("Features")){
                        let featuresModel = productViewContainer.documentModel.GetData("Features");
                        if (featuresModel.ContainsKey(licenseId)){
                            let licenseFeaturesModel = featuresModel.GetData(licenseId);

                            for (let i = 0; licenseFeaturesModel.GetItemsCount(); i++){
                                let featureId = licenseFeaturesModel.GetData("Id", i);
                                if (featureId === selectedlFeatureId){
                                    licenseFeaturesModel.RemoveItem(i);
                                    break;
                                }
                            }
                            productViewContainer.updateModel();
                            tableView.removeByIndex(tableView.selectedIndex);
                        }
                    }
                }
            }
        } }
    }
}
