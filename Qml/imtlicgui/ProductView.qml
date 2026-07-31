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
            { id: "featureName", name: qsTr("Feature Name"), tree: true, editable: false },
            { id: "featureId", name: qsTr("Feature-ID"), tree: false, editable: false },
            { id: "description", name: qsTr("Description"), tree: false, editable: false }
        ]
    }

    function __buildFeaturesTreeModel() {
        if (!productViewContainer.productFeaturesViewModel)
            return [];

        return TreeModelBuilder.fromTreeItemModelByFields(productViewContainer.productFeaturesViewModel, {
            key: FeatureItemTypeMetaInfo.s_id,
            children: FeatureItemTypeMetaInfo.s_subFeatures,
            checkable: FeatureItemTypeMetaInfo.s_optional,
            columns: {
                id: FeatureItemTypeMetaInfo.s_id,
                featureName: FeatureItemTypeMetaInfo.s_featureName,
                featureId: FeatureItemTypeMetaInfo.s_featureId,
                description: FeatureItemTypeMetaInfo.s_description,
                optional: FeatureItemTypeMetaInfo.s_optional
            }
        });
    }

    // Initializes the check state of optional feature nodes from the product's
    // stored composite ids ("<rootFeatureUuid>/<featureId>").
    function __applyOptionalSelection() {
        let nodes = tableView_.allNodes();
        for (let i = 0; i < nodes.length; ++i) {
            let node = nodes[i];
            if (!node.data || node.data[FeatureItemTypeMetaInfo.s_optional] !== true)
                continue;

            let rootKey = node.key;
            let parentKey = node.parentKey;
            while (parentKey && parentKey !== "") {
                rootKey = parentKey;
                let parentNode = tableView_.nodeForKey(parentKey);
                parentKey = parentNode ? parentNode.parentKey || "" : "";
            }

            let rootNode = tableView_.nodeForKey(rootKey);
            let rootUuid = rootNode && rootNode.data ? rootNode.data[FeatureItemTypeMetaInfo.s_id] || "" : "";
            let compositeId = rootUuid + "/" + (node.data[FeatureItemTypeMetaInfo.s_featureId] || "");

            let checked = tableView_.selectedOptionalFeatures.indexOf(compositeId) >= 0;
            tableView_.setCheckStateSilent(node.key, checked ? Qt.Checked : Qt.Unchecked);
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
        tableView_.selectedOptionalFeatures = [];

        let features = productData.m_features;
        if (!features){
            tableView_.model = [];
            return;
        }

        let featureIds = features.split(';')

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

        tableView_.model = __buildFeaturesTreeModel();
        tableView_.expandAll();
        __applyOptionalSelection();
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
        anchors.right: scrollbar_.left;

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

            // Resolve the top-level ancestor's uuid to build the composite id.
            let rootKey = node.key;
            let parentKey = node.parentKey;
            while (parentKey && parentKey !== "") {
                rootKey = parentKey;
                let parentNode = tableView_.nodeForKey(parentKey);
                parentKey = parentNode ? parentNode.parentKey || "" : "";
            }

            let rootNode = tableView_.nodeForKey(rootKey);
            let rootFeatureUuid = rootNode && rootNode.data ? rootNode.data[FeatureItemTypeMetaInfo.s_id] || "" : "";

            let compositeId = rootFeatureUuid + "/" + featureId;

            // Read the current state live so check/uncheck stays consistent
            // even when selectedOptionalFeatures has not been rebuilt yet.
            let features = productViewContainer.productData.m_features;
            let featureIds = features !== "" ? features.split(';') : [];

            if (state === Qt.Checked) {
                if (!featureIds.includes(compositeId))
                    productViewContainer.addFeature(compositeId);
            } else if (state === Qt.Unchecked) {
                if (featureIds.includes(compositeId))
                    productViewContainer.removeFeature(compositeId);
            }

            tableView_.selectedOptionalFeatures =
                productViewContainer.productData.m_features !== ""
                    ? productViewContainer.productData.m_features.split(';')
                    : [];
        }
    }

    CustomScrollbar {
        id: scrollbar_;
        z: parent.z + 1;
        anchors.right: parent.right;
        anchors.top: tableView_.top;
        anchors.topMargin: tableView_.showHeader ? tableView_.headerHeight + 1 : 0;
        anchors.bottom: tableView_.bottom;
        secondSize: Style.marginM;
        targetItem: tableView_.contentListView;
    }
}
