import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import Qt5Compat.GraphicalEffects 6.0

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
	property bool hideScrollbars: false;

	property real contentWidth: backgroundRec.width;
	property real contentX: -canvas.deltaX;
	property real originX: 0;

	property real scaleStep: 0.1;
	property real minZoomLevel: -1;
	property real maxZoomLevel: 10;
	property var translateXPositiveLimit;
	property var translateYPositiveLimit;
	property var translateXNegativeLimit;
	property var translateYNegativeLimit;
	property bool restrictZoom: false;
	property bool restrictMove: false;
	property int restrictMoveMargin: -1;
	property bool fitToBorders: false;

	property bool autoFit: false;

	property bool isSelectionMode: true;
	property bool isEditMode: false;
	property bool isPointsEditMode: false;
	property bool isPointsAdditionMode: false;
	property bool isPointsDeletionMode: false;

	property bool hasHoverReaction: true;

	property bool isMultiSelection: true;

	property real drawingAreaWidth: width;
	property real drawingAreaHeight: height;

	property var layerModel: []

	property Component controlPanelComp: null
	property Item controlPanelItem: null

	property alias scaleCoeff: canvas.scaleCoeff;
	property alias deltaX: canvas.deltaX;
	property alias deltaY: canvas.deltaY;
	property alias viewMatrix: canvasMatrix;

	property var hintShapeModel: []
	property var hintShapePointModel: []
	property var selectedShapeModel: []
	property var selectedShapePointModel: []

	property bool hasLeftButtonMenu: false;
	property bool hasRightButtonMenu: false;
	property var leftMenuCoordinates: Qt.point(Style.marginM, Style.marginS);
	property var rightMenuCoordinates: Qt.point(width - Style.marginM - Style.buttonWidthM, Style.marginS);

	signal copySignal(int index);
	signal pasteSignal(int index);
	signal deleteSignal(int index);
	signal renameSignal(int index);
	signal revertSignal();
	signal modelDataChanged();

	signal cursorPosition(point position)
	signal hintShape(int shapeIndex)
	signal hintShapePoint(int shapeIndex,int pointIndex)

	Component.onCompleted: {
		Events.subscribeEvent("DesignSchemeChanged", designSchemeChanged);
		Events.subscribeEvent("AppSizeChanged", appSizeChanged)

		let layerBackground = layerComp.createObject(this);
		layerBackground.layerId = "background";
		layerBackground.viewItem = graphicsView;
		layerModel.push(layerBackground);

		let layerInactive = layerComp.createObject(this);
		layerInactive.layerId = "inactive";
		layerInactive.viewItem = graphicsView;
		layerModel.push(layerInactive);

		let layerActive = layerComp.createObject(this);
		layerActive.layerId = "active";
		layerActive.viewItem = graphicsView;
		layerActive.isActive = true;
		layerModel.push(layerActive);

		let layerTools = layerComp.createObject(this);
		layerTools.layerId = "tools";
		layerTools.viewItem = graphicsView;
		layerTools.canApplyViewTransform = false;
		layerModel.push(layerTools);

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
		requestPaintPause.restart();
	}

	onAutoFitChanged: {
		if (autoFit){
			appSizeChanged();
		}
	}

	function requestPaint(){
		//console.log("Canvas::requestPaint")
		requestPaintPause.restart();
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
	function getToolsLayer(){
		return getLayer("tools");
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
		requestPaintPause.restart()
	}

	function appSizeChanged(params){
		if (autoFit){
			resetView(false);

			requestPaint()
		}
	}

	function setAutoFit(autoFit){
		graphicsView.autoFit = autoFit;
	}

	function zoomIn(){
		if (canvas.scaleCoeff < graphicsView.maxZoomLevel){
			let scaleCoeff_ = canvas.scaleCoeff;
			scaleCoeff_ += scaleStep * canvas.scaleCoeff;
			canvas.setScale(scaleCoeff_, canvas.width / 2, canvas.height / 2)
		}
	}

	function zoomOut(){
		if(canvas.scaleCoeff > graphicsView.minZoomLevel){
			let scaleCoeff_ = canvas.scaleCoeff;
			scaleCoeff_ -= scaleStep * canvas.scaleCoeff;
			canvas.setScale(scaleCoeff_, canvas.width / 2, canvas.height / 2)
		}
	}

	function setZoom(scaleCoeff_, scaleX, scaleY){
		if(scaleX == undefined){
			scaleX = canvas.width / 2;
		}
		if(scaleY == undefined){
			scaleY = canvas.height / 2;
		}
		canvas.setScale(scaleCoeff_, canvas.width / 2, canvas.height / 2)
	}

	function resetZoom(){
		canvas.scaleCoeff = 1.0;
		canvas.scaleCoeffPrev = 1.0;

		requestPaint()
	}

	function resetView(requestPaint_){
		canvas.scaleCoeff = 1
		canvas.scaleCoeffPrev = 1
		canvas.deltaX = 0;
		canvas.deltaY = 0;

		canvasMatrix.setXScale(canvas.scaleCoeff)
		canvasMatrix.setYScale(canvas.scaleCoeff)
		canvasMatrix.setXTranslation(canvas.deltaX)
		canvasMatrix.setYTranslation(canvas.deltaY)

		if(requestPaint_ == undefined){
			requestPaint_ = false;
		}
		if(requestPaint_){
			requestPaintPause.restart()		}

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
			//console.log("Go inside");
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

		canvasMatrix.setXTranslation(canvas.deltaX);
		canvasMatrix.setYTranslation(canvas.deltaY);
		canvasMatrix.setXScale(canvas.scaleCoeff);
		canvasMatrix.setYScale(canvas.scaleCoeff);

		for(let i = 0; i < graphicsView.layerModel.length ; i++){
			let layer = graphicsView.layerModel[i];
			if(layer.layerId == "active" /*|| layer.layerId == "inactive"*/){
				let shapeModel = layer.shapeModel;

				for(let j = 0; j < shapeModel.length; j++){
					let shape = shapeModel[j];

					let isInside =  shape.isInside(mouseX, mouseY)
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
	}

	function hoverReaction(position){
		if(!graphicsView.hasHoverReaction){
			return;
		}
		let activeLayer = getActiveLayer()

		let matrix = LinearAlgebra.multiplyByMatrix3x3(canvasMatrix.matrix, activeLayer.layerMatrix.matrix)
		matrix = LinearAlgebra.getInvertedMatrix3x3(matrix)

		position = LinearAlgebra.transformPoint2d(position, matrix);

		for (let i = 0; i < activeLayer.shapeModel.length; i++){
			let shape = activeLayer.shapeModel[i];
			shape.mousePositionChanged(position)
		}
	}

	function fitToShapeList(shapeList, layer){
		resetView();

		let onlyOneLayer = layer !==undefined;
		let margin_ = Style.marginXXL;

		let minX = 1000000;
		let minY = 1000000;
		let maxX = -1000000;
		let maxY = -1000000;

		let screenPointBottomLeftX = 0;
		let screenPointBottomLeftY = graphicsView.height;

		let clipRectWidth = graphicsView.width;
		let clipRectHeight = graphicsView.height;

		if(onlyOneLayer){
			let clipRect = layer.clipRect;
			if(clipRect.width == 0 && clipRect.height == 0){
				clipRect.width = canvas.width
				clipRect.height = canvas.height
			}
			clipRectWidth = clipRect.width;
			clipRectHeight = clipRect.height;
			screenPointBottomLeftX = clipRect.x
			screenPointBottomLeftY = clipRect.y + clipRect.height
		}

		for (let i = 0; i < shapeList.length; i++){
			let shape = shapeList[i];
			let boundingBoxPoints = shape.getBoundingBoxPoints(true);
			if(!Object.keys(boundingBoxPoints).length){
				continue;
			}

			if((boundingBoxPoints.topLeftPoint).x < minX){
				minX = boundingBoxPoints.topLeftPoint.x
			}
			if((boundingBoxPoints.topRightPoint).x > maxX){
				maxX = boundingBoxPoints.topRightPoint.x
			}
			if((boundingBoxPoints.topLeftPoint).y < minY){
				minY = boundingBoxPoints.topLeftPoint.y
			}
			if((boundingBoxPoints.bottomRightPoint).y > maxY){
				maxY = boundingBoxPoints.bottomRightPoint.y
			}

			if(!onlyOneLayer){
				let layerCurr = shape.layer;
				let clipRectCurr = layerCurr.clipRect;
				if(clipRectCurr.width == 0 && clipRectCurr.height == 0){
					clipRectCurr.width = canvas.width
					clipRectCurr.height = canvas.height
				}

				if(clipRectCurr.width < clipRectWidth){
					clipRectWidth = clipRectCurr.width;
				}
				if(clipRectCurr.height < clipRectHeight){
					clipRectHeight = clipRectCurr.height;
				}
				if(clipRectCurr.x > screenPointBottomLeftX){
					screenPointBottomLeftX = clipRectCurr.x
				}
				if(clipRectCurr.y + clipRectCurr.height < screenPointBottomLeftY){
					screenPointBottomLeftY = clipRectCurr.y + clipRectCurr.height
				}
			}
		}

		let zoomX = (clipRectWidth - margin_)/((maxX - minX)!==0 ? Math.abs(maxX - minX) : 1)
		let zoomY = (clipRectHeight - margin_)/((maxY - minY)!==0 ? Math.abs(maxY - minY) : 1)
		let zoom = Math.min(zoomX, zoomY)

		graphicsView.setZoom(zoom)
		canvasMatrix.setXScale(zoom)
		canvasMatrix.setYScale(zoom)
		canvasMatrix.setXTranslation(canvas.deltaX)
		canvasMatrix.setYTranslation(canvas.deltaY)

		minX = 1000000;
		maxY = -1000000;

		for (let i = 0; i < shapeList.length; i++){
			let shape = shapeList[i];
			let boundingBoxPoints = shape.getBoundingBoxPoints(true);
			if(!Object.keys(boundingBoxPoints).length){
				continue;
			}

			if((boundingBoxPoints.bottomLeftPoint).x < minX){
				minX = boundingBoxPoints.bottomLeftPoint.x
			}
			if((boundingBoxPoints.bottomLeftPoint).y > maxY){
				maxY = boundingBoxPoints.bottomLeftPoint.y
			}
		}

		let screenPointBottomLeft = Qt.point(screenPointBottomLeftX + margin_/2, screenPointBottomLeftY - margin_/2)
		let screenPointMinXMaxY = Qt.point(minX, maxY)

		canvas.deltaX += (screenPointBottomLeft.x - screenPointMinXMaxY.x)
		canvas.deltaY += (screenPointBottomLeft.y - screenPointMinXMaxY.y)
	}

	function fitToLayer(layer){
		fitToShapeList(layer.shapeModel, layer)
	}

	function fitToShape(shape){
		let shapeList = []
		shapeList.push(shape)
		fitToShapeList(shapeList, shape.layer)
	}

	function fitToActiveLayer(){
		let activeLayer = getActiveLayer();
		fitToLayer(activeLayer)
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

					requestPaintPause.restart()

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
				requestPaintPause.restart();
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
				let activeLayer = graphicsView.getActiveLayer()
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

				//moving all scene
				if(!found){
					graphicsView.autoFit = false;

					let borderMargin = graphicsView.restrictMoveMargin;

					let wasLimitCorrection = canvas.deltaCorrection(canvas.deltaX + delta.x, canvas.deltaY + delta.y)

					let okScale = graphicsView.restrictMove && canvas.scaleCoeff <= graphicsView.minZoomLevel ? false : true
					let okWidth = borderMargin < 0 ? true : canvas.deltaX + delta.x > -1*(canvas.width * canvas.scaleCoeff  - borderMargin)  && canvas.deltaX + delta.x < (canvas.width - borderMargin);
					let okHeight = borderMargin < 0 ? true : canvas.deltaY + delta.y > -1*(canvas.height * canvas.scaleCoeff  - borderMargin)  && canvas.deltaY + delta.y < (canvas.height - borderMargin);

					let ok = okScale && okWidth && okHeight ;

					//console.log(wasLimitCorrection, okScale, okWidth, okHeight)

					if(!wasLimitCorrection && ok){
						canvas.deltaX += delta.x
						canvas.deltaY += delta.y
					}
				}

				canvas.requestPaint()
				//requestPaintPause.restart();
			}

			onWheel: {
				if (canvas.scaleCoeff == 0 || graphicsView.restrictZoom){
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
							// if(graphicsView.fitToBorders){
							// 	graphicsView.resetView(false)
							// }
						}
						else {
							scaleCoeff_ += scaleStep  * canvas.scaleCoeff;
						}
						if(scaleCoeff_ > graphicsView.maxZoomLevel){
							scaleCoeff_ = graphicsView.maxZoomLevel
						}
						// if(Math.abs(scaleCoeff_ - 1) <  0.000001){
						// 	if(graphicsView.fitToBorders){
						// 		graphicsView.resetView(false)
						// 	}
						// }
					}
				}
				else{//down
					let minZoom = graphicsView.minZoomLevel > -1 ? graphicsView.minZoomLevel : scaleStep * 2;
					if(canvas.scaleCoeff > minZoom){
						if(canvas.scaleCoeff > 1 && (canvas.scaleCoeff - scaleStep) < 1){
							scaleCoeff_ = 1
							// if(graphicsView.fitToBorders){
							// 	graphicsView.resetView(false)
							// }
						}
						else {
							scaleCoeff_ -= scaleStep * canvas.scaleCoeff;
						}
						if(scaleCoeff_ < minZoom){
							scaleCoeff_ = minZoom;
						}
						// if(Math.abs(scaleCoeff_ - 1) <  0.000001){
						// 	if(graphicsView.fitToBorders){
						// 		graphicsView.resetView(false)
						// 	}
						// }
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
					graphicsView.hoverReaction(position);
					graphicsView.cursorPosition(position)
				}
			}
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
			renderStrategy: Canvas.Threaded

			//antialiasing: true;

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

				let wasLimitCorrection = deltaCorrection(canvas.deltaX - (newX - scaleX), canvas.deltaY - (newY - scaleY), newScale);

				if(!wasLimitCorrection){
					canvas.deltaX -= (newX - scaleX)
					canvas.deltaY -= (newY - scaleY)
				}

				canvas.scaleCoeff = newScale
				canvas.scaleCoeffPrev = newScale;
			}

			function deltaCorrection(deltaXArg, deltaYArg, scale_){
				if(scale_ == undefined){
					scale_ = canvas.scaleCoeff;
				}
				let wasLimitCorrection = false;
				if(graphicsView.translateXPositiveLimit !== undefined && deltaXArg > -graphicsView.translateXPositiveLimit * (scale_ - 1)){
					canvas.deltaX = -graphicsView.translateXPositiveLimit * (scale_ - 1)
					wasLimitCorrection = true
				}
				if(graphicsView.translateYPositiveLimit !== undefined && deltaYArg > -graphicsView.translateYPositiveLimit * (scale_ - 1)){
					canvas.deltaY = -graphicsView.translateYPositiveLimit* (scale_ - 1)
					wasLimitCorrection = true
				}
				if(graphicsView.translateXNegativeLimit !== undefined && deltaXArg < graphicsView.translateXNegativeLimit * (scale_ - 1) - canvas.width  *(scale_-1)){
					canvas.deltaX = graphicsView.translateXNegativeLimit * (scale_ - 1) - canvas.width * (scale_-1)
					wasLimitCorrection = true
				}
				if(graphicsView.translateYNegativeLimit !== undefined && deltaYArg < graphicsView.translateYNegativeLimit *(scale_-1) - canvas.height  *(scale_-1)){
					canvas.deltaY = graphicsView.translateYNegativeLimit *(scale_-1) - canvas.height * (scale_-1)
					wasLimitCorrection = true
				}
				return wasLimitCorrection;
			}

			onPaint: {
				//console.log("Canvas::onPaint")
				if (graphicsView.autoFit){
					graphicsView.resetView(false);
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
			graphicsView.resetView(true);
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

			requestPaintPause.restart();
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

		requestPaintPause.restart();
	}


	CustomScrollbar{
		id: scrollVert;

		anchors.right: parent.right;
		anchors.rightMargin: 1;
		anchors.bottom: parent.bottom;

		secondSize: 12;
		visible: graphicsView.hideScrollbars ? false : (backgroundRec.topY >=0 && backgroundRec.bottomY <= graphicsView.height) ? false : true;
		alwaysVisible: true;
		canDragOutOfBounds: true;
		targetItem: parent;
		z: (scrollVert.isOnTop && scrollHoriz.isOnTop) ? 21 : (!scrollVert.isOnTop && !scrollHoriz.isOnTop) ? 21 : 20 + scrollVert.isOnTop;
		property bool isOnTop: true;
		onContentYSignal: {
			requestPaintPause.restart();
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

		visible: graphicsView.hideScrollbars ? false : (backgroundRec.leftX >=0 && backgroundRec.rightX <= graphicsView.width) ? false : true;
		alwaysVisible: true;
		canDragOutOfBounds: true;

		vertical: false;
		targetItem: parent;
		z: (scrollHoriz.isOnTop && scrollVert.isOnTop) ? 20 : (!scrollHoriz.isOnTop && !scrollVert.isOnTop) ? 20 : 20 + scrollHoriz.isOnTop;
		property bool isOnTop: false;
		onContentXSignal: {
			requestPaintPause.restart();
		}
		onMovingSignal: {
			scrollVert.isOnTop = false;
			scrollHoriz.isOnTop = true;
		}
	}

	// left button panel
	Button{
		id: leftMenuButton;

		x: graphicsView.leftMenuCoordinates.x
		y: graphicsView.leftMenuCoordinates.y

		width: Style.buttonWidthM;
		height: width;
		visible: graphicsView.hasLeftButtonMenu;

		iconSource: "../../../" + Style.getIconPath("Icons/More", Icon.State.On, Icon.Mode.Normal)
		onClicked: {
			leftMenuButtonPanel.isOpen = !leftMenuButtonPanel.isOpen
		}
	}

	DropShadow {
		anchors.fill: leftMenuButtonPanel;

		horizontalOffset: 1;
		verticalOffset: 1;

		radius: leftMenuButtonPanel.radius;
		color: Style.shadowColor;
		visible: leftMenuButton.visible;

		source: leftMenuButtonPanel;
	}

	Rectangle{
		id: leftMenuButtonPanel;

		anchors.left: leftMenuButton.right;
		anchors.leftMargin: Style.marginXS;
		anchors.verticalCenter: leftMenuButton.verticalCenter;

		height: Style.buttonWidthM + Style.marginXS;
		radius: 4;

		color: Style.baseColor;
		border.color: Style.shadowColor;
		clip: true;
		visible: leftMenuButton.visible;

		property bool isOpen: false;
		onIsOpenChanged: {
			if(isOpen){
				leftMenuWidthAnim.from = leftMenuButtonPanel.width;
				leftMenuWidthAnim.to = leftMenuButtonRow.width + 2*leftMenuButtonRow.anchors.leftMargin;
				leftMenuWidthAnim.start()
			}
			else {
				leftMenuWidthAnim.from = leftMenuButtonPanel.width;
				leftMenuWidthAnim.to = 0;
				leftMenuWidthAnim.start()
			}
		}

		NumberAnimation {
			id: leftMenuWidthAnim

			target: leftMenuButtonPanel
			property: "width"
			duration: 100
		}

		Row{
			id: leftMenuButtonRow;

			anchors.left: parent.left;
			anchors.leftMargin: Style.marginXS;
			anchors.verticalCenter: parent.verticalCenter;

			height: leftMenuButton.height
			spacing: Style.marginXS;

			Button{
				id: selectModeButton;

				anchors.verticalCenter: parent.verticalCenter;

				width: Style.buttonWidthM;
				height: width;

				property bool isActive: graphicsView.isSelectionMode;

				iconSource: "../../../" + Style.getIconPath("Icons/Pointer", Icon.State.On, isActive ? Icon.Mode.Normal : Icon.Mode.Disabled)
				onClicked: {
					graphicsView.isSelectionMode = true;
					graphicsView.isEditMode = false;
					graphicsView.isPointsEditMode = false;
					graphicsView.isPointsAdditionMode = false;
					graphicsView.isPointsDeletionMode = false;
				}
			}
			Button{
				id: editModeButton;

				anchors.verticalCenter: parent.verticalCenter;

				width: Style.buttonWidthM;
				height: width;

				property bool isActive: graphicsView.isEditMode;

				iconSource: "../../../" + Style.getIconPath("Icons/Edit", Icon.State.On, isActive ? Icon.Mode.Normal : Icon.Mode.Disabled)
				onClicked: {
					graphicsView.isSelectionMode = false;
					graphicsView.isEditMode = true;
					graphicsView.isPointsEditMode = false;
					graphicsView.isPointsAdditionMode = false;
					graphicsView.isPointsDeletionMode = false;
				}
			}
			Button{
				id: editPointsModeButton;

				anchors.verticalCenter: parent.verticalCenter;

				width: Style.buttonWidthM;
				height: width;

				property bool isActive: graphicsView.isPointsEditMode;

				iconSource: "../../../" + Style.getIconPath("Icons/PointsMove", Icon.State.On, isActive ? Icon.Mode.Normal : Icon.Mode.Disabled)
				onClicked: {
					graphicsView.isSelectionMode = false;
					graphicsView.isEditMode = false;
					graphicsView.isPointsEditMode = true;
					graphicsView.isPointsAdditionMode = false;
					graphicsView.isPointsDeletionMode = false;
				}
			}
			Button{
				id: addPointsModeButton;

				anchors.verticalCenter: parent.verticalCenter;

				width: Style.buttonWidthM;
				height: width;

				property bool isActive: graphicsView.isPointsAdditionMode;

				iconSource: "../../../" + Style.getIconPath("Icons/PointsAdd", Icon.State.On, isActive ? Icon.Mode.Normal : Icon.Mode.Disabled)
				onClicked: {
					graphicsView.isSelectionMode = false;
					graphicsView.isEditMode = false;
					graphicsView.isPointsEditMode = false;
					graphicsView.isPointsAdditionMode = true;
					graphicsView.isPointsDeletionMode = false;
				}
			}
			Button{
				id: deletePointsModeButton;

				anchors.verticalCenter: parent.verticalCenter;

				width: Style.buttonWidthM;
				height: width;

				property bool isActive: graphicsView.isPointsDeletionMode;

				iconSource: "../../../" + Style.getIconPath("Icons/PointsSub", Icon.State.On, isActive ? Icon.Mode.Normal : Icon.Mode.Disabled)
				onClicked: {
					graphicsView.isSelectionMode = false;
					graphicsView.isEditMode = false;
					graphicsView.isPointsEditMode = false;
					graphicsView.isPointsAdditionMode = false;
					graphicsView.isPointsDeletionMode = true;
				}
			}
		}
	}


	// right button panel
	Button{
		id: rightMenuButton;

		x: graphicsView.rightMenuCoordinates.x
		y: graphicsView.rightMenuCoordinates.y

		width: Style.buttonWidthM;
		height: width;

		rotation: 90;
		visible: graphicsView.hasRightButtonMenu;

		iconSource: "../../../" + Style.getIconPath("Icons/More", Icon.State.On, Icon.Mode.Normal)
		onClicked: {
			rightMenuButtonPanel.isOpen = !rightMenuButtonPanel.isOpen
		}
	}
	DropShadow {

		anchors.fill: rightMenuButtonPanel;

		horizontalOffset: 1;
		verticalOffset: 1;

		radius: rightMenuButtonPanel.radius;
		visible: rightMenuButton.visible;
		color: Style.shadowColor;

		source: rightMenuButtonPanel;
	}

	Rectangle{
		id: rightMenuButtonPanel;

		anchors.top: rightMenuButton.bottom;
		anchors.topMargin: Style.marginXS;
		anchors.horizontalCenter: rightMenuButton.horizontalCenter;

		width: Style.buttonWidthM + Style.marginXS;
		radius: 4;

		color: Style.baseColor;
		border.color: Style.shadowColor;
		clip: true;
		visible: rightMenuButton.visible;

		property bool isOpen: false;
		onIsOpenChanged: {
			if(isOpen){
				rightMenuWidthAnim.from = rightMenuButtonPanel.height;
				rightMenuWidthAnim.to = rightMenuButtonColumn.height + 2*rightMenuButtonColumn.anchors.topMargin;
				rightMenuWidthAnim.start()
			}
			else {
				rightMenuWidthAnim.from = rightMenuButtonPanel.height;
				rightMenuWidthAnim.to = 0;
				rightMenuWidthAnim.start()
			}
		}

		NumberAnimation {
			id: rightMenuWidthAnim

			target: rightMenuButtonPanel
			property: "height"
			duration: 100
		}

		Column{
			id: rightMenuButtonColumn;

			anchors.top: parent.top;
			anchors.topMargin: Style.marginXS;
			anchors.horizontalCenter: parent.horizontalCenter;

			width: rightMenuButton.width
			spacing: Style.marginXS;

			Button{
				id: zoomIncreaseButton;

				width: parent.width;
				height: width;

				iconSource: "../../../" + Style.getIconPath("Icons/ZoomIn", Icon.State.On, Icon.Mode.Normal)
				onClicked: {
					graphicsView.zoomIn();
				}
				onPressed: {
					graphicsView.zoomIn()
					zoomInAnim.start();
				}
				onReleased: {
					zoomInAnim.stop();
				}
			}
			Button{
				id: zoomDecreaseButton;

				width: parent.width;
				height: width;

				iconSource: "../../../" + Style.getIconPath("Icons/ZoomOut", Icon.State.On, Icon.Mode.Normal)
				onClicked: {

				}
				onPressed: {
					graphicsView.zoomOut()
					zoomOutAnim.start();
				}
				onReleased: {
					zoomOutAnim.stop();
				}

			}

			Button{
				id: fitToScreenButton;

				width: parent.width;
				height: width;

				iconSource: "../../../" + Style.getIconPath("Icons/ZoomToFit", Icon.State.On, Icon.Mode.Normal)
				onClicked: {
					graphicsView.fitToActiveLayer()
				}
			}

			Button{
				id: resetViewButton;

				width: parent.width;
				height: width;

				//iconSource: "../../../" + Style.getIconPath("Icons/ZoomReset", Icon.State.On, Icon.Mode.Normal)
				iconSource: "../../../" + Style.getIconPath("Icons/FitToScreen", Icon.State.On, Icon.Mode.Normal)
				onClicked: {
					graphicsView.resetView(true);
				}

			}

		}


		PauseAnimation {
			id: zoomInAnim;
			duration: 500
			onFinished: {
				graphicsView.zoomIn()
				start()
			}
		}
		PauseAnimation {
			id: zoomOutAnim;
			duration: 500
			onFinished: {
				graphicsView.zoomOut()
				start()
			}

		}
	}



}
