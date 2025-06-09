import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

// ElementView {
// 	id: root
	
// 	height: contentHeight
// 	// contentMargin: 0
	
// 	Component {
// 		id: separatorComp
// 		Rectangle {
// 			width: root.width
// 			height: 1
// 			color: "#cccccc"
// 			objectName: "Separator"
// 		}
// 	}
	
// 	Component.onCompleted: {
// 		for (let i = 0; i < root.children.length; ++i){
// 			console.log("Root child", i, root.children[i])
// 		}

// 		console.log("bottomItem", bottomItem)
		
// 		updateElements()
// 	}
	
// 	function updateElements(){
// 		for (let i = 0; i < root.children.length; ++i){
// 			let child = root.children[i];
// 			if (child.objectName == "Separator"){
// 				child.destroy()
// 			}
// 		}
		
// 		let y = 0
// 		for (let i = 0; i < root.children.length; ++i){
// 			let child = root.children[i];
			
// 			console.log("Item child", i, child)
// 			if (!child.visible || child.objectName == "ElementView" || child.objectName == "Separator"){
// 				continue;
// 			}
			
// 			console.log("ok", i, child)

// 			child.width = root.width
// 			child.visible = false;
// 			// child.parent = bottomItem;
// 			child.border.width = 0
// 			child.y = y;
// 			child.x = 0;
// 			child.visible = true;
// 			y += child.height;
	
// 			if (i < root.children.length - 1){
// 				let sep = separatorComp.createObject(root);
// 				sep.y = y;
// 				y += sep.height;
// 			}
// 		}

// 		bottomItem.height = y;
// 	}
	
// 	bottomComp: Component {
// 		Item {
// 			id: groupContent
// 		}
// 	}
// }

Rectangle {
	id: container;

	width: Style.sizeHintXXL;
	height: Style.sizeHintBXS;

	border.width: 1;
	border.color: Style.borderColor;

	radius: Style.marginXS;

	color: Style.baseColor;

	property int contentHeight: -1;

	clip: true

	function calcHeight(){
		let h = 0;

		for (let i = 0; i < children.length; i++){
			if (children[i].visible && children[i].height >= 0){
				h += children[i].height;

				if (i > 0){
					h -= children[i].border.width
				}
			}
		}

		container.contentHeight = h;
		container.height = container.contentHeight;
	}

	QtObject {
		id: internal;

		property bool block: false;

		function onVisibleChanged(){
			if (!block){
				container.update();
			}
		}
	}

	Component.onCompleted: {
		for (let i = 0; i < children.length; i++){
			children[i].visibleChanged.connect(internal.onVisibleChanged);
			children[i].heightChanged.connect(calcHeight);
		}

		update();
	}

	Component.onDestruction:  {
		for (let i = 0; i < children.length; i++){
			children[i].visibleChanged.disconnect(internal.onVisibleChanged);
			children[i].heightChanged.disconnect(calcHeight);
		}
	}

	function update(){
		internal.block = true;

		let visibleItems = []
		for (let i = 0; i < children.length; i++){
			if (children[i].visible){
				visibleItems.push(children[i])
			}
		}

		for (let i = 0; i < visibleItems.length; i++){
			visibleItems[i].anchors.top = undefined;
			visibleItems[i].anchors.topMargin = 0;

			if (visibleItems[i].bottomRoundedCorners !== undefined){
				visibleItems[i].bottomRoundedCorners = true;
			}

			if (visibleItems[i].topRoundedCorners !== undefined){
				visibleItems[i].topRoundedCorners = true;
			}

			if (visibleItems[i].radius !== undefined){
				visibleItems[i].radius = 0;
			}

			visibleItems[i].anchors.left = container.left;
			visibleItems[i].anchors.right = container.right;

			if (i == 0){
				visibleItems[i].anchors.top = container.top;
				if (visibleItems[i].radius !== undefined){
					visibleItems[i].radius = container.radius;
				}
			}

			if (i == 0 && visibleItems.length > 1){
				if (visibleItems[i].bottomRoundedCorners !== undefined){
					visibleItems[i].bottomRoundedCorners = false;
				}
			}

			if (i != 0 && i == visibleItems.length - 1){
				if (visibleItems[i].radius !== undefined){
					visibleItems[i].radius = container.radius;
				}

				if (visibleItems[i].topRoundedCorners !== undefined){
					visibleItems[i].topRoundedCorners = false;
				}
			}

			if (i > 0){
				visibleItems[i].anchors.top = visibleItems[i - 1].bottom;
				visibleItems[i].anchors.topMargin = -visibleItems[i].border.width;
			}
		}

		internal.block = false;

		calcHeight();
	}
}


