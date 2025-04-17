import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

QtObject {
    id: root;

	property TableBase table: null;

	function onMouseEvent(mouse, index){
		if (!table){
			console.error("Unable to handle clicked on the item. Error: Attribute 'Table' was not set");
			return;
		}

		let tableSelection = table.tableSelection;

		if (mouse.modifiers & Qt.ControlModifier && table.isMultiSelect){
			tableSelection.toggleSelect(index);
		}
		else if (mouse.modifiers & Qt.ShiftModifier && table.isMultiSelect){
			tableSelection.rangeSelect(tableSelection.firstSelectedIndex, index);
		}
		else{
			tableSelection.singleSelect(index);
		}
	}

	function onKeysEvent(event){
		if (!table){
			console.error("Unable to handle clicked on the item. Error: Attribute 'Table' was not set");
			return;
		}

		let tableSelection = table.tableSelection;

		if (event.key === Qt.Key_Up){
			if (tableSelection.focusedIndex > 0){
				tableSelection.focusedIndex--;
				if (event.modifiers & Qt.ShiftModifier && table.isMultiSelect){
					tableSelection.rangeSelect(tableSelection.firstSelectedIndex, tableSelection.focusedIndex);
				}
				else{
					tableSelection.singleSelect(tableSelection.focusedIndex);
				}
			}
			event.accepted = true;
		}
		else if (event.key === Qt.Key_Down){
			if (tableSelection.focusedIndex < table.elementsCount - 1){
				tableSelection.focusedIndex++;
				if (event.modifiers & Qt.ShiftModifier && table.isMultiSelect){
					tableSelection.rangeSelect(tableSelection.firstSelectedIndex, tableSelection.focusedIndex);
				}
				else{
					tableSelection.singleSelect(tableSelection.focusedIndex);
				}
			}
			event.accepted = true;
		}
		else if (event.key === Qt.Key_Space){
			if (event.modifiers & Qt.ControlModifier && table.isMultiSelect){
				tableSelection.toggleSelect(tableSelection.focusedIndex);
			}
			else{
				tableSelection.singleSelect(tableSelection.focusedIndex);
			}
			event.accepted = true;
		}
		else if (event.key === Qt.Key_A){
			if (event.modifiers & Qt.ControlModifier && table.isMultiSelect){
				tableSelection.rangeSelect(0, table.elementsCount - 1);

				event.accepted = true;
			}
		}
	}
}

