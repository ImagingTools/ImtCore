import QtQuick 2.12
import imtcontrols 1.0
import Acf 1.0

QtObject{
	id: focusManager

	property var targetList: []
	property Item currentItem: null

	property bool enabled: true

	Component.onCompleted: {
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
												}
											})
		}
	}

	property var shortcut: Shortcut {
		sequence: "Tab"
		enabled: focusManager.enabled
		onActivated: {
			//console.log("TAB")
			if(!focusManager.targetList.length){
				return
			}
			let index = 0
			if(!focusManager.currentItem){
				focusManager.targetList[index].focus = true
				focusManager.targetList[index].forceActiveFocus()
				return
			}
			for(let i = 0; i < focusManager.targetList.length; i++){
				if(focusManager.targetList[i] === focusManager.currentItem){
					if(i < focusManager.targetList.length - 1){
						index = i + 1
					}
					break
				}
			}
			//console.log("INDEX__", index)
			focusManager.targetList[index].focus = true
			focusManager.targetList[index].forceActiveFocus()
		}
	}
}



