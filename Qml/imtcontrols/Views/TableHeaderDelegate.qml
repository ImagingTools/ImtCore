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

	property var rightMA: null;

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

	function createRightMA(){
		if(headerDelegate.columnIndex == getLastHeaderIndex()){
			if(headerDelegate.rightMA == null){
				headerDelegate.rightMA = movingRightComp.createObject(headerDelegate);
				headerDelegate.rightMA.z = 30;
			}
		}
		else if(headerDelegate.columnIndex !== getLastHeaderIndex()){
			if(headerDelegate.rightMA){
				headerDelegate.rightMA.destroy();
				headerDelegate.rightMA = null;
			}
		}
	}

	function setCellWidth(){
		if (!headerDelegate || headerDelegate.columnCount === 0){
			return
		}

		let isFlickableTable = headerDelegate.tableItem.isFlickable;

		var defaultWidth = !isFlickableTable ? (headerDelegate.width)/headerDelegate.columnCount : headerDelegate.tableItem.defaultColumnWidth;
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

		createRightMA();
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
		anchors.rightMargin: headerDelegate.columnIndex == headerDelegate.columnCount - 1 ? 3 * Style.size_mainMargin : Style.size_mainMargin;

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
					if(!headerDelegate.tableItem.nonSortableColumns.includes(model.Id)){
						let currentHeaderId = headerDelegate.tableItem.currentHeaderId;
						let currentSortOrder = headerDelegate.tableItem.currentSortOrder;

						headerDelegate.tableItem.currentHeaderId = model.Id;
						if (currentHeaderId !== model.Id){
							headerDelegate.tableItem.currentSortOrder = "ASC";
						}
						else{
							headerDelegate.tableItem.currentSortOrder = currentSortOrder == "ASC" ? "DESC" : "ASC";
						}

						headerDelegate.tableItem.headerClicked(model.Id);
					}
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

	function getLastHeaderIndex(){
		for (let i = headerDelegate.tableItem.widthDecoratorDynamic.getItemsCount() - 1; i >= 0; i--){
			let width = headerDelegate.tableItem.widthDecoratorDynamic.getData("Width", i);
			if (width > 0){
				return i;
			}
		}

		return -1;
	}


	MouseArea{
		id: moving;

		anchors.left: parent.left;

		height: parent.height;
		width: 15;

		visible: headerDelegate.tableItem.canMoveColumns && headerDelegate.columnIndex > 0 && headerDelegate.visible;
		enabled: visible;

		hoverEnabled: true;
		cursorShape: pressed ?  Qt.SplitHCursor : isWithinBorder ? Qt.SplitHCursor : Qt.ArrowCursor;
		property bool isWithinBorder:  mouseX <= splitterWidth;
		property int splitterWidth: 6;
		property var coord: mapToItem(moving,0,0);
		property bool  blocked: false;

		onPressed: {
			moving.coord = mapToItem(moving,mouse.x,mouse.y)
		}

		onPositionChanged: {
			if(pressed){
				let isFlickableTable = headerDelegate.tableItem.isFlickable;

				let newCoords = mapToItem(moving,mouse.x,mouse.y);
				let deltaX = Math.trunc(newCoords.x - moving.coord.x);

				let width_ = headerDelegate.tableItem.widthDecoratorDynamic.getData("Width", headerDelegate.columnIndex);
				let width_min = headerDelegate.tableItem.widthDecoratorDynamic.isValidData("MinWidth", headerDelegate.columnIndex) ? headerDelegate.tableItem.widthDecoratorDynamic.getData("MinWidth", headerDelegate.columnIndex) : headerDelegate.tableItem.minCellWidth;

				width_ = width_ - deltaX;

				let prevIndex; let width_prev; let width_prev_min;let prevPercent;

				prevIndex = headerDelegate.getPrevHeaderIndex();
				if(prevIndex >=0){
					width_prev = headerDelegate.tableItem.widthDecoratorDynamic.getData("Width", prevIndex);
					width_prev_min = headerDelegate.tableItem.widthDecoratorDynamic.isValidData("MinWidth", prevIndex) ? headerDelegate.tableItem.widthDecoratorDynamic.getData("MinWidth", prevIndex) : headerDelegate.tableItem.minCellWidth;
					width_prev = width_prev + deltaX;
					prevPercent = (width_prev / headerDelegate.tableItem.elementsList.width) * 100;
				}

				let currentPercent = (width_ / headerDelegate.tableItem.elementsList.width) * 100;

				let widthOk = isFlickableTable ? width_prev > width_prev_min : width_ > width_min && width_prev > width_prev_min;

				if(widthOk){
					if(!isFlickableTable){
						headerDelegate.tableItem.widthDecorator.setData("Width", width_, headerDelegate.columnIndex);
					}
					if(prevIndex >=0){
						headerDelegate.tableItem.widthDecorator.setData("Width", width_prev, prevIndex);
					}

					if (headerDelegate.tableItem.elementsList.width > 0){
						if(!isFlickableTable){
							headerDelegate.tableItem.widthDecorator.setData("WidthPercent", currentPercent, headerDelegate.columnIndex);
						}
						if(prevIndex >=0){
							headerDelegate.tableItem.widthDecorator.setData("WidthPercent", prevPercent, prevIndex);
						}
					}

					headerDelegate.tableItem.setWidth();

					if(!isFlickableTable){
						headerDelegate.tableItem.tableViewParams.setHeaderSize(headerDelegate.headerId, currentPercent)
					}

					if(prevIndex >=0){
						let prevHeaderId = headerDelegate.tableItem.headers.getData("Id", prevIndex);
						headerDelegate.tableItem.tableViewParams.setHeaderSize(prevHeaderId, prevPercent)
					}

				}
			}

			blocked = true;
			blockmovingPause.restart();
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
		id: blockmovingPause;

		duration: 200

		onFinished: {
			moving.blocked = false;
		}
	}

	Component{
		id: movingRightComp
		MouseArea{
			id: movingRight;

			anchors.right: parent.right;

			height: parent.height;
			width: 15;

			visible: headerDelegate.tableItem.canMoveColumns && headerDelegate.columnIndex > 0 && headerDelegate.visible;
			enabled: visible;

			hoverEnabled: true;
			cursorShape: pressed ?  Qt.SplitHCursor : isWithinBorder ? Qt.SplitHCursor : Qt.ArrowCursor;
			property bool isWithinBorder:  mouseX >=  width - splitterWidth;
			property int splitterWidth: 6;
			property var coord: mapToItem(movingRight,0,0);
			property bool  blocked: moving.blocked;

			onPressed: {
				movingRight.coord = mapToItem(movingRight,mouse.x,mouse.y)
			}

			onPositionChanged: {
				if(pressed){
					let isFlickableTable = headerDelegate.tableItem.isFlickable;
					if(!isFlickableTable){
						return;
					}
					let newCoords = mapToItem(movingRight,mouse.x,mouse.y);
					let deltaX = Math.trunc(newCoords.x - movingRight.coord.x);

					let width_ = headerDelegate.tableItem.widthDecoratorDynamic.getData("Width", headerDelegate.columnIndex);
					let width_min = headerDelegate.tableItem.widthDecoratorDynamic.isValidData("MinWidth", headerDelegate.columnIndex) ? headerDelegate.tableItem.widthDecoratorDynamic.getData("MinWidth", headerDelegate.columnIndex) : headerDelegate.tableItem.minCellWidth;

					width_ = width_ + deltaX;

					let currentPercent = (width_ / headerDelegate.tableItem.elementsList.width) * 100;

					let widthOk =  width_ > width_min;

					if(widthOk){
						headerDelegate.tableItem.widthDecorator.setData("Width", width_, headerDelegate.columnIndex);

						if (headerDelegate.tableItem.elementsList.width > 0){
							headerDelegate.tableItem.widthDecorator.setData("WidthPercent", currentPercent, headerDelegate.columnIndex);
						}

						headerDelegate.tableItem.setWidth();

						headerDelegate.tableItem.tableViewParams.setHeaderSize(headerDelegate.headerId, currentPercent)
					}
				}

				moving.blocked = true;
				blockmovingPause.restart();
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

	}

}//delegate

