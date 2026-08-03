import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Item{
	id: headerDelegate;
	objectName: headerId

	property TableBase tableItem
	property int columnCount: tableItem ? tableItem.columnCount : 0
	property int columnIndex: model.index
	property string headerId: model.item ? model.item.m_id : model["id"]
	property string headerName: model.item ? model.item.m_name : model["name"]

	property bool compl: false;
	property bool complCompl: headerDelegate.compl && headerDelegate.tableItem;

	property bool textIsCropped: helperText.width > name.width;

	// Whether a click on this one would do anything, and whether it is the
	// column the rows are ordered by.
	readonly property bool sortable: headerDelegate.tableItem
			&& headerDelegate.tableItem.hasSort
			&& !headerDelegate.tableItem.nonSortableColumns.includes(headerDelegate.headerId);
	readonly property bool activeSort: headerDelegate.tableItem
			&& headerDelegate.tableItem.hasSort
			&& headerDelegate.tableItem.currentHeaderId === headerDelegate.headerId;

	property var rightMA: null;

	Component.onCompleted: {
		headerDelegate.compl = true;
		// Seeded, not bound: the underline is driven by the animations below, and
		// a delegate can be built for a column that is already the sorted one.
		sortUnderline.opacity = headerDelegate.activeSort ? 1 : 0;
	}

	onActiveSortChanged: {
		if (headerDelegate.activeSort){
			underlineOut.stop();
			underlineIn.restart();
		}
		else{
			underlineIn.stop();
			underlineOut.restart();
		}
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

	// Whether anything is drawn to the left of this one. Recomputed rather than
	// bound: hiding a column leaves the indexes alone, so the second column
	// became the first on screen while still drawing a divider against nothing.
	property bool firstVisibleColumn: true;

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

		headerDelegate.firstVisibleColumn = headerDelegate.getPrevHeaderIndex() < 0;

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
		width: headerDelegate.tableItem.isRightBorder ? headerDelegate.tableItem.verticalBorderSize * headerDelegate.tableItem.visibleLeftBorderFirst * headerDelegate.firstVisibleColumn
													  : headerDelegate.tableItem.visibleLeftBorderFirst ? headerDelegate.tableItem.verticalBorderSize : !headerDelegate.firstVisibleColumn ? headerDelegate.tableItem.verticalBorderSize : 0;
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

		// Tells the reader the title is a control before they click it, in the
		// tab strip's manner: a light wash inset from the cell's edges, and not
		// on the column the rows are already ordered by - that one has its own
		// accent along the bottom and does not want a second mark over it.
		//
		// Under the corner patches, so a decorated header keeps the corners it
		// asked for.
		Rectangle {
			anchors.fill: parent;
			anchors.topMargin: Style.marginXS;
			anchors.bottomMargin: Style.marginXS;
			anchors.leftMargin: 2;
			anchors.rightMargin: 2;

			visible: headerDelegate.sortable && headerMa.containsMouse && !headerDelegate.activeSort;
			color: Style.alternateBaseColor;
			radius: Style.marginS;
		}

		// The press itself. A wash that deepens under the finger and drains away
		// after it, rather than the whole cell twitching 1.5% smaller: scaling the
		// delegate dragged its borders and its resize handles along with it.
		Rectangle {
			id: pressWash;

			// Same shape as the hover wash above, so pressing deepens what the
			// pointer already lit rather than painting a second, wider patch.
			anchors.fill: parent;
			anchors.topMargin: Style.marginXS;
			anchors.bottomMargin: Style.marginXS;
			anchors.leftMargin: 2;
			anchors.rightMargin: 2;

			color: Style.selectedColor;
			opacity: 0;
			radius: Style.marginS;

			// Quick to arrive, slower to leave: snapping straight back reads as a
			// glitch, while a short drain reads as a release.
			NumberAnimation {
				id: pressIn;

				target: pressWash;
				property: "opacity";
				to: 1;
				duration: 90;
				easing.type: Easing.OutQuad;
			}

			NumberAnimation {
				id: pressOut;

				target: pressWash;
				property: "opacity";
				to: 0;
				duration: 220;
				easing.type: Easing.InQuad;
			}
		}

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

		// The column the rows are ordered by, marked along its whole width: a
		// small arrow alone is easy to miss in a wide table. Faded in rather than
		// switched on, so it reads as the answer to the click that just landed.
		Rectangle {
			id: sortUnderline;

			anchors.left: parent.left;
			anchors.right: parent.right;
			anchors.bottom: parent.bottom;

			height: 2;
			opacity: 0;
			color: Style.titleColor;

			NumberAnimation {
				id: underlineIn;

				target: sortUnderline;
				property: "opacity";
				to: 1;
				duration: 160;
				easing.type: Easing.OutQuad;
			}

			NumberAnimation {
				id: underlineOut;

				target: sortUnderline;
				property: "opacity";
				to: 0;
				duration: 120;
				easing.type: Easing.InQuad;
			}
		}
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


		// A title, not another row of data: smaller and quieter than the cells
		// below it, so the eye lands on the content first. The delegate itself
		// carries the press scale - multiplying it in here applied it twice.
		font.pixelSize: headerDelegate.tableItem.emptyDecorHeader ? Style.fontSizeS :
																	headerDelegate.tableItem.headerDecorator.isValidData("FontSize", headerDelegate.columnIndex) ?
																		headerDelegate.tableItem.headerDecorator.getData("FontSize", headerDelegate.columnIndex) :
																		Style.fontSizeS;


		font.family: headerDelegate.tableItem.emptyDecorHeader ? Style.fontFamilyBold :
																 headerDelegate.tableItem.headerDecorator.isValidData("FontFamily", headerDelegate.columnIndex) ?
																	 headerDelegate.tableItem.headerDecorator.getData("FontFamily", headerDelegate.columnIndex) :
																	 Style.fontFamilyBold;

		font.bold: headerDelegate.tableItem.emptyDecorHeader ? true :
															   headerDelegate.tableItem.headerDecorator.isValidData("FontBold", headerDelegate.columnIndex) ?
																   headerDelegate.tableItem.headerDecorator.getData("FontBold", headerDelegate.columnIndex) :
																   true;


		color: headerDelegate.tableItem.emptyDecorHeader ? (headerDelegate.activeSort ? Style.titleColor : Style.subtitleColor) :
														   headerDelegate.tableItem.headerDecorator.isValidData("FontColor", headerDelegate.columnIndex) ?
															   headerDelegate.tableItem.headerDecorator.getData("FontColor", headerDelegate.columnIndex) :
															   headerDelegate.activeSort ? Style.titleColor : Style.subtitleColor;
		elide: headerDelegate.tableItem.elideMode;

		wrapMode: headerDelegate.tableItem.wrapMode;

		onLinkActivated: {
			Qt.openUrlExternally(link)
		}

		text: headerDelegate.headerName
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
		anchors.rightMargin: headerDelegate.columnIndex == headerDelegate.columnCount - 1 ? 3 * Style.marginM : Style.marginM;

		width: visible ? Style.iconSizeXS : 0;
		height: width;

		// Shown faintly under the pointer as well, so a column advertises that
		// it can be sorted before anyone tries.
		visible: headerDelegate.activeSort || (headerDelegate.sortable && headerMa.containsMouse);
		opacity: headerDelegate.activeSort ? 1 : 0.4;
		rotation: headerDelegate.activeSort && headerDelegate.tableItem.currentSortOrder == "ASC" ? 180 : 0

		sourceSize.width: width;
		sourceSize.height: height;

		source: headerDelegate.tableItem.sortIndicatorIcon
	}

	////
	MouseArea {
		id: headerMa;
		objectName: "MouseArea"

		anchors.fill: parent;

		hoverEnabled: true;
		visible: headerDelegate.tableItem.hasSort || headerDelegate.textIsCropped || headerDelegate.tableItem.editableHeaderParams;
		acceptedButtons: Qt.LeftButton | Qt.RightButton;
		// A hand over a column that would reorder the rows, and nothing over one
		// that would not - the wash alone left it to be found by clicking. The
		// resize handles sit above this at either edge and keep their own cursor.
		cursorShape: headerDelegate.sortable ? Qt.PointingHandCursor : Qt.ArrowCursor;


		// Only a column that would actually reorder anything presses in.
		onPressed: {
			if(headerDelegate.sortable){
				pressOut.stop();
				pressIn.restart();
			}
		}

		onReleased: {
			pressIn.stop();
			pressOut.restart();
		}

		// The grab can be taken away without a release - the pointer leaving the
		// window, or the view scrolling out from under it.
		onCanceled: {
			pressIn.stop();
			pressOut.restart();
		}

		onClicked: {
			if(headerDelegate.tableItem.hasSort || headerDelegate.tableItem.editableHeaderParams){
				if (mouse.button === Qt.LeftButton){
					if(!headerDelegate.tableItem.nonSortableColumns.includes(headerDelegate.headerId)){
						let currentHeaderId = headerDelegate.tableItem.currentHeaderId;
						let currentSortOrder = headerDelegate.tableItem.currentSortOrder;
						
						let sortOrder = currentSortOrder

						if (currentHeaderId !== headerDelegate.headerId){
							sortOrder = "ASC";
						}
						else{
							sortOrder = currentSortOrder == "ASC" ? "DESC" : "ASC";
						}
						
						headerDelegate.tableItem.setSortingInfo(headerDelegate.headerId, sortOrder);

						headerDelegate.tableItem.headerClicked(headerDelegate.headerId);
					}
				}
				else if (mouse.button === Qt.RightButton){
					headerDelegate.tableItem.headerRightMouseClicked(headerDelegate.headerId);
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
		// The grab zone and the zone that says so are now the same width: two
		// thirds of the old area showed an arrow while still dragging.
		width: Style.marginS;

		visible: headerDelegate.tableItem.canMoveColumns && !headerDelegate.firstVisibleColumn && headerDelegate.visible;
		enabled: visible;

		hoverEnabled: true;
		cursorShape: Qt.SplitHCursor;
		property real lastX: 0;
		property bool  blocked: false;

		Rectangle {
			anchors.left: parent.left;
			anchors.top: parent.top;
			anchors.bottom: parent.bottom;

			width: 2;
			visible: moving.containsMouse || moving.pressed;
			color: Style.titleColor;
		}

		onPressed: {
			moving.lastX = moving.mapToItem(headerDelegate.tableItem, moving.mouseX, 0).x;
		}

		onPositionChanged: {
			if(pressed){
				let isFlickableTable = headerDelegate.tableItem.isFlickable;

				// Measured against the table rather than against this handle: the
				// handle travels with the edge it drags, so a delta taken here
				// would cancel itself out. Taken since the previous move, not
				// since the press, so the widths do not compound.
				let currentX = moving.mapToItem(headerDelegate.tableItem, moving.mouseX, 0).x;
				let deltaX = Math.trunc(currentX - moving.lastX);
				if (deltaX === 0){
					return;
				}

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
						let prevHeaderId = headerDelegate.tableItem.getHeaderId(prevIndex);
						headerDelegate.tableItem.tableViewParams.setHeaderSize(prevHeaderId, prevPercent)
					}

					// Only a delta that was actually applied is consumed: at the
					// minimum width the pointer can be walked back without first
					// having to make up the distance it ran on.
					moving.lastX = currentX;

					moving.blocked = true;
					blockmovingPause.restart();
				}
			}
		}

		onReleased: {
			if (moving.blocked){
				headerDelegate.tableItem.saveWidth();
			}
		}

		// Swallowed only after a real resize. This used to be set from any
		// pointer move, hover included, so a plain click near a column edge
		// was eaten and the column never sorted.
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
			width: Style.marginS;

			// The last column has no neighbour to give width back to, so this is
			// the only edge it can be dragged by - including when it is also the
			// first one, which the old index test ruled out.
			visible: headerDelegate.tableItem.canMoveColumns && headerDelegate.visible;
			enabled: visible;

			hoverEnabled: true;
			cursorShape: Qt.SplitHCursor;
			property real lastX: 0;
			property bool  blocked: moving.blocked;

			Rectangle {
				anchors.right: parent.right;
				anchors.top: parent.top;
				anchors.bottom: parent.bottom;

				width: 2;
				visible: movingRight.containsMouse || movingRight.pressed;
				color: Style.titleColor;
			}

			onPressed: {
				movingRight.lastX = movingRight.mapToItem(headerDelegate.tableItem, movingRight.mouseX, 0).x;
			}

			onPositionChanged: {
				if(pressed){
					let isFlickableTable = headerDelegate.tableItem.isFlickable;
					if(!isFlickableTable){
						return;
					}

					// See the left handle: measured against the table, and since
					// the previous move rather than since the press.
					let currentX = movingRight.mapToItem(headerDelegate.tableItem, movingRight.mouseX, 0).x;
					let deltaX = Math.trunc(currentX - movingRight.lastX);
					if (deltaX === 0){
						return;
					}

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

						movingRight.lastX = currentX;

						moving.blocked = true;
						blockmovingPause.restart();
					}
				}
			}

			onReleased: {
				if (movingRight.blocked){
					headerDelegate.tableItem.saveWidth();
				}
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

