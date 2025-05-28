import QtQuick 2.12

QtObject{
	id: root

	property var table: null

	function selectMultipleRange(fromIndex, toIndex){
		if (!table.isMultiSelect) return;

		if (fromIndex === -1 || toIndex === -1) return;

		let result = [];
		if (fromIndex <= toIndex){
			for (let i = fromIndex; i <= toIndex; i++){
				result.push(i);
			}
		} else{
			for (let i = fromIndex; i >= toIndex; i--){
				result.push(i);
			}
		}
		table.selectionManager.selectMultiple(result);
	}

	function onMouseEvent(mouse, index){
		if (table === null){
			console.error("Unable to handle click. Error: Attribute 'table' was not set");
			return;
		}

		let selectionManager = table.selectionManager;

		if (mouse.modifiers & Qt.ControlModifier && table.isMultiSelect){
			selectionManager.toggleSelect(index);
		}
		else if (mouse.modifiers & Qt.ShiftModifier && table.isMultiSelect){
			selectMultipleRange(selectionManager.firstSelectedIndex, index);
		}
		else{
			selectionManager.singleSelect(index);
		}
	}

	function onKeysEvent(event){
		if (table === null){
			console.error("Unable to handle key event. Error: Attribute 'table' was not set");
			return;
		}

		let selectionManager = table.selectionManager;

		let focusedIndex = selectionManager.focusedIndex;
		if (focusedIndex === -1){
			focusedIndex = 0;
		}

		if (event.key === Qt.Key_Up){
			if (focusedIndex > 0){
				focusedIndex--;
				if ((event.modifiers & Qt.ShiftModifier) && table.isMultiSelect){
					selectMultipleRange(selectionManager.firstSelectedIndex, focusedIndex);
				} else{
					selectionManager.singleSelect(focusedIndex);
				}
			}
			event.accepted = true;
		}
		else if (event.key === Qt.Key_Down){
			if (focusedIndex < table.elementsCount - 1){
				focusedIndex++;
				if ((event.modifiers & Qt.ShiftModifier) && table.isMultiSelect){
					selectMultipleRange(selectionManager.firstSelectedIndex, focusedIndex);
				} else{
					selectionManager.singleSelect(focusedIndex);
				}
			}
			event.accepted = true;
		}
		else if (event.key === Qt.Key_Space){
			let index = (selectionManager.focusedIndex !== -1)
				? selectionManager.focusedIndex
				: (table.elementsCount > 0 ? 0 : -1);

			if (index !== -1){
				if ((event.modifiers & Qt.ControlModifier) && table.isMultiSelect){
					selectionManager.toggleSelect(index);
				} else{
					selectionManager.singleSelect(index);
				}
			}
			event.accepted = true;
		}
		else if (event.key === Qt.Key_A){
			if ((event.modifiers & Qt.ControlModifier) && table.isMultiSelect){
				let allIndexes = [];
				for (let i = 0; i < table.elementsCount; i++){
					allIndexes.push(i);
				}
				selectionManager.selectMultiple(allIndexes);
				event.accepted = true;
			}
		}
	}
}
