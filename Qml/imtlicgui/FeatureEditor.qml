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
		dependenciesHeadersModel.updateHeaders();
	}
	
	onFeatureDataChanged: {
		if (featureData){
			if (!featureData.hasSubFeatures()){
				featureData.createSubFeatures()
			}

			tableView_.rowModel = featureData.m_subFeatures;
		}
	}
	
	function getAllParents(selectedIndex){
		let retVal = []
		
		if (selectedIndex != null){
			let parent = selectedIndex.parentIndex;
			
			while (parent && parent.itemData){
				let parentId = parent.itemData.m_featureId
				retVal.push(parentId);
				parent = parent.parentIndex;
			}
		}
		
		return retVal;
	}
	
	function updateTreeViewGui(){
		console.log("FeatureEditor.qml updateTreeViewGui")
		let selectedIndex = null;
		if (tableView.tableSelection.items.length > 0){
			selectedIndex = tableView.tableSelection.items[0];
		}
		
		if (selectedIndex == null || !selectedIndex.itemData){
			return;
		}
		
		featureDependenciesView.delegateUpdatingBlock = true;
		
		let selectedId = "";
		
		if (selectedIndex != null && selectedIndex.itemData){
			selectedId = selectedIndex.itemData.m_featureId;
		}
		
		let childrenFeatureList = [];
		tableView.findChildrenFeatureDependencies(selectedId, childrenFeatureList);
		
		let inactiveElements = [];
		tableView.findParentFeatureDependencies(selectedId, inactiveElements);
		
		let parentIds = featureEditor.getAllParents(selectedIndex);
		inactiveElements = inactiveElements.concat(parentIds);
		
		for (let i = 0; i < parentIds.length; i++){
			let parentId = parentIds[i];
			tableView.findParentFeatureDependencies(parentId, inactiveElements);
			
			tableView.findChildrenFeatureDependencies(parentId, childrenFeatureList);
		}
		
		let dependenciesList = []
		
		let dependencies = selectedIndex.itemData.m_dependencies;
		if (dependencies && dependencies !== ""){
			dependenciesList = dependencies.split(';');
		}
		
		let itemsDataList = featureDependenciesView.getItemsDataAsList();
		for (let i = 0; i < itemsDataList.length; i++){
			let delegateItem = itemsDataList[i]
			let itemData = delegateItem.getItemData();
			let itemId = itemData.featureId;
			
			delegateItem.isVisible = itemId !== selectedId;
			delegateItem.isActive = !inactiveElements.includes(itemId);
			delegateItem.isCheckable = itemId !== "";
			delegateItem.checkState = Qt.Unchecked;
			
			if (childrenFeatureList.includes(itemId) && !dependenciesList.includes(itemId)){
				delegateItem.isActive = false;
				delegateItem.checkState = Qt.Checked;
			}
			else if (childrenFeatureList.includes(itemId) && dependenciesList.includes(itemId)){
				delegateItem.isActive = true;
				delegateItem.checkState = Qt.Checked;
			}
		}
		
		featureDependenciesView.delegateUpdatingBlock = false;
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
		tableView_.rowModel = featureData.m_subFeatures
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
			readOnly: false;
			rowDelegate: Component { PackageViewItemDelegate {
					root: tableView_;
					
					function canRename(id){
						return true;
					}
					
					function featureIsValid(featureId, featureName){
						if (featureEditor.featureData.m_featureId === featureId){
							return false;
						}
						
						let delegates = tableView_.getItemsDataAsList();
						for (let delegate of delegates){
							if (delegate.itemData.m_featureId === featureId){
								return false;
							}
						}
						
						return true;
					}
				} }
			
			Component.onCompleted: {
				let ok = PermissionsController.checkPermission("ChangeFeature");
				tableView_.readOnly = !ok;
			}
			
			onSelectionChanged: {
				let featureId = "";
				
				let selectedIndex =  null;
				if (tableSelection.items.length > 0){
					selectedIndex = tableSelection.items[0];
				}
				
				if (selectedIndex != null){
					featureId = selectedIndex.itemData.m_featureId;
					if (featureId !== ""){
						featureEditor.updateTreeViewGui();
					}
				}
				
				featureDependenciesView.contentVisible = featureId !== "" && selectedIndex != null;
				
				let newIsEnabled = selectedIndex != null;
				let removeIsEnabled = selectedIndex != null;
				
				if (featureEditor.commandsController){
					featureEditor.commandsController.setCommandIsEnabled("RemoveFeature", removeIsEnabled)
				}
			}
			
			function findParentFeatureDependencies(featureId, retVal){
				let itemsDataList = tableView_.getItemsDataAsList();
				for (let i = 0; i < itemsDataList.length; i++){
					let delegateItem = itemsDataList[i]
					let itemData = delegateItem.getItemData();
					let id = itemData.m_featureId;
					let dependencies = itemData.m_dependencies;
					if (dependencies && dependencies !== ""){
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
				let itemsDataList = tableView_.getItemsDataAsList();
				for (let i = 0; i < itemsDataList.length; i++){
					let delegateItem = itemsDataList[i]
					let itemData = delegateItem.getItemData();
					let id = itemData.m_featureId;
					
					if (featureId === id){
						let dependencies = itemData.m_dependencies;
						if (dependencies && dependencies !== ""){
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
			contentVisible: false;
			
			property bool delegateUpdatingBlock: false;
			
			Component.onCompleted: {
				let ok = PermissionsController.checkPermission("ChangeFeature");
				featureDependenciesView.readOnly = !ok;
				dependenciesHeadersModel.updateHeaders();
				featureEditor.dependenciewViewModel.copy(CachedFeatureCollection.collectionModel);
				featureDependenciesView.rowModel = featureEditor.dependenciewViewModel;
				CachedFeatureCollection.modelUpdated.connect(featureDependenciesView.onFeaturesProviderModelChanged);
			}
			
			Component.onDestruction: {
				CachedFeatureCollection.modelUpdated.disconnect(featureDependenciesView.onFeaturesProviderModelChanged);
			}
			
			TreeItemModel {
				id: dependenciesHeadersModel;
				
				function updateHeaders(){
					dependenciesHeadersModel.clear();
					
					let index = dependenciesHeadersModel.insertNewItem();
					dependenciesHeadersModel.setData("id", FeatureItemTypeMetaInfo.s_featureName, index)
					dependenciesHeadersModel.setData("name", qsTr("Dependencies"), index)
					
					dependenciesHeadersModel.refresh();
					
					featureDependenciesView.columnModel = dependenciesHeadersModel;
				}
			}
			
			function onFeaturesProviderModelChanged(){
				featureEditor.dependenciewViewModel.copy(CachedFeatureCollection.collectionModel)
				featureEditor.dependenciewViewModel.refresh();
				featureEditor.updateTreeViewGui();
			}
			
			rowDelegate: Component {TreeViewItemDelegateBase {
					id: delegate;
					root: featureDependenciesView;
					childModelKey: FeatureItemTypeMetaInfo.s_subFeatures;
					onCheckStateChanged: {
						let selectedIndex =  null;
						if (tableView_.tableSelection.items.length > 0){
							selectedIndex = tableView_.tableSelection.items[0];
						}

						if (selectedIndex != null){
							if (!featureDependenciesView.delegateUpdatingBlock){
								if (delegate.itemData.featureId !== ""){
									let featureId = delegate.itemData.featureId;
									let state = delegate.checkState;
									
									let selectedId = selectedIndex.itemData.m_featureId;
									let dependencies = selectedIndex.itemData.m_dependencies;
									
									let dependencyList = []
									if (dependencies != ""){
										dependencyList = dependencies.split(';')
									}
									
									if (state == Qt.Checked){
										if (!dependencyList.includes(featureId)){
											dependencyList.push(featureId);
										}
									}
									else{
										let pos = dependencyList.indexOf(featureId);
										if (pos >= 0){
											dependencyList.splice(pos, 1)
										}
									}
									
									if (dependencyList.length > 0){
										selectedIndex.itemData.m_dependencies = dependencyList.join(';')
									}
									else{
										selectedIndex.itemData.m_dependencies = "";
									}
								}
							}
						}
					}
				}
			}
			
			function findParentFeatureDependencies(featureId, retVal){
				let itemsDataList = featureDependenciesView.getItemsDataAsList();
				for (let i = 0; i < itemsDataList.length; i++){
					let delegateItem = itemsDataList[i]
					let itemData = delegateItem.getItemData();
					let id = itemData.featureId;
					let dependencies = itemData.Dependencies;
					
					if (dependencies && dependencies !== ""){
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
				let itemsDataList = featureDependenciesView.getItemsDataAsList();
				for (let i = 0; i < itemsDataList.length; i++){
					let delegateItem = itemsDataList[i]
					let itemData = delegateItem.getItemData();
					// let rootId = itemData.RootFeatureId;
					
					// if (rootId === featureEditor.featureId){
					//     continue;
					// }
					
					let id = itemData.featureId;
					
					if (featureId === id){
						let dependencies = itemData.Dependencies;
						if (dependencies && dependencies !== ""){
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
	
	function updateHeaders(){
		headersModel.clear();
		
		let index = headersModel.insertNewItem();
		headersModel.setData("id", FeatureItemTypeMetaInfo.s_featureName, index);
		headersModel.setData("name", qsTr("Feature Name"), index);
		
		index = headersModel.insertNewItem();
		headersModel.setData("id", FeatureItemTypeMetaInfo.s_featureId, index);
		headersModel.setData("name", qsTr("Feature-ID"), index);
		
		index = headersModel.insertNewItem();
		headersModel.setData("id", FeatureItemTypeMetaInfo.s_description, index);
		headersModel.setData("name", qsTr("Feature Description"), index);
		
		index = headersModel.insertNewItem();
		headersModel.setData("id", FeatureItemTypeMetaInfo.s_optional, index);
		headersModel.setData("name", qsTr("Optional"), index);
		
		index = headersModel.insertNewItem();
		headersModel.setData("id", FeatureItemTypeMetaInfo.s_isPermission, index);
		headersModel.setData("name", qsTr("Is Permission"), index);
		
		headersModel.refresh();
		
		tableView_.columnModel = headersModel;
	}
	
	TreeItemModel {
		id: headersModel;
		
		Component.onCompleted: {
			featureEditor.updateHeaders();
		}
	}
}
