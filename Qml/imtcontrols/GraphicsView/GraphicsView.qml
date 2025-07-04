import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0


Rectangle {
	id: graphicsView;

	clip: true;

	color: Style.imagingToolsGradient0;

	property BaseModel objectsModel: BaseModel {}

	property alias selectedIndex: canvas.selectedIndex;

	//for scrollBars
	property real contentHeight: backgroundRec.height;
	property real contentY: -canvas.deltaY;
	property real originY: 0;

	property real contentWidth: backgroundRec.width;
	property real contentX: -canvas.deltaX;
	property real originX: 0;

	property real scaleStep: 0.1;
	property real minZoomLevel: -1;
	property real maxZoomLevel: 3;
	property bool restrictMove: false;
	property int restrictMoveMargin: 80;
	property bool fitToBorders: true;

	property bool autoFit: false;

	property bool isSelectionMode: true;
	property bool isEditMode: false;
	property bool isMultiSelection: true;

	property real drawingAreaWidth: width//1024;
	property real drawingAreaHeight: height//768;

	property var layerModel: []

	property Component controlPanelComp: null
	property Item controlPanelItem: null

	property alias scaleCoeff: canvas.scaleCoeff;
	property alias deltaX: canvas.deltaX;
	property alias deltaY: canvas.deltaY;

	signal copySignal(int index);
	signal pasteSignal(int index);
	signal deleteSignal(int index);
	signal renameSignal(int index);
	signal revertSignal();
	signal modelDataChanged();

	Component.onCompleted: {
		Events.subscribeEvent("DesignSchemeChanged", designSchemeChanged);
		Events.subscribeEvent("AppSizeChanged", appSizeChanged)

		let layerBackground = layerComp.createObject(this);
		layerBackground.layerId = "background";
		layerModel.push(layerBackground);

		let layerInactive = layerComp.createObject(this);
		layerInactive.layerId = "inactive";
		layerModel.push(layerInactive);

		let layerActive = layerComp.createObject(this);
		layerActive.layerId = "active";
		layerModel.push(layerActive);

		let layerInstrumental = layerComp.createObject(this);
		layerInstrumental.layerId = "instrumental";
		layerInstrumental.canApplyViewTransform = false;
		layerModel.push(layerInstrumental);

	}

	Component.onDestruction: {
		Events.unSubscribeEvent("DesignSchemeChanged", designSchemeChanged);
		Events.unSubscribeEvent("AppSizeChanged", appSizeChanged)
	}

	onContentXChanged: {
		canvas.deltaX = -contentX
	}

	onContentYChanged: {
		canvas.deltaY = -graphicsView.contentY
	}

	onObjectsModelChanged: {
		canvas.requestPaint();
	}

	onAutoFitChanged: {
		if (autoFit){
			appSizeChanged();
		}
	}

	function createLayer(layerId){
		let layer = layerComp.createObject(this);
		layer.layerId = layerId;

		if(layerModel.length){
			layerModel.splice (layerModel.length - 2, 0,layer);
			return layer;
		}

		return null;
	}

	function getLayer(layerId){
		for(let i = 0; i < layerModel.length; i++){
			if(layerModel[i].layerId == layerId){
				return layerModel[i];
			}
		}
		return null;
	}

	function getActiveLayer(){
		return getLayer("active");
	}
	function getInactiveLayer(){
		return getLayer("inactive");
	}
	function getBackgroundLayer(){
		return getLayer("background");
	}
	function getInstrumentalLayer(){
		return getLayer("instrumental");
	}

	Component{
		id: layerComp;
		GraphicsLayer{
			onLoadImageSignal:{
				canvas.loadImage(source);
			}
		}
	}


	function designSchemeChanged(scheme){
		canvas.requestPaint();
	}

	function appSizeChanged(params){
		if (autoFit){
			zoomToFit(false);

			requestPaint()
		}
	}

	function setAutoFit(autoFit){
		graphicsView.autoFit = autoFit;
	}

	function zoomIn(){
		if (canvas.scaleCoeff < 3){
			let scaleCoeff_ = canvas.scaleCoeff;
			scaleCoeff_ += scaleStep;
			canvas.setScale(scaleCoeff_, canvas.width / 2, canvas.height / 2)
		}
	}

	function zoomOut(){
		if(canvas.scaleCoeff > scaleStep * 2){
			let scaleCoeff_ = canvas.scaleCoeff;
			scaleCoeff_ -= scaleStep;
			canvas.setScale(scaleCoeff_, canvas.width / 2, canvas.height / 2)
		}
	}

	function resetZoom(){
		canvas.scaleCoeff = 1.0;
		canvas.scaleCoeffPrev = 1.0;

		requestPaint()
	}

	function zoomToFit(requestPaint_){
		canvas.scaleCoeff = 1
		canvas.scaleCoeffPrev = 1
		canvas.deltaX = 0;
		canvas.deltaY = 0;

		if(requestPaint_ == undefined){
			requestPaint_ = false;
		}
		if(requestPaint_){
			canvas.requestPaint();
		}

		// if (canvas.backgroundWidth == 0 | canvas.backgroundHeight == 0){
		// 	return
		// }

		// let scaleCoeff_ = (canvas.width - Style.marginM * 2) / canvas.backgroundWidth;
		// let scaleCoeff_h = (canvas.height - Style.marginM * 2) / canvas.backgroundHeight;
		// if (scaleCoeff_ > scaleCoeff_h){
		// 	scaleCoeff_ = scaleCoeff_h
		// }

		// canvas.setScale(scaleCoeff_, canvas.width / 2, canvas.height / 2)

		// canvas.deltaX = graphicsView.width / 2 - backgroundRec.width / 2
		// canvas.deltaY = graphicsView.height / 2 - backgroundRec.height / 2
	}

	function requestPaint(){
		console.log("Canvas::requestPaint")
		canvas.requestPaint();
	}

	function findModelIndex(id){
		let ind = -1;
		if(!id) return ind
		for(let i = 0; i < objectsModel.count; i++){
			if(objectsModel.get(i).item.m_id == id){
				ind = i;
				break;
			}
		}
		return ind;
	}

	function goInside(){
		if(objectsModel.count > canvas.selectedIndex && canvas.selectedIndex >= 0){
			// let isComposite = objectsModel.isValidData("isComposite", canvas.selectedIndex) ? objectsModel.getData("isComposite", canvas.selectedIndex) : false;
			// if(!isComposite){
			//     return;
			// }
			// let id = objectsModel.getData("id", canvas.selectedIndex);
			console.log("Go inside");
		}
	}

	function correctPosition(addX, addY){
		for(let i = 0; i < graphicsView.objectsModel.count; i++){
			let item = graphicsView.objectsModel.get(i).item;

			item.m_x = item.m_x + addX;
			item.m_y = item.m_y + addY;

			// graphicsView.objectsModel.set(i, item);
		}
	}

	function correctPositionScaled(addX, addY, scale_){
		for(let i = 0; i < objectsModel.count; i++){
			let item = objectsModel.get(i).item;

			let newWidth = scale_ * item.m_width;
			item.m_width = newWidth;

			// graphicsView.objectsModel.set(i, item);
		}
	}

	function findObject(mouseX, mouseY){
		//console.log("findObject", mouseX, mouseY)

		for(let i = 0; i < graphicsView.layerModel.length ; i++){
			let layer = graphicsView.layerModel[i];
			if(layer.layerId == "active" || layer.layerId == "inactive"){
				let shapeModel = layer.shapeModel;
				for(let j = 0; j < shapeModel.length; j++){
					let shape = shapeModel[j];

					canvasMatrix.setXTranslation(canvas.deltaX);
					canvasMatrix.setYTranslation(canvas.deltaY);
					canvasMatrix.setXScale(canvas.scaleCoeff);
					let isInside =  shape.isInside(mouseX, mouseY, canvasMatrix)

					if(isInside){
						return shape;
					}
				}
			}


		}

		return null
	}


	function clearSelection(exeptShape){
		for(let i = 0; i < graphicsView.layerModel.length ; i++){
			let layer = graphicsView.layerModel[i];
			if(layer.layerId == "active" || layer.layerId == "inactive"){
				let shapeModel = layer.shapeModel;
				for(let j = 0; j < shapeModel.length; j++){
					let shape = shapeModel[j];
					if(exeptShape !==undefined && exeptShape == shape){
						continue;
					}
					shape.isSelected = false;
				}
			}
		}
		if(exeptShape ==undefined){
			canvas.currentTouchedShape = null
		}
		else {
			canvas.currentTouchedShape = exeptShape
		}
	}


	TreeItemModel {
		id: bufferModel;
	}

	TreeItemModel {
		id: emptyModel;
	}

	Item{
		id: mainContainer;

		anchors.fill: parent

		ControlArea {
			id: controlArea;

			anchors.fill: parent;
			cursorShape: Qt.ArrowCursor//Qt.OpenHandCursor;

			onClicked: {
				//console.log("CLICKED!!!")
			}

			onPressed: {
				//console.log("PRESSED!!!")
				let found = false;
				if(!(mouse.modifiers & Qt.ControlModifier)){// without Ctrl
					let shape = graphicsView.findObject(mouse.x, mouse.y)
					if(shape !== null){
						found = true;
						canvas.currentTouchedShape = shape;
						if(canvas.selectedShapeCount < 2){
							graphicsView.clearSelection(shape)
							canvas.selectedShapeCount = 1;
						}
						shape.isSelected = true;
					}
					else {
						graphicsView.clearSelection()
						canvas.selectedShapeCount = 0;
					}

					canvas.requestPaint();

					if(!found){
						controlArea.cursorShape = Qt.ClosedHandCursor;
					}

				}
			}

			onReleased: {
				//console.log("RELEASED!!!")
				let shape = graphicsView.findObject(mouse.x, mouse.y)
				if(mouse.modifiers & Qt.ControlModifier){//Ctrl
					if(shape !== null){
						if(!shape.isSelected){
							shape.isSelected = true;
							canvas.selectedShapeCount++
						}
						else {
							shape.isSelected = false;
							canvas.selectedShapeCount--
						}
					}
					else {
						graphicsView.clearSelection()
						canvas.selectedShapeCount = 0;
					}
				}
				else {//without Ctrl
					if(!wasMoving && shape !== null){
						if(canvas.selectedShapeCount > 1){
							graphicsView.clearSelection(shape)
							canvas.selectedShapeCount = 1;
						}
						shape.isSelected = true;
					}
				}
				canvas.requestPaint();
				controlArea.cursorShape = Qt.ArrowCursor;
			}

			onDoubleClicked: {
				wasMoving = false;
				isPressed = false;

				if(canvas.selectedIndex >= 0){
					graphicsView.goInside();
				}
			}

			onDeltaSignal: {

				let found = false;
				let margin_ = Style.marginM;
				let activeLayer = graphicsView.getLayer("active")
				for (let i = 0; i < activeLayer.shapeModel.length; i++){
					let shape = activeLayer.shapeModel[i];
					if(shape.isSelected){
						found = true;

						//correction of position relative to borders
						let topLeftPoint = shape.getBoundingBoxCoordinate()
						let x_ = topLeftPoint.x;
						let y_ = topLeftPoint.y;

						let width_ = shape.getBoundingBoxWidth();
						let height_ = shape.getBoundingBoxHeight();

						x_ = x_ * canvas.scaleCoeff + canvas.deltaX;
						y_ = y_ * canvas.scaleCoeff + canvas.deltaY;
						width_ = width_ * canvas.scaleCoeff;
						height_ = height_  * canvas.scaleCoeff;

						if (canvas.scaleCoeff < 0.0000001){
							return;
						}

						let newX = (topLeftPoint.x + delta.x / canvas.scaleCoeff);
						let newY = (topLeftPoint.y + delta.y / canvas.scaleCoeff);

						//fit to borders

						if(newX < margin_){
							newX = margin_;
						}
						if(newY < margin_){
							newY = margin_;
						}

						if (canvas.scaleCoeff - margin_ === 0){
							return;
						}

						if(newX > (backgroundRec.width  - width_) / canvas.scaleCoeff - margin_){
							newX = (backgroundRec.width  - width_) / canvas.scaleCoeff - margin_
						}
						if(newY > (backgroundRec.height  - height_) / canvas.scaleCoeff - margin_){
							newY = (backgroundRec.height  - height_) / canvas.scaleCoeff - margin_
						}

						// let deltaX_ = delta.x / canvas.scaleCoeff
						// let deltaY_ = delta.y / canvas.scaleCoeff

						let deltaX_ = newX - topLeftPoint.x
						let deltaY_ = newY - topLeftPoint.y
						//correction of position relative to borders

						shape.setCoordinateShift(deltaX_, deltaY_)

					}
				}

				if(!found){
					graphicsView.autoFit = false;

					let borderMargin = graphicsView.restrictMoveMargin;


					let ok1 = graphicsView.restrictMove && canvas.scaleCoeff <= graphicsView.minZoomLevel ? false : true
					let okWidth = canvas.deltaX + delta.x > -1*(canvas.width * canvas.scaleCoeff  - borderMargin)  && canvas.deltaX + delta.x < (canvas.width - borderMargin);
					let okHeight = canvas.deltaY + delta.y > -1*(canvas.height * canvas.scaleCoeff  - borderMargin)  && canvas.deltaY + delta.y < (canvas.height - borderMargin);
					let ok = ok1 && okWidth && okHeight;
					if(ok){
						canvas.deltaX += delta.x
						canvas.deltaY += delta.y
					}
				}


				canvas.requestPaint();
			}

			onWheel: {
				if (canvas.scaleCoeff == 0){
					return
				}

				graphicsView.autoFit = false;

				let deltaX = (wheel.x + canvas.deltaX) / canvas.scaleCoeff
				let wheelDelta = wheel.angleDelta.y
				let scaleCoeff_ = canvas.scaleCoeff;
				if(wheelDelta > 0){//up
					if (canvas.scaleCoeff < graphicsView.maxZoomLevel){
						if(canvas.scaleCoeff < 1 && (canvas.scaleCoeff + scaleStep) > 1){
							scaleCoeff_ = 1
							if(graphicsView.fitToBorders){
								graphicsView.zoomToFit(false)
							}
						}
						else {
							scaleCoeff_ += scaleStep;
						}
						if(scaleCoeff_ > graphicsView.maxZoomLevel){
							scaleCoeff_ = graphicsView.maxZoomLevel
						}
						if(Math.abs(scaleCoeff_ - 1) <  0.000001){
							if(graphicsView.fitToBorders){
								graphicsView.zoomToFit(false)
							}
						}
					}
				}
				else{//down
					let minZoom = graphicsView.minZoomLevel > -1 ? graphicsView.minZoomLevel : scaleStep * 2;
					if(canvas.scaleCoeff > minZoom){
						if(canvas.scaleCoeff > 1 && (canvas.scaleCoeff - scaleStep) < 1){
							scaleCoeff_ = 1
							if(graphicsView.fitToBorders){
								graphicsView.zoomToFit(false)
							}
						}
						else {
							scaleCoeff_ -= scaleStep;
						}
						if(scaleCoeff_ < minZoom){
							scaleCoeff_ = minZoom;
						}
						if(Math.abs(scaleCoeff_ - 1) <  0.000001){
							if(graphicsView.fitToBorders){
								graphicsView.zoomToFit(false)
							}
						}
					}
				}

				canvas.setScale(scaleCoeff_, wheel.x, wheel.y)
			}


			function movingFunction(delta){//reimplemented
			}

			onPositionSignal: {
				positionChangedPause.position = position;
				positionChangedPause.restart();
			}
			PauseAnimation {
				id: positionChangedPause;

				duration: 100;
				property point position;
				onFinished: {
					controlArea.hoverReaction(position);
				}
			}

			function hoverReaction(position){
				// canvas.hoverIndex = -1;
				// for(let i = 0; i < graphicsView.objectsModel.count; i++){
				// 	let item = graphicsView.objectsModel.get(i).item;

				// 	let x_  = item.m_x;
				// 	let y_  = item.m_y;
				// 	let width_ = item.m_width ? item.m_width  : canvas.mainRec_width;
				// 	let height_ = canvas.mainRec_height ;

				// 	x_ = x_ * canvas.scaleCoeff + canvas.deltaX;
				// 	y_ = y_ * canvas.scaleCoeff + canvas.deltaY;
				// 	width_ = width_ * canvas.scaleCoeff;
				// 	height_ = height_  * canvas.scaleCoeff;

				// 	let ok = controlArea.checkHoverItem(x_, y_, width_, height_, position);

				// 	if(ok){
				// 		canvas.hoverIndex = i;

				// 	}
				// }
				if(canvas.hoverIndex >=0){
					//controlArea.cursorShape = Qt.ArrowCursor;

					// canvas.linkSelected = true;
					// canvas.requestPaint();
				}
				else {
					//controlArea.cursorShape = Qt.OpenHandCursor;

					// if(canvas.linkSelected){
					// 	canvas.linkSelected = false;
					// 	canvas.requestPaint();
					// }
				}
			}
			//hover reaction

		}

		Rectangle{
			id: backgroundRec;

			x: canvas.deltaX;
			y: canvas.deltaY;

			width: canvas.backgroundWidth * canvas.scaleCoeff;
			height: canvas.backgroundHeight * canvas.scaleCoeff;
			property real topY: y;
			property real bottomY: y + height;
			property real leftX: x;
			property real rightX: x + width;

			color: "transparent";//Style.baseColor;
		}

		Canvas {
			id: canvas

			anchors.fill: parent;

			antialiasing: true;

			property var currentTouchedShape: null
			property int selectedShapeCount: 0;

			property real scaleCoeff: 1.0;
			property real scaleCoeffPrev: 1.0;
			property real deltaX: 0.0;
			property real deltaY: 0.0;

			property int selectedIndex: -1;
			property int hoverIndex: -1;
			property bool hasTailSelection: false;

			//sizes
			property real mainRec_width: 250
			property real mainRec_height: 60

			property int fontSize: 20
			property int fontSizeS: 14
			property int radius_: 2
			property int borderShift: 4
			property int shadowSize: 6

			property int backgroundStep: 30
			property int backgroundWidth: graphicsView.drawingAreaWidth
			property int backgroundHeight: graphicsView.drawingAreaHeight

			property int intersectionSize: 16
			property int arcRadius: 8
			property int textMargin: 8
			property int textVerticalOffset: 22
			property real imageSize: 20
			property real imageMargin: 4

			//colors
			property string selectedColor: Style.selectedColor
			property string mainColor: Style.borderColor2
			property string errorColor: Style.errorColor
			property string compositeColor: "#bcd2e8";
			property string compositeSelectedColor: "#1167b1";
			property string selectedLinkFromColor: Style.selectedLinkFromColor
			property string selectedLinkToColor: Style.selectedLinkToColor
			property string linkColor: Style.borderColor2
			property string mainTextColor: Style.textColor
			property string secondTextColor: Style.inactiveTextColor
			property string gridColor: Style.imagingToolsGradient1;
			property string backgroundBorderColor: Style.borderColor; // Style.alternateBaseColor;
			property string backgroundColor: Style.baseColor;
			property string innerFrameColor: "transparent";

			onWidthChanged: {
				requestPaintPause.restart();
			}

			onHeightChanged: {
				requestPaintPause.restart();
			}

			onImageLoaded: {
				requestPaintPause.restart();
			}

			onScaleCoeffChanged: {
				//console.log("scaleCoeff:::", scaleCoeff)
				requestPaintPause.restart();
			}

			onDeltaXChanged: {
				graphicsView.contentX = -deltaX
			}

			onDeltaYChanged: {
				graphicsView.contentY = -canvas.deltaY
			}


			PauseAnimation {
				id: requestPaintPause;
				duration: 10
				onFinished: {
					canvas.requestPaint();
				}
			}

			CanvasMatrix{
				id: canvasMatrix;
			}
			CanvasMatrix{
				id: canvasMatrixDefault;
			}

			function setScale(newScale, scaleX, scaleY){
				if (newScale < 0.0000001){
					return;
				}

				let scaleCoeffBack = canvas.scaleCoeff;
				if (scaleCoeffBack * newScale + canvas.deltaX === 0 ||
						scaleCoeffBack * newScale + canvas.deltaY === 0){

					return;
				}

				let newX = (scaleX - canvas.deltaX) / scaleCoeffBack * newScale + canvas.deltaX
				let newY = (scaleY - canvas.deltaY) / scaleCoeffBack * newScale + canvas.deltaY
				canvas.deltaX -= (newX - scaleX)
				canvas.deltaY -= (newY - scaleY)

				canvas.scaleCoeff = newScale
				canvas.scaleCoeffPrev = newScale;
			}

			onPaint: {
				//console.log("Canvas::onPaint")
				if (graphicsView.autoFit){
					graphicsView.zoomToFit(false);
				}

				var ctx = canvas.getContext('2d');
				ctx.reset()
				ctx.clearRect(0, 0, canvas.width, canvas.height)

				canvasMatrix.setXScale(canvas.scaleCoeff);
				canvasMatrix.setYScale(canvas.scaleCoeff);
				canvasMatrix.setXTranslation(canvas.deltaX);
				canvasMatrix.setYTranslation(canvas.deltaY);

				for(let i = 0; i < graphicsView.layerModel.length; i++){
					let layer = graphicsView.layerModel[i];
					if(layer.canApplyViewTransform){
						layer.draw(ctx, canvasMatrix)
					}
					else {
						layer.draw(ctx, canvasMatrixDefault)
					}
				}

			}//onPaint

		}//canvas


	}//mainContainer


	Shortcut {
		sequence: "Ctrl+C";
		enabled: true;
		onActivated: {
			console.log("Ctrl+C");
			graphicsView.copySignal(canvas.selectedIndex);
		}
	}

	Shortcut {
		id: pasteShortcut;

		sequence: "Ctrl+V";
		enabled: true;
		onActivated: {
			console.log("Ctrl+V");
			graphicsView.pasteSignal(canvas.selectedIndex);
		}
	}


	Shortcut {
		sequence: "Delete";
		enabled: true;
		onActivated: {
			console.log("Delete");
			graphicsView.deleteSignal(canvas.selectedIndex);
		}
	}

	Shortcut {
		sequence: "Ctrl+Z";
		enabled: true;
		onActivated: {
			console.log("Ctrl+Z");
			graphicsView.revertSignal();
		}
	}

	Shortcut {
		sequence: "F2";
		enabled: true;
		onActivated: {
			console.log("F2");
			graphicsView.renameSignal(canvas.selectedIndex);
		}
	}

	Shortcut {
		sequence: "Ctrl+0";
		enabled: true;
		onActivated: {
			//console.log("Ctrl + 0");
			graphicsView.zoomToFit(true);
		}
	}

	function copyObjectFunc(index){
		if(index >= 0){
			// bufferModel.clear();
			// bufferModel.copyItemDataFromModel(0, graphicsView.objectsModel, index);
		}
	}

	function pasteObjectFunc(){
		if(bufferModel.getItemsCount()){
			bufferModel.setExternTreeModel("links", emptyModel, 0);
			let mainText = bufferModel.getData("mainText") + "_1";
			let secondText = bufferModel.getData("secondText") + "_1";
			let x_ = bufferModel.getData("x") + 10;
			let y_ = bufferModel.getData("y") + 10;

			//TO DO: unique Id
			var date = new Date();
			let datString = String(date.valueOf());
			bufferModel.setData("id", datString, 0);

			bufferModel.setData("mainText", mainText, 0);
			bufferModel.setData("secondText", secondText, 0);
			bufferModel.setData("x", x_, 0);
			bufferModel.setData("y", y_, 0);
			bufferModel.setData("Selected", false, 0);
			// let index = graphicsView.objectsModel.insertNewItem();
			// graphicsView.objectsModel.copyItemDataFromModel(index, bufferModel,0);

			canvas.selectedIndex = 100//index;

			canvas.requestPaint();
		}
	}

	function deleteObjectFunc(index){
		console.log("DELETE OBJECT: ", index);
		let id = graphicsView.objectsModel.get(index).item.m_id;

		//remove links
		for(let i = 0; i < graphicsView.objectsModel.count; i++){
			let item = graphicsView.objectsModel.get(i).item;
			let links = item.m_links;
			if(links !== undefined){
				for(let k = 0; k < links.count; k++){
					let itemLink = links.get(k).item;
					let objectId = itemLink.m_objectId;
					if(objectId == id){
						links.removeItem(k);
						break;
					}
				}
			}
		}

		//remove object
		graphicsView.objectsModel.removeItem(index);

		canvas.selectedIndex = -1;

		canvas.requestPaint();
	}


	CustomScrollbar{
		id: scrollVert;

		anchors.right: parent.right;
		anchors.rightMargin: 1;
		anchors.bottom: parent.bottom;

		secondSize: 12;
		visible: (backgroundRec.topY >=0 && backgroundRec.bottomY <= graphicsView.height) ? false : true;
		alwaysVisible: true;
		canDragOutOfBounds: true;
		targetItem: parent;
		z: (scrollVert.isOnTop && scrollHoriz.isOnTop) ? 21 : (!scrollVert.isOnTop && !scrollHoriz.isOnTop) ? 21 : 20 + scrollVert.isOnTop;
		property bool isOnTop: true;
		onContentYSignal: {
			canvas.requestPaint();
		}
		onMovingSignal: {
			scrollVert.isOnTop = true;
			scrollHoriz.isOnTop = false;
		}

	}

	CustomScrollbar{
		id: scrollHoriz;

		anchors.right: parent.right;
		anchors.bottom: parent.bottom;
		anchors.bottomMargin: 1;

		secondSize: 12;

		visible: (backgroundRec.leftX >=0 && backgroundRec.rightX <= graphicsView.width) ? false : true;
		alwaysVisible: true;
		canDragOutOfBounds: true;

		vertical: false;
		targetItem: parent;
		z: (scrollHoriz.isOnTop && scrollVert.isOnTop) ? 20 : (!scrollHoriz.isOnTop && !scrollVert.isOnTop) ? 20 : 20 + scrollHoriz.isOnTop;
		property bool isOnTop: false;
		onContentXSignal: {
			canvas.requestPaint();
		}
		onMovingSignal: {
			scrollVert.isOnTop = false;
			scrollHoriz.isOnTop = true;
		}
	}

	Column{
		id: rightButtonColumn;

		anchors.right: parent.right;
		anchors.top: parent.top;
		anchors.rightMargin: Style.marginL
		anchors.topMargin: Style.marginS

		width: Style.iconSizeM

		spacing: Style.marginM;

		Button{
			id: zoomToFitButton;

			width: parent.width;
			height: width;

			iconSource: "../../../" + Style.getIconPath("Icons/FitToScreen", Icon.State.On, Icon.Mode.Normal)
			onClicked: {
				graphicsView.zoomToFit(true);
			}

		}
	}

}
