import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtbaseImtBaseTypesSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0

FilterDelegateBase {
	id: filterDelegate
	isActive: selectionParam ? selectionParam.m_selectedIndex >= 0 : false
	property SelectionParam selectionParam: SelectionParam {
		m_selectedIndex: -1
		onM_selectedIndexChanged: {
			if (m_selectedIndex >= 0 && filterDelegate.isValidModel()){
				filterDelegate.mainButtonText = m_constraints.m_options.get(m_selectedIndex).item.m_name
			}
			else{
				filterDelegate.mainButtonText = filterDelegate.name
			}
		}
	}

	property int visibleItemCount: 5

	signal optionSelectionChanged(var optionIds, var optionIndexes)

	Connections {
		target: filterDelegate.collectionFilter
		
		function onCleared(){
			filterDelegate.clearFilter()
		}
	}

	function setOptionsList(optionsList){
		if (!selectionParam){
			console.error("Unable to set options list. Error: selectionParam is invaid")
			return false
		}

		if (!selectionParam.hasConstraints()){
			selectionParam.createConstraints()
		}

		selectionParam.m_constraints.m_options = optionsList

		return true
	}

	function addOption(option){
		if (!selectionParam){
			console.error("Unable to add option. Error: selectionParam is invaid")
			return false
		}

		if (!selectionParam.hasConstraints()){
			selectionParam.createConstraints()
		}

		if (!selectionParam.m_constraints.hasOptions()){
			selectionParam.m_constraints.createOptions()
		}

		selectionParam.m_constraints.m_options.addElement(option)

		return true
	}

	Component {
		id: optionFactory
		Option {}
	}

	function createAndAddOption(id, name, description, enabled){
		let optionObj = optionFactory.createObject(selectionParam)
		optionObj.m_id = id
		optionObj.m_name = name
		optionObj.m_description = description
		optionObj.m_enabled = enabled

		return addOption(optionObj)
	}

	function removeOption(optionId){
		if (!isValidModel()){
			console.error("Unable to get option index. Error: Invalid model")
			return false
		}

		let optionIndex = getOptionIndex(optionId)
		return removeOptionByIndex(optionIndex)
	}

	function removeOptionByIndex(optionIndex){
		if (!isValidModel()){
			console.error("Unable to get option index. Error: Invalid model")
			return false
		}

		let options = selectionParam.m_constraints.m_options
		if (optionIndex < 0 || optionIndex >= options.count){
			return false
		}

		options.remove(optionIndex)

		return true
	}

	function isValidModel(){
		return	selectionParam &&
				selectionParam.hasConstraints() &&
				selectionParam.m_constraints.hasOptions()
	}

	function getOptionId(optionIndex){
		if (!isValidModel()){
			console.error("Unable to get option ID. Error: Invalid model")
			return ""
		}

		let options = selectionParam.m_constraints.m_options
		if (optionIndex < 0 || optionIndex >= options.count){
			return false
		}

		return options.get(optionIndex).item.m_id
	}

	function getOptionIndex(optionId){
		if (!isValidModel()){
			console.error("Unable to get option index. Error: Invalid model")
			return -1
		}

		let options = selectionParam.m_constraints.m_options
		for (let i = 0; i < options.count; i++){
			let currentOptionId = options.get(i).item.m_id
			if (currentOptionId === optionId){
				return i
			}
		}

		return -1
	}

	onOpenFilter: {
		var point = mapToItem(null, x, y + height);
		ModalDialogManager.openDialog(popupMenuDialogComp, {"x": point.x, "y": point.y})
	}

	onClearFilter: {
		if (selectionParam){
			selectionParam.m_selectedIndex = -1
		}
		
		optionSelectionChanged([], [])
	}

	Component {
		id: popupMenuDialogComp
		PopupMenuDialog {
			id: popupMenuContainer
			model:	filterDelegate.isValidModel() ? filterDelegate.selectionParam.m_constraints.m_options : null
			selectedIndex: filterDelegate.selectionParam ? filterDelegate.selectionParam.m_selectedIndex : -1
			itemWidth: Style.sizeHintXS
			moveToIndex: selectedIndex
			shownItemsCount: filterDelegate.visibleItemCount
			onFinished: {
				if (filterDelegate.selectionParam && index >= 0){
					filterDelegate.selectionParam.m_selectedIndex = index
					if (filterDelegate.selectionParam.m_selectedIndex >= 0 && filterDelegate.isValidModel()){
						let optionId = filterDelegate.selectionParam.m_constraints.m_options.get(index).item.m_id
						filterDelegate.optionSelectionChanged([optionId], [index])
					}
				}
			}

			delegate: Component {PopupMenuDelegate {
					width: popupMenuContainer.width
					height: popupMenuContainer.itemHeight
					text: model.item["m_name"]
					enabled: model.item["m_enabled"]
					selected: filterDelegate.selectionParam.m_selectedIndex === model.index;
					icon.source: selected ? "qrc:/" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal) : ""
					
					onClicked: {
						popupMenuContainer.finished(model.item.m_id, model.index);
					}
				}
			}
		}
	}
}


