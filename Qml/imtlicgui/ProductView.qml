import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
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

    property int __keyCounter: 0

    Component.onCompleted: {
        CachedFeatureCollection.updateModel();

        CachedFeatureCollection.modelUpdated.connect(productViewContainer.onFeaturesChanged);
    }

    Component.onDestruction: {
        CachedFeatureCollection.modelUpdated.disconnect(productViewContainer.onFeaturesChanged);
    }
	
	LocalizationEvent {
		onLocalizationChanged: {
			productViewContainer.onLocalizationChanged(langId)
		}
	}

    function onLocalizationChanged(language){
        updateHeaders();
    }

    function onFeaturesChanged(){
        productViewContainer.updateFeaturesGui();
    }

    function updateHeaders(){
        tableView_.columns = __buildColumns();
    }

    function __buildColumns() {
        return [
            { name: "featureName", title: qsTr("Feature Name"), display: "data.featureName", tree: true, editable: false },
            { name: "featureId", title: qsTr("Feature-ID"), display: "data.featureId", tree: false, editable: false },
            { name: "featureDescription", title: qsTr("Description"), display: "data.featureDescription", tree: false, editable: false }
        ]
    }

    function __convertTreeModel(treeModel, childKey) {
        if (!treeModel) return [];
        var count = 0;
        if (treeModel.getItemsCount) count = treeModel.getItemsCount();
        else if (treeModel.count !== undefined) count = treeModel.count;
        else return [];

        var items = [];
        for (var i = 0; i < count; ++i)
            items.push(__convertTreeModelItem(treeModel, i, childKey));
        return items;
    }

    function __convertTreeModelItem(treeModel, row, childKey) {
        var keys = [];
        if (treeModel.getKeys)
            keys = treeModel.getKeys(row);
        else if (treeModel.get) {
            var obj = treeModel.get(row);
            if (obj) keys = Object.keys(obj).filter(function(k) { return k !== "index" && k !== "model" && k !== "context" });
        }

        var data = {};
        var children = [];
        var keyVal = "";
        var textVal = "";

        for (var j = 0; j < keys.length; ++j) {
            var k = keys[j];
            var value;
            if (treeModel.getData)
                value = treeModel.getData(k, row);
            else if (treeModel.get)
                value = treeModel.get(row)[k];
            else
                value = undefined;

            if (childKey && k === childKey && value && typeof value === "object") {
                var childCount = 0;
                if (value.getItemsCount) childCount = value.getItemsCount();
                else if (value.count !== undefined) childCount = value.count;
                for (var c = 0; c < childCount; ++c)
                    children.push(__convertTreeModelItem(value, c, childKey));
            } else {
                data[k] = value;
            }

            if (!keyVal && (k === "id" || k === "key" || k === "m_id"))
                keyVal = String(value || "");
            if (!textVal && (k === "name" || k === "text" || k === "featureName" || k === "m_featureName"))
                textVal = String(value || "");
        }

        if (!keyVal) keyVal = "row_" + row + "_" + (++productViewContainer.__keyCounter);

        return {
            key: keyVal,
            text: textVal,
            data: data,
            children: children,
            checkable: false,
            enabled: true,
            expanded: false,
            checked: Qt.Unchecked
        };
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

        productViewContainer.__keyCounter = 0;
        tableView_.model = __convertTreeModel(productViewContainer.productFeaturesViewModel, FeatureItemTypeMetaInfo.s_subFeatures);
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
        anchors.leftMargin: Style.marginM
		height: Style.headerHeight;

        spacing: Style.marginM;

        Text {
            id: productNameLable;

            anchors.verticalCenter: headerPanel.verticalCenter;

            color: Style.buttonTextColor;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSizeM;

            text: qsTr("Product Name");
        }

        CustomTextField {
            id: productNameInput;

            anchors.verticalCenter: headerPanel.verticalCenter;

			height: Style.controlHeightM;
			width: Style.sizeHintXXS;

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
            font.pixelSize: Style.fontSizeM;

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

        tristate: true;

        columns: productViewContainer.__buildColumns()

        property var selectedOptionalFeatures: [];

        Component.onCompleted: {
            let ok = PermissionsController.checkPermission("ChangeProduct");
            tableView_.editable = ok;
        }

        onCheckStateChanged: {
            let node = index;
            if (!node || !node.data) return;

            let featureId = node.data[FeatureItemTypeMetaInfo.s_featureId] || "";
            if (featureId === "") return;

            // For optional features, build composite id with root feature
            let rootKey = node.key;
            let parentKey = node.parentKey;
            while (parentKey && parentKey !== "") {
                rootKey = parentKey;
                let parentNode = tableView_.nodeForKey(parentKey);
                parentKey = parentNode ? parentNode.parentKey || "" : "";
            }

            let rootNode = tableView_.nodeForKey(rootKey);
            let rootFeatureId = rootNode && rootNode.data ? rootNode.data["id"] || "" : "";

            let compositeId = rootFeatureId + "/" + featureId;

            if (state === Qt.Checked) {
                if (!tableView_.selectedOptionalFeatures.includes(compositeId)) {
                    productViewContainer.addFeature(compositeId);
                }
            } else if (state === Qt.Unchecked) {
                if (tableView_.selectedOptionalFeatures.includes(compositeId)) {
                    productViewContainer.removeFeature(compositeId);
                }
            }
        }
    }
}
