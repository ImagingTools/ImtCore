import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtauthgui 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtlicFeaturesSdl 1.0

ViewBase {
	id: featureEditor;
	
	property TreeItemModel dependenciewViewModel: TreeItemModel {}
	
	property string featureId: "";
	property alias tableView: tableView_;
	
	property FeatureData featureData: model;
	
	property Component treeItemModelComp: Component {
		TreeItemModel {}
	}

	property int __keyCounter: 0
	
	Component.onCompleted: {
		CachedFeatureCollection.updateModel();
	}
	
	LocalizationEvent {
		onLocalizationChanged: {
			featureEditor.onLocalizationChanged(langId)
		}
	}
	
	function onLocalizationChanged(language){
		featureEditor.updateHeaders();
		featureDependenciesView.columns = __buildDependencyColumns();
	}
	
	onFeatureDataChanged: {
		if (featureData){
			if (!featureData.hasSubFeatures()){
				featureData.emplaceSubFeatures()
			}

			tableView_.model = __convertTreeModel(featureData.m_subFeatures, FeatureItemTypeMetaInfo.s_subFeatures);
		}
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

		// Store reference to the original model item for mutation operations
		var itemData = treeModel.get ? treeModel.get(row) : null;

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

		if (!keyVal) keyVal = "row_" + row + "_" + (++featureEditor.__keyCounter);

		return {
			key: keyVal,
			text: textVal,
			data: data,
			children: children,
			checkable: false,
			enabled: true,
			expanded: false,
			checked: Qt.Unchecked,
			itemData: itemData,
			sourceModel: treeModel,
			sourceRow: row
		};
	}
	
	function getAllParents(selectedIndex){
		let retVal = []
		
		if (selectedIndex != null){
			let parentKey = selectedIndex.parentKey;
			
			while (parentKey && parentKey !== "") {
				let parentNode = tableView_.nodeForKey(parentKey);
				if (parentNode) {
					let parentId = parentNode.data ? parentNode.data[FeatureItemTypeMetaInfo.s_featureId] : "";
					if (parentId) retVal.push(parentId);
					parentKey = parentNode.parentKey || "";
				} else {
					break;
				}
			}
		}
		
		return retVal;
	}
	
	function updateTreeViewGui(){
		let selectedIndex = tableView_.currentIndex;
		
		if (selectedIndex == null || !selectedIndex.data){
			return;
		}
		
		featureDependenciesView.__delegateUpdatingBlock = true;
		
		let selectedId = selectedIndex.data[FeatureItemTypeMetaInfo.s_featureId] || "";
		
		let childrenFeatureList = [];
		tableView_.findChildrenFeatureDependencies(selectedId, childrenFeatureList);
		
		let inactiveElements = [];
		tableView_.findParentFeatureDependencies(selectedId, inactiveElements);
		
		let parentIds = featureEditor.getAllParents(selectedIndex);
		inactiveElements = inactiveElements.concat(parentIds);
		
		for (let i = 0; i < parentIds.length; i++){
			let parentId = parentIds[i];
			tableView_.findParentFeatureDependencies(parentId, inactiveElements);
			
			tableView_.findChildrenFeatureDependencies(parentId, childrenFeatureList);
		}
		
		let dependenciesList = []
		
		let dependencies = selectedIndex.data[FeatureItemTypeMetaInfo.s_dependencies] || "";
		if (dependencies && dependencies !== ""){
			dependenciesList = dependencies.split(';');
		}
		
		let allDepNodes = featureDependenciesView.allNodes();
		for (let i = 0; i < allDepNodes.length; i++){
			let node = allDepNodes[i];
			let itemData = node.data || {};
			let itemId = itemData[FeatureItemTypeMetaInfo.s_featureId] || "";
			
			let isVisible = itemId !== selectedId;
			let isActive = !inactiveElements.includes(itemId);
			let isCheckable = itemId !== "";
			let checkState = Qt.Unchecked;
			
			if (childrenFeatureList.includes(itemId) && !dependenciesList.includes(itemId)){
				isActive = false;
				checkState = Qt.Checked;
			}
			else if (childrenFeatureList.includes(itemId) && dependenciesList.includes(itemId)){
				isActive = true;
				checkState = Qt.Checked;
			}
			
			featureDependenciesView.setNodeVisible(node.key, isVisible);
			featureDependenciesView.setNodeEnabled(node.key, isActive);
			featureDependenciesView.setNodeCheckable(node.key, isCheckable);
			if (isCheckable) {
				featureDependenciesView.setCheckStateSilent(node.key, checkState);
			}
		}
		
		featureDependenciesView.__delegateUpdatingBlock = false;
	}
	
	function updateGui(){
		descriptionInput.text = featureData.m_description
		featureNameInput.text = featureData.m_featureName
		featureIdInput.text = featureData.m_featureId
		optionalSwitch.setChecked(featureData.m_optional)
		permissionSwitch.setChecked(featureData.m_isPermission)
		featureEditor.updateTreeViewGui()
	}
	
	function updateModel(){
		featureData.m_description = descriptionInput.text
		featureData.m_featureName = featureNameInput.text
		featureData.m_featureId = featureIdInput.text
		featureData.m_optional = optionalSwitch.checked
		featureData.m_isPermission = permissionSwitch.checked
		tableView_.model = __convertTreeModel(featureData.m_subFeatures, FeatureItemTypeMetaInfo.s_subFeatures)
	}
	
	Rectangle {
		anchors.fill: parent;
		color: Style.backgroundColor2;
	}
	
	Row {
		id: headerPanel;
		anchors.left: parent.left;
		anchors.leftMargin: Style.marginM
		anchors.right: parent.right;
		anchors.rightMargin: Style.marginM
		spacing: Style.marginM;
		height: Style.headerHeight;
		
		Text {
			anchors.verticalCenter: parent.verticalCenter;
			color: Style.buttonTextColor;
			font.family: Style.fontFamilyBold;
			font.pixelSize: Style.fontSizeM;
			text: qsTr("Feature Name");
		}
		
		CustomTextField {
			id: featureNameInput;
			anchors.verticalCenter: parent.verticalCenter;
			width: Style.sizeHintXXS;
			height: Style.controlHeightM;
			placeHolderText: qsTr("Enter the feature name");
			autoEditingFinished: false;
			onEditingFinished: {
				if (featureIdInput.text === ""){
					featureIdInput.text = featureNameInput.text.replace(/\s+/g, '');
				}
				
				featureEditor.doUpdateModel();
			}
		}
		
		Text {
			anchors.verticalCenter: parent.verticalCenter;
			color: Style.buttonTextColor;
			font.family: Style.fontFamilyBold;
			font.pixelSize: Style.fontSizeM;
			text: qsTr("Feature-ID");
		}
		
		CustomTextField {
			id: featureIdInput;
			anchors.verticalCenter: parent.verticalCenter;
			width: Style.sizeHintXXS;
			height: Style.controlHeightM;
			placeHolderText: qsTr("Enter the feature ID");
			onEditingFinished: {
				featureEditor.doUpdateModel();
			}
		}
		
		Text {
			anchors.verticalCenter: parent.verticalCenter;
			color: Style.buttonTextColor;
			font.family: Style.fontFamilyBold;
			font.pixelSize: Style.fontSizeM;
			text: qsTr("Description");
		}
		
		CustomTextField {
			anchors.verticalCenter: parent.verticalCenter;
			id: descriptionInput;
			width: 200;
			height: 30;
			placeHolderText: qsTr("Enter the description");
			onEditingFinished: {
				featureEditor.doUpdateModel();
			}
		}
		
		BaseText {
			anchors.verticalCenter: parent.verticalCenter
			color: Style.buttonTextColor
			font.family: Style.fontFamilyBold
			text: qsTr("Is Optional")
		}
		
		SwitchCustom {
			id: optionalSwitch
			anchors.verticalCenter: parent.verticalCenter
			onCheckedChanged: {
				featureEditor.doUpdateModel()
			}
		}
		
		BaseText {
			anchors.verticalCenter: parent.verticalCenter
			color: Style.buttonTextColor
			font.family: Style.fontFamilyBold
			text: qsTr("Is Permission")
		}
		
		SwitchCustom {
			id: permissionSwitch
			anchors.verticalCenter: parent.verticalCenter
			onCheckedChanged: {
				featureEditor.doUpdateModel()
			}
		}
	}
	
	Item {
		id: centerPanel;
		
		anchors.top: headerPanel.bottom;
		anchors.left: parent.left;
		anchors.leftMargin: Style.marginM;
		anchors.right: rightBlock.left;
		anchors.rightMargin: Style.marginM;
		anchors.bottom: parent.bottom;
		anchors.bottomMargin: Style.marginM;
		
		BasicTreeView {
			id: tableView_;
			anchors.top: parent.top;
			anchors.left: parent.left;
			anchors.bottom: parent.bottom;
			width: parent.width;

			columns: featureEditor.__buildMainColumns()

			onSelectionChanged: {
				let featureId = "";
				
				let selectedIndex = tableView_.currentIndex;
				
				if (selectedIndex != null && selectedIndex.data){
					featureId = selectedIndex.data[FeatureItemTypeMetaInfo.s_featureId] || "";
					if (featureId !== ""){
						featureEditor.updateTreeViewGui();
					}
				}
				
				featureDependenciesView.visible = featureId !== "" && selectedIndex != null;
				
				let removeIsEnabled = selectedIndex != null;
				
				if (featureEditor.commandsController){
					featureEditor.commandsController.setCommandIsEnabled("RemoveFeature", removeIsEnabled)
				}
			}
			
			function findParentFeatureDependencies(featureId, retVal){
				let nodes = tableView_.allNodes();
				for (let i = 0; i < nodes.length; i++){
					let node = nodes[i];
					let itemData = node.data || {};
					let id = itemData[FeatureItemTypeMetaInfo.s_featureId] || "";
					let dependencies = itemData[FeatureItemTypeMetaInfo.s_dependencies] || "";
					if (dependencies !== ""){
						let dependencyList = dependencies.split(';');
						
						if (dependencyList.includes(featureId)){
							if (!retVal.includes(id)){
								retVal.push(id);
								
								tableView_.findParentFeatureDependencies(id, retVal);
								featureDependenciesView.findParentFeatureDependencies(id, retVal);
							}
						}
					}
				}
				
				featureDependenciesView.findParentFeatureDependencies(featureId, retVal);
			}
			
			function findChildrenFeatureDependencies(featureId, retVal){
				let nodes = tableView_.allNodes();
				for (let i = 0; i < nodes.length; i++){
					let node = nodes[i];
					let itemData = node.data || {};
					let id = itemData[FeatureItemTypeMetaInfo.s_featureId] || "";
					
					if (featureId === id){
						let dependencies = itemData[FeatureItemTypeMetaInfo.s_dependencies] || "";
						if (dependencies !== ""){
							let dependencyList = dependencies.split(';');
							
							for (let dependencyId of dependencyList){
								if (!retVal.includes(dependencyId)){
									retVal.push(dependencyId)
									
									tableView_.findChildrenFeatureDependencies(dependencyId, retVal);
									featureDependenciesView.findChildrenFeatureDependencies(dependencyId, retVal);
								}
							}
						}
					}
				}
			}
		}
	}
	
	Item {
		id: rightBlock;
		
		anchors.top: headerPanel.bottom;
		anchors.bottom: parent.bottom;
		anchors.bottomMargin: Style.marginM;
		anchors.right: parent.right;
		anchors.rightMargin: Style.marginM;
		
		width: Style.sizeHintXS;
		
		BasicTreeView {
			id: featureDependenciesView;
			anchors.top: parent.top;
			anchors.bottom: parent.bottom;
			anchors.right: parent.right;
			anchors.left: parent.left;
			clip: true;
			tristate: true;

			columns: featureEditor.__buildDependencyColumns()

			property bool __delegateUpdatingBlock: false
			
			Component.onCompleted: {
				let ok = PermissionsController.checkPermission("ChangeFeature");
				featureDependenciesView.editable = ok;
				featureEditor.dependenciewViewModel.copy(CachedFeatureCollection.collectionModel);
				featureDependenciesView.model = featureEditor.__convertTreeModel(featureEditor.dependenciewViewModel, FeatureItemTypeMetaInfo.s_subFeatures);
				CachedFeatureCollection.modelUpdated.connect(featureDependenciesView.onFeaturesProviderModelChanged);
			}
			
			Component.onDestruction: {
				CachedFeatureCollection.modelUpdated.disconnect(featureDependenciesView.onFeaturesProviderModelChanged);
			}

			function onFeaturesProviderModelChanged(){
				featureEditor.dependenciewViewModel.copy(CachedFeatureCollection.collectionModel)
				featureDependenciesView.model = featureEditor.__convertTreeModel(featureEditor.dependenciewViewModel, FeatureItemTypeMetaInfo.s_subFeatures);
				featureEditor.updateTreeViewGui();
			}

			onCheckStateChanged: {
				if (featureDependenciesView.__delegateUpdatingBlock) return;

				let selectedIndex = tableView_.currentIndex;
				if (selectedIndex == null || !selectedIndex.data) return;

				let nodeData = index.data || {};
				let featureIdVal = nodeData[FeatureItemTypeMetaInfo.s_featureId] || "";
				if (featureIdVal === "") return;

				let selectedId = selectedIndex.data[FeatureItemTypeMetaInfo.s_featureId] || "";
				let dependencies = selectedIndex.data[FeatureItemTypeMetaInfo.s_dependencies] || "";

				let dependencyList = [];
				if (dependencies !== ""){
					dependencyList = dependencies.split(';');
				}

				if (state === Qt.Checked){
					if (!dependencyList.includes(featureIdVal)){
						dependencyList.push(featureIdVal);
					}
				} else {
					let pos = dependencyList.indexOf(featureIdVal);
					if (pos >= 0){
						dependencyList.splice(pos, 1);
					}
				}

				// Update the source model via the main tree
				let newDeps = dependencyList.length > 0 ? dependencyList.join(';') : "";
				let mainNode = tableView_.nodeForKey(selectedIndex.key);
				if (mainNode && mainNode.sourceItem) {
					if (mainNode.sourceItem.setData) {
						mainNode.sourceItem.setData(FeatureItemTypeMetaInfo.s_dependencies, newDeps, 0);
					} else if (mainNode.data) {
						mainNode.data[FeatureItemTypeMetaInfo.s_dependencies] = newDeps;
					}
				}
			}
			
			function findParentFeatureDependencies(featureId, retVal){
				let nodes = featureDependenciesView.allNodes();
				for (let i = 0; i < nodes.length; i++){
					let node = nodes[i];
					let itemData = node.data || {};
					let id = itemData[FeatureItemTypeMetaInfo.s_featureId] || "";
					let dependencies = itemData[FeatureItemTypeMetaInfo.s_dependencies] || "";
					
					if (dependencies !== ""){
						let dependencyList = dependencies.split(';');
						
						if (dependencyList.includes(featureId)){
							if (!retVal.includes(id)){
								retVal.push(id);
								
								featureDependenciesView.findParentFeatureDependencies(id, retVal);
							}
						}
					}
				}
			}
			
			function findChildrenFeatureDependencies(featureId, retVal){
				let nodes = featureDependenciesView.allNodes();
				for (let i = 0; i < nodes.length; i++){
					let node = nodes[i];
					let itemData = node.data || {};
					let id = itemData[FeatureItemTypeMetaInfo.s_featureId] || "";
					
					if (featureId === id){
						let dependencies = itemData[FeatureItemTypeMetaInfo.s_dependencies] || "";
						if (dependencies !== ""){
							let dependencyList = dependencies.split(';');
							
							for (let dependencyId of dependencyList){
								if (!retVal.includes(dependencyId)){
									retVal.push(dependencyId)
									
									featureDependenciesView.findChildrenFeatureDependencies(dependencyId, retVal);
								}
							}
						}
					}
				}
			}
		}
	}
	
	function __buildMainColumns() {
		return [
			{ name: FeatureItemTypeMetaInfo.s_featureName, title: qsTr("Feature Name"), display: "data." + FeatureItemTypeMetaInfo.s_featureName, tree: true, editable: false },
			{ name: FeatureItemTypeMetaInfo.s_featureId, title: qsTr("Feature-ID"), display: "data." + FeatureItemTypeMetaInfo.s_featureId, tree: false, editable: false },
			{ name: FeatureItemTypeMetaInfo.s_description, title: qsTr("Feature Description"), display: "data." + FeatureItemTypeMetaInfo.s_description, tree: false, editable: false },
			{ name: FeatureItemTypeMetaInfo.s_optional, title: qsTr("Optional"), display: "data." + FeatureItemTypeMetaInfo.s_optional, tree: false, editable: false },
			{ name: FeatureItemTypeMetaInfo.s_isPermission, title: qsTr("Is Permission"), display: "data." + FeatureItemTypeMetaInfo.s_isPermission, tree: false, editable: false }
		]
	}

	function __buildDependencyColumns() {
		return [
			{ name: FeatureItemTypeMetaInfo.s_featureName, title: qsTr("Dependencies"), display: "data." + FeatureItemTypeMetaInfo.s_featureName, tree: true, editable: false }
		]
	}

	function updateHeaders(){
		tableView_.columns = __buildMainColumns();
	}
}
