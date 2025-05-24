import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

QtObject {
	id: root;

	property var selectedIndexes: []
	property int lastSelectedIndex: -1;
	property int firstSelectedIndex: -1;
	property int focusedIndex: 0;

	signal selectionChanged();

	function resetSelection(){
		selectedIndexes = []

		selectionChanged();
	}

	function toggleSelect(index){
		let position = selectedIndexes.indexOf(index);
		if (position >= 0){
			selectedIndexes.splice(position, 1)
		}
		else{
			selectedIndexes.push(index)
		}

		firstSelectedIndex = index;
		lastSelectedIndex = index;
		focusedIndex = index;

		selectionChanged();
	}

	function singleSelect(index){
		if (selectedIndexes.length == 1 && isSelected(index)){
			return
		}

		selectedIndexes = []
		toggleSelect(index);
	}

	function rangeSelect(fromIndex, toIndex){
		let range = [];
		let start = Math.min(fromIndex, toIndex);
		let end = Math.max(fromIndex, toIndex);

		for (let i = start; i <= end; i++){
			range.push(i);
		}

		firstSelectedIndex = fromIndex;
		selectedIndexes = range;
		lastSelectedIndex = toIndex;
		focusedIndex = toIndex;

		selectionChanged();
	}

	function isSelected(index){
		return selectedIndexes.includes(index);
	}
}


