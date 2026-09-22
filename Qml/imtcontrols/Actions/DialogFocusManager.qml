import QtQuick 2.12
import imtcontrols 1.0
import Acf 1.0

QtObject{
	id: focusManager

	property var targetList: []
	property Item currentItem: null
	property ListModel buttonsModel: ListModel{}
	property Item dialog: null
	property int selectedButtonIndex: -1

	property bool enabled: true

	Component.onCompleted: {
	}

	onTargetListChanged: {
		if(targetList && targetList.length){
			setConnections()
		}
	}

	function setConnections(){
		for(let i = 0; i < targetList.length; i++){
			let item = targetList[i]
			item.activeFocusChanged.connect(()=>{if(item.activeFocus) {
													//console.log("SET_ACTIVE_ITEM", item)
													for(let i = 0; i < focusManager.targetList.length; i++){
														if(focusManager.targetList[i] !== item){
															focusManager.targetList[i].focus = false
														}
													}
													focusManager.currentItem = item
													focusManager.dialog.setButtonActive("")
												}
											})
		}
	}

	Shortcut{
		sequence: "Tab"
		enabled: focusManager.enabled
		onActivated: {
			//console.log("TAB")
			if(!focusManager.enabled){
				return
			}
			if(!focusManager.targetList.length && !focusManager.buttonsModel.count){
				return
			}
			if(!focusManager.currentItem){
				focusManager.checkButtons()
			}
			let index = -1
			if(focusManager.targetList.length){
				if(!focusManager.currentItem && focusManager.selectedButtonIndex < 0){
					focusManager.checkButtons()
					if(focusManager.selectedButtonIndex < 0){
						focusManager.targetList[0].focus = true
						focusManager.targetList[0].forceActiveFocus()
						focusManager.dialog.setButtonActive("")
					}

					return
				}
			}
			for(let i = 0; i < focusManager.targetList.length; i++){
				if(focusManager.targetList[i] === focusManager.currentItem){
					if(i < focusManager.targetList.length - 1){
						index = i + 1
					}
					focusManager.selectedButtonIndex = -1
					focusManager.dialog.setButtonActive("")
					break
				}
			}
			if(index == -1){
				focusManager.setButtonFocus()
				if(focusManager.selectedButtonIndex == -1){
					index = 0
					focusManager.dialog.setButtonActive("")
				}
			}
			if(index >=0){
				focusManager.targetList[index].focus = true
				focusManager.targetList[index].forceActiveFocus()
			}
		}
	}

	function setButtonFocus(){
		//console.log("Set button focus")
		if(!buttonsModel.count){
			selectedButtonIndex = -1
			return
		}
		if(selectedButtonIndex == buttonsModel.count - 1){
			if(focusManager.targetList.length){
				selectedButtonIndex = -1
			}
			else {
				let foundIndex = findNearestEnabledButton(0)
				if(foundIndex >=0){
					selectedButtonIndex = findNearestEnabledButton(foundIndex)
					dialog.setButtonActive(buttonsModel.get(foundIndex).id)
				}
				else {
					selectedButtonIndex = -1
				}
			}
			return
		}

		for(let i = 0; i < focusManager.targetList.length; i++){
			focusManager.targetList[i].focus = false
		}

		let foundIndex = findNearestEnabledButton(selectedButtonIndex + 1)
		if(foundIndex >=0){
			selectedButtonIndex = findNearestEnabledButton(selectedButtonIndex + 1)
			let id = buttonsModel.get(selectedButtonIndex).id
			dialog.setButtonActive(id)
			currentItem = null
		}
		else {
			selectedButtonIndex = -1
		}
	}

	function checkButtons(){
		for(let i = 0; i < buttonsModel.count; i++){
			let obj = buttonsModel.get(i)
			if(obj.enabled && obj.active){
				selectedButtonIndex = i
				break
			}
			else if(!obj.enabled && obj.active){
				dialog.setButtonActive("")
				break
			}
		}
	}

	function findNearestEnabledButton(startIndex){
		for(let i = startIndex; i < buttonsModel.count; i++){
			let obj = buttonsModel.get(i)
			if(obj.enabled){
				return i
			}
		}

		return -1;
	}
}



