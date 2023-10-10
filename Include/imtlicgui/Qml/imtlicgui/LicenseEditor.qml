import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0
import imtlicgui 1.0

DocumentBase {
    id: root;

    property TreeItemModel allFeaturesModel: TreeItemModel {}

    property TreeItemModel featuresModel: TreeItemModel {}

    Component.onCompleted: {
        productCollection.updateModel();
    }

    property bool allReady: modelIsReady && productCollection.completed;

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

        AuxTable {
            id: featuresTable;

            width: parent.width;
            height: 300;

            checkable: true;

            radius: 0;

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
    }

    TreeItemModel {
        id: featuresHeaders;

        Component.onCompleted: {
            root.updateHeaders();
        }
    }

    function updateHeaders(){
        featuresHeaders.Clear();

        let index = featuresHeaders.InsertNewItem();

        featuresHeaders.SetData("Id", "FeatureName", index);
        featuresHeaders.SetData("Name", qsTr("Feature Name"), index);

        featuresTable.headers = featuresHeaders;
    }
}
