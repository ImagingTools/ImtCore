import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
	id: container

	width: Style.sizeHintXXL
	height: contentHeight > 0 ? contentHeight : Style.sizeHintBXS

	border.width: 1
	border.color: Style.borderColor

	radius: Style.marginXS

	color: Style.baseColor

	property int contentHeight: -1

	clip: true // is obligatory !

	function calcHeight(){
		let h = border.width * 2

		for (let i = 0; i < children.length; i++) {
			let child = children[i]

			if (!child.visible) continue

			if (child.objectName === "Separator"){
				h += child.height
			}
			else{
				h += child.height

				if (i > 0 && child.anchors.topMargin !== undefined){
					h += child.anchors.topMargin
				}
			}
		}

		contentHeight = h
		height = contentHeight
	}

	QtObject {
		id: internal
		property bool block: false
	}

	Component.onCompleted: {
		for (let i = 0; i < children.length; i++){
			children[i].visibleChanged.connect(update)
			children[i].heightChanged.connect(update)
		}

		update()
		calcHeight()
	}

	Component.onDestruction: {
		for (let i = 0; i < children.length; i++){
			children[i].visibleChanged.disconnect(update)
			children[i].heightChanged.disconnect(update)
		}
	}

	Component {
		id: separatorComp
		Rectangle {
			width: container.width
			height: 1
			color: Style.borderColor
			objectName: "Separator"
		}
	}

	function update(){
		if (internal.block) return

		internal.block = true

		// Remove old separators
		for (let i = children.length - 1; i >= 0; i--){
			let child = children[i]
			if (child.objectName === "Separator"){
				child.destroy()
			}
		}

		// Collect only visible non-separator items
		let visibleItems = []
		for (let i = 0; i < children.length; i++){
			let child = children[i]
			if (child.visible && child.objectName !== "Separator") {
				visibleItems.push(child)
			}
		}

		// Position items and add separators
		for (let i = 0; i < visibleItems.length; i++){
			let item = visibleItems[i]
			
			if (item.border){
				item.border.width = 0
			}

			// Reset anchors
			item.anchors.top = undefined
			item.anchors.bottom = undefined

			item.anchors.left = container.left
			item.anchors.right = container.right
			item.anchors.margins = container.border.width

			if (i === 0){
				item.anchors.top = container.top
				// if (item.hasOwnProperty('topRoundedCorners')) {
				// 	item.topRoundedCorners = true
				// }
			} 
			else {
				// Add separator between items
				let separator = separatorComp.createObject(container)

				separator.anchors.top = visibleItems[i-1].bottom
				separator.anchors.left = container.left
				separator.anchors.right = container.right

				item.anchors.top = separator.bottom

				// if (i === visibleItems.length - 1 && item.hasOwnProperty('bottomRoundedCorners')) {
				// 	item.bottomRoundedCorners = true
				// }
			}
		}

		internal.block = false
		calcHeight()
	}
}


