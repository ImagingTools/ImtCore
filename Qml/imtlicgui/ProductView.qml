import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcontrols 1.0

ViewBase {
    id: productViewContainer;

    property TreeItemModel allFeaturesModel: CachedFeatureCollection.collectionModel;
    property TreeItemModel licensesModel: TreeItemModel {}
    property TreeItemModel productFeaturesViewModel: TreeItemModel {}

    property alias tableView: tableView_;

    Component.onCompleted: {
        CachedFeatureCollection.updateModel();

        CachedFeatureCollection.modelUpdated.connect(productViewContainer.onFeaturesChanged);
        Events.subscribeEvent("OnLocalizationChanged", onLocalizationChanged)
    }

    Component.onDestruction: {
        CachedFeatureCollection.modelUpdated.disconnect(productViewContainer.onFeaturesChanged);
        Events.unSubscribeEvent("OnLocalizationChanged", onLocalizationChanged)
    }

    function onLocalizationChanged(language){
        updateHeaders();
    }

    function onFeaturesChanged(){
        productViewContainer.updateFeaturesGui();
    }

    function updateHeaders(){
        productViewContainer.softwareHeadersModel.Clear();

        let index = productViewContainer.softwareHeadersModel.InsertNewItem();
        productViewContainer.softwareHeadersModel.SetData("Id", "FeatureName", index);
        productViewContainer.softwareHeadersModel.SetData("Name", qsTr("Feature Name"), index);

        index = productViewContainer.softwareHeadersModel.InsertNewItem();
        productViewContainer.softwareHeadersModel.SetData("Id", "FeatureId", index);
        productViewContainer.softwareHeadersModel.SetData("Name", qsTr("Feature-ID"), index);

        index = productViewContainer.softwareHeadersModel.InsertNewItem();
        productViewContainer.softwareHeadersModel.SetData("Id", "FeatureDescription", index);
        productViewContainer.softwareHeadersModel.SetData("Name", qsTr("Description"), index);

        productViewContainer.softwareHeadersModel.Refresh();

        tableView_.columnModel = productViewContainer.softwareHeadersModel;
    }

    property TreeItemModel softwareHeadersModel: TreeItemModel {
        Component.onCompleted: {
            productViewContainer.updateHeaders();
        }
    }

    function updateModel(){
        productViewContainer.model.SetData("ProductName", productNameInput.text);

        let productId = productNameInput.text.replace(/\s+/g, '');
        productViewContainer.model.SetData("ProductId", productId);

        if (categoryComboBox.currentIndex == 0){
            productViewContainer.model.SetData("CategoryId", "Software");
        }
        else if (categoryComboBox.currentIndex == 1){
            productViewContainer.model.SetData("CategoryId", "Hardware");
        }
        else{
            productViewContainer.model.SetData("CategoryId", "");
        }

        if (!productViewContainer.model.ContainsKey("Features")){
            productViewContainer.model.SetData("Features", "");
        }
    }

    function updateGui(){
        if (productViewContainer.model.ContainsKey("ProductName")){
            productNameInput.text = productViewContainer.model.GetData("ProductName")
        }
        else{
            productNameInput.text = "";
        }

        let categoryFound = false;
        if (productViewContainer.model.ContainsKey("CategoryId")){
            let categoryId = productViewContainer.model.GetData("CategoryId")
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
        productViewContainer.productFeaturesViewModel.Clear();

        let features = productViewContainer.model.GetData("Features")
        if (!features){
            return;
        }

        let featureIds = features.split(';')

        tableView_.selectedOptionalFeatures = [];

        for (let featureId of featureIds){
            if (featureId.includes('/')){
                tableView_.selectedOptionalFeatures.push(featureId);

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

        tableView_.rowModel = 0;
        tableView_.rowModel = productViewContainer.productFeaturesViewModel;
    }

    function addFeature(featureId){
        let features = productViewContainer.model.GetData("Features")

        let featureIds = []
        if (features !== ""){
            featureIds = features.split(';')
        }

        if (!featureIds.includes(featureId)){
            featureIds.push(featureId)
        }

        productViewContainer.model.SetData("Features", featureIds.join(';'))
    }

    function removeFeature(featureId){
        let features = productViewContainer.model.GetData("Features")

        let featureIds = []

        if (features !== ""){
            featureIds = features.split(';')
        }

        let i = featureIds.indexOf(featureId);
        if (i >= 0){
            featureIds.splice(i, 1);
        }

        productViewContainer.model.SetData("Features", featureIds.join(';'))
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

            changeable: !productViewContainer.readOnly;

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
        id: tableView_;

        anchors.top: headerPanel.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left;
        anchors.right: parent.right;

        readOnly: productViewContainer.readOnly;

        tristate: true;

        property var selectedOptionalFeatures: [];

        Component.onCompleted: {
            let ok = PermissionsController.checkPermission("ChangeProduct");
            tableView_.readOnly = !ok;
        }

        rowDelegate: Component {
            TreeViewItemDelegateBase {
                id: delegate;

                root: tableView_;

                isCheckable: model.Optional ? model.Optional : false;

                onRootDelegateChanged: {
                    if (delegate.rootDelegate !== null){
                        let featureId = model.FeatureId;
                        let rootFeatureUuid = delegate.rootDelegate.itemData.Id;

                        let id = rootFeatureUuid + "/" + featureId;

                        if (tableView_.selectedOptionalFeatures.includes(id)){
                            delegate.checkState = Qt.Checked;
                        }
                    }
                }

                onCheckStateChanged: {
                    let featureId = model.FeatureId;
                    let rootFeatureUuid = delegate.rootDelegate.itemData.Id;

                    let id = rootFeatureUuid + "/" + featureId;

                    let features = productViewContainer.model.GetData("Features");

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
