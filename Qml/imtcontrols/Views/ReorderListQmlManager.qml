import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

QtObject {
	id: manager

	property BaseModel reorderModel: BaseModel{}	
	property Item list

	property int fromIndex: -1
	property int toIndex: -1
	property string selectedId: ""

	property var enteredInfo: []


	function clearSelection(){
		for(let i = 0; i < manager.reorderModel.count; i++){
			enteredInfo[i] = false
		}
	}

	function moveItemDown(index) {
		if (index < manager.reorderModel.count - 1) {
			manager.reorderModel.move(index, index + 1, 1)
			manager.reloadModel()
		}
	}

	function moveItemUp(index) {
		if (index > 0) {
			manager.reorderModel.move(index, index - 1, 1)
			manager.reloadModel()
		}
	}

	function moveItem(fromIndex, toIndex){
		if(fromIndex < 0 || toIndex < 0 || fromIndex == toIndex){
			return
		}
		manager.reorderModel.move(fromIndex, toIndex, 1)
		manager.reloadModel()
	}


	function initEnteredInfo(){
		enteredInfo = []
		for (let i = 0; i < manager.reorderModel.count; i++) {
			enteredInfo.push(false)
		}
	}

	signal reloadModel()

	onReloadModel: {
		list.model = 0
		list.model = manager.reorderModel
	}

	onReorderModelChanged: {
		if(reorderModel){
			initEnteredInfo()
		}
	}
}
