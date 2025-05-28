import QtQuick 2.15

/*!
	\qmltype IndexSelectionManager
	\inqmlmodule imtcontrols
	\brief Manages selection of items by their integer indexes with support for single and multiple selection modes.
*/
QtObject {
	/*!
		\qmlproperty bool multiSelect
		Enables or disables multiple selection. If false, only single item selection is allowed.
		Default value is true.
	*/
	property bool multiSelect: true
	
	/*!
		\qmlproperty var selectedIndexes
		Holds an array of currently selected item indexes (integers).
	*/
	property var selectedIndexes: []
	
	/*!
		\qmlproperty int focusedIndex
		The index of the currently focused item. -1 if none.
	*/
	property int focusedIndex: -1
	
	/*!
		\qmlproperty int firstSelectedIndex
		The index of the first selected item. -1 if none.
	*/
	property int firstSelectedIndex: -1
	
	/*!
		\qmlsignal selectionChanged(var selectedIndexes)
		Emitted whenever the selection changes.
		\param selectedIndexes The new array of selected item indexes.
	*/
	signal selectionChanged(var selectedIndexes)
	
	/*!
		\qmlmethod void singleSelect(int index)
		Selects a single item by its index, clearing any previous selection.
		Also updates \l focusedIndex and \l firstSelectedIndex to the selected index.
		Emits the \l selectionChanged signal.
		\param index The index of the item to select.
	*/
	function singleSelect(index, beQuiet){
		selectedIndexes = [index]
		focusedIndex = index
		firstSelectedIndex = index
		if (!beQuiet){
			selectionChanged(selectedIndexes)
		}
	}
	
	/*!
		\qmlmethod void toggleSelect(int index)
		Toggles the selection state of the item with the specified index.
		If the item is not selected, it is added to the selection.
		If it is already selected, it is removed.
		Updates \l focusedIndex and \l firstSelectedIndex to the toggled index.
		Emits the \l selectionChanged signal.
		\param index The index of the item to toggle.
	*/
	function toggleSelect(index, beQuiet){
		let idx = selectedIndexes.indexOf(index)
		if (idx === -1){
			selectedIndexes.push(index)
		} 
		else{
			selectedIndexes.splice(idx, 1)
		}
		focusedIndex = index
		firstSelectedIndex = index
		if (!beQuiet){
			selectionChanged(selectedIndexes)
		}
	}
	
	/*!
		\qmlmethod void selectMultiple(array indexes)
		Selects multiple items by their indexes.
		Does nothing if \l multiSelect is false.
		Updates \l selectedIndexes with the provided indexes.
		Sets \l focusedIndex to the last index in the array and \l firstSelectedIndex to the first.
		Emits the \l selectionChanged signal.
		\param indexes An array of item indexes to select.
	*/
	function selectMultiple(indexes, beQuiet){
		if (!beQuiet){
			beQuiet = false
		}

		if (!multiSelect) return
		selectedIndexes = indexes.slice()
		if (indexes.length > 0){
			focusedIndex = indexes[indexes.length - 1]
			firstSelectedIndex = indexes[0]
		} else {
			focusedIndex = -1
			firstSelectedIndex = -1
		}
		
		if (!beQuiet){
			selectionChanged(selectedIndexes)
		}
	}
	
	/*!
		\qmlmethod void clear()
		Clears the current selection and resets \l focusedIndex and \l firstSelectedIndex.
		Emits the \l selectionChanged signal.
	*/
	function clear(beQuiet){
		if (!beQuiet){
			beQuiet = false
		}

		selectedIndexes = []
		focusedIndex = -1
		firstSelectedIndex = -1

		if (!beQuiet){
			selectionChanged(selectedIndexes)
		}
	}
	
	/*!
		\qmlmethod bool isSelected(int index)
		Returns true if the item with the given index is currently selected, otherwise returns false.
		\param index The index to check for selection.
		\return true if selected, false otherwise.
	*/
	function isSelected(index){
		return selectedIndexes.indexOf(index) !== -1
	}
	
	/*!
		\qmlmethod void rangeSelect(int from, int to)
		Selects all indexes from `from` to `to` (inclusive).
	*/
	function rangeSelect(from, to, beQuiet){
		if (!multiSelect) return
		
		let start = Math.min(from, to)
		let end = Math.max(from, to)
		let range = []
		for (let i = start; i <= end; ++i){
			range.push(i)
		}
		
		selectedIndexes = range
		firstSelectedIndex = from
		focusedIndex = to

		if (!beQuiet){
			selectionChanged(selectedIndexes)
		}
	}
}
