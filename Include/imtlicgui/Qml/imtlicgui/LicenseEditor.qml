import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtqml 1.0
import imtlicgui 1.0

DocumentBase {
    id: root;

    property TreeItemModel allFeaturesModel: TreeItemModel {}
    property TreeItemModel featuresModel: TreeItemModel {}
    property TreeItemModel licensesModel: TreeItemModel {}

    Component.onCompleted: {
        productCollection.updateModel();
        licenseCollection.updateModel();
    }

    property bool allReady: modelIsReady && productCollection.completed && licenseCollection.completed;

    onAllReadyChanged: {
        if (allReady){
            root.updateGui();

            undoRedoManager.registerModel(documentModel);
        }
    }

    function blockEditing(){
    }

    UndoRedoManager {
        id: undoRedoManager;

        documentBase: root;
        onModelStateChanged: {
            console.log("UndoRedoManager onModelStateChanged");

            root.updateGui();
        }
    }

    function updateModel(){
        if (root.blockUpdatingModel){
            return;
        }

        undoRedoManager.beginChanges();

        root.documentModel.SetData("LicenseName", licenseNameInput.text);
        root.documentModel.SetData("LicenseId", licenseIdInput.text);
        root.documentModel.SetData("LicenseDescription", descriptionInput.text);

        root.documentModel.SetData("ProductId", "");

        if (productsCB.currentIndex >= 0 && productsCB.model){
            let productId = productsCB.model.GetData("Id", productsCB.currentIndex);
            root.documentModel.SetData("ProductId", productId);
        }

        if (!root.documentModel.ContainsKey("Features")){
            root.documentModel.SetData("Features", "");
        }

        if (!root.documentModel.ContainsKey("ParentLicenses")){
            root.documentModel.SetData("ParentLicenses", "");
        }

        undoRedoManager.endChanges();
    }

    function updateGui(){
        root.blockUpdatingModel = true;

        if (root.documentModel.ContainsKey("LicenseId")){
            licenseIdInput.text = root.documentModel.GetData("LicenseId")
        }

        if (root.documentModel.ContainsKey("LicenseName")){
            licenseNameInput.text = root.documentModel.GetData("LicenseName")
        }

        if (root.documentModel.ContainsKey("LicenseDescription")){
            descriptionInput.text = root.documentModel.GetData("LicenseDescription")
        }

        productsCB.currentIndex = -1;
        if (root.documentModel.ContainsKey("ProductId")){
            let productId = root.documentModel.GetData("ProductId");
            let productModel = productsCB.model;
            for (let i = 0; i < productModel.GetItemsCount(); i++){
                let id = productModel.GetData("Id", i);
                if (id === productId){
                    productsCB.currentIndex = i;
                    break;
                }
            }
        }

        updateFeaturesGui();
        updateLicensesGui();

        root.blockUpdatingModel = false;
    }

    function updateLicensesGui(){
        console.log("updateLicensesGui");
        root.blockUpdatingModel = true;

        root.licensesModel.Clear();

        if (productsCB.currentIndex >= 0){
            let productId = productsCB.model.GetData("Id", productsCB.currentIndex);

            for (let i = 0; i < licenseCollection.collectionModel.GetItemsCount(); i++){
                let id = licenseCollection.collectionModel.GetData("Id", i)
                let licenses = licenseCollection.collectionModel.GetData("ParentLicenses", i)

                if (id === root.itemId || licenses.split(';').includes(root.itemId)){
                    continue;
                }

                let productUuid = licenseCollection.collectionModel.GetData("ProductUuid", i)
                if (productUuid === productId){
                    let index = root.licensesModel.InsertNewItem();

                    root.licensesModel.CopyItemDataFromModel(index, licenseCollection.collectionModel, i);
                }
            }
        }

        console.log("root.licensesModel", root.licensesModel.toJSON());

        licensesTable.elements = root.licensesModel;

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

        root.blockUpdatingModel = false;
    }

    function updateFeaturesGui(){
        root.blockUpdatingModel = true;

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

        root.blockUpdatingModel = false;
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
                let oldText = root.documentModel.GetData("LicenseName");
                if (oldText && oldText !== licenseNameInput.text || !oldText && licenseNameInput.text !== ""){
                    root.updateModel();
                }
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
                let oldText = root.documentModel.GetData("LicenseId");
                if (oldText && oldText !== licenseIdInput.text || !oldText && licenseIdInput.text !== ""){
                    root.updateModel();
                }
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
                let oldText = root.documentModel.GetData("LicenseDescription");
                if (oldText && oldText !== descriptionInput.text || !oldText && descriptionInput.text !== ""){
                    root.updateModel();
                }
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
                if (root.blockUpdatingModel){
                    return;
                }

                if (productsCB.currentIndex >= 0){
                    let productId = productsCB.model.GetData("Id", productsCB.currentIndex);
                    let productFeatures = productsCB.model.GetData("Features", productsCB.currentIndex);

                    root.documentModel.SetData("ProductId", productId);
                    root.documentModel.SetData("Features", "");

                    root.updateFeaturesGui();
                    root.updateLicensesGui();

                    undoRedoManager.makeChanges();
                }
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
            if (root.blockUpdatingModel){
                return;
            }

            let indexes = featuresTable.getCheckedItems();
            let features = root.documentModel.GetData("Features");
            let featuresIds = [];
            for (let index of indexes){
                let featureId = featuresTable.elements.GetData("Id", index);
                featuresIds.push(featureId);
            }

            let newFeatures = featuresIds.join(';');
            if (features !== newFeatures){
                root.documentModel.SetData("Features", featuresIds.join(';'));

                undoRedoManager.makeChanges();
            }
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
//            height: parent.height - 50;

            checkable: true;

            radius: 0;

            Component.onCompleted: {
                let ok = PermissionsController.checkPermission("ChangeLicenseDefinition");
                licensesTable.readOnly = !ok;
            }

            onCheckedItemsChanged: {
                if (root.blockUpdatingModel){
                    return;
                }

                let indexes = licensesTable.getCheckedItems();
                let licenses = root.documentModel.GetData("ParentLicenses");
                let licenseIds = [];
                for (let index of indexes){
                    let id = licensesTable.elements.GetData("Id", index);
                    licenseIds.push(id);
                }

                let newLicenses = licenseIds.join(';');
                if (licenses !== newLicenses){
                    root.documentModel.SetData("ParentLicenses", licenseIds.join(';'));

                    undoRedoManager.makeChanges();
                }
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
