import QtQuick 2.12
import imtcontrols 1.0

Item{

	id: resizeItem;

	property int mainSize: 15;
	property Item targetItem: null;
	property Item globalParent: ModalDialogManager.activeView;
	property int gap: 100;
	property int minSize: 100;
	property int cursorAreaSize: 6;
	property bool canCorrectCoord: true;

	signal sizeChanged(real deltaWidth, real deltaHeight);

	function resize(item, globalCoord, deltaX, deltaY){

		if(!resizeItem.targetItem){
			return;
		}

		if(resizeItem.targetItem.centered !== undefined){
			resizeItem.targetItem.centered = false;
		}

		let width_ = resizeItem.targetItem.width
		let height_ = resizeItem.targetItem.height

		let globalParentWidth_ = resizeItem.globalParent.width;
		let globalParentHeight_ = resizeItem.globalParent.height;

		if((globalCoord.x > globalParent.width - gap)||
				(globalCoord.x < gap)||
				(globalCoord.y > globalParent.height - gap) ||
				(globalCoord.y < gap)){

			return;
		}

		if(item === left){
			let newWidth = width_ - deltaX;
			let okX = (newWidth > resizeItem.minSize && newWidth < globalParentWidth_ - resizeItem.gap)

			if(okX){
				resizeItem.targetItem.width -= deltaX;
				if(resizeItem.canCorrectCoord){
					resizeItem.targetItem.x += deltaX;
				}
				resizeItem.sizeChanged(-deltaX, 0);
			}
		}
		else if(item === right){
			let newWidth = width_ + deltaX;
			let okX = (newWidth > resizeItem.minSize && newWidth < globalParentWidth_ - resizeItem.gap)

			if(okX){
				resizeItem.targetItem.width += deltaX;
				resizeItem.sizeChanged(deltaX, 0);
			}
		}
		else if(item === top){
			let newHeight = height_ - deltaY;
			let okY = (newHeight > resizeItem.minSize && newHeight < globalParentHeight_ - resizeItem.gap)

			if(okY){
				resizeItem.targetItem.height -= deltaY;
				if(resizeItem.canCorrectCoord){
					resizeItem.targetItem.y += deltaY;
				}
				resizeItem.sizeChanged(0, -deltaY);
			}
		}
		else if(item === bottom){
			let newHeight = height_ + deltaY;
			let okY = (newHeight > resizeItem.minSize && newHeight < globalParentHeight_ - resizeItem.gap)

			if(okY){
				resizeItem.targetItem.height += deltaY;
				resizeItem.sizeChanged(0, deltaY);
			}
		}
		else if(item === leftTop){
			let newWidth = width_ - deltaX;
			let newHeight = height_ - deltaY;
			let okX = (newWidth > resizeItem.minSize && newWidth < globalParentWidth_ - resizeItem.gap)
			let okY = (newHeight > resizeItem.minSize && newHeight < globalParentHeight_ - resizeItem.gap)
			if(okX && okY){
				resizeItem.targetItem.width -= deltaX;
				resizeItem.targetItem.height -= deltaY;
				if(resizeItem.canCorrectCoord){
					resizeItem.targetItem.x += deltaX;
					resizeItem.targetItem.y += deltaY;
				}
				resizeItem.sizeChanged(-deltaX, -deltaY);
			}
		}
		else if(item === rightTop){
			let newWidth = width_ + deltaX;
			let newHeight = height_ - deltaY;
			let okX = (newWidth > resizeItem.minSize && newWidth < globalParentWidth_ - resizeItem.gap)
			let okY = (newHeight > resizeItem.minSize && newHeight < globalParentHeight_ - resizeItem.gap)
			if(okX && okY){
				resizeItem.targetItem.width += deltaX;
				resizeItem.targetItem.height -= deltaY;
				if(resizeItem.canCorrectCoord){
					resizeItem.targetItem.y += deltaY;
				}
				resizeItem.sizeChanged(deltaX, -deltaY);
			}
		}
		else if(item === leftBottom){
			let newWidth = width_ - deltaX;
			let newHeight = height_ + deltaY;
			let okX = (newWidth > resizeItem.minSize && newWidth < globalParentWidth_ - resizeItem.gap)
			let okY = (newHeight > resizeItem.minSize && newHeight < globalParentHeight_ - resizeItem.gap)
			if(okX && okY){
				resizeItem.targetItem.width -= deltaX;
				resizeItem.targetItem.height += deltaY;
				if(resizeItem.canCorrectCoord){
					resizeItem.targetItem.x += deltaX;
				}
				resizeItem.sizeChanged(-deltaX, deltaY);
			}
		}
		else if(item === rightBottom){
			let newWidth = width_ + deltaX;
			let newHeight = height_ + deltaY;
			let okX = (newWidth > resizeItem.minSize && newWidth < globalParentWidth_ - resizeItem.gap)
			let okY = (newHeight > resizeItem.minSize && newHeight < globalParentHeight_ - resizeItem.gap)
			if(okX && okY){
				resizeItem.targetItem.width += deltaX;
				resizeItem.targetItem.height += deltaY;
				resizeItem.sizeChanged(deltaX, deltaY);
			}
		}
	}

	MouseArea{
		id: leftTop;

		anchors.top: parent.top;
		anchors.left: parent.left;

		width: parent.mainSize;
		height: width;

		cursorShape: isWithinBorder ? Qt.SizeFDiagCursor: Qt.ArrowCursor;
		preventStealing: true;
		hoverEnabled: true;

		property var coord: mapToItem(this,0,0);
		property bool isWithinBorder:  mouseX <= splitterWidth && mouseY <= splitterWidth;
		property int splitterWidth: resizeItem.cursorAreaSize;


		onPressed: {
			if(resizeItem.targetItem && resizeItem.globalParent){
				let coord_ = mapToItem(this,mouse.x,mouse.y);
				this.coord = coord_;
			}
			mouse.accepted = true;
		}
		onPositionChanged: {
			if(resizeItem.targetItem && resizeItem.globalParent && pressed){

				let newCoords = mapToItem(this, mouse.x,mouse.y);
				let deltaX = newCoords.x - this.coord.x;
				let deltaY = newCoords.y - this.coord.y;

				let globalCoord = mapToItem(null, mouse.x,mouse.y);

				resizeItem.resize(this, globalCoord,  deltaX, deltaY)

			}
		}
		// Rectangle{
		// 	anchors.fill: parent
		// 	color: "green"
		// }
	}

	MouseArea{
		id: rightTop;

		anchors.top: parent.top;
		anchors.right: parent.right;

		width: parent.mainSize;
		height: width;

		cursorShape: isWithinBorder ? Qt.SizeBDiagCursor: Qt.ArrowCursor;
		preventStealing: true;
		hoverEnabled: true;

		property var coord: mapToItem(this,0,0);

		property bool isWithinBorder:  mouseX >=  width - splitterWidth && mouseY <= splitterWidth;
		property int splitterWidth: resizeItem.cursorAreaSize;


		onPressed: {
			if(resizeItem.targetItem && resizeItem.globalParent){
				let coord_ = mapToItem(this,mouse.x,mouse.y);
				this.coord = coord_;
			}
			mouse.accepted = true;
		}
		onPositionChanged: {
			if(resizeItem.targetItem && resizeItem.globalParent && pressed){
				let newCoords = mapToItem(this, mouse.x,mouse.y);
				let deltaX = newCoords.x - this.coord.x;
				let deltaY = newCoords.y - this.coord.y;

				let globalCoord = mapToItem(null, mouse.x,mouse.y);

				resizeItem.resize(this, globalCoord,  deltaX, deltaY)

			}
		}
		// Rectangle{
		// 	anchors.fill: parent
		// 	color: "green"
		// }
	}

	MouseArea{
		id: leftBottom;

		anchors.bottom: parent.bottom;
		anchors.left: parent.left;

		width: parent.mainSize;
		height: width;

		cursorShape: isWithinBorder ? Qt.SizeBDiagCursor : Qt.ArrowCursor;
		preventStealing: true;
		hoverEnabled: true;

		property var coord: mapToItem(this,0,0);
		property bool isWithinBorder:  mouseX <= splitterWidth && mouseY >=  height - splitterWidth;
		property int splitterWidth: resizeItem.cursorAreaSize;


		onPressed: {
			if(resizeItem.targetItem && resizeItem.globalParent){
				let coord_ = mapToItem(this,mouse.x,mouse.y);
				this.coord = coord_;
			}
			mouse.accepted = true;
		}
		onPositionChanged: {
			if(resizeItem.targetItem && resizeItem.globalParent && pressed){
				let newCoords = mapToItem(this, mouse.x,mouse.y);
				let deltaX = newCoords.x - this.coord.x;
				let deltaY = newCoords.y - this.coord.y;

				let globalCoord = mapToItem(null, mouse.x,mouse.y);

				resizeItem.resize(this, globalCoord,  deltaX, deltaY)

			}
		}
		// Rectangle{
		// 	anchors.fill: parent
		// 	color: "green"
		// }
	}

	MouseArea{
		id: rightBottom;

		anchors.bottom: parent.bottom;
		anchors.right: parent.right;

		width: parent.mainSize;
		height: width;

		cursorShape: isWithinBorder ? Qt.SizeFDiagCursor : Qt.ArrowCursor;
		preventStealing: true;
		hoverEnabled: true;

		property var coord: mapToItem(this,0,0);

		property bool isWithinBorder:  mouseX >=  width - splitterWidth && mouseY >=  height - splitterWidth;
		property int splitterWidth: resizeItem.cursorAreaSize;

		onPressed: {
			if(resizeItem.targetItem && resizeItem.globalParent){
				let coord_ = mapToItem(this,mouse.x,mouse.y);
				this.coord = coord_;
			}
			mouse.accepted = true;
		}
		onPositionChanged: {
			if(resizeItem.targetItem && resizeItem.globalParent && pressed){
				let newCoords = mapToItem(this, mouse.x,mouse.y);
				let deltaX = newCoords.x - this.coord.x;
				let deltaY = newCoords.y - this.coord.y;

				let globalCoord = mapToItem(null, mouse.x,mouse.y);

				resizeItem.resize(this, globalCoord,  deltaX, deltaY)

			}
		}
		// Rectangle{
		// 	anchors.fill: parent
		// 	color: "green"
		// }
	}

	MouseArea{
		id: top;

		anchors.top: parent.top;
		anchors.left: leftTop.right;
		anchors.right: rightTop.left;

		height: parent.mainSize;

		cursorShape: isWithinBorder ? Qt.SizeVerCursor: Qt.ArrowCursor;
		preventStealing: true;
		hoverEnabled: true;

		property var coord: mapToItem(this,0,0);

		property bool isWithinBorder:  mouseY <= splitterWidth;
		property int splitterWidth: resizeItem.cursorAreaSize;

		onPressed: {
			if(resizeItem.targetItem && resizeItem.globalParent){
				let coord_ = mapToItem(this,mouse.x,mouse.y);
				this.coord = coord_;
			}
			mouse.accepted = true;
		}
		onPositionChanged: {
			if(resizeItem.targetItem && resizeItem.globalParent && pressed){
				let newCoords = mapToItem(this, mouse.x,mouse.y);
				let deltaX = newCoords.x - this.coord.x;
				let deltaY = newCoords.y - this.coord.y;

				let globalCoord = mapToItem(null, mouse.x,mouse.y);

				resizeItem.resize(this, globalCoord,  deltaX, deltaY)

			}
		}

		// Rectangle{
		// 	anchors.fill: parent
		// 	color: "yellow"
		// }
	}

	MouseArea{
		id: bottom;

		anchors.bottom: parent.bottom;
		anchors.left: leftBottom.right;
		anchors.right: rightBottom.left;

		height: parent.mainSize;

		cursorShape: isWithinBorder ? Qt.SizeVerCursor: Qt.ArrowCursor;
		preventStealing: true;
		hoverEnabled: true;

		property var coord: mapToItem(this,0,0);

		property bool isWithinBorder:  mouseY >=  height - splitterWidth;
		property int splitterWidth: resizeItem.cursorAreaSize;


		onPressed: {
			if(resizeItem.targetItem && resizeItem.globalParent){
				let coord_ = mapToItem(this,mouse.x,mouse.y);
				this.coord = coord_;
			}
			mouse.accepted = true;
		}
		onPositionChanged: {
			if(resizeItem.targetItem && resizeItem.globalParent && pressed){
				let newCoords = mapToItem(this, mouse.x,mouse.y);
				let deltaX = newCoords.x - this.coord.x;
				let deltaY = newCoords.y - this.coord.y;

				let globalCoord = mapToItem(null, mouse.x,mouse.y);

				resizeItem.resize(this, globalCoord,  deltaX, deltaY)

			}
		}

		// Rectangle{
		// 	anchors.fill: parent
		// 	color: "yellow"
		// }
	}

	MouseArea{
		id: left;

		anchors.left: parent.left;
		anchors.top: leftTop.bottom;
		anchors.bottom: leftBottom.top;

		width: parent.mainSize;

		cursorShape: isWithinBorder ? Qt.SizeHorCursor : Qt.ArrowCursor;
		preventStealing: true;
		hoverEnabled: true;

		property var coord: mapToItem(this,0,0);

		property bool isWithinBorder:  mouseX <= splitterWidth;
		property int splitterWidth: resizeItem.cursorAreaSize;

		onPressed: {
			if(resizeItem.targetItem && resizeItem.globalParent){
				let coord_ = mapToItem(this,mouse.x,mouse.y);
				this.coord = coord_;
			}
			mouse.accepted = true;
		}
		onPositionChanged: {
			if(resizeItem.targetItem && resizeItem.globalParent && pressed){
				let newCoords = mapToItem(this, mouse.x,mouse.y);
				let deltaX = newCoords.x - this.coord.x;
				let deltaY = newCoords.y - this.coord.y;

				let globalCoord = mapToItem(null, mouse.x,mouse.y);

				resizeItem.resize(this, globalCoord,  deltaX, deltaY)

			}
		}

		// Rectangle{
		// 	anchors.fill: parent
		// 	color: "yellow"
		// }
	}

	MouseArea{
		id: right;

		anchors.right: parent.right;
		anchors.top: rightTop.bottom;
		anchors.bottom: rightBottom.top;

		width: parent.mainSize;

		cursorShape: isWithinBorder ? Qt.SizeHorCursor : Qt.ArrowCursor;
		preventStealing: true;
		hoverEnabled: true;

		property var coord: mapToItem(this,0,0);

		property bool isWithinBorder:  mouseX >=  width - splitterWidth;
		property int splitterWidth: resizeItem.cursorAreaSize;

		onPressed: {

			if(resizeItem.targetItem && resizeItem.globalParent){
				let coord_ = mapToItem(this,mouse.x,mouse.y);
				this.coord = coord_;
			}
			mouse.accepted = true;
		}
		onPositionChanged: {
			if(resizeItem.targetItem && resizeItem.globalParent && pressed){
				let newCoords = mapToItem(this, mouse.x,mouse.y);
				let deltaX = newCoords.x - this.coord.x;
				let deltaY = newCoords.y - this.coord.y;

				let globalCoord = mapToItem(null, mouse.x,mouse.y);

				resizeItem.resize(this, globalCoord,  deltaX, deltaY)

			}
		}

		// Rectangle{
		// 	anchors.fill: parent
		// 	color: "yellow"
		// }
	}
}


// //////////////////////////TEST///////////////////////////////////////

// Rectangle{
// 	anchors.fill: parent;

// 	Rectangle{
// 	x: 700;
// 	y:300;
// 		width: 400;
// 		height: 400;
// 		color: "red";
// 		ResizeItem{
// 			anchors.fill: parent;
// 			targetItem: parent;
// 		}
// 	}
// }
