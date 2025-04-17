import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
	id: container;
	
	width: 700;
	height: 100;

	border.width: 1;
	border.color: Style.borderColor;
	
	radius: Style.sizeSmallMargin;
	
	color: Style.baseColor;
	
	clip: true;
	
	property int contentHeight: -1;
	
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


