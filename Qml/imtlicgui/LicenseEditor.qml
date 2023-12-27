import QtQuick 2.12
import Acf 1.0
import imtauthgui 1.0
import imtqml 1.0
import imtlicgui 1.0
import imtdocgui 1.0
import imtgui 1.0
import imtcolgui 1.0

DocumentData {
    id: root;

    property TreeItemModel allFeaturesModel: TreeItemModel {}
    property TreeItemModel featuresModel: TreeItemModel {}
    property TreeItemModel licensesModel: TreeItemModel {}

    documentCompleted: productCollection.completed && licenseCollection.completed;

    Component.onCompleted: {
        productCollection.updateModel();
        licenseCollection.updateModel();
    }

    function blockEditing(){
    }

    function updateModel(){
        console.log("updateModel");
        root.documentModel.SetData("LicenseName", licenseNameInput.text);
        root.documentModel.SetData("LicenseId", licenseIdInput.text);
        root.documentModel.SetData("LicenseDescription", descriptionInput.text);

        if (productsCB.currentIndex >= 0 && productsCB.model){
            let productId = productsCB.model.GetData("Id", productsCB.currentIndex);
            root.documentModel.SetData("ProductId", productId);
        }
        else{
            root.documentModel.SetData("ProductId", "")
        }

        let indexes = featuresTable.getCheckedItems();
        if (indexes.length > 0 && featuresTable.elements){
            let featuresIds = [];
            for (let index of indexes){
                let featureId = featuresTable.elements.GetData("Id", index);
                featuresIds.push(featureId);
            }

            root.documentModel.SetData("Features", featuresIds.join(';'));
        }
        else{
            root.documentModel.SetData("Features", "");
        }

        let licensesIndexes = licensesTable.getCheckedItems();
        if (licensesIndexes.length > 0 && licensesTable.elements){
            let licenseIds = [];
            for (let index of licensesIndexes){
                let id = licensesTable.elements.GetData("Id", index);
                licenseIds.push(id);
            }

            root.documentModel.SetData("ParentLicenses", licenseIds.join(';'));
        }
        else{
            root.documentModel.SetData("ParentLicenses", "");
        }
    }

    function updateGui(){
        console.log("updateGui");

        if (root.documentModel.ContainsKey("LicenseId")){
            licenseIdInput.text = root.documentModel.GetData("LicenseId")
        }

        if (root.documentModel.ContainsKey("LicenseName")){
            licenseNameInput.text = root.documentModel.GetData("LicenseName")
        }

        if (root.documentModel.ContainsKey("LicenseDescription")){
            descriptionInput.text = root.documentModel.GetData("LicenseDescription")
        }

        let productFound = false;

        if (root.documentModel.ContainsKey("ProductId")){
            let productId = root.documentModel.GetData("ProductId");
            if (productsCB.model){
                let productModel = productsCB.model;
                for (let i = 0; i < productModel.GetItemsCount(); i++){
                    let id = productModel.GetData("Id", i);
                    if (id === productId){
                        productsCB.currentIndex = i;
                        productFound = true;
                        break;
                    }
                }
            }
        }

        if (!productFound){
            productsCB.currentIndex = -1;
        }
        else{
            updateFeaturesModel();
            updateLicensesModel();
        }

        updateFeaturesGui();
        updateLicensesGui();
    }

    function updateLicensesGui(){
        if (root.documentModel.ContainsKey("ParentLicenses")){
            let licenses = root.documentModel.GetData("ParentLicenses");
            let licenseIds = licenses.split(';');

            licensesTable.uncheckAll();

            if (licensesTable.elements){
                for (let i = 0; i < licensesTable.elements.GetItemsCount(); i++){
                    let id = licensesTable.elements.GetData("Id", i);
                    if (licenseIds.includes(id)){
                        licensesTable.checkItem(i);
                    }
                }
            }
        }
    }

    function updateLicensesModel(){
        root.licensesModel.Clear();

        if (productsCB.currentIndex >= 0 && productsCB.model){
            let productId = productsCB.model.GetData("Id", productsCB.currentIndex);

            for (let i = 0; i < licenseCollection.collectionModel.GetItemsCount(); i++){
                let id = licenseCollection.collectionModel.GetData("Id", i)
                let licenses = licenseCollection.collectionModel.GetData("ParentLicenses", i)

                if (id === root.documentId || licenses.split(';').includes(root.documentId)){
                    continue;
                }

                let productUuid = licenseCollection.collectionModel.GetData("ProductUuid", i)
                if (productUuid === productId){
                    let index = root.licensesModel.InsertNewItem();

                    root.licensesModel.CopyItemDataFromModel(index, licenseCollection.collectionModel, i);
                }
            }
        }

        licensesTable.elements = root.licensesModel;
    }

    function updateFeaturesModel(){
        root.featuresModel.Clear();

        if (productsCB.currentIndex >= 0){
            let features = productCollection.collectionModel.GetData("Features", productsCB.currentIndex);
            let featureIds = features.split(';');

            for (let i = 0; i < FeaturesProvider.model.GetItemsCount(); i++){
                let featureId = FeaturesProvider.model.GetData("Id", i)
                if (featureIds.includes(featureId)){
                    let index = root.featuresModel.InsertNewItem();

                    root.featuresModel.CopyItemDataFromModel(index, FeaturesProvider.model, i);
                }
            }
        }

        featuresTable.elements = root.featuresModel;
    }

    function updateFeaturesGui(){
        if (root.documentModel.ContainsKey("Features")){
            let licenseFeatures = root.documentModel.GetData("Features");
            let licenseFeatureIds = licenseFeatures.split(';');

            featuresTable.uncheckAll();

            if (featuresTable.elements){
                for (let i = 0; i < featuresTable.elements.GetItemsCount(); i++){
                    let id = featuresTable.elements.GetData("Id", i);
                    if (licenseFeatureIds.includes(id)){
                        featuresTable.checkItem(i);
                    }
                }
            }
        }
    }

    CollectionDataProvider {
        id: productCollection;

        commandId: "Products";

        fields: ["Id", "ProductId", "ProductName", "Features"];

        onCollectionModelChanged: {
            if (productCollection.collectionModel){

                productsCB.model = productCollection.collectionModel;
            }
        }
    }

    CollectionDataProvider {
        id: licenseCollection;

        commandId: "Licenses";

        fields: ["Id", "ProductId", "LicenseId", "LicenseName", "ProductUuid", "ParentLicenses"];
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }

    Column {
        id: column;

        width: 500;

        spacing: 7;

        Text {
            id: titleLicense;

            color: Style.textColor;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSize_common;

            text: qsTr("License Name");
        }

        CustomTextField {
            id: licenseNameInput;

            height: 30;
            width: parent.width;

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

            height: 30;
            width: parent.width;

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

            height: 30;
            width: parent.width;

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
            height: 23;

            radius: 3;

            nameId: "ProductName";

            Component.onCompleted: {
                let ok = PermissionsController.checkPermission("ChangeLicenseDefinition");
                productsCB.changeable = ok;
            }

            onCurrentIndexChanged: {
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

    AuxTable {
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
            root.doUpdateModel();
        }
    }

    Item {
        id: dependenciesBlock;

        width: 300;

        anchors.top: parent.top;
        anchors.left: column.right;
        anchors.leftMargin: 10;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;

        Text {
            id: dependenciesTitle;

            color: Style.textColor;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSize_common;

            text: qsTr("Dependencies");
        }

        AuxTable {
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
        licensesHeaders.Clear();

        let index = licensesHeaders.InsertNewItem();

        licensesHeaders.SetData("Id", "LicenseName", index);
        licensesHeaders.SetData("Name", qsTr("License Name"), index);

        index = licensesHeaders.InsertNewItem();

        licensesHeaders.SetData("Id", "LicenseId", index);
        licensesHeaders.SetData("Name", qsTr("License-ID"), index);

        licensesTable.headers = licensesHeaders;
    }

    function updateHeaders(){
        featuresHeaders.Clear();

        let index = featuresHeaders.InsertNewItem();

        featuresHeaders.SetData("Id", "FeatureName", index);
        featuresHeaders.SetData("Name", qsTr("Feature Name"), index);

        featuresTable.headers = featuresHeaders;
    }
}
