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

    onCommandsIdChanged: {
        if (productViewContainer.itemId === ""){
            productViewContainer.commandsProvider.setCommandIsEnabled("Save", true);
        }
    }

    onWidthChanged: {
        if (productViewContainer.width > 0 && productViewContainer.width - rightPanel.width > 250){
            splitter.x = productViewContainer.width - rightPanel.width;
        }
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

        tableView.columnModel = headers;

        productViewContainer.blockUpdatingModel = true;

        if (!productViewContainer.documentModel.ContainsKey("Items")){
            productViewContainer.documentModel.AddTreeModel("Items")
        }

        if (!productViewContainer.documentModel.ContainsKey("Features")){
            productViewContainer.documentModel.AddTreeModel("Features")
        }

        if (!productViewContainer.documentModel.ContainsKey("Dependencies")){
            productViewContainer.documentModel.AddTreeModel("Dependencies")
        }

        productViewContainer.licensesModel.Copy(productViewContainer.documentModel.GetData("Items"))

        productViewContainer.updateGui();

        productViewContainer.blockUpdatingModel = false;

        undoRedoManager.registerModel(documentModel);
    }

    onCommandsDelegateLoaded: {
        productViewContainer.commandsDelegate.tableData = tableView;
    }

    function blockEditing(){
        tableView.readOnly = true;
        categoryComboBox.changeable = false;
    }

    function onFeaturesUpdated(){
        console.log( "ProductView onFeaturesUpdated", productViewContainer.featuresUpdated);

        productViewContainer.checkValidFeatures();

        productViewContainer.updateFeaturesModel();

        productViewContainer.updateGui();
    }

    function updateFeaturesModel(){
        console.log("updateFeaturesModel")
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
        console.time('updateModel')

        if (productViewContainer.blockUpdatingModel){
            return;
        }

        undoRedoManager.beginChanges();

        productViewContainer.documentModel.SetData("Id", productViewContainer.itemId);
        productViewContainer.documentModel.SetData("Name", productViewContainer.itemName);

        if (categoryComboBox.currentIndex == 0){
            productViewContainer.documentModel.SetData("CategoryId", "Software");
        }
        else if (categoryComboBox.currentIndex == 1){
            productViewContainer.documentModel.SetData("CategoryId", "Hardware");
        }
        else{
            productViewContainer.documentModel.SetData("CategoryId", "");
        }

        if (!productViewContainer.documentModel.ContainsKey("Items")){
            productViewContainer.documentModel.AddTreeModel("Items")
        }

        if (!productViewContainer.documentModel.ContainsKey("Features")){
            productViewContainer.documentModel.AddTreeModel("Features")
        }

        if (!productViewContainer.documentModel.ContainsKey("Dependencies")){
            productViewContainer.documentModel.AddTreeModel("Dependencies")
        }

        undoRedoManager.endChanges();

        console.timeEnd('updateModel')

    }

    function updateGui(){
        console.time('updateGui')

        productViewContainer.blockUpdatingModel = true;

        let categoryId = productViewContainer.documentModel.GetData("CategoryId");
        if (categoryId === "Hardware"){
            categoryComboBox.currentIndex = 1;
        }
        else{
            categoryComboBox.currentIndex = 0;
        }

        tableView.rowModel = 0;
        dependenciesTable.elements = 0;
        productViewContainer.licensesModel.Clear();

        if (productViewContainer.documentModel.ContainsKey("Items")){
            let itemsModel = productViewContainer.documentModel.GetData("Items");

            productViewContainer.licensesModel.Copy(itemsModel);
        }

        productViewContainer.updateLicenseFeaturesModel();
        tableView.rowModel = productViewContainer.licensesModel;

        dependenciesTable.elements = productViewContainer.licensesModel;

        productViewContainer.blockUpdatingModel = false;

        console.timeEnd('updateGui')
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

    function checkValidFeatures(){
        let featuresModel = productViewContainer.documentModel.GetData("Features");
        let licenseIDs = featuresModel.GetKeys();

        for (let licenseId of licenseIDs){
            let licenseFeaturesModel = featuresModel.GetData(licenseId);

            let removedIndexes = []
            for (let j = 0; j < licenseFeaturesModel.GetItemsCount(); j++){
                let featureId = licenseFeaturesModel.GetData("Id", j);

                let ok = featuresProvider.featureIsExists(featureId);
                if (!ok){
                    console.log("Removed feature", featureId);
                    removedIndexes.push(j);
                }
            }

            let removedCount = 0
            for (let i = 0; i < removedIndexes.length; i++){
                licenseFeaturesModel.RemoveItem(removedIndexes[i] - removedCount);
                removedCount++;
            }
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
            font.pixelSize: Style.fontSize_common;

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

    TreeItemModel {
        id: filterFeatureModel;
    }

    function updateFeaturesModelWithNewLicenseID(oldLicenseID, newLicenseID){
        if (productViewContainer.documentModel.ContainsKey("Features")){
            let featuresModel = productViewContainer.documentModel.GetData("Features");
            if (featuresModel.ContainsKey(oldLicenseID)){
                let features = featuresModel.GetData(oldLicenseID);

                let newFeaturesModel = featuresModel.AddTreeModel(newLicenseID);
                newFeaturesModel.Copy(features);

                featuresModel.RemoveData(oldLicenseID);
            }
        }
    }

    BasicTreeView {
        id: tableView;

        anchors.top: headerPanel.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left;
        anchors.right: splitter.left;

        readOnly: false;

        function openLicenseErrorDialog(message){
            productViewContainer.documentManager.openErrorDialog(message);
        }

        function licenseIdExists(licenseId){
            let delegateItems = tableView.getItemsDataAsList();
            for (let item of delegateItems){
                let itemData = item.getItemData();
                if (itemData.Id === licenseId){
                    return true;
                }
            }

            return false;
        }

        rowDelegate: Component { ProductViewItemDelegate {

                root: tableView;

                onEmptyIdError: {
                    let message = qsTr("It is not possible to add features to a license with an empty ID");
                    productViewContainer.documentManager.openErrorDialog(message);
                }

                onDataChanged: {
                    let itemsModel = productViewContainer.documentModel.GetData("Items");

                    let licenseId = tableView.rowModel.GetData("Id", model.index);
                    let licenseName = tableView.rowModel.GetData("Name", model.index);
                    let licenseDescription = tableView.rowModel.GetData("Description", model.index);

                    let oldLicenseId = itemsModel.GetData("Id", model.index);

                    if (oldLicenseId !== "" && oldLicenseId !== licenseId){
                        productViewContainer.updateFeaturesModelWithNewLicenseID(oldLicenseId, licenseId);
                    }

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

        onSelectedIndexChanged: {
            if (tableView.selectedIndex == null){
                dependenciesTable.visible = false;

                return;
            }

            let depth = tableView.selectedIndex.getDepth();
            if (depth === 0){
                dependenciesTable.visible = true;
                productViewContainer.updateDependenciesGui();
            }
            else{
                dependenciesTable.visible = false;
            }
        }
    }

    function updateDependenciesGui(){
        productViewContainer.blockUpdatingModel = true;

        if (tableView.selectedIndex != null){
            let selectedLicenseId = tableView.selectedIndex.getData("Id");
            let dependentLicenseIds = []
            if (productViewContainer.documentModel.ContainsKey("Dependencies")){
                let dependenciesModel = productViewContainer.documentModel.GetData("Dependencies");
                if (dependenciesModel.ContainsKey(selectedLicenseId)){
                    let dependencies = dependenciesModel.GetData(selectedLicenseId);
                    if (dependencies !== ""){
                        dependentLicenseIds = dependencies.split(';')
                    }
                }
            }

            dependenciesTable.allEnabled();
            dependenciesTable.allVisible();
            dependenciesTable.uncheckAll();

            let parentItems = []
            prop.getAllParents(selectedLicenseId, parentItems);

            let childrenItems = []
            for (let dependsId of dependentLicenseIds){
                prop.getAllChildren(dependsId, childrenItems);
            }

            console.log("childrenItems", childrenItems);

            if (dependenciesTable.elements){
                for (let i = 0; i < dependenciesTable.elements.GetItemsCount(); i++){
                    let id = dependenciesTable.elements.GetData("Id", i);
                    if (selectedLicenseId === id){
                        dependenciesTable.setVisibleItem(i, false);
                    }

                    if (parentItems.includes(id)){
                        dependenciesTable.setIsEnabledItem(i, false);
                    }

                    if (dependentLicenseIds.includes(id)){
                        dependenciesTable.checkItem(i);
                    }

                    if (childrenItems.includes(id)){
//                        dependenciesTable.checkItem(i);
                        dependenciesTable.setIsEnabledItem(i, false);
                    }
                }
            }
        }

        productViewContainer.blockUpdatingModel = false;
    }

    function getDisabledItems(){
        let result = []
        if (tableView.selectedIndex != null){

        }
    }

    QtObject {
        id: prop;

        function getAllParents(licenseId, retVal){
            if (productViewContainer.documentModel.ContainsKey("Dependencies")){
                let dependenciesModel = productViewContainer.documentModel.GetData("Dependencies");
                let keys = dependenciesModel.GetKeys();
                for (let key of keys){
                    let value = dependenciesModel.GetData(key);

                    let licenseIDs = value.split(';')
                    if (licenseIDs.includes(licenseId)){
                        retVal.push(key);
                        prop.getAllParents(key, retVal);
                    }
                }
            }
        }

        function getAllChildren(licenseId, retVal){
            if (productViewContainer.documentModel.ContainsKey("Dependencies")){
                let dependenciesModel = productViewContainer.documentModel.GetData("Dependencies");

                if (dependenciesModel.ContainsKey(licenseId)){
                    let value = dependenciesModel.GetData(licenseId);
                    let licenseIDs = value.split(';')

                    for (let lic of licenseIDs){
                        retVal.push(lic)

                        prop.getAllChildren(lic, retVal);
                    }
                }
            }
        }
    }

    Splitter {
        id: splitter;
        x: productViewContainer.width - 250;

        height: parent.height;
        width: 4;

        onXChanged: {
            if (!productViewContainer.visible){
                return;
            }

            if (splitter.x <= 250){
                splitter.x = 250;
            }
            else if (splitter.x > productViewContainer.width){
                splitter.x = productViewContainer.width - splitter.width;
            }
        }
    }

    Item {
        id: rightPanel;

        anchors.left: splitter.right;
        anchors.top: headerPanel.bottom;
        anchors.bottom: parent.bottom;

        width: productViewContainer.width > 0 ? productViewContainer.width - tableView.width : 250;

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

        TreeItemModel {
            id: dependentHeadersModel;
            Component.onCompleted: {
                let index = dependentHeadersModel.InsertNewItem();

                dependentHeadersModel.SetData("Id", "Name", index);
                dependentHeadersModel.SetData("Name", "Name", index);

                dependenciesTable.headers = dependentHeadersModel;
            }
        }

        Text {
            id: text;

            anchors.top: headerTreeView.bottom;
            anchors.left: headerTreeView.left;
            anchors.leftMargin: 10;
            anchors.topMargin: 10;

            color: Style.textColor;
            font.pixelSize: Style.fontSize_common;
            font.family: Style.fontFamily;

            text: qsTr("Dependencies cannot be setted");

            visible: !dependenciesTable.visible;
        }

        AuxTable {
            id: dependenciesTable;

            anchors.right: parent.right;
            anchors.left: parent.left;
            anchors.bottom: parent.bottom;

            anchors.top: headerTreeView.bottom;

            radius: 0;

            checkable: true;
            canSelectAll: false;
            visible: false;

            showHeaders: false;

            onCheckedItemsChanged: {
                if (productViewContainer.blockUpdatingModel){
                    return;
                }

                if (tableView.selectedIndex != null){
                    let index = tableView.selectedIndex.getIndex();
                    if (index >= 0){
                        let licenseId = tableView.rowModel.GetData("Id", index);
                        if (licenseId === ""){
                            dependenciesTable.visible = false;

                            return;
                        }

                        dependenciesTable.visible = true;

                        let indexes = dependenciesTable.getCheckedItems();
                        let licenseIDs = []

                        for (let i of indexes){
                            let licenseId = tableView.rowModel.GetData("Id", i);
                            licenseIDs.push(licenseId);
                        }

                        let dependencies = productViewContainer.documentModel.GetData("Dependencies")
                        let oldLicenses = "";
                        if (dependencies.ContainsKey(licenseId)){
                            oldLicenses = dependencies.GetData(licenseId);
                        }

                        let newLicenses = licenseIDs.join(';');
                        if (oldLicenses !== newLicenses){
                            if (newLicenses === ""){
                                dependencies.RemoveData(licenseId);
                            }
                            else{
                                dependencies.SetData(licenseId, newLicenses);
                            }

                            undoRedoManager.makeChanges();
                        }

                        productViewContainer.updateDependenciesGui();
                    }
                }
            }
        }
    }
}
