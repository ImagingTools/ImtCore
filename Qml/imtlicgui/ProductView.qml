import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcontrols 1.0
import imtlicProductsSdl 1.0
import imtlicFeaturesSdl 1.0

ViewBase {
    id: productViewContainer;

    property TreeItemModel allFeaturesModel: CachedFeatureCollection.collectionModel;
    property TreeItemModel licensesModel: TreeItemModel {}
    property TreeItemModel productFeaturesViewModel: TreeItemModel {}

    property alias tableView: tableView_;

    property ProductData productData: model ? model : null;

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
        productViewContainer.softwareHeadersModel.clear();

        let index = productViewContainer.softwareHeadersModel.insertNewItem();
        productViewContainer.softwareHeadersModel.setData("id", "FeatureName", index);
        productViewContainer.softwareHeadersModel.setData("name", qsTr("Feature Name"), index);

        index = productViewContainer.softwareHeadersModel.insertNewItem();
        productViewContainer.softwareHeadersModel.setData("id", "FeatureId", index);
        productViewContainer.softwareHeadersModel.setData("name", qsTr("Feature-ID"), index);

        index = productViewContainer.softwareHeadersModel.insertNewItem();
        productViewContainer.softwareHeadersModel.setData("id", "FeatureDescription", index);
        productViewContainer.softwareHeadersModel.setData("name", qsTr("Description"), index);

        productViewContainer.softwareHeadersModel.refresh();

        tableView_.columnModel = productViewContainer.softwareHeadersModel;
    }

    property TreeItemModel softwareHeadersModel: TreeItemModel {
        Component.onCompleted: {
            productViewContainer.updateHeaders();
        }
    }

    function updateModel(){
        productData.m_productName = productNameInput.text;
        productData.m_productId = productNameInput.text.replace(/\s+/g, '');

        if (categoryComboBox.currentIndex == 0){
            productData.m_categoryId = "Software";
        }
        else if (categoryComboBox.currentIndex == 1){
            productData.m_categoryId = "Hardware";
        }
        else{
            productData.m_categoryId = "";
        }
    }

    function updateGui(){
        productNameInput.text = productData.m_productName;

        categoryComboBox.currentIndex = -1;
        let categoryId = productData.m_categoryId;
        if (categoryId === "Software"){
            categoryComboBox.currentIndex = 0;
        }
        else if (categoryId === "Hardware"){
            categoryComboBox.currentIndex = 1;
        }

        updateFeaturesGui();
    }

    function updateFeaturesGui(){
        productViewContainer.productFeaturesViewModel.clear();

        let features = productData.m_features;
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

            for (let i = 0; i < productViewContainer.allFeaturesModel.getItemsCount(); i++){
                let id = productViewContainer.allFeaturesModel.getData("id", i);
                if (featureId === id){
                    let index = productViewContainer.productFeaturesViewModel.insertNewItem();
                    productViewContainer.productFeaturesViewModel.copyItemDataFromModel(index, productViewContainer.allFeaturesModel, i);
                }
            }
        }

        tableView_.rowModel = 0;
        tableView_.rowModel = productViewContainer.productFeaturesViewModel;
    }

    function addFeature(featureId){
        let features = productData.m_features;

        let featureIds = []
        if (features !== ""){
            featureIds = features.split(';')
        }

        if (!featureIds.includes(featureId)){
            featureIds.push(featureId)
        }

        productData.m_features = featureIds.join(';');
    }

    function removeFeature(featureId){
        let features = productData.m_features;

        let featureIds = []

        if (features !== ""){
            featureIds = features.split(';')
        }

        let i = featureIds.indexOf(featureId);
        if (i >= 0){
            featureIds.splice(i, 1);
        }

        productData.m_features = featureIds.join(';');
    }

    TreeItemModel {
        id: modelCategogy;

        Component.onCompleted: {
            let index = modelCategogy.insertNewItem();

            modelCategogy.setData("id", "Software", index);
            modelCategogy.setData("name", "Software", index);

            index = modelCategogy.insertNewItem();

            modelCategogy.setData("id", "Hardware", index);
            modelCategogy.setData("name", "Hardware", index);

            categoryComboBox.model = modelCategogy;
        }
    }

    Rectangle {
        anchors.fill: headerPanel;
        color: Style.backgroundColor2;
    }

    Row {
        id: headerPanel;

        anchors.left: parent.left;
        anchors.leftMargin: Style.sizeMainMargin
        height: 50;

        spacing: Style.sizeMainMargin;

        Text {
            id: productNameLable;

            anchors.verticalCenter: headerPanel.verticalCenter;

            color: Style.buttonTextColor;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSizeNormal;

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

            color: Style.buttonTextColor;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSizeNormal;

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
                childModelKey: FeatureItemTypeMetaInfo.s_subFeatures;

                onRootDelegateChanged: {
                    if (delegate.rootDelegate !== null){
                        let featureId = model.FeatureId;
                        let rootFeatureUuid = delegate.rootDelegate.itemData.id;

                        let id = rootFeatureUuid + "/" + featureId;

                        if (tableView_.selectedOptionalFeatures.includes(id)){
                            delegate.checkState = Qt.Checked;
                        }
                    }
                }

                onCheckStateChanged: {
                    let featureId = model.FeatureId;
                    let rootFeatureUuid = delegate.rootDelegate.itemData.id;

                    let id = rootFeatureUuid + "/" + featureId;

                    let features = productViewContainer.productData.m_features;

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
