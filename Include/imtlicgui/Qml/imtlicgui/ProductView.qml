import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtqml 1.0
import imtdocgui 1.0

DocumentData {
    id: productViewContainer;

    commandsDelegate: ProductViewCommandsDelegate {
        tableData: tableView;
        documentPtr: productViewContainer;
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

    function blockEditing(){
        tableView.readOnly = true;
        categoryComboBox.changeable = false;
    }

    function updateModel(){
        productViewContainer.documentModel.SetData("ProductId", productNameInput.text);
        productViewContainer.documentModel.SetData("ProductName", productNameInput.text);

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
    }

    function updateGui(){
        if (productViewContainer.documentModel.ContainsKey("ProductName")){
            productNameInput.text = productViewContainer.documentModel.GetData("ProductName")
        }
        else{
            productNameInput.text = "";
        }

        let categoryFound = false;
        if (productViewContainer.documentModel.ContainsKey("CategoryId")){
            let categoryId = productViewContainer.documentModel.GetData("CategoryId")
            if (categoryId === "Software"){
                categoryComboBox.currentIndex = 0;
                categoryFound = true;
            }
            else if (categoryId === "Hardware"){
                categoryComboBox.currentIndex = 1;
                categoryFound = true;
            }
        }

        if (!categoryFound){
            categoryComboBox.currentIndex = -1;
        }

        updateFeaturesGui();
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
    }

    function removeFeature(featureId){
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
        anchors.fill: headerPanel;

        color: Style.backgroundColor;
    }

    Row {
        id: headerPanel;

        width: parent.width;
        height: 50;

        spacing: 10;

        Text {
            id: productNameLable;

            anchors.verticalCenter: headerPanel.verticalCenter;

            color: Style.buttonText;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSize_common;

            text: qsTr("Product Name");
        }

        CustomTextField {
            id: productNameInput;

            anchors.verticalCenter: headerPanel.verticalCenter;

            height: 30;
            width: 200;

            placeHolderText: qsTr("Enter the product name");

            Component.onCompleted: {
                let ok = PermissionsController.checkPermission("ChangeProduct");
                productNameInput.readOnly = !ok;
            }

            onEditingFinished: {
                productViewContainer.doUpdateModel();
            }
        }

        Text {
            id: categoryLable;

            anchors.verticalCenter: headerPanel.verticalCenter;

            color: Style.buttonText;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSize_common;

            text: qsTr("Category");
        }

        ComboBox {
            id: categoryComboBox;

            anchors.verticalCenter: headerPanel.verticalCenter;

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
                productViewContainer.doUpdateModel();
            }
        }
    }

    BasicTreeView {
        id: tableView;

        anchors.top: headerPanel.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left;
        anchors.right: parent.right;

        readOnly: false;

        tristate: true;

        property var selectedOptionalFeatures: [];

        Component.onCompleted: {
            let ok = PermissionsController.checkPermission("ChangeProduct");
            tableView.readOnly = !ok;
        }

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

                onCheckStateChanged: {
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
}
