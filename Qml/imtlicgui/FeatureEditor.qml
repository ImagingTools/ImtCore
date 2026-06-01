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
	
	Component.onCompleted: {
		CachedFeatureCollection.updateModel();
		featureEditor.__updateCommandStates();
	}

	function __updateCommandStates() {
		if (!commandsController) return
		commandsController.setCommandIsEnabled("InsertFeature", true)
		commandsController.setCommandIsEnabled("RemoveFeature", tableView_.selectedCount > 0)
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
		__connectedSubModels = ({})
		if (featureData){
			if (!featureData.hasSubFeatures()){
				featureData.emplaceSubFeatures()
			}

			tableView_.model = __rebuildMainTree()
		}
	}

	// Track BaseModel instances we've already wired so we don't double-connect.
	property var __connectedSubModels: ({})

	function __rebuildMainTree() {
		let model = __buildMainTreeModel()
		tableView_.model = model
		tableView_.expandAll()
		__connectAllNestedModels()
		return model
	}

	function __onSubFeaturesChanged() {
		__rebuildMainTree()
	}

	function __connectAllNestedModels() {
		if (!featureData || !featureData.m_subFeatures) return
		__walkConnect(featureData.m_subFeatures)
	}

	function __walkConnect(baseModel) {
		if (!baseModel) return
		let key = "" + baseModel
		if (!__connectedSubModels[key]) {
			__connectedSubModels[key] = true
			if (baseModel.countChanged)
				baseModel.countChanged.connect(__onSubFeaturesChanged)
		}
		for (let i = 0; i < baseModel.getItemsCount(); ++i) {
			let entry = baseModel.get(i)
			if (entry && entry.item && entry.item.hasSubFeatures && entry.item.hasSubFeatures())
				__walkConnect(entry.item.m_subFeatures)
		}
	}

	function __buildMainTreeModel() {
		if (!featureData || !featureData.m_subFeatures)
			return []

		return TreeModelBuilder.fromBaseModelByFields(featureData.m_subFeatures, {
			key: "m_featureId",
			children: "m_subFeatures",
			columns: {
				featureName: "m_featureName",
				featureId: "m_featureId",
				description: "m_description",
				optional: "m_optional",
				isPermission: "m_isPermission",
				dependencies: "m_dependencies"
			}
		})
	}

	function __buildDependencyTreeModel() {
		if (!featureEditor.dependenciewViewModel)
			return []

		return TreeModelBuilder.fromTreeItemModelByFields(featureEditor.dependenciewViewModel, {
			key: FeatureItemTypeMetaInfo.s_featureId,
			children: FeatureItemTypeMetaInfo.s_subFeatures,
			columns: {
				featureName: FeatureItemTypeMetaInfo.s_featureName,
				featureId: FeatureItemTypeMetaInfo.s_featureId,
				dependencies: FeatureItemTypeMetaInfo.s_dependencies
			}
		})
	}

	function __getParentSubFeaturesModel(selectedIndex) {
		if (!selectedIndex || !selectedIndex.parentKey)
			return featureData ? featureData.m_subFeatures : null

		let parentNode = tableView_.nodeForKey(selectedIndex.parentKey)
		if (!parentNode || !parentNode.data || !parentNode.data.sourceItem)
			return null

		let parentItem = parentNode.data.sourceItem
		if (!parentItem.m_subFeatures && parentItem.emplaceSubFeatures)
			parentItem.emplaceSubFeatures()
		return parentItem.m_subFeatures
	}

	commandsDelegateComp: Component {
		ViewCommandsDelegateBase {
			view: featureEditor;

			onCommandActivated: {
				let selectedIndex = featureEditor.tableView.currentIndex;

				if (commandId === "InsertFeature") {
					if (!featureEditor.featureData)
						return
					if (!featureEditor.featureData.hasSubFeatures())
						featureEditor.featureData.emplaceSubFeatures()

					let childModel = featureEditor.featureData.m_subFeatures
					if (selectedIndex != null) {
						let selectedNode = featureEditor.tableView.nodeForKey(selectedIndex.key)
						let selectedItem = selectedNode && selectedNode.data ? selectedNode.data.sourceItem : null
						if (selectedItem) {
							if (!selectedItem.hasSubFeatures())
								selectedItem.emplaceSubFeatures()
							childModel = selectedItem.m_subFeatures
						}
					}

					let newFeatureData = featureEditor.featureData.createSubFeaturesArrayElement()
					newFeatureData.m_isPermission = true
					newFeatureData.m_featureName = qsTr("Feature Name")
					childModel.addElement(newFeatureData)

					featureEditor.__rebuildMainTree()
				}
				else if (commandId === "RemoveFeature") {
					if (selectedIndex == null)
						return

					let parentModel = featureEditor.__getParentSubFeaturesModel(selectedIndex)
					if (!parentModel)
						return

					let selectedNode = featureEditor.tableView.nodeForKey(selectedIndex.key)
					let target = selectedNode && selectedNode.data ? selectedNode.data.sourceItem : null
					if (!target)
						return

					for (let i = 0; i < parentModel.getItemsCount(); ++i) {
						let entry = parentModel.get(i)
						if (entry && entry.item === target) {
							parentModel.remove(i)
							break
						}
					}

					featureEditor.__rebuildMainTree()
				}
			}
		}
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
		
		if (selectedIndex == null || tableView_.selectedCount !== 1){
			return;
		}

		let selectedNode = tableView_.nodeForKey(selectedIndex.key);
		let selectedItem = selectedNode && selectedNode.data ? selectedNode.data.sourceItem : null;
		if (!selectedItem) return;

		featureDependenciesView.__delegateUpdatingBlock = true;

		let selectedId = selectedItem.m_featureId || "";

		let childrenFeatureList = [];
		featureDependenciesView.findChildrenFeatureDependencies(selectedId, childrenFeatureList);
		
		let inactiveElements = [];
		featureDependenciesView.findParentFeatureDependencies(selectedId, inactiveElements);
		
		let parentIds = featureEditor.getAllParents(selectedIndex);
		inactiveElements = inactiveElements.concat(parentIds);
		
		for (let i = 0; i < parentIds.length; i++){
			let parentId = parentIds[i];
			featureDependenciesView.findParentFeatureDependencies(parentId, inactiveElements);
			
			featureDependenciesView.findChildrenFeatureDependencies(parentId, childrenFeatureList);
		}
		
		let dependenciesList = []

		// Read dependencies live from the underlying QObject (undo/redo safe).
		let dependencies = selectedItem.m_dependencies || "";
		if (dependencies && dependencies !== ""){
			dependenciesList = dependencies.split(';');
		}
		
		let allDepNodes = featureDependenciesView.allNodes();
		for (let i = 0; i < allDepNodes.length; i++){
			let node = allDepNodes[i];
			let itemData = node.data || {};
			let itemId = itemData[FeatureItemTypeMetaInfo.s_featureId] || "";
			
			let isSelf = itemId === selectedId;
			let isActive = !inactiveElements.includes(itemId) && !isSelf;
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
			else if (dependenciesList.includes(itemId)){
				checkState = Qt.Checked;
			}

			// Disabled nodes (self / cycles / ancestors) keep the checkbox visible
			// but inactive (via setNodeEnabled(false) → CheckBox.isActive=false in the delegate).
			featureDependenciesView.setNodeVisible(node.key, true);
			featureDependenciesView.setNodeEnabled(node.key, isActive);
			featureDependenciesView.setNodeCheckable(node.key, isCheckable);
			featureDependenciesView.setCheckStateSilent(node.key, checkState);
		}
		
		featureDependenciesView.__delegateUpdatingBlock = false;
	}
	
	function updateGui(){
		descriptionInput.text = featureData.m_description
		featureNameInput.text = featureData.m_featureName
		featureIdInput.text = featureData.m_featureId
		optionalSwitch.setChecked(featureData.m_optional)
		permissionSwitch.setChecked(featureData.m_isPermission)
		// Refresh tree cells from the underlying QObjects (undo/redo support).
		// Cells with column.source read live from sourceItem, so a tick bump is enough.
		tableView_.refreshAll()
		featureEditor.updateTreeViewGui()
	}
	
	function updateModel(){
		featureData.m_description = descriptionInput.text
		featureData.m_featureName = featureNameInput.text
		featureData.m_featureId = featureIdInput.text
		featureData.m_optional = optionalSwitch.checked
		featureData.m_isPermission = permissionSwitch.checked
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
			anchors.topMargin: Style.marginS;
			anchors.left: parent.left;
			anchors.bottom: parent.bottom;
			anchors.right: mainScrollbar_.left;
			editable: true
			columns: featureEditor.__buildMainColumns()

			onSelectionChanged: {
				featureEditor.updateTreeViewGui()
				featureEditor.__updateCommandStates()
			}

			onCellEdited: {
				// Auto-fill m_featureId from m_featureName (stripping whitespace)
				// when a new feature's name is first entered.
				if (!column || tableView_.columnKey(column) !== "featureName") return
				let node = tableView_.nodeForKey(index.key)
				let item = node && node.data ? node.data.sourceItem : null
				if (!item) return
				if (item.m_featureId === undefined || item.m_featureId === "") {
					item.m_featureId = String(value).replace(/\s+/g, '')
					tableView_.refreshNode(index.key)
				}
			}
		}

		CustomScrollbar {
			id: mainScrollbar_;
			z: parent.z + 1;
			anchors.right: parent.right;
			anchors.top: tableView_.top;
			anchors.bottom: tableView_.bottom;
			secondSize: Style.marginM;
			targetItem: tableView_.contentListView;
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
			anchors.right: depScrollbar_.left;
			anchors.left: parent.left;
			clip: true;
			tristate: true;

			// Visible only when exactly one element is selected in the main tree.
			visible: tableView_.selectedCount === 1

			columns: featureEditor.__buildDependencyColumns()

			property bool __delegateUpdatingBlock: false
			
			Component.onCompleted: {
				let ok = PermissionsController.checkPermission("ChangeFeature");
				featureDependenciesView.editable = ok;
				featureEditor.dependenciewViewModel.copy(CachedFeatureCollection.collectionModel);
				featureDependenciesView.model = featureEditor.__buildDependencyTreeModel();
				featureDependenciesView.expandAll();
				featureEditor.__markAllDepNodesCheckable();
				CachedFeatureCollection.modelUpdated.connect(featureDependenciesView.onFeaturesProviderModelChanged);
			}
			
			Component.onDestruction: {
				CachedFeatureCollection.modelUpdated.disconnect(featureDependenciesView.onFeaturesProviderModelChanged);
			}

			function onFeaturesProviderModelChanged(){
				featureEditor.dependenciewViewModel.copy(CachedFeatureCollection.collectionModel)
				featureDependenciesView.model = featureEditor.__buildDependencyTreeModel();
				featureDependenciesView.expandAll();
				featureEditor.__markAllDepNodesCheckable();
				featureEditor.updateTreeViewGui();
			}

			onCheckStateChanged: {
				if (featureDependenciesView.__delegateUpdatingBlock) return;

				let selectedIndex = tableView_.currentIndex;
				if (selectedIndex == null) return;

				let selectedNode = tableView_.nodeForKey(selectedIndex.key);
				let selectedItem = selectedNode && selectedNode.data ? selectedNode.data.sourceItem : null;
				if (!selectedItem) return;

				let nodeData = index.data || {};
				let featureIdVal = nodeData[FeatureItemTypeMetaInfo.s_featureId] || "";
				if (featureIdVal === "") return;

				let dependencies = selectedItem.m_dependencies || "";
				let dependencyList = dependencies !== "" ? dependencies.split(';') : [];

				if (state === Qt.Checked){
					if (!dependencyList.includes(featureIdVal))
						dependencyList.push(featureIdVal);
				} else {
					let pos = dependencyList.indexOf(featureIdVal);
					if (pos >= 0) dependencyList.splice(pos, 1);
				}

				let newDeps = dependencyList.length > 0 ? dependencyList.join(';') : "";
				selectedItem.m_dependencies = newDeps;

				// Refresh the main tree cell so the "dependencies" column reflects the change.
				tableView_.refreshNode(selectedIndex.key);
				featureEditor.updateTreeViewGui();
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

		CustomScrollbar {
			id: depScrollbar_;
			z: parent.z + 1;
			anchors.right: parent.right;
			anchors.top: featureDependenciesView.top;
			anchors.bottom: featureDependenciesView.bottom;
			secondSize: Style.marginM;
			targetItem: featureDependenciesView.contentListView;
		}
	}
	
	function __buildMainColumns() {
		return [
			{ id: "featureName", source: "m_featureName", name: qsTr("Feature Name"), tree: true, editable: true },
			{ id: "featureId", source: "m_featureId", name: qsTr("Feature-ID"), tree: false, editable: true },
			{ id: "description", source: "m_description", name: qsTr("Feature Description"), tree: false, editable: true },
			{ id: "optional", source: "m_optional", name: qsTr("Optional"), type: "bool", tree: false, editable: true },
			{ id: "isPermission", source: "m_isPermission", name: qsTr("Is Permission"), type: "bool", tree: false, editable: true }
		]
	}

	function __buildDependencyColumns() {
		return [
			{ id: "featureName", name: qsTr("Dependencies"), tree: true, editable: false }
		]
	}

	// Mark every node in the dependency tree as checkable so the tri-state
	// CheckBox renders. Per-node enabled/checkable refinement happens in
	// updateTreeViewGui based on the current selection in the main tree.
	function __markAllDepNodesCheckable() {
		let nodes = featureDependenciesView.allNodes();
		for (let i = 0; i < nodes.length; ++i)
			featureDependenciesView.setNodeCheckable(nodes[i].key, true);
	}

	function updateHeaders(){
		tableView_.columns = __buildMainColumns();
	}
}
