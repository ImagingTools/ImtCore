import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Item{
	id: headerDelegate;

	property TableBase tableItem
	property int columnCount: tableItem ? tableItem.columnCount : 0
	property int columnIndex: model.index
	property string headerId: model.Id;

	property bool compl: false;
	property bool complCompl: headerDelegate.compl && headerDelegate.tableItem;

	property bool textIsCropped: helperText.width > name.width;

	Component.onCompleted: {
		headerDelegate.compl = true;
	}

	Component.onDestruction: {
		if (headerDelegate.tableItem){
			headerDelegate.tableItem.widthRecalc.disconnect(headerDelegate.setCellWidth);
		}
	}

	onComplComplChanged: {
		if(headerDelegate.complCompl){
			headerDelegate.tableItem.widthRecalc.connect(headerDelegate.setCellWidth);
			headerDelegate.setCellWidth();
		}
	}

	function setCellWidth(){
		if (!headerDelegate || headerDelegate.columnCount === 0){
			return
		}

		var defaultWidth = (headerDelegate.width)/headerDelegate.columnCount;
		var widthFromModel = headerDelegate.tableItem.widthDecoratorDynamic.isValidData("Width", headerDelegate.columnIndex) ? headerDelegate.tableItem.widthDecoratorDynamic.getData("Width", headerDelegate.columnIndex) : -1;

		if(!headerDelegate.tableItem.widthDecoratorDynamic.getItemsCount()){
			width = defaultWidth;
		}
		else if(widthFromModel >= 0){
			width = widthFromModel;
		}
		else{
			width = defaultWidth;
		}

		visible = width > 0;
	}
	//borders

	Rectangle{
		id: topBorder;
		anchors.top: parent.top;
		anchors.left: parent.left;
		anchors.right: parent.right;
		height: headerDelegate.tableItem.visibleTopBorderFirst  ? headerDelegate.tableItem.horizontalBorderSize : 0;
		color: headerDelegate.tableItem.borderColorHorizontal;
	}

	Rectangle{
		id: bottomBorder;
		anchors.bottom: parent.bottom;
		anchors.left: parent.left;
		anchors.right: parent.right;
		height: headerDelegate.tableItem.visibleBottomBorderLast ? headerDelegate.tableItem.horizontalBorderSize : 0;
		color:  headerDelegate.tableItem.borderColorHorizontal;
	}

	Rectangle{
		id: leftBorder;
		z: -1;
		anchors.verticalCenter: parent.verticalCenter;
		anchors.left: parent.left;
		width: headerDelegate.tableItem.isRightBorder ? headerDelegate.tableItem.verticalBorderSize * headerDelegate.tableItem.visibleLeftBorderFirst * (headerDelegate.columnIndex == 0)
													  : headerDelegate.tableItem.visibleLeftBorderFirst ? headerDelegate.tableItem.verticalBorderSize : headerDelegate.columnIndex > 0 ? headerDelegate.tableItem.verticalBorderSize : 0;
		height: headerDelegate.tableItem.verticalBorderHeight >= 0 ? headerDelegate.tableItem.verticalBorderHeight :parent.height;
		color:  headerDelegate.tableItem.borderColorVertical;
	}

	Rectangle{
		id: rightBorder;
		anchors.right: parent.right;
		anchors.top: parent.top;
		anchors.bottom: parent.bottom;
		z: -2;
		width: !headerDelegate.tableItem.isRightBorder ?
				   headerDelegate.tableItem.verticalBorderSize * headerDelegate.tableItem.visibleRightBorderLast  * (headerDelegate.columnIndex == (headerDelegate.columnCount -1)) * (headerDelegate.columnCount > 0) :
				   headerDelegate.tableItem.visibleRightBorderLast ? headerDelegate.tableItem.verticalBorderSize  :
																	 headerDelegate.tableItem.verticalBorderSize * (headerDelegate.columnIndex < (headerDelegate.columnCount -1));

		color: headerDelegate.tableItem.borderColorVertical;
	}
	//borders

	Rectangle{
		id: mainRec;
		anchors.top: topBorder.bottom;
		anchors.left: leftBorder.right;
		anchors.right: rightBorder.left;
		anchors.bottom: bottomBorder.top;
		color: headerDelegate.tableItem.emptyDecorHeader ? "transparent" :
														   headerDelegate.tableItem.headerDecorator.isValidData("Color", headerDelegate.columnIndex) ?
															   headerDelegate.tableItem.headerDecorator.getData("Color", headerDelegate.columnIndex) :
															   "transparent";

		opacity:  headerDelegate.tableItem.emptyDecorHeader ? 1 :
															  headerDelegate.tableItem.headerDecorator.isValidData("Opacity", headerDelegate.columnIndex) ?
																  headerDelegate.tableItem.headerDecorator.getData("Opacity", headerDelegate.columnIndex) :
																  1;

		radius: headerDelegate.tableItem.emptyDecorHeader ? 0 :
															headerDelegate.tableItem.headerDecorator.isValidData("CellRadius", headerDelegate.columnIndex) ?
																headerDelegate.tableItem.headerDecorator.getData("CellRadius", headerDelegate.columnIndex) :0;

		//cornerPatches
		Rectangle{
			id: leftTopCornerPatch;
			anchors.left: parent.left;
			anchors.top: parent.top;
			width: parent.width/2;
			height: parent.height/2;
			color: parent.color;
			visible: headerDelegate.tableItem.emptyDecorHeader ? true :
																 headerDelegate.tableItem.headerDecorator.isValidData("LeftTopRound", headerDelegate.columnIndex) ?
																	 !headerDelegate.tableItem.headerDecorator.getData("LeftTopRound", headerDelegate.columnIndex) :true;
		}

		Rectangle{
			id: rightTopCornerPatch;
			anchors.right: parent.right;
			anchors.top: parent.top;
			width: parent.width/2;
			height: parent.height/2;
			color: parent.color;
			visible: headerDelegate.tableItem.emptyDecorHeader ? true :
																 headerDelegate.tableItem.headerDecorator.isValidData("RightTopRound", headerDelegate.columnIndex) ?
																	 !headerDelegate.tableItem.headerDecorator.getData("RightTopRound", headerDelegate.columnIndex) :true;
		}

		Rectangle{
			id: leftBottomCornerPatch;
			anchors.left: parent.left;
			anchors.bottom: parent.bottom;
			width: parent.width/2;
			height: parent.height/2;
			color: parent.color;
			visible: headerDelegate.tableItem.emptyDecorHeader ? true :
																 headerDelegate.tableItem.headerDecorator.isValidData("LeftBottomRound", headerDelegate.columnIndex) ?
																	 !headerDelegate.tableItem.headerDecorator.getData("LeftBottomRound", headerDelegate.columnIndex) :true;
		}

		Rectangle{
			id: rightBottomCornerPatch;
			anchors.right:  parent.right;
			anchors.bottom: parent.bottom;
			width: parent.width/2;
			height: parent.height/2;
			color: parent.color;
			visible: headerDelegate.tableItem.emptyDecorHeader ? true :
																 headerDelegate.tableItem.headerDecorator.isValidData("RightBottomRound", headerDelegate.columnIndex) ?
																	 !headerDelegate.tableItem.headerDecorator.getData("RightBottomRound", headerDelegate.columnIndex) :true;
		}
		//cornerPatches
	}//mainRec

	Text {
		id: name;

		anchors.verticalCenter: mainRec.verticalCenter;
		anchors.left: mainRec.left;
		anchors.leftMargin: headerDelegate.tableItem.textMarginHor;
		anchors.right: iconSort.visible ? iconSort.left : mainRec.right;
		anchors.rightMargin: iconSort.visible ? 0 : headerDelegate.tableItem.textMarginHor;

		verticalAlignment: Text.AlignVCenter;
		horizontalAlignment: headerDelegate.tableItem.emptyDecorHeader ? Text.AlignLeft :
																		 headerDelegate.tableItem.headerDecorator.isValidData("TextPosition", headerDelegate.columnIndex) ?
																			 headerDelegate.tableItem.headerDecorator.getData("TextPosition", headerDelegate.columnIndex) :
																			 Text.AlignLeft;


		font.pixelSize: headerDelegate.tableItem.emptyDecorHeader ? Style.fontSize_common * headerDelegate.scale :
																	headerDelegate.tableItem.headerDecorator.isValidData("FontSize", headerDelegate.columnIndex) ?
																		headerDelegate.tableItem.headerDecorator.getData("FontSize", headerDelegate.columnIndex) :
																		Style.fontSize_common * headerDelegate.scale;


		font.family: Style.fontFamilyBold;

		font.bold: headerDelegate.tableItem.emptyDecorHeader ? true :
															   headerDelegate.tableItem.headerDecorator.isValidData("FontBold", headerDelegate.columnIndex) ?
																   headerDelegate.tableItem.headerDecorator.getData("FontBold", headerDelegate.columnIndex) :
																   true;


		color: headerDelegate.tableItem.emptyDecorHeader ? Style.textColor :
														   headerDelegate.tableItem.headerDecorator.isValidData("FontColor", headerDelegate.columnIndex) ?
															   headerDelegate.tableItem.headerDecorator.getData("FontColor", headerDelegate.columnIndex) :
															   Style.textColor;
		elide: headerDelegate.tableItem.elideMode;

		wrapMode: headerDelegate.tableItem.wrapMode;

		onLinkActivated: {
			Qt.openUrlExternally(link)
		}

		text: model.Name;
	}

	Text {
		id: helperText;

		font.pixelSize: name.font.pixelSize;
		font.family: name.font.family;
		font.bold: name.font.bold;

		visible: false;

		text: name.text;
	}

	Image {
		id: iconSort;

		anchors.verticalCenter: mainRec.verticalCenter;
		anchors.right: mainRec.right;
		anchors.rightMargin: Style.size_mainMargin;

		width: visible ? 12 : 0;
		height: width;

		visible: headerDelegate.tableItem.currentHeaderId === model.Id && headerDelegate.tableItem.hasSort;
		rotation: headerDelegate.tableItem.currentSortOrder == "ASC" ? 180 : 0

		sourceSize.width: width;
		sourceSize.height: height;

		source: headerDelegate.tableItem.sortIndicatorIcon
	}

	////
	MouseArea {
		id: headerMa;
		anchors.fill: parent;
		hoverEnabled: true;
		visible: headerDelegate.tableItem.hasSort || headerDelegate.textIsCropped || headerDelegate.tableItem.editableHeaderParams;
		acceptedButtons: Qt.LeftButton | Qt.RightButton;


		onReleased: {
			if(headerDelegate.tableItem.hasSort){
				headerDelegate.scale = 1;
			}
		}

		onPressed: {
			if(headerDelegate.tableItem.hasSort){
				headerDelegate.scale = 0.985;
			}
		}

		onClicked: {
			if(headerDelegate.tableItem.hasSort || headerDelegate.tableItem.editableHeaderParams){
				if (mouse.button === Qt.LeftButton) {
					headerDelegate.tableItem.headerClicked(model.Id);
				}
				else if (mouse.button === Qt.RightButton){
					headerDelegate.tableItem.headerRightMouseClicked(model.Id);
				}
			}
		}
	}

	TooltipArea {
		id: tooltipArea;

		anchors.fill: parent;

		mouseArea: headerMa;

		text: headerDelegate.textIsCropped ? name.text : "";
	}

	function getPrevHeaderIndex(){
		for (let i = headerDelegate.columnIndex - 1; i >= 0; i--){
			let width = headerDelegate.tableItem.widthDecoratorDynamic.getData("Width", i);
			if (width > 0){
				return i;
			}
		}

		return -1;
	}

	function getNextHeaderIndex(){
		for (let i = headerDelegate.columnIndex + 1; i < headerDelegate.columnCount; i++){
			let width = headerDelegate.tableItem.widthDecoratorDynamic.getData("Width", i);
			if (width > 0){
				return i;
			}
		}

		return -1;
	}

	//    MouseArea{
	//        id: movingRight;

	//        anchors.right:  parent.right;

	//        width: 15;
	//        height: parent.height;

	//        visible: headerDelegate.tableItem.canMoveColumns && headerDelegate.columnIndex < headerDelegate.columnCount -1 && headerDelegate.visible;
	//        enabled: visible;

	//        hoverEnabled: true;
	//        cursorShape: pressed ?  Qt.SplitHCursor : isWithinBorder ? Qt.SplitHCursor : Qt.ArrowCursor;

	//        property bool isWithinBorder: mouseX >= width - splitterWidth //&& mouseX <= width/2 + splitterWidth/2;
	//        property int splitterWidth: 6;
	//        property var coord: mapToItem(movingRight,0,0);
	//        property bool  blocked: false;

	//        onPressed: {
	//            movingRight.coord = mapToItem(movingRight,mouse.x,mouse.y)
	//        }

	//        onPositionChanged: {
	//            if(pressed){
	//                console.log("movingRight onPositionChanged");

	//                var newCoords = mapToItem(movingRight,mouse.x,mouse.y);
	//                var deltaX = Math.trunc(newCoords.x - movingRight.coord.x);
	//                var width_ = headerDelegate.tableItem.widthDecoratorDynamic.getData("Width", headerDelegate.columnIndex);

	//                let nextIndex = headerDelegate.getNextHeaderIndex();

	//                var width_next = headerDelegate.tableItem.widthDecoratorDynamic.getData("Width", nextIndex);
	//                var width_min = headerDelegate.tableItem.widthDecoratorDynamic.isValidData("MinWidth", headerDelegate.columnIndex) ? headerDelegate.tableItem.widthDecoratorDynamic.getData("MinWidth", headerDelegate.columnIndex) : headerDelegate.tableItem.minCellWidth;
	//                var width_next_min = headerDelegate.tableItem.widthDecoratorDynamic.isValidData("MinWidth", nextIndex) ? headerDelegate.tableItem.widthDecoratorDynamic.getData("MinWidth", nextIndex) : headerDelegate.tableItem.minCellWidth;

	//                width_ += deltaX;
	//                width_next -= deltaX;

	//                let currentPercent = (width_ / headerDelegate.tableItem.width) * 100;
	//                let nextPercent = (width_next / headerDelegate.tableItem.width) * 100;

	//                if(width_ > width_min && width_next > width_next_min){
	//                    headerDelegate.tableItem.widthDecorator.setData("Width", width_, headerDelegate.columnIndex);
	//                    headerDelegate.tableItem.widthDecorator.setData("Width", width_next, nextIndex);

	//                    if (headerDelegate.tableItem.width > 0){
	//                        headerDelegate.tableItem.widthDecorator.setData("WidthPercent", currentPercent, headerDelegate.columnIndex);
	//                        headerDelegate.tableItem.widthDecorator.setData("WidthPercent", nextPercent, nextIndex);
	//                    }

	//                    headerDelegate.tableItem.setWidth();

	//                    headerDelegate.tableItem.tableViewParams.setHeaderSize(headerDelegate.headerId, currentPercent)
	//                    let nextHeaderId = headerDelegate.tableItem.headers.getData("Id", nextIndex);
	//                    headerDelegate.tableItem.tableViewParams.setHeaderSize(nextHeaderId, nextPercent)
	//                }
	//            }

	//            blocked = true;
	//            blockmovingRightPause.restart();
	//        }

	//        onReleased: {
	//            headerDelegate.tableItem.saveWidth();
	//        }

	//        onClicked: {
	//            if(blocked){
	//                mouse.accepted = true;
	//            }
	//            else {
	//                mouse.accepted = false;
	//            }
	//        }
	//    }

	// PauseAnimation {
	//     id: blockmovingRightPause;

	//     duration: 200

	//     onFinished: {
	//         movingRight.blocked = false;
	//     }
	// }

	MouseArea{
		id: movingLeft;

		anchors.left:  parent.left;

		height: parent.height;
		width: 15;

		visible: headerDelegate.tableItem.canMoveColumns && headerDelegate.columnIndex > 0 && headerDelegate.visible;
		enabled: visible;

		hoverEnabled: true;
		cursorShape: pressed ?  Qt.SplitHCursor : isWithinBorder ? Qt.SplitHCursor : Qt.ArrowCursor;
		property bool isWithinBorder:  mouseX <= splitterWidth;
		property int splitterWidth: 6;
		property var coord: mapToItem(movingLeft,0,0);
		property bool  blocked: false;

		onPressed: {
			movingLeft.coord = mapToItem(movingLeft,mouse.x,mouse.y)
		}

		onPositionChanged: {
			if(pressed){
				var newCoords = mapToItem(movingLeft,mouse.x,mouse.y);
				var deltaX = Math.trunc(newCoords.x - movingLeft.coord.x);

				var width_ = headerDelegate.tableItem.widthDecoratorDynamic.getData("Width", headerDelegate.columnIndex);
				var width_min = headerDelegate.tableItem.widthDecoratorDynamic.isValidData("MinWidth", headerDelegate.columnIndex) ? headerDelegate.tableItem.widthDecoratorDynamic.getData("MinWidth", headerDelegate.columnIndex) : headerDelegate.tableItem.minCellWidth;

				let prevIndex = headerDelegate.getPrevHeaderIndex();
				var width_prev = headerDelegate.tableItem.widthDecoratorDynamic.getData("Width", prevIndex);
				var width_prev_min = headerDelegate.tableItem.widthDecoratorDynamic.isValidData("MinWidth", prevIndex) ? headerDelegate.tableItem.widthDecoratorDynamic.getData("MinWidth", prevIndex) : headerDelegate.tableItem.minCellWidth;

				width_ = width_ - deltaX;
				width_prev = width_prev + deltaX;

				let currentPercent = (width_ / headerDelegate.tableItem.width) * 100;
				let prevPercent = (width_prev / headerDelegate.tableItem.width) * 100;

				if(width_ > width_min && width_prev > width_prev_min){
					headerDelegate.tableItem.widthDecorator.setData("Width", width_, headerDelegate.columnIndex);
					headerDelegate.tableItem.widthDecorator.setData("Width", width_prev, prevIndex);

					if (headerDelegate.tableItem.width > 0){
						headerDelegate.tableItem.widthDecorator.setData("WidthPercent", currentPercent, headerDelegate.columnIndex);
						headerDelegate.tableItem.widthDecorator.setData("WidthPercent", prevPercent, prevIndex);
					}

					headerDelegate.tableItem.setWidth();

					headerDelegate.tableItem.tableViewParams.setHeaderSize(headerDelegate.headerId, currentPercent)
					let prevHeaderId = headerDelegate.tableItem.headers.getData("Id", prevIndex);
					headerDelegate.tableItem.tableViewParams.setHeaderSize(prevHeaderId, prevPercent)
				}
			}

			blocked = true;
			blockmovingLeftPause.restart();
		}

		onReleased: {
			headerDelegate.tableItem.saveWidth();
		}

		onClicked: {
			if(blocked){
				mouse.accepted = true;
			}
			else {
				mouse.accepted = false;
			}
		}
	}

	PauseAnimation {
		id: blockmovingLeftPause;

		duration: 200

		onFinished: {
			movingLeft.blocked = false;
		}
	}
}//delegate

