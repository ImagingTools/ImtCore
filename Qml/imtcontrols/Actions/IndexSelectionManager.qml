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

	/* ---------- Helpers ---------- */

	function arraysEqual(a, b){
		if (a.length !== b.length)
			return false

		for (let i = 0; i < a.length; ++i){
			if (a[i] !== b[i])
				return false
		}
		return true
	}

	function emitIfChanged(prev, beQuiet){
		if (arraysEqual(prev, selectedIndexes))
			return

		if (!beQuiet)
			selectionChanged(selectedIndexes)
	}

	/* ---------- API ---------- */

	/*!
		\qmlmethod void singleSelect(int index)
	*/
	function singleSelect(index, beQuiet){
		let prev = selectedIndexes.slice()

		if (prev.length === 1 && prev[0] === index)
			return

		selectedIndexes = [index]
		focusedIndex = index
		firstSelectedIndex = index

		emitIfChanged(prev, beQuiet)
	}

	/*!
		\qmlmethod void toggleSelect(int index)
	*/
	function toggleSelect(index, beQuiet){
		let prev = selectedIndexes.slice()
		let idx = selectedIndexes.indexOf(index)

		if (idx === -1){
			selectedIndexes.push(index)
		}
		else{
			if (!multiSelect && selectedIndexes.length === 1)
				return

			selectedIndexes.splice(idx, 1)
		}

		if (arraysEqual(prev, selectedIndexes))
			return

		focusedIndex = index
		firstSelectedIndex = index

		if (!beQuiet)
			selectionChanged(selectedIndexes)
	}

	/*!
		\qmlmethod void selectMultiple(array indexes)
	*/
	function selectMultiple(indexes, beQuiet){
		if (!multiSelect)
			return

		let prev = selectedIndexes.slice()
		let next = indexes.slice()

		if (arraysEqual(prev, next))
			return

		selectedIndexes = next

		if (next.length > 0){
			focusedIndex = next[next.length - 1]
			firstSelectedIndex = next[0]
		}
		else{
			focusedIndex = -1
			firstSelectedIndex = -1
		}

		if (!beQuiet)
			selectionChanged(selectedIndexes)
	}

	/*!
		\qmlmethod void rangeSelect(int from, int to)
	*/
	function rangeSelect(from, to, beQuiet){
		if (!multiSelect)
			return

		let start = Math.min(from, to)
		let end = Math.max(from, to)

		let range = []
		for (let i = start; i <= end; ++i)
			range.push(i)

		if (arraysEqual(selectedIndexes, range))
			return

		selectedIndexes = range
		firstSelectedIndex = from
		focusedIndex = to

		if (!beQuiet)
			selectionChanged(selectedIndexes)
	}

	/*!
		\qmlmethod void clear()
	*/
	function clear(beQuiet){
		if (selectedIndexes.length === 0)
			return

		selectedIndexes = []
		focusedIndex = -1
		firstSelectedIndex = -1

		if (!beQuiet)
			selectionChanged(selectedIndexes)
	}

	/*!
		\qmlmethod bool isSelected(int index)
	*/
	function isSelected(index){
		return selectedIndexes.indexOf(index) !== -1
	}
}
