import QtQuick 2.12
import Acf 1.0
import imtauthgui 1.0
import imtlicgui 1.0
import imtcolgui 1.0
import imtgui 1.0
import imtcontrols 1.0

ViewBase {
    id: root;

    property TreeItemModel allFeaturesModel: CachedFeatureCollection.collectionModel
    property TreeItemModel featuresModel: TreeItemModel {}
    property TreeItemModel licensesModel: TreeItemModel {}

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
        console.log("updateModel1", root.model.toJson());
        root.model.setData("LicenseName", licenseNameInput.text);
        root.model.setData("LicenseId", licenseIdInput.text);
        root.model.setData("LicenseDescription", descriptionInput.text);

        if (productsCB.currentIndex >= 0 && productsCB.model){
            let productId = productsCB.model.getData("Id", productsCB.currentIndex);
            root.model.setData("ProductId", productId);
        }
        else{
            root.model.setData("ProductId", "")
        }

        let indexes = featuresTable.getCheckedItems();
        if (indexes.length > 0 && featuresTable.elements){
            let featuresIds = [];
            for (let index of indexes){
                let featureId = featuresTable.elements.getData("Id", index);
                featuresIds.push(featureId);
            }

            root.model.setData("Features", featuresIds.join(';'));
        }
        else{
            root.model.setData("Features", "");
        }

        let licensesIndexes = licensesTable.getCheckedItems();
        if (licensesIndexes.length > 0 && licensesTable.elements){
            let licenseIds = [];
            for (let index of licensesIndexes){
                let id = licensesTable.elements.getData("Id", index);
                licenseIds.push(id);
            }

            root.model.setData("ParentLicenses", licenseIds.join(';'));
        }
        else{
            root.model.setData("ParentLicenses", "");
        }

//        root.updateFeaturesModel()
//        root.updateLicensesModel()

        console.log("updateModel2", root.model.toJson());
    }

    function updateGui(){
        console.log("updateGui1", root.model.toJson());

        if (root.model.containsKey("LicenseId")){
            licenseIdInput.text = root.model.getData("LicenseId")
        }

        if (root.model.containsKey("LicenseName")){
            licenseNameInput.text = root.model.getData("LicenseName")
        }

        if (root.model.containsKey("LicenseDescription")){
            descriptionInput.text = root.model.getData("LicenseDescription")
        }

        let productFound = false;

        if (root.model.containsKey("ProductId")){
            let productId = root.model.getData("ProductId");
            if (productsCB.model){
                let productModel = productsCB.model;
                for (let i = 0; i < productModel.getItemsCount(); i++){
                    let id = productModel.getData("Id", i);
                    if (id === productId){
                        productsCB.currentIndex = i;
                        productFound = true;
                        break;
                    }
                }
            }
        }

        console.log("productFound", productFound);

        if (!productFound){
            productsCB.currentIndex = -1;
        }
        else{
            updateFeaturesModel();
            updateLicensesModel();
        }

        if (root.model.containsKey("Features")){
            let licenseFeatures = root.model.getData("Features");
            let licenseFeatureIds = licenseFeatures.split(';');

            featuresTable.uncheckAll();

            if (featuresTable.elements){
                for (let i = 0; i < featuresTable.elements.getItemsCount(); i++){
                    let id = featuresTable.elements.getData("Id", i);
                    console.log("id", id);

                    if (licenseFeatureIds.includes(id)){
                        console.log("ok", id);

                        featuresTable.checkItem(i);
                    }
                }
            }
        }

        if (root.model.containsKey("ParentLicenses")){
            let licenses = root.model.getData("ParentLicenses");
            let licenseIds = licenses.split(';');

            licensesTable.uncheckAll();

            if (licensesTable.elements){
                for (let i = 0; i < licensesTable.elements.getItemsCount(); i++){
                    let id = licensesTable.elements.getData("Id", i);
                    if (licenseIds.includes(id)){
                        licensesTable.checkItem(i);
                    }
                }
            }
        }

        console.log("updateGui2", root.model.toJson());
    }

    function updateLicensesGui(){
        if (root.model.containsKey("ParentLicenses")){
            let licenses = root.model.getData("ParentLicenses");
            let licenseIds = licenses.split(';');

            licensesTable.uncheckAll();

            if (licensesTable.elements){
                for (let i = 0; i < licensesTable.elements.getItemsCount(); i++){
                    let id = licensesTable.elements.getData("Id", i);
                    if (licenseIds.includes(id)){
                        licensesTable.checkItem(i);
                    }
                }
            }
        }
    }

    function updateLicensesModel(){
        licensesTable.elements = 0;

        root.licensesModel.clear();

        if (productsCB.currentIndex >= 0 && productsCB.model){
            let productId = productsCB.model.getData("Id", productsCB.currentIndex);

            for (let i = 0; i < CachedLicenseCollection.collectionModel.getItemsCount(); i++){
                let id = CachedLicenseCollection.collectionModel.getData("Id", i)
                let licenses = CachedLicenseCollection.collectionModel.getData("ParentLicenses", i)

                if (root.model.containsKey("Id")){
                    let currentObjectId = root.model.getData("Id")
                    if (id === currentObjectId || licenses.split(';').includes(currentObjectId)){
                        continue;
                    }
                }

                let productUuid = CachedLicenseCollection.collectionModel.getData("ProductUuid", i)
                if (productUuid === productId){
                    let index = root.licensesModel.insertNewItem();

                    root.licensesModel.copyItemDataFromModel(index, CachedLicenseCollection.collectionModel, i);
                }
            }
        }

        licensesTable.elements = root.licensesModel;
    }

    function updateFeaturesModel(){
        console.log("updateFeaturesModel");

        featuresTable.elements = 0;
        root.featuresModel.clear();

        if (productsCB.currentIndex >= 0){
            let features = productsCB.model.getData("Features", productsCB.currentIndex);
            let featureIds = features.split(';');

            for (let i = 0; i < root.allFeaturesModel.getItemsCount(); i++){
                let featureId = root.allFeaturesModel.getData("Id", i)
                if (featureIds.includes(featureId)){
                    let index = root.featuresModel.insertNewItem();

                    root.featuresModel.copyItemDataFromModel(index, root.allFeaturesModel, i);
                }
            }
        }

        featuresTable.elements = root.featuresModel;
    }

    function updateFeaturesGui(){
        console.log("updateFeaturesGui");
        if (root.model.containsKey("Features")){
            let licenseFeatures = root.model.getData("Features");
            let licenseFeatureIds = licenseFeatures.split(';');
            console.log("licenseFeatureIds", licenseFeatureIds);
            console.log("featuresTable.elements", featuresTable.elements);

            featuresTable.uncheckAll();

            if (featuresTable.elements){
                for (let i = 0; i < featuresTable.elements.getItemsCount(); i++){
                    let id = featuresTable.elements.getData("Id", i);
                    console.log("id", id);

                    if (licenseFeatureIds.includes(id)){
                        console.log("ok", id);
                        featuresTable.checkItem(i);
                    }
                }
            }
        }
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }

    Column {
        id: column;

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
        anchors.topMargin: column.spacing;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;

        width: column.width;
        height: 300;

        checkable: true;

        radius: 0;

        Component.onCompleted: {
            let ok = PermissionsController.checkPermission("ChangeLicenseDefinition");
            featuresTable.readOnly = !ok;
        }

        onCheckedItemsChanged: {
            console.log("Table onCheckedItemsChanged", getCheckedItems());

            root.doUpdateModel();
        }
    }

    Item {
        id: dependenciesBlock;

        anchors.top: parent.top;
        anchors.left: column.right;
        anchors.leftMargin: 10;
        anchors.right: parent.right;
        anchors.rightMargin: 10;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;

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

        licensesHeaders.setData("Id", "LicenseName", index);
        licensesHeaders.setData("Name", qsTr("License Name"), index);

        index = licensesHeaders.insertNewItem();

        licensesHeaders.setData("Id", "LicenseId", index);
        licensesHeaders.setData("Name", qsTr("License-ID"), index);

        licensesHeaders.refresh();

        licensesTable.headers = licensesHeaders;
    }

    function updateHeaders(){
        featuresHeaders.clear();

        let index = featuresHeaders.insertNewItem();

        featuresHeaders.setData("Id", "FeatureName", index);
        featuresHeaders.setData("Name", qsTr("Feature Name"), index);

        featuresHeaders.refresh();

        featuresTable.headers = featuresHeaders;
    }
}
