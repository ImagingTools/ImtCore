import QtQuick 2.15

/*!
	\qmltype IdSelectionManager
	\inqmlmodule imtcontrols
	\brief Manages selection of items by their IDs with support for single and multiple selection modes.
	
	The SelectionManager provides functionality to manage the selection state of items using their unique IDs.
	It supports single and multiple selection modes and exposes properties to track the current selection,
	the focused item, and the first selected item. A selectionChanged signal is emitted whenever the selection changes.
*/
QtObject {
	/*!
		\qmlproperty bool multiSelect
		Enables or disables multiple selection. If false, only single item selection is allowed.
		Default value is true.
	*/
	property bool multiSelect: true
	
	/*!
		\qmlproperty var selectedIds
		Holds an array of currently selected item IDs (as strings).
	*/
	property var selectedIds: []
	
	/*!
		\qmlproperty string focusedId
		The ID of the currently focused item.
	*/
	property string focusedId: ""
	
	/*!
		\qmlproperty string firstSelectedId
		The ID of the first selected item in the current selection.
	*/
	property string firstSelectedId: ""
	
	/*!
		\qmlsignal selectionChanged(var selectedIds)
		Emitted whenever the selection changes.
		\param selectedIds The new array of selected item IDs.
	*/
	signal selectionChanged(var selectedIds)
	
	/*!
		\qmlmethod void singleSelect(string id)
		Selects a single item by its ID, clearing any previous selection.
		Also updates \l focusedId and \l firstSelectedId to the selected ID.
		Emits the \l selectionChanged signal.
		\param id The ID of the item to select.
	*/
	function singleSelect(id){
		selectedIds = [id]
		focusedId = id
		firstSelectedId = id
		selectionChanged(selectedIds)
	}

	function deselect(ids){
		let wasChanges = false
		for (let i = 0; i < ids.length; ++i){
			let index = selectedIds.indexOf(ids[i])
			if (index >= 0){
				wasChanges = true
				selectedIds.splice(index, 1)
			}
		}

		if (wasChanges){
			selectionChanged(selectedIds)
		}
	}
	/*!
		\qmlmethod void toggleSelect(string id)
		Toggles the selection state of the item with the specified ID.
		If the item is not selected, it is added to the selection.
		If it is already selected, it is removed.
		Updates \l focusedId and \l firstSelectedId to the toggled ID.
		Emits the \l selectionChanged signal.
		\param id The ID of the item to toggle.
	*/
	function toggleSelect(id){
		let index = selectedIds.indexOf(id)
		if (index === -1){
			selectedIds.push(id)
		} 
		else{
			selectedIds.splice(index, 1)
		}
		focusedId = id
		firstSelectedId = id
		selectionChanged(selectedIds)
	}
	
	/*!
		\qmlmethod void selectMultiple(array ids)
		Selects multiple items by their IDs.
		Does nothing if \l multiSelect is false.
		Updates \l selectedIds with the provided IDs.
		Sets \l focusedId to the last ID in the array and \l firstSelectedId to the first ID.
		If the array is empty, \l focusedId and \l firstSelectedId are cleared.
		Emits the \l selectionChanged signal.
		\param ids An array of item IDs to select.
	*/
	function selectMultiple(ids){
		if (!multiSelect) return
		selectedIds = ids.slice()
		if (ids.length > 0){
			focusedId = ids[ids.length - 1]
			firstSelectedId = ids[0]
		}
		else {
			focusedId = ""
			firstSelectedId = ""
		}
		selectionChanged(selectedIds)
	}
	
	/*!
		\qmlmethod void clear()
		Clears the current selection and resets \l focusedId and \l firstSelectedId.
		Emits the \l selectionChanged signal.
	*/
	function clear(){
		selectedIds = []
		focusedId = ""
		firstSelectedId = ""
		selectionChanged(selectedIds)
	}
	
	/*!
		\qmlmethod bool isSelected(string id)
		Returns true if the item with the given ID is currently selected, otherwise returns false.
		\param id The ID to check for selection.
		\return true if selected, false otherwise.
	*/
	function isSelected(id){
		return selectedIds.indexOf(id) !== -1
	}
}
