import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import Qt5Compat.GraphicalEffects 6.0

import imtcontrols 1.0


Rectangle {
	id: graphicsView;

	clip: true;

	color: Style.baseColor;

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
	property real maxZoomLevel: 1000;
	property var translateXPositiveLimit;
	property var translateYPositiveLimit;
	property var translateXNegativeLimit;
	property var translateYNegativeLimit;
	property bool receiveMouseEvents: false;
	property bool restrictZoom: false;
	property bool restrictMove: false;
	property bool restrictSelect: false;
	property bool restrictMouseReacion: false;
	property bool zoomOnlyInCenter: false;
	property int restrictMoveMargin: -1;
	property bool fitToBorders: false;

	property bool fitToViewMode: false;

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
	property alias mouseArea: controlArea;
	property alias canvasAlias: canvas;

	property var hintShapeModel: []
	property var hintShapePointModel: []
	property var selectedShapeModel: []
	property var selectedShapePointModel: []

	property bool hasLeftButtonMenu: false;
	property bool hasRightButtonMenu: false;
	property var leftMenuCoordinates: Qt.point(Style.marginM, Style.marginS);
	property var rightMenuCoordinates: Qt.point(width - Style.marginL - Style.buttonWidthL, Style.marginS);
	property bool leftMenuIsOpen: false
	property bool rightMenuIsOpen: false

	property bool hasZoomInButton: true;
	property bool hasZoomOutButton: true;
	property bool hasFitToViewButton: true;
	property bool hasResetViewButton: true;

	property bool propagateWheelEvents: false;
	property bool propagateMouseEvents: false;

	property bool restrictDrawing: false;
	property int resizePauseDuration: 100;

	property bool canvasAntialiasing: true
	property int renderStrategy: Canvas.Threaded
	property int paintPauseDuration: 10

	signal copySignal(int index);
	signal pasteSignal(int index);
	signal deleteSignal(int index);
	signal renameSignal(int index);
	signal revertSignal();
	signal modelDataChanged();

	signal cursorPosition(point position)
	signal hintShape(int shapeIndex)
	signal hintShapePoint(int shapeIndex,int pointIndex)

	signal paintTime(int time)

	signal mouseClicked(var mouse)
	signal mousePressed(var mouse)
	signal mouseReleased(var mouse)
	signal mouseDoubleClicked(var mouse)
	signal mousePositionChanged(var mouse)

	signal painted()
	signal imageLoaded()

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

	onWidthChanged: {
		graphicsView.restrictDrawing = true
		sizeChangedPause.restart()
	}

	onHeightChanged: {
		graphicsView.restrictDrawing = true
		sizeChangedPause.restart()
	}


	PauseAnimation {
		id: sizeChangedPause;
		duration: graphicsView.resizePauseDuration
		onFinished: {
			graphicsView.restrictDrawing = false
			graphicsView.resize()
		}
	}

	function resize(){
		if(graphicsView.fitToViewMode){
			fitToView();
		}
		else {
			requestPaintPause.restart();
		}
	}

	function fitToView(){
		fitToActiveLayer();
	}

	function requestPaint(){
		//console.log("Canvas::requestPaint")
		requestPaintPause.restart();
	}

	function paintImmediately(){
		canvas.requestPaint()
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

	function loadImage(source){
		canvas.loadImage(source)
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
		console.log("designSchemeChanged!!!!")
		requestPaintPauseVeryLong.restart()
	}

	function appSizeChanged(params){
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

		canvas.setViewMatrixParams();

		if(requestPaint_ == undefined){
			requestPaint_ = false;
		}
		if(requestPaint_){
			requestPaintPause.restart()		}

	}

	function findObject(mouseX, mouseY, selectionCodition){
		//console.log("findObject", mouseX, mouseY)

		canvas.setViewMatrixParams();

		if(graphicsView.isEditMode && canvas.selectedShapeCount){
			if(canvas.editShape && canvas.editShape.isInsideBoundingBox(mouseX, mouseY)){
				return canvas.editShape
			}
		}
		for(let i = 0; i < graphicsView.layerModel.length ; i++){
			let layer = graphicsView.layerModel[i];
			if(layer.layerId == "active" /*|| layer.layerId == "inactive"*/){
				let shapeModel = layer.shapeModel;

				for(let j = 0; j < shapeModel.length; j++){
					let shape = shapeModel[j];
					if(!shape.canSelect && selectionCodition){
						continue
					}

					//let isInside =  shape.isInside(mouseX, mouseY)
					let isInside = graphicsView.isEditMode && shape.isSelected ? shape.isInsideBoundingBox(mouseX, mouseY) : shape.isInside(mouseX, mouseY)
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
			if(layer.layerId == "active" /*|| layer.layerId == "inactive"*/){
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

		for (let i = 0; i < activeLayer.shapeModel.length; i++){
			let shape = activeLayer.shapeModel[i];
			shape.mousePositionChanged(position)
		}
	}

	function fitToShapeList(shapeList, layer){
		if(shapeList == undefined || !shapeList.length || width == 0 || height == 0){
			console.log("fitToShapeList return", width, height)
			return
		}

		resetView();

		let onlyOneLayer = layer !==undefined;
		let margin_ = Style.marginXXL;

		let minX = 0;
		let minY = 0;
		let maxX = 0;
		let maxY = 0;

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
			let boundingBoxPoints = graphicsView.boundingBoxPointsToScreen(shape.getBoundingBoxPoints(), shape);

			if(!Object.keys(boundingBoxPoints).length){
				continue;
			}

			if(i == 0 || (boundingBoxPoints.topLeftPoint).x < minX){
				minX = boundingBoxPoints.topLeftPoint.x
			}
			if(i == 0 || (boundingBoxPoints.topRightPoint).x > maxX){
				maxX = boundingBoxPoints.topRightPoint.x
			}
			if(i == 0 || (boundingBoxPoints.topLeftPoint).y < minY){
				minY = boundingBoxPoints.topLeftPoint.y
			}
			if(i == 0 || (boundingBoxPoints.bottomRightPoint).y > maxY){
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

		let zoomX = 1
		let zoomY = 1
		let zoom
		let tolerance = 0.0001
		if((maxX - minX) > tolerance && (maxY - minY) > tolerance){
			zoomX = (clipRectWidth - margin_)/((maxX - minX)!==0 ? Math.abs(maxX - minX) : 1)
			zoomY = (clipRectHeight - margin_)/((maxY - minY)!==0 ? Math.abs(maxY - minY) : 1)
			zoom = Math.min(zoomX, zoomY)
		}
		else {
			zoom = 1.05
		}

		graphicsView.setZoom(zoom)

		canvas.setViewMatrixParams();

		minX = 0;
		maxY = 0;

		for (let i = 0; i < shapeList.length; i++){
			let shape = shapeList[i];
			let boundingBoxPoints = graphicsView.boundingBoxPointsToScreen(shape.getBoundingBoxPoints(), shape);
			if(!Object.keys(boundingBoxPoints).length){
				continue;
			}

			if(i == 0 || (boundingBoxPoints.bottomLeftPoint).x < minX){
				minX = boundingBoxPoints.bottomLeftPoint.x
			}
			if(i == 0 || (boundingBoxPoints.bottomLeftPoint).y > maxY){
				maxY = boundingBoxPoints.bottomLeftPoint.y
			}
		}

		let screenPointBottomLeft = Qt.point(screenPointBottomLeftX + margin_/2, screenPointBottomLeftY - margin_/2)
		let screenPointMinXMaxY = Qt.point(minX, maxY)

		canvas.deltaX += (screenPointBottomLeft.x - screenPointMinXMaxY.x)
		canvas.deltaY += (screenPointBottomLeft.y - screenPointMinXMaxY.y)
	}

	function fitToLayer(layer){
		if(layer.shapeModel == undefined || !layer.shapeModel.length){
			return
		}
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

	function fitToInactivAndActiveLayer(){
		let activeLayer = getActiveLayer()
		let inactioveLayer = getInactiveLayer()
		if(!activeLayer || !inactioveLayer){
			requestPaint()
			return;
		}
		let activeShapes = activeLayer.shapeModel
		let inactiveShapes = inactioveLayer.shapeModel
		let shapeList = []
		for(let i = 0; i < activeShapes.length; i++){
			shapeList.push(activeShapes[i])
		}
		for(let i = 0; i < inactiveShapes.length; i++){
			shapeList.push(inactiveShapes[i])
		}
		fitToShapeList(shapeList)
	}

	function boundingBoxPointsToScreen(pointsObjLog, shape){

		let pointsObj = ({});

		let minX = 0;
		let minY = 0;
		let maxX = 0
		let maxY = 0;

		let points = []

		points.push(Qt.point(pointsObjLog.topLeftPoint.x, pointsObjLog.topLeftPoint.y))
		points.push(Qt.point(pointsObjLog.topRightPoint.x, pointsObjLog.topRightPoint.y))
		points.push(Qt.point(pointsObjLog.bottomLeftPoint.x, pointsObjLog.bottomLeftPoint.y))
		points.push(Qt.point(pointsObjLog.bottomRightPoint.x, pointsObjLog.bottomRightPoint.y))

		for(let i = 0; i < points.length; i++){
			let point = points[i]
			point = shape.getScreenPosition(point);
			if (i == 0){
				minX = maxX = point.x
				minY = maxY = point.y

				continue
			}

			let x_ = point.x
			let y_ = point.y
			if(x_ < minX){
				minX = x_
			}
			if(y_ < minY){
				minY = y_
			}
			if(x_ > maxX){
				maxX = x_
			}
			if(y_ > maxY){
				maxY = y_
			}
		}

		pointsObj.topLeftPoint = Qt.point(minX, minY)
		pointsObj.topRightPoint = Qt.point(maxX, minY)
		pointsObj.bottomLeftPoint = Qt.point(minX, maxY)
		pointsObj.bottomRightPoint = Qt.point(maxX, maxY)

		return pointsObj;
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

			propagateMouseEvents: graphicsView.propagateMouseEvents
			propagateWheelEvents: graphicsView.propagateWheelEvents

			onClicked: {
				//console.log("CLICKED!!!")
				if(graphicsView.receiveMouseEvents){
					graphicsView.mouseClicked(mouse)
				}
				if(graphicsView.restrictMouseReacion){
					return
				}

				let activeLayer = graphicsView.getActiveLayer()

				for (let i = 0; i < activeLayer.shapeModel.length; i++){
					let shape = activeLayer.shapeModel[i];
					shape.mouseClicked(mouse)
				}
			}

			onPressed: {
				//console.log("PRESSED!!!")
				if(graphicsView.receiveMouseEvents){
					graphicsView.mousePressed(mouse)
				}
				if(graphicsView.restrictMouseReacion){
					return
				}

				let activeLayer = graphicsView.getActiveLayer()

				for (let i = 0; i < activeLayer.shapeModel.length; i++){
					let shape = activeLayer.shapeModel[i];
					shape.mousePressed(mouse)
				}

				let found = false;
				if(!(mouse.modifiers & Qt.ControlModifier) && !graphicsView.restrictSelect){// without Ctrl
					let shape = graphicsView.findObject(mouse.x, mouse.y, true)
					if(shape !== null){
						found = true;
						if(canvas.selectedShapeCount < 2){
							graphicsView.clearSelection(shape)
							canvas.selectedShapeCount = 1;
							if(graphicsView.isEditMode){
								canvas.editShape = shape;
							}
						}
						if(shape.canSelect){
							shape.isSelected = true;
						}
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
				if(graphicsView.receiveMouseEvents){
					graphicsView.mouseReleased(mouse)
				}
				if(graphicsView.restrictMouseReacion){
					return
				}

				let activeLayer = graphicsView.getActiveLayer()

				for (let i = 0; i < activeLayer.shapeModel.length; i++){
					let shape = activeLayer.shapeModel[i];
					shape.mouseReleased(mouse)
				}

				let shape = graphicsView.findObject(mouse.x, mouse.y)
				if(graphicsView.isSelectionMode && mouse.modifiers & Qt.ControlModifier && !graphicsView.restrictSelect){//Ctrl
					if(shape !== null){
						if(!shape.isSelected && shape.canSelect){
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
					if(!wasMoving && shape !== null && !graphicsView.restrictSelect){
						if(canvas.selectedShapeCount > 1){
							graphicsView.clearSelection(shape)
							canvas.selectedShapeCount = 1;
							if(graphicsView.isEditMode){
								canvas.editShape = shape;
							}
						}
						if(shape.canSelect){
							shape.isSelected = true;
						}
					}
				}
				requestPaintPause.restart();
				controlArea.cursorShape = Qt.ArrowCursor;
			}

			onDoubleClicked: {
				if(graphicsView.receiveMouseEvents){
					graphicsView.mouseDoubleClicked(mouse)
				}
				if(graphicsView.restrictMouseReacion){
					return
				}

				wasMoving = false;
				isPressed = false;

			}

			onDeltaSignal: {
				if (canvas.scaleCoeff < 0.0000001 || graphicsView.restrictMove){
					return;
				}

				//console.log("delta:::::: ", delta.x, delta.y)

				let found = false;
				let margin_ = Style.marginM;
				let activeLayer = graphicsView.getActiveLayer()
				for (let i = 0; i < activeLayer.shapeModel.length; i++){
					let shape = activeLayer.shapeModel[i];
					if(shape.isSelected /*&& graphicsView.isSelectionMode*/){
						found = true;

						let deltaX_ = delta.x
						let deltaY_ = delta.y

						// fit to borders
						let correctShapePosition = false
						if(correctShapePosition){
							//correction of position relative to borders
							let boundingBoxPoints = graphicsView.boundingBoxPointsToScreen(shape.getBoundingBoxPoints(), shape)
							let topLeftPoint = boundingBoxPoints.topLeftPoint

							// let topRightPoint = boundingBoxPoints.topRightPoint
							// let bottomLeftPoint = boundingBoxPoints.bottomLeftPoint
							// let bottomRightPoint = boundingBoxPoints.bottomRightPoint

							let width_ = (boundingBoxPoints.topRightPoint.x - boundingBoxPoints.topLeftPoint.x);
							let height_ = (boundingBoxPoints.bottomLeftPoint.y - boundingBoxPoints.topLeftPoint.y);

							let newX = (topLeftPoint.x + delta.x);
							let newY = (topLeftPoint.y + delta.y );

							if(newX < margin_ && delta.x < 0){
								newX = margin_;
							}
							if(newY < margin_ && delta.y < 0){
								newY = margin_;
							}

							if(newX > (canvas.width  - width_) - margin_ && delta.x > 0){
								newX = (canvas.width  - width_) - margin_
							}
							if(newY > (canvas.height  - height_) - margin_&& delta.y > 0){
								newY = (canvas.height  - height_) - margin_
							}

							deltaX_ = newX - topLeftPoint.x
							deltaY_ = newY - topLeftPoint.y
						}// fit to borders

						if(graphicsView.isSelectionMode){
							shape.setCoordinateShift(deltaX_, deltaY_)
						}
					}
				}

				//moving all scene
				if(!found){
					let borderMargin = graphicsView.restrictMoveMargin;

					let wasLimitCorrection = false//canvas.deltaCorrection(canvas.deltaX + delta.x, canvas.deltaY + delta.y)

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
					if(controlArea.propagateWheelEvents){
						wheel.accepted = false;
					}

					return
				}

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

				if(graphicsView.zoomOnlyInCenter){
					canvas.setScale(scaleCoeff_, canvas.width / 2, canvas.height / 2)
				}
				else {
					canvas.setScale(scaleCoeff_, wheel.x, wheel.y)
				}
			}


			function movingFunction(delta){//reimplemented
			}

			onPositionSignal: {
				if(graphicsView.isSelectionMode || graphicsView.isPointsEditMode){
					positionChangedPause.position = position;
					positionChangedPause.restart();
				}
			}

			onPositionChanged: {
				if(graphicsView.receiveMouseEvents){
					graphicsView.mousePositionChanged(mouse)
				}
				if(graphicsView.restrictMouseReacion){
					return
				}

				if(!graphicsView.isSelectionMode && controlArea.isPressed){
					let activeLayer = graphicsView.getActiveLayer()

					for (let i = 0; i < activeLayer.shapeModel.length; i++){
						let shape = activeLayer.shapeModel[i];
						shape.mousePositionChanged(mouse)
					}
				}

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

			width: graphicsView.drawingAreaWidth * canvas.scaleCoeff;
			height: graphicsView.drawingAreaHeight * canvas.scaleCoeff;
			property real topY: y;
			property real bottomY: y + height;
			property real leftX: x;
			property real rightX: x + width;

			color: "transparent";//Style.baseColor;
		}

		Canvas {
			id: canvas

			anchors.fill: parent;

			renderStrategy: graphicsView.renderStrategy

			antialiasing: graphicsView.canvasAntialiasing;

			property int selectedShapeCount: 0;
			property var editShape

			property real scaleCoeff: 1.0;
			property real scaleCoeffPrev: 1.0;
			property real deltaX: 0.0;
			property real deltaY: 0.0;

			property int selectedIndex: -1;

			onImageLoaded: {
				graphicsView.imageLoaded();
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
				duration: graphicsView.paintPauseDuration
				onFinished: {
					canvas.requestPaint();
				}
			}

			PauseAnimation {
				id: requestPaintPauseLong;
				duration: 100
				onFinished: {
					canvas.requestPaint();
				}
			}

			PauseAnimation {
				id: requestPaintPauseVeryLong;
				duration: 500
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

				let wasLimitCorrection = false//deltaCorrection(canvas.deltaX - (newX - scaleX), canvas.deltaY - (newY - scaleY), newScale);

				if(!wasLimitCorrection){
					canvas.deltaX -= (newX - scaleX)
					canvas.deltaY -= (newY - scaleY)
				}

				canvas.scaleCoeff = newScale
				canvas.scaleCoeffPrev = newScale;

				canvas.setViewMatrixParams();
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

			function setViewMatrixParams(){
				canvasMatrix.setXScale(canvas.scaleCoeff);
				canvasMatrix.setYScale(canvas.scaleCoeff);
				canvasMatrix.setXTranslation(canvas.deltaX);
				canvasMatrix.setYTranslation(canvas.deltaY);
			}

			property var startPaintTime

			onPaint: {
				if(graphicsView.restrictDrawing){
					return;
				}
				startPaintTime = new Date().valueOf()
				//console.log("Canvas::onPaint")

				var ctx = canvas.getContext('2d');
				ctx.reset()
				ctx.clearRect(0, 0, canvas.width, canvas.height)

				canvas.setViewMatrixParams();

				for(let i = 0; i < graphicsView.layerModel.length; i++){
					let layer = graphicsView.layerModel[i];
					if(layer.canApplyViewTransform){
						layer.draw(ctx, canvasMatrix)
					}
					else {
						layer.draw(ctx, canvasMatrixDefault)
					}
				}

				graphicsView.painted()

			}//onPaint

			onPainted: {
				let endPaintTime = new Date().valueOf()
				//console.log("PAINT TIME:: ", endPaintTime - startPaintTime)
				graphicsView.paintTime(endPaintTime - startPaintTime)
				//graphicsView.painted()
			}

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

	// Shortcut {
	// 	sequence: "F2";
	// 	enabled: true;
	// 	onActivated: {
	// 		//console.log("F2");
	// 		graphicsView.renameSignal(canvas.selectedIndex);
	// 	}
	// }

	Shortcut {
		sequence: "Ctrl+0";
		enabled: true;
		onActivated: {
			//console.log("Ctrl + 0");
			graphicsView.resetView(true);
		}
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

		property bool isOpen: graphicsView.leftMenuIsOpen;
		property bool canRunAnim: !graphicsView.leftMenuIsOpen;
		onIsOpenChanged: {
			if(isOpen){
				if(canRunAnim){
					leftMenuWidthAnim.from = leftMenuButtonPanel.width;
					leftMenuWidthAnim.to = leftMenuButtonRow.width + 2*leftMenuButtonRow.anchors.leftMargin;
					leftMenuWidthAnim.start()
				}
				canRunAnim = true
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

			onWidthChanged: {
				if(graphicsView.leftMenuIsOpen){
					leftMenuButtonPanel.width = leftMenuButtonRow.width + 2*leftMenuButtonRow.anchors.leftMargin;
				}

			}

			Button{
				id: selectModeButton;

				anchors.verticalCenter: parent.verticalCenter;

				width: Style.buttonWidthM;
				height: width;

				property bool isActive: graphicsView.isSelectionMode;

				iconSource: "../../../" + Style.getIconPath("Icons/Pointer", Icon.State.On, isActive ? Icon.Mode.Normal : Icon.Mode.Disabled)

				tooltipText: qsTr("Selection mode")

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

				tooltipText: qsTr("Edit mode")
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
				tooltipText: qsTr("Edit points mode")
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
				tooltipText: qsTr("Add points mode")
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
				tooltipText: qsTr("Delete points mode")
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

		property bool isOpen: graphicsView.rightMenuIsOpen;
		property bool canRunAnim: !graphicsView.rightMenuIsOpen;

		onIsOpenChanged: {
			if(isOpen){
				if(canRunAnim){
					rightMenuWidthAnim.from = rightMenuButtonPanel.height;
					rightMenuWidthAnim.to = rightMenuButtonColumn.height + 2*rightMenuButtonColumn.anchors.topMargin;
					rightMenuWidthAnim.start()
				}
				canRunAnim = true
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

			onHeightChanged: {
				if(graphicsView.rightMenuIsOpen){
					rightMenuButtonPanel.height = rightMenuButtonColumn.height + 2*rightMenuButtonColumn.anchors.topMargin
				}
			}

			Button{
				id: zoomIncreaseButton;

				width: parent.width;
				height: width;

				iconSource: "../../../" + Style.getIconPath("Icons/ZoomIn", Icon.State.On, Icon.Mode.Normal)

				tooltipText: qsTr("Zoom in")
				visible: graphicsView.hasZoomInButton

				onClicked: {
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
				tooltipText: qsTr("Zoom out")
				visible: graphicsView.hasZoomOutButton

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
				id: resetViewButton;

				width: parent.width;
				height: width;

				//iconSource: "../../../" + Style.getIconPath("Icons/ZoomReset", Icon.State.On, Icon.Mode.Normal)
				iconSource: "../../../" + Style.getIconPath("Icons/FitToScreen", Icon.State.On, Icon.Mode.Normal)
				tooltipText: qsTr("Reset view")
				visible: graphicsView.hasResetViewButton

				onClicked: {
					graphicsView.resetView(true);
				}
			}

			Button{
				id: fitToScreenButton;

				width: parent.width;
				height: width;

				iconSource: "../../../" + Style.getIconPath("Icons/ZoomToFit", Icon.State.On, Icon.Mode.Normal)
				tooltipText: qsTr("Fit to view")
				visible: graphicsView.hasFitToViewButton

				onClicked: {
					graphicsView.fitToActiveLayer()
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
