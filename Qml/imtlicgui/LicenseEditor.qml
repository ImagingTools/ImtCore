import QtQuick 2.12
import Acf 1.0
import imtauthgui 1.0
import imtlicgui 1.0
import imtcolgui 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtlicLicensesSdl 1.0
import imtlicProductsSdl 1.0
import imtlicFeaturesSdl 1.0

ViewBase {
    id: root;

    property TreeItemModel allFeaturesModel: CachedFeatureCollection.collectionModel
    property TreeItemModel featuresModel: TreeItemModel {}
    property TreeItemModel licensesModel: TreeItemModel {}

    property LicenseDefinitionData licenseData: model ? model : null;

    Component.onCompleted: {
        CachedFeatureCollection.updateModel();
        CachedProductCollection.updateModel();
        CachedLicenseCollection.updateModel();
        Events.subscribeEvent("OnLocalizationChanged", onLocalizationChanged)
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", onLocalizationChanged)
    }

    function onLocalizationChanged(language){
        updateLicensesHeaders();
        updateHeaders();
    }

    function updateModel(){
        licenseData.m_licenseName = licenseNameInput.text;
        licenseData.m_licenseId = licenseIdInput.text;
        licenseData.m_description = descriptionInput.text;

        if (productsCB.currentIndex >= 0 && productsCB.model){
            let productId = productsCB.model.getData(ProductItemTypeMetaInfo.s_id, productsCB.currentIndex);
            licenseData.m_productId = productId;
        }
        else{
            licenseData.m_productId = "";
        }

        let indexes = featuresTable.getCheckedItems();
        if (indexes.length > 0 && featuresTable.elements){
            let featuresIds = [];
            for (let index of indexes){
                let featureId = featuresTable.elements.getData(FeatureItemTypeMetaInfo.s_id, index);
                featuresIds.push(featureId);
            }

            licenseData.m_features = featuresIds.join(';');
        }
        else{
            licenseData.m_features = "";
        }

        let licensesIndexes = licensesTable.getCheckedItems();
        if (licensesIndexes.length > 0 && licensesTable.elements){
            let licenseIds = [];
            for (let index of licensesIndexes){
                let id = licensesTable.elements.getData(LicenseItemTypeMetaInfo.s_id, index);
                licenseIds.push(id);
            }

            licenseData.m_parentLicenses = licenseIds.join(';');
        }
        else{
            licenseData.m_parentLicenses = "";
        }
    }

    function updateGui(){
        licenseIdInput.text = licenseData.m_licenseId;
        licenseNameInput.text = licenseData.m_licenseName;
        descriptionInput.text = licenseData.m_description;

        productsCB.currentIndex = -1;

        let productId = licenseData.m_productId;
        if (productsCB.model){
            let productModel = productsCB.model;
            for (let i = 0; i < productModel.getItemsCount(); i++){
                let id = productModel.getData(ProductItemTypeMetaInfo.s_id, i);
                if (id === productId){
                    productsCB.currentIndex = i;
                    break;
                }
            }
        }

        if (productsCB.currentIndex >= 0){
            updateFeaturesModel();
            updateLicensesModel();
        }

        let licenseFeatures = licenseData.m_features;
        let licenseFeatureIds = licenseFeatures.split(';');

        featuresTable.uncheckAll();

        if (featuresTable.elements){
            for (let i = 0; i < featuresTable.elements.getItemsCount(); i++){
                let id = featuresTable.elements.getData(FeatureItemTypeMetaInfo.s_id, i);
                if (licenseFeatureIds.includes(id)){
                    featuresTable.checkItem(i);
                }
            }
        }

        let licenses = licenseData.m_parentLicenses;
        let licenseIds = licenses.split(';');

        licensesTable.uncheckAll();

        if (licensesTable.elements){
            for (let i = 0; i < licensesTable.elements.getItemsCount(); i++){
                let id = licensesTable.elements.getData(LicenseItemTypeMetaInfo.s_id, i);
                if (licenseIds.includes(id)){
                    licensesTable.checkItem(i);
                }
            }
        }
    }

    function updateLicensesGui(){
        let licenses = licenseData.m_parentLicenses;
        let licenseIds = licenses.split(';');

        licensesTable.uncheckAll();

        if (licensesTable.elements){
            for (let i = 0; i < licensesTable.elements.getItemsCount(); i++){
                let id = licensesTable.elements.getData(LicenseItemTypeMetaInfo.s_id, i);
                if (licenseIds.includes(id)){
                    licensesTable.checkItem(i);
                }
            }
        }
    }

    function updateLicensesModel(){
        licensesTable.elements = 0;

        root.licensesModel.clear();

        if (productsCB.currentIndex >= 0 && productsCB.model){
            let productId = productsCB.model.getData(ProductItemTypeMetaInfo.s_id, productsCB.currentIndex);

            for (let i = 0; i < CachedLicenseCollection.collectionModel.getItemsCount(); i++){
                let id = CachedLicenseCollection.collectionModel.getData(LicenseItemTypeMetaInfo.s_id, i)
                let licenses = CachedLicenseCollection.collectionModel.getData(LicenseItemTypeMetaInfo.s_parentLicenses, i)

                let currentObjectId = licenseData.m_id;
                if (id === currentObjectId || licenses.split(';').includes(currentObjectId)){
                    continue;
                }

                let productUuid = CachedLicenseCollection.collectionModel.getData(LicenseItemTypeMetaInfo.s_productUuid, i)
                if (productUuid === productId){
                    let index = root.licensesModel.insertNewItem();

                    root.licensesModel.copyItemDataFromModel(index, CachedLicenseCollection.collectionModel, i);
                }
            }
        }

        licensesTable.elements = root.licensesModel;
    }

    function updateFeaturesModel(){
        featuresTable.elements = 0;
        root.featuresModel.clear();

        if (productsCB.currentIndex >= 0){
            let features = productsCB.model.getData(ProductItemTypeMetaInfo.s_features, productsCB.currentIndex);
            let featureIds = features.split(';');

            for (let i = 0; i < root.allFeaturesModel.getItemsCount(); i++){
                let featureId = root.allFeaturesModel.getData(FeatureItemTypeMetaInfo.s_id, i)
                if (featureIds.includes(featureId)){
                    let index = root.featuresModel.insertNewItem();

                    root.featuresModel.copyItemDataFromModel(index, root.allFeaturesModel, i);
                }
            }
        }

        featuresTable.elements = root.featuresModel;
    }

    function updateFeaturesGui(){
        let licenseFeatures = licenseData.m_features;
        let licenseFeatureIds = licenseFeatures.split(';');
        featuresTable.uncheckAll();

        if (featuresTable.elements){
            for (let i = 0; i < featuresTable.elements.getItemsCount(); i++){
                let id = featuresTable.elements.getData(FeatureItemTypeMetaInfo.s_id, i);

                if (licenseFeatureIds.includes(id)){
                    featuresTable.checkItem(i);
                }
            }
        }
    }

    Rectangle {
        anchors.fill: parent;
        color: Style.backgroundColor2;
    }

    Column {
        id: column;
        anchors.top: parent.top;
        anchors.topMargin: Style.size_mainMargin;
        anchors.left: parent.left;
        anchors.leftMargin: Style.size_mainMargin;
        width: 500;
        spacing: Style.size_mainMargin;

        Text {
            id: titleLicense;

            color: Style.textColor;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSize_common;

            text: qsTr("License Name");
        }

        CustomTextField {
            id: licenseNameInput;

            width: parent.width;
            height: 30;

            placeHolderText: qsTr("Enter the license name");

            Component.onCompleted: {
                let ok = PermissionsController.checkPermission("ChangeLicenseDefinition");
                licenseNameInput.readOnly = !ok;
            }

            onEditingFinished: {
                root.doUpdateModel();
            }

            KeyNavigation.tab: licenseIdInput;
        }

        Text {
            id: titleLicenseId;

            color: Style.textColor;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSize_common;

            text: qsTr("License-ID");
        }

        CustomTextField {
            id: licenseIdInput;

            width: parent.width;
            height: 30;

            placeHolderText: qsTr("Enter the license ID");

            Component.onCompleted: {
                let ok = PermissionsController.checkPermission("ChangeLicenseDefinition");
                licenseIdInput.readOnly = !ok;
            }

            onEditingFinished: {
                root.doUpdateModel();
            }

            KeyNavigation.tab: descriptionInput;
        }

        Text {
            id: titleLicenseDescription;

            color: Style.textColor;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSize_common;

            text: qsTr("Description");
        }

        CustomTextField {
            id: descriptionInput;

            width: parent.width;
            height: 30;

            placeHolderText: qsTr("Enter the license description");

            Component.onCompleted: {
                let ok = PermissionsController.checkPermission("ChangeLicenseDefinition");
                descriptionInput.readOnly = !ok;
            }

            onEditingFinished: {
                root.doUpdateModel();
            }

            KeyNavigation.tab: licenseNameInput;
        }

        Text {
            id: titleProduct;
            text: qsTr("Product");
            color: Style.textColor;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSize_common;
        }

        ComboBox {
            id: productsCB;

            width: parent.width;
            height: 30;

            radius: 3;

            nameId: "ProductName";

            model: CachedProductCollection.collectionModel;

            Component.onCompleted: {
                let ok = PermissionsController.checkPermission("ChangeLicenseDefinition");
                productsCB.changeable = ok;
            }

            onModelChanged: {
                root.doUpdateGui();
            }

            onCurrentIndexChanged: {
                if (root.guiIsBlocked()){
                    return;
                }

                root.updateFeaturesModel()
                root.updateLicensesModel()

                root.doUpdateModel();
            }
        }

        Text {
            color: Style.textColor;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSize_common;

            text: qsTr("Features");
        }
    }

    Table {
        id: featuresTable;

        anchors.top: column.bottom;
        anchors.topMargin: Style.size_mainMargin;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: Style.size_mainMargin;

        width: column.width;
        height: 300;

        checkable: true;

        radius: 0;

        Component.onCompleted: {
            let ok = PermissionsController.checkPermission("ChangeLicenseDefinition");
            featuresTable.readOnly = !ok;
        }

        onCheckedItemsChanged: {
            root.doUpdateModel();
        }
    }

    Item {
        id: dependenciesBlock;

        anchors.top: parent.top;
        anchors.topMargin: Style.size_mainMargin;
        anchors.left: column.right;
        anchors.leftMargin: Style.size_mainMargin;
        anchors.right: parent.right;
        anchors.rightMargin: Style.size_mainMargin;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: Style.size_mainMargin;

        Text {
            id: dependenciesTitle;

            color: Style.textColor;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSize_common;

            text: qsTr("Dependencies");
        }

        Table {
            id: licensesTable;

            anchors.top: dependenciesTitle.bottom;
            anchors.topMargin: 7;
            anchors.bottom: parent.bottom;

            width: parent.width;

            checkable: true;

            radius: 0;

            Component.onCompleted: {
                let ok = PermissionsController.checkPermission("ChangeLicenseDefinition");
                licensesTable.readOnly = !ok;
            }

            onCheckedItemsChanged: {
                root.doUpdateModel();
            }
        }
    }

    TreeItemModel {
        id: featuresHeaders;

        Component.onCompleted: {
            root.updateHeaders();
        }
    }

    TreeItemModel {
        id: licensesHeaders;

        Component.onCompleted: {
            root.updateLicensesHeaders();
        }
    }

    function updateLicensesHeaders(){
        licensesHeaders.clear();

        let index = licensesHeaders.insertNewItem();

        licensesHeaders.setData("Id", LicenseDataTypeMetaInfo.s_licenseName, index);
        licensesHeaders.setData("Name", qsTr("License Name"), index);

        index = licensesHeaders.insertNewItem();

        licensesHeaders.setData("Id", LicenseDataTypeMetaInfo.s_licenseId, index);
        licensesHeaders.setData("Name", qsTr("License-ID"), index);

        licensesHeaders.refresh();

        licensesTable.headers = licensesHeaders;
    }

    function updateHeaders(){
        featuresHeaders.clear();

        let index = featuresHeaders.insertNewItem();

        featuresHeaders.setData("Id", FeatureItemTypeMetaInfo.s_featureName, index);
        featuresHeaders.setData("Name", qsTr("Feature Name"), index);

        featuresHeaders.refresh();

        featuresTable.headers = featuresHeaders;
    }
}
