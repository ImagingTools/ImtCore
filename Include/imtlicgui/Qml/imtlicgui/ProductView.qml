import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtqml 1.0

DocumentBase {
    id: productViewContainer;

    nameOutsideEditor: true;

    commandsDelegateSourceComp: Component {
        ProductViewCommandsDelegate {
            tableData: tableView;
        }
    }

    property TreeItemModel allFeaturesModel: TreeItemModel {}

    property TreeItemModel licensesModel: TreeItemModel {}
    property TreeItemModel productFeaturesViewModel: TreeItemModel {}

    Component.onCompleted: {
        productViewContainer.allFeaturesModel = FeaturesProvider.model;

        FeaturesProvider.onModelChanged.connect(productViewContainer.onFeaturesChanged);
    }

    Component.onDestruction: {
        FeaturesProvider.onModelChanged.disconnect(productViewContainer.onFeaturesChanged);
    }

    function onFeaturesChanged(){
        productViewContainer.allFeaturesModel = FeaturesProvider.model;

        availableFeaturesTable.elements = productViewContainer.allFeaturesModel;

        productViewContainer.updateFeaturesGui();
    }

    property TreeItemModel softwareHeadersModel: TreeItemModel {
        Component.onCompleted: {
            let index = productViewContainer.softwareHeadersModel.InsertNewItem();
            productViewContainer.softwareHeadersModel.SetData("Id", "FeatureName", index);
            productViewContainer.softwareHeadersModel.SetData("Name", qsTr("Feature Name"), index);

            index = productViewContainer.softwareHeadersModel.InsertNewItem();
            productViewContainer.softwareHeadersModel.SetData("Id", "FeatureId", index);
            productViewContainer.softwareHeadersModel.SetData("Name", qsTr("Feature-ID"), index);

            index = productViewContainer.softwareHeadersModel.InsertNewItem();
            productViewContainer.softwareHeadersModel.SetData("Id", "FeatureDescription", index);
            productViewContainer.softwareHeadersModel.SetData("Name", qsTr("Description"), index);

            tableView.columnModel = productViewContainer.softwareHeadersModel;
        }
    }

    property TreeItemModel hardwareHeadersModel: TreeItemModel {
        Component.onCompleted: {
            let index = productViewContainer.hardwareHeadersModel.InsertNewItem();
            productViewContainer.hardwareHeadersModel.SetData("Id", "FeatureName", index);
            productViewContainer.hardwareHeadersModel.SetData("Name", qsTr("Model Name"), index);

            index = productViewContainer.hardwareHeadersModel.InsertNewItem();
            productViewContainer.hardwareHeadersModel.SetData("Id", "FeatureId", index);
            productViewContainer.hardwareHeadersModel.SetData("Name", qsTr("Model-ID"), index);

            index = productViewContainer.hardwareHeadersModel.InsertNewItem();
            productViewContainer.hardwareHeadersModel.SetData("Id", "FeatureDescription", index);
            productViewContainer.hardwareHeadersModel.SetData("Name", qsTr("Description"), index);
        }
    }

    onWidthChanged: {
        if (productViewContainer.width > 0 && productViewContainer.width - rightPanel.width > 250){
            splitter.x = productViewContainer.width - rightPanel.width;
        }
    }

    onModelIsReadyChanged: {
        productViewContainer.updateGui();

        undoRedoManager.registerModel(documentModel);
    }

    function blockEditing(){
        tableView.readOnly = true;
        categoryComboBox.changeable = false;
    }

    UndoRedoManager {
        id: undoRedoManager;

        documentBase: productViewContainer;
        onModelStateChanged: {
            console.log("UndoRedoManager onModelStateChanged");

            productViewContainer.updateGui();
        }
    }

    function onEntered(value){
        productViewContainer.blockUpdatingModel = true;
        productViewContainer.documentModel.SetData("ProductId", value);
        productViewContainer.documentModel.SetData("ProductName", value);
        productViewContainer.blockUpdatingModel = false;
    }

    function updateModel(){
        if (productViewContainer.blockUpdatingModel){
            return;
        }

        undoRedoManager.beginChanges();

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

        if (!productViewContainer.documentModel.ContainsKey("Features")){
            productViewContainer.documentModel.SetData("Features", "");
        }

        undoRedoManager.endChanges();
    }

    function updateGui(){
        productViewContainer.blockUpdatingModel = true;

        categoryComboBox.currentIndex = -1;
        if (productViewContainer.documentModel.ContainsKey("CategoryId")){
            let categoryId = productViewContainer.documentModel.GetData("CategoryId")
            if (categoryId == "Software"){
                categoryComboBox.currentIndex = 0;
            }
            else if (categoryId == "Hardware"){
                categoryComboBox.currentIndex = 1;
            }
        }

        updateFeaturesGui();

        productViewContainer.blockUpdatingModel = false;
    }

    function updateFeaturesGui(){
        console.log("updateFeaturesGui");

        productViewContainer.productFeaturesViewModel.Clear();

        let features = productViewContainer.documentModel.GetData("Features")
        if (!features){
            return;
        }

        let featureIds = features.split(';')
        console.log("featureIds", featureIds);

        tableView.selectedOptionalFeatures = [];

        for (let featureId of featureIds){
            if (featureId.includes('/')){
                tableView.selectedOptionalFeatures.push(featureId);

                continue;
            }

            for (let i = 0; i < productViewContainer.allFeaturesModel.GetItemsCount(); i++){
                let id = productViewContainer.allFeaturesModel.GetData("Id", i);

                if (featureId === id){
                    let index = productViewContainer.productFeaturesViewModel.InsertNewItem();

                    productViewContainer.productFeaturesViewModel.CopyItemDataFromModel(index, productViewContainer.allFeaturesModel, i);
                }
            }
        }

        tableView.rowModel = 0;
        tableView.rowModel = productViewContainer.productFeaturesViewModel;
    }

    function addFeature(featureId){
        if (productViewContainer.blockUpdatingModel){
            return;
        }

        undoRedoManager.beginChanges();
        console.log("addFeature", featureId);
        let features = productViewContainer.documentModel.GetData("Features")

        let featureIds = []
        if (features !== ""){
            featureIds = features.split(';')
        }

        if (!featureIds.includes(featureId)){
            featureIds.push(featureId)
        }

        productViewContainer.documentModel.SetData("Features", featureIds.join(';'))

        console.log("productViewContainer.documentModel", productViewContainer.documentModel.toJSON());

        undoRedoManager.endChanges();
    }

    function removeFeature(featureId){
        if (productViewContainer.blockUpdatingModel){
            return;
        }

        undoRedoManager.beginChanges();

        let features = productViewContainer.documentModel.GetData("Features")

        let featureIds = []

        if (features !== ""){
            featureIds = features.split(';')
        }

        let i = featureIds.indexOf(featureId);
        if (i >= 0){
            featureIds.splice(i, 1);
        }

        productViewContainer.documentModel.SetData("Features", featureIds.join(';'))

        undoRedoManager.endChanges();
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

            Component.onCompleted: {
                let ok = PermissionsController.checkPermission("ChangeProduct");
                categoryComboBox.changeable = ok;
            }

            onCurrentIndexChanged: {
                if (productViewContainer.blockUpdatingModel){
                    return;
                }

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

    BasicTreeView {
        id: tableView;

        anchors.top: headerPanel.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left;
        anchors.right: splitter.left;

        readOnly: false;

        tristate: true;

        property var selectedOptionalFeatures: [];

        rowDelegate: Component {
            TreeViewItemDelegateBase {
                id: delegate;

                root: tableView;

                isCheckable: model.Optional ? model.Optional : false;

                onRootDelegateChanged: {
                    if (rootDelegate !== null){
                        let featureId = model.FeatureId;
                        let rootFeatureUuid = rootDelegate.itemData.Id;

                        let id = rootFeatureUuid + "/" + featureId;

                        if (tableView.selectedOptionalFeatures.includes(id)){
                            delegate.checkState = Qt.Checked;
                        }
                    }
                }

//                AuxButton {
//                    anchors.right: parent.right;
//                    anchors.rightMargin: 10;

//                    width: 18;
//                    height: 25;

//                    iconSource: "../../../" + Style.getIconPath("Icons/Delete", Icon.State.On, Icon.Mode.Normal);

//                    visible: delegate.modelIndex == tableView.selectedIndex && delegate.level === 0;

//                    iconWidth: 15;
//                    iconHeight: iconWidth;

//                    onClicked: {
//                        let selectedIndex = tableView.selectedIndex;
//                        if (selectedIndex != null){
//                            let index = selectedIndex.index;

//                            let featureId = tableView.rowModel.GetData("Id", index);

//                            productViewContainer.removeFeature(featureId);

//                            productViewContainer.updateFeaturesGui();
//                        }
//                    }
//                }

                onCheckStateChanged: {
                    console.log("onCheckStateChanged", checkState);

                    let featureId = model.FeatureId;
                    let rootFeatureUuid = rootDelegate.itemData.Id;

                    let id = rootFeatureUuid + "/" + featureId;

                    let features = productViewContainer.documentModel.GetData("Features");

                    let featureIds = []
                    if (features !== ""){
                        featureIds = features.split(';')
                    }

                    if (checkState == Qt.Checked){
                        if (!featureIds.includes(id)){
                            productViewContainer.addFeature(id);
                        }

                    }
                    else if (checkState == Qt.Unchecked){
                        if (featureIds.includes(id)){
                            productViewContainer.removeFeature(id);
                        }
                    }
                }
            }
        }
    }

    Splitter {
        id: splitter;
        x: productViewContainer.width - 250;

        height: parent.height;
        width: 0;

        visible: false;

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

        AuxButton {
            id: addButton;

            enabled: false;

            anchors.verticalCenter: parent.verticalCenter;

            width: 18;
            height: 25;

            iconSource: enabled ? "../../../" + Style.getIconPath("Icons/Left", Icon.State.On, Icon.Mode.Normal):
                                  "../../../" + Style.getIconPath("Icons/Left", Icon.State.Off, Icon.Mode.Disabled)

            iconWidth: 15;
            iconHeight: iconWidth;

            onClicked: {
                let indexes = availableFeaturesTable.getSelectedIndexes();
                if (indexes.length > 0){
                    let index = indexes[0];

                    let featureId = availableFeaturesTable.elements.GetData("Id", index);

                    productViewContainer.addFeature(featureId);

                    productViewContainer.updateFeaturesGui();

                    addButton.enabled = false;
                }
            }
        }
    }

    Item {
        id: rightPanel;

        anchors.left: splitter.right;
        anchors.top: headerPanel.bottom;
        anchors.bottom: parent.bottom;

        width: 0;

        visible: false;
//        width: productViewContainer.width > 0 ? productViewContainer.width - tableView.width : 250;

        AuxTable {
            id: availableFeaturesTable;

            anchors.fill: parent;

            backgroundHeadersColor: Style.alternateBaseColor;

            Component.onCompleted: {
                availableFeaturesTable.elements = FeaturesProvider.model;
            }

            onSelectionChanged: {
                let indexes = availableFeaturesTable.getSelectedIndexes();
                if (indexes.length === 1){
                    let index = indexes[0];

                    let selectedFeatureId = availableFeaturesTable.elements.GetData("Id", index);

                    let features = productViewContainer.documentModel.GetData("Features");

                    let featureIds = []
                    if (features !== ""){
                        featureIds = features.split(';');
                    }

                    addButton.enabled = !featureIds.includes(selectedFeatureId);

                    return;
                }

                addButton.enabled = false;
            }
        }
    }

    TreeItemModel {
        id: featuresHeaders;

        Component.onCompleted: {
            let index = featuresHeaders.InsertNewItem();

            featuresHeaders.SetData("Id", "FeatureName", index);
            featuresHeaders.SetData("Name", "Available features", index);

            availableFeaturesTable.headers = featuresHeaders;
        }
    }
}
