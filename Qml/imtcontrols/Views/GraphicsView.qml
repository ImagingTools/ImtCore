import QtQuick 2.12
import Acf 1.0

import imtcontrols 1.0


Rectangle {
	id: canvasPage;

	clip: true;

	color: Style.imagingToolsGradient0;

	property BaseModel objectsModel: BaseModel {}

	// property TreeItemModel objectsModel: TreeItemModel{};
	property alias selectedIndex: canvas.selectedIndex;

	//for scrollBars
	property real contentHeight: backgroundRec.height;
	property real contentY: -canvas.deltaY;
	property real originY: 0;

	property real contentWidth: backgroundRec.width;
	property real contentX: -canvas.deltaX;
	property real originX: 0;

	property real scaleStep: 0.1;

	property bool autoFit: true;

	signal copySignal(int index);
	signal pasteSignal(int index);
	signal deleteSignal(int index);
	signal renameSignal(int index);
	signal revertSignal();
	signal modelDataChanged();

	Component.onCompleted: {
		Events.subscribeEvent("DesignSchemeChanged", designSchemeChanged);
		Events.subscribeEvent("AppSizeChanged", appSizeChanged)

		let layer = layerComp.createObject(this);
		layer.layerId = "selection";
		layerModel.push(layer);
	}

	Component.onDestruction: {
		Events.unSubscribeEvent("DesignSchemeChanged", designSchemeChanged);
		Events.unSubscribeEvent("AppSizeChanged", appSizeChanged)
	}

	onContentXChanged: {
		canvas.deltaX = -contentX
	}

	onContentYChanged: {
		canvas.deltaY = -canvasPage.contentY
	}

	onObjectsModelChanged: {
		canvas.requestPaint();
	}

	onAutoFitChanged: {
		if (autoFit){
			appSizeChanged();
		}
	}

	property var layerModel: []
	property var selectedIndexes: []

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

	Component{
		id: layerComp;
		GraphicsLayer{

		}
	}

	//


	function designSchemeChanged(scheme){
		canvas.requestPaint();
	}

	function appSizeChanged(params){
		if (autoFit){
			zoomToFit();

			requestPaint()
		}
	}

	function setAutoFit(autoFit){
		canvasPage.autoFit = autoFit;
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

	function zoomToFit(){
		if (canvas.backgroundWidth == 0 | canvas.backgroundHeight == 0){
			return
		}

		let scaleCoeff_ = (canvas.width - Style.sizeMainMargin * 2) / canvas.backgroundWidth;
		let scaleCoeff_h = (canvas.height - Style.sizeMainMargin * 2) / canvas.backgroundHeight;
		if (scaleCoeff_ > scaleCoeff_h){
			scaleCoeff_ = scaleCoeff_h
		}
		canvas.setScale(scaleCoeff_, canvas.width / 2, canvas.height / 2)

		canvas.deltaX = canvasPage.width / 2 - backgroundRec.width / 2
		canvas.deltaY = canvasPage.height / 2 - backgroundRec.height / 2
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
		for(let i = 0; i < canvasPage.objectsModel.count; i++){
			let item = canvasPage.objectsModel.get(i).item;

			item.m_x = item.m_x + addX;
			item.m_y = item.m_y + addY;

			// canvasPage.objectsModel.set(i, item);
		}
	}

	function correctPositionScaled(addX, addY, scale_){
		for(let i = 0; i < objectsModel.count; i++){
			let item = objectsModel.get(i).item;

			let newWidth = scale_ * item.m_width;
			item.m_width = newWidth;

			// canvasPage.objectsModel.set(i, item);
		}
	}

	function findObject(mouseX, mouseY){
		//console.log("findObject", mouseX, mouseY)

		for(let i = 0; i < canvasPage.layerModel.length - 1; i++){
			let layer = canvasPage.layerModel[i];
			let shapeModel = layer.shapeModel;
			for(let j = 0; j < shapeModel.length; j++){
				let shape = shapeModel[j];
				let params = shape.getParams(layer.layerId, j);
				let point = params.point;
				let width_ = params.width * canvas.scaleCoeff;
				let height_ = params.height * canvas.scaleCoeff;
				let x_ = point.x * canvas.scaleCoeff + canvas.deltaX;
				let y_ = point.y * canvas.scaleCoeff + canvas.deltaY;
				let isInside =
					mouseX > x_
					&& mouseX < x_ + width_
					&& mouseY > y_
					&& mouseY < y_ + height_
				if(isInside){
					canvasPage.selectedIndexes.push(j)
					console.log("COLOR::", params.color, j)
					return shape;
				}
			}

		}

		return null
	}


	function clearSelection(){
		layerModel.pop();
		let layer = layerComp.createObject(this);
		layer.layerId = "selection";
		layerModel.push(layer);
		selectedIndexes = []
	}


	function moveShape(shape, deltaX, deltaY){
		// let point = shape.params[0].point
		// point.x += deltaX
		// point.y += deltaY
		// shape.params[0].point = point;
		// canvas.requestPaint();
	}

	function containsShape(layer, shape){
		for(let i = 0; i < layer.shapeModel.lenght; i++){

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
			cursorShape: Qt.OpenHandCursor;

			onClicked: {
				let shape = canvasPage.findObject(mouse.x, mouse.y)
				let selectionLayer = canvasPage.getLayer("selection");
				if(shape !== null){
					selectionLayer.addShape(shape);

				}
				else {
					canvasPage.clearSelection()
					//selectionLayer.clear();
				}
				canvas.requestPaint();

				// if(!wasMoving){
				// 	if(canvas.selectedIndex >= 0){
				// 		for(let i = 0; i < canvasPage.objectsModel.count; i++){
				// 			let item = canvasPage.objectsModel.get(i).item;
				// 			item.m_selected = (i == canvas.selectedIndex);
				// 		}
				// 	}
				// 	else {
				// 		for(let i = 0; i < canvasPage.objectsModel.count; i++){
				// 			let item = canvasPage.objectsModel.get(i).item;
				// 			item.m_selected = false;
				// 		}
				// 	}
				// 	canvas.requestPaint();
				// }
			}

			onPressed: {
				canvas.selectedIndex = -1;
				for(let i = 0; i < canvasPage.objectsModel.count; i++){
					let item = canvasPage.objectsModel.get(i).item;
					let x_ = item.m_x;
					let y_ = item.m_y;

					let width_ = item.m_width ? item.m_width : canvas.mainRec_width;
					let height_ = canvas.mainRec_height;

					x_ = x_ * canvas.scaleCoeff + canvas.deltaX;
					y_ = y_ * canvas.scaleCoeff + canvas.deltaY;
					width_ = width_ * canvas.scaleCoeff;
					height_ = height_  * canvas.scaleCoeff;

					let ok = checkInsideMovingItem(x_, y_, width_, height_);

					if(ok){
						canvas.selectedIndex = i;
					}
				}
				if(canvas.selectedIndex == -1){
					controlArea.cursorShape = Qt.ClosedHandCursor;
				}
			}

			onReleased: {
				if(canvas.selectedIndex >= 0){
					controlArea.cursorShape = Qt.ArrowCursor;
				}
				else {
					controlArea.cursorShape = Qt.OpenHandCursor
				}
			}

			onDoubleClicked: {
				wasMoving = false;
				isPressed = false;

				if(canvas.selectedIndex >= 0){
					canvasPage.goInside();
				}
			}

			onDeltaSignal: {
				//let shape = findObject(mouseX, mouseY)
				//canvasPage.moveShape(shape, delta.x, delta.y)

				if(canvas.selectedIndex < 0){
					canvasPage.autoFit = false;

					canvas.deltaX += delta.x
					canvas.deltaY += delta.y
					canvas.requestPaint();
				}
				else {
					movingFunction(delta);
				}
			}

			onWheel: {
				if (canvas.scaleCoeff == 0){
					return
				}

				canvasPage.autoFit = false;

				let deltaX = (wheel.x + canvas.deltaX) / canvas.scaleCoeff
				let wheelDelta = wheel.angleDelta.y
				let scaleCoeff_ = canvas.scaleCoeff;
				if(wheelDelta > 0){//up
					if (canvas.scaleCoeff < 3){
						scaleCoeff_ += scaleStep;
					}
				}
				else{//down
					if(canvas.scaleCoeff > scaleStep * 2){
						scaleCoeff_ -= scaleStep;
					}
				}

				canvas.setScale(scaleCoeff_, wheel.x, wheel.y)
			}


			function movingFunction(delta){//reimplemented
				if(canvas.selectedIndex >= 0 && canvas.scaleCoeff != 0){
					let item = canvasPage.objectsModel.get(canvas.selectedIndex).item;
					if (!item){
						return;
					}

					let x_ = item.m_x;
					let y_ = item.m_y;
					let width_ = item.m_width ? item.m_width : canvas.mainRec_width;
					let height_ = canvas.mainRec_height;

					x_ = x_ * canvas.scaleCoeff + canvas.deltaX;
					y_ = y_ * canvas.scaleCoeff + canvas.deltaY;
					width_ = width_ * canvas.scaleCoeff;
					height_ = height_  * canvas.scaleCoeff;

					let withinBorders_ = true;
					if(withinBorders_){
						if (canvas.scaleCoeff < 0.0000001){
							return;
						}

						let newX = (item.m_x + delta.x / canvas.scaleCoeff);
						let newY = (item.m_y + delta.y / canvas.scaleCoeff);

						//fit to borders
						let margin_ = 10;
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

						item.m_x = newX;
						item.m_y = newY;

						// canvasPage.objectsModel.set(canvas.selectedIndex, item)

						canvas.requestPaint();

						canvasPage.modelDataChanged()

					}
				}

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
				canvas.hoverIndex = -1;
				for(let i = 0; i < canvasPage.objectsModel.count; i++){
					let item = canvasPage.objectsModel.get(i).item;

					let x_  = item.m_x;
					let y_  = item.m_y;
					let width_ = item.m_width ? item.m_width  : canvas.mainRec_width;
					let height_ = canvas.mainRec_height ;

					x_ = x_ * canvas.scaleCoeff + canvas.deltaX;
					y_ = y_ * canvas.scaleCoeff + canvas.deltaY;
					width_ = width_ * canvas.scaleCoeff;
					height_ = height_  * canvas.scaleCoeff;

					let ok = controlArea.checkHoverItem(x_, y_, width_, height_, position);

					if(ok){
						canvas.hoverIndex = i;

					}
				}
				if(canvas.hoverIndex >=0){
					controlArea.cursorShape = Qt.ArrowCursor;
					canvas.linkSelected = true;
					canvas.requestPaint();
				}
				else {
					controlArea.cursorShape = Qt.OpenHandCursor;
					if(canvas.linkSelected){
						canvas.linkSelected = false;
						canvas.requestPaint();

					}
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

			color: Style.baseColor;
		}

		Canvas {
			id: canvas

			anchors.fill: parent;

			antialiasing: true;

			property real scaleCoeff: 1.0;
			property real scaleCoeffPrev: 1.0;
			property real deltaX: 0.0;
			property real deltaY: 0.0;

			property int selectedIndex: -1;
			property int hoverIndex: -1;
			property bool linkSelected: false;
			property bool hasTailSelection: false;

			//sizes
			property real mainRec_width: 250
			property real mainRec_height: 60

			property int fontSize: 20
			property int fontSizeSmall: 14
			property int radius_: 2
			property int borderShift: 4
			property int shadowSize: 6

			property int backgroundStep: 30
			property int backgroundWidth: 1024
			property int backgroundHeight: 768

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
				requestPaint()
			}

			onHeightChanged: {
				requestPaint()
			}

			onImageLoaded: {
				requestPaint()
			}

			onScaleCoeffChanged: {
				requestPaint()
			}

			onDeltaXChanged: {
				canvasPage.contentX = -deltaX
			}

			onDeltaYChanged: {
				canvasPage.contentY = -canvas.deltaY
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
				if (canvasPage.autoFit){
					canvasPage.zoomToFit();
				}

				var ctx = canvas.getContext('2d');
				ctx.reset()
				ctx.clearRect(0, 0, canvas.width, canvas.height)

				let transtateX = canvas.deltaX
				let transtateY = canvas.deltaY
				ctx.translate(transtateX, transtateY)
				ctx.scale(canvas.scaleCoeff, canvas.scaleCoeff)

				drawBackground(ctx);

				for(let i = 0; i < canvasPage.layerModel.length; i++){
					let layer = canvasPage.layerModel[i];
					if(layer.layerId !== "selection"){
						layer.draw(ctx);
					}
					else {
						layer.drawSelection(ctx, canvasPage.selectedIndexes);
						layer.drawBorderPoints(ctx, canvasPage.selectedIndexes);
					}
				}

				//width calculation
				// for(let i = 0; i < canvasPage.objectsModel.count; i++){
				// 	setObjectWidth(ctx, i);
				// }

				// //drawLink
				// for(let i = 0; i < canvasPage.objectsModel.count; i++){
				// 	let item = canvasPage.objectsModel.get(i).item;

				// 	let links = item.m_links;
				// 	for(let k = 0; k < links.count; k++){
				// 		let objectId = links.get(k).item.m_id;
				// 		let ind = canvasPage.findModelIndex(objectId);
				// 		if(ind === -1) continue
				// 		canvas.drawLink(ctx, ind, i);
				// 	}
				// }


				// //drawObject
				// for(let i = 0; i < canvasPage.objectsModel.count; i++){
				// 	console.log("Canvas::drawObject")
				// 	if(i !== canvas.selectedIndex){
				// 		drawObject(ctx, i);
				// 	}
				// }
				// if(canvas.selectedIndex >= 0){
				// 	drawObject(ctx, canvas.selectedIndex);
				// }
			}//onPaint


			function drawBackground(ctx){
				let step = canvas.backgroundStep;

				ctx.lineCap = "round"
				ctx.lineJoin = "round"
				ctx.lineWidth = 1;

				ctx.fillStyle = canvas.gridColor;
				ctx.strokeStyle = canvas.gridColor;


				for(let i = 0; i * step < canvas.backgroundWidth; i++){//vertical lines
					let x1 = i * step;
					let y1 =  0 ;
					let x2 = i * step;
					let y2 = canvas.backgroundHeight ;

					ctx.beginPath()
					ctx.moveTo(x1, y1);
					ctx.lineTo(x2, y2);
					ctx.stroke();
				}

				for(let i = 0; i * step < canvas.backgroundHeight; i++){//horizontal lines
					let x1 = 0 ;
					let y1 =  i * step ;
					let x2 =  canvas.backgroundWidth;
					let y2 =  i * step;

					ctx.beginPath()
					ctx.moveTo(x1, y1);
					ctx.lineTo(x2, y2);
					ctx.stroke();
				}

				ctx.strokeStyle = canvas.backgroundBorderColor;
				ctx.lineWidth = 2;
				ctx.beginPath()
				ctx.moveTo(1, 1);
				ctx.lineTo(canvas.backgroundWidth, 1);
				ctx.lineTo(canvas.backgroundWidth, canvas.backgroundHeight - 1);
				ctx.lineTo(1, canvas.backgroundHeight - 1);
				ctx.lineTo(1, 1);
				ctx.stroke();
			}

			function setObjectWidth(ctx, index){
				//width calculation
				let item = canvasPage.objectsModel.get(index).item;

				let mainText  = item.m_mainText
				let secondText  = item.m_secondText
				let width_ = item.m_width ? item.m_width : canvas.mainRec_width;

				ctx.lineWidth = 1;
				let fontStr_main = String(canvas.fontSize) + "px sans-serif"
				ctx.font = fontStr_main; //"20px sans-serif";
				let textStr_main = mainText
				let textWidth_main = ctx.measureText(mainText).width

				ctx.lineWidth = 0.5;
				let fontStr_second = String(canvas.fontSizeSmall) + "px sans-serif"
				ctx.font = fontStr_second;
				let textStr_second = secondText;
				let textWidth_second = ctx.measureText(secondText).width

				let add = 2 * canvas.imageSize + 2 * canvas.imageMargin + 2 * canvas.borderShift + 30 /** scaleCoeff*/;
				let mainRecWidth = Math.max(textWidth_main + add, textWidth_second + add, width_)
				// canvasPage.objectsModel.setData("width", mainRecWidth /*/ scaleCoeff*/, index);

				item.m_width = mainRecWidth;

				return mainRecWidth;
			}

			function drawObject(ctx, index){
				console.log("drawObject::::")
				let item = canvasPage.objectsModel.get(index).item;

				let x_  = item.m_x
				let y_  = item.m_y
				let width_ = item.m_width ? item.m_width : canvas.mainRec_width;
				let mainText  =  item.m_mainText
				let secondText  = item.m_secondText
				let thirdText  = item.m_thirdText
				let backgroundColor = item.m_backgroundColor ? item.m_backgroundColor : canvas.backgroundColor;
				let icon = item.m_icon1
				let iconUrl_1  = icon === "" ? "" : "../../../" + Style.getIconPath(icon, Icon.State.On, Icon.Mode.Normal)
				icon = item.m_icon2
				let iconUrl_2  = icon === "" ? "" : "../../../" + Style.getIconPath(icon, Icon.State.On, Icon.Mode.Normal)
				// for future multiselect
				// let selected = item.m_selected ? item.m_selected : false;
				let selected = index === canvasPage.selectedIndex

				let hasError = item.m_hasError
				let isComposite = item.m_isComposite

				isComposite = false

				ctx.lineCap = "round"
				ctx.lineJoin = "round"

				//width calculation
				let mainRecWidth = setObjectWidth(ctx, index);

				//shadow rectangle
				let shadowSize = canvas.shadowSize;
				ctx.lineWidth = 2 ;
				// ctx.fillStyle = selected ? Qt.rgba(0.2, 0.8, 0, 0.3) : Qt.rgba(0, 0, 0, 0.2);
				ctx.fillStyle = Qt.rgba(0, 0, 0, 0.2);
				ctx.beginPath()
				ctx.roundedRect(x_ + shadowSize, y_ + shadowSize, mainRecWidth, canvas.mainRec_height , canvas.radius_, canvas.radius_);
				ctx.fill();

				//main rectangle
				ctx.lineWidth = 2 ;
				ctx.strokeStyle = hasError ? canvas.errorColor : canvas.mainColor;
				ctx.fillStyle = (!selected && !isComposite) ? backgroundColor :
															  (!selected && isComposite) ? canvas.compositeColor:
																						   (selected && isComposite) ? canvas.compositeSelectedColor: (selected && !isComposite) ? canvas.selectedColor : backgroundColor;
				ctx.beginPath()
				ctx.roundedRect(x_, y_, mainRecWidth, canvas.mainRec_height , canvas.radius_, canvas.radius_);
				ctx.fill();
				ctx.stroke();

				//inner rectangle
				ctx.strokeStyle = canvas.innerFrameColor;

				ctx.beginPath()
				ctx.roundedRect(x_ + canvas.borderShift, y_ + canvas.borderShift, mainRecWidth - 2 * canvas.borderShift, canvas.mainRec_height  - 2 * canvas.borderShift, canvas.radius_, canvas.radius_);
				ctx.stroke();

				//Main text
				ctx.strokeStyle = canvas.mainTextColor;
				ctx.fillStyle = canvas.mainTextColor;
				ctx.lineWidth = 1;
				let fontStr = "bold " + String(canvas.fontSize) + "px sans-serif"
				ctx.font = fontStr; //"20px sans-serif";
				let textStr = mainText
				let textWidth = ctx.measureText(mainText).width

				let text_x = x_ + canvas.borderShift + canvas.textMargin;
				let text_y = y_ + canvas.borderShift + canvas.textVerticalOffset;

				ctx.beginPath()
				ctx.fillText(textStr, text_x, text_y);
				//                ctx.strokeText(textStr, text_x, text_y);

				//Second text
				ctx.strokeStyle = canvas.secondTextColor;
				ctx.fillStyle = canvas.secondTextColor;
				ctx.lineWidth = 0.5;
				let fontStr2 = String(canvas.fontSizeSmall) + "px sans-serif"
				ctx.font = fontStr2;
				let textStr2 = secondText;
				let textWidth2 = ctx.measureText(secondText).width

				let text_x2 = x_ + canvas.borderShift + canvas.textMargin;
				let text_y2 = y_ + canvas.mainRec_height - canvas.borderShift - canvas.textMargin;

				ctx.beginPath()
				ctx.fillText(textStr2, text_x2, text_y2);
				ctx.strokeText(textStr2, text_x2, text_y2);

				//Third text
				ctx.strokeStyle = canvas.secondTextColor;
				ctx.fillStyle = canvas.secondTextColor;
				ctx.lineWidth = 0.5;
				ctx.font = fontStr2;
				let textStr3 = thirdText;
				let textWidth3 = ctx.measureText(thirdText).width

				let text_x3 = x_ + mainRecWidth - textWidth3 - canvas.borderShift - canvas.textMargin ;
				let text_y3 = y_ + canvas.mainRec_height - canvas.borderShift - canvas.textMargin;

				ctx.beginPath()
				ctx.fillText(textStr3, text_x3, text_y3);
				ctx.strokeText(textStr3, text_x3, text_y3);

				//images
				let image1_x = x_ + mainRecWidth - canvas.borderShift - canvas.imageSize - canvas.imageMargin;
				let image1_y = text_y - canvas.imageSize + canvas.imageMargin;
				let image2_x = image1_x - canvas.imageSize - canvas.imageMargin;
				let image2_y = text_y - canvas.imageSize + canvas.imageMargin;
				ctx.beginPath()
				if (iconUrl_1 !== ""){
					ctx.drawImage(iconUrl_1, image1_x, image1_y, canvas.imageSize, canvas.imageSize);
				}
				if (iconUrl_2 !== ""){
					ctx.drawImage(iconUrl_2, image2_x, image2_y, canvas.imageSize, canvas.imageSize);
				}

			}

			function drawLink(ctx, fromIndex, toIndex){
				let fromItem = canvasPage.objectsModel.get(fromIndex).item;
				let toItem = canvasPage.objectsModel.get(toIndex).item;

				let x1 = fromItem.m_x;
				let y1 = fromItem.m_y;
				let x2 = toItem.m_x;
				let y2 = toItem.m_y;

				let width1 = fromItem.m_width ? fromItem.m_width : canvas.mainRec_width;
				let width2 = toItem.m_width ? toItem.m_width : canvas.mainRec_width;

				let x1_link = x1 + width1/2;
				let y1_link = y1 + canvas.mainRec_height/2;
				let x2_link = x2 + width2/2;
				let y2_link = y2 + canvas.mainRec_height/2;

				ctx.lineCap = "round"
				ctx.lineJoin = "round"
				ctx.lineWidth = 2;

				ctx.strokeStyle = canvas.hoverIndex === fromIndex ? canvas.selectedLinkFromColor : canvas.hoverIndex === toIndex ? canvas.selectedLinkToColor : canvas.linkColor;
				ctx.fillStyle = ctx.strokeStyle

				ctx.beginPath()
				ctx.moveTo(x1_link, y1_link);
				ctx.lineTo(x2_link, y2_link);
				ctx.stroke();

				//draw intersection
				let x1_rec2;
				let y1_rec2;
				let x2_rec2;
				let y2_rec2;

				let intersection;

				if(y1 > y2){
					//bottom line
					x1_rec2 = x2;
					y1_rec2 = y2 + canvas.mainRec_height;
					x2_rec2 = x2 + width2;
					y2_rec2 = y2 + canvas.mainRec_height;

					intersection = findIntersection(x1_link, y1_link, x2_link, y2_link, x1_rec2, y1_rec2, x2_rec2, y2_rec2);

					if(intersection.x < x2 + width2 && intersection.x > x2){
						//bottom line
						x1_rec2 = x2;
						y1_rec2 = y2 + canvas.mainRec_height;
						x2_rec2 = x2 + width2;
						y2_rec2 = y2 + canvas.mainRec_height;
					}
					else if(intersection.x >= x2 + width2){
						//right line
						x1_rec2 = x2  + width2;
						y1_rec2 = y2 + canvas.mainRec_height;
						x2_rec2 = x2 + width2;
						y2_rec2 = y2;
					}
					else if(intersection.x <= x2){
						//left line
						x1_rec2 = x2;
						y1_rec2 = y2 + canvas.mainRec_height;
						x2_rec2 = x2;
						y2_rec2 = y2;
					}
				}//y1 > y2

				else if (y1 < y2){
					//top line
					x1_rec2 = x2;
					y1_rec2 = y2;
					x2_rec2 = x2 + width2;
					y2_rec2 = y2;

					intersection = findIntersection(x1_link, y1_link, x2_link, y2_link, x1_rec2, y1_rec2, x2_rec2, y2_rec2);

					if(intersection.x < x2 + width2 && intersection.x > x2){
						//top line
						x1_rec2 = x2;
						y1_rec2 = y2;
						x2_rec2 = x2 + width2;
						y2_rec2 = y2;
					}
					else if(intersection.x >= x2 + width2){
						//right line
						x1_rec2 = x2  + width2;
						y1_rec2 = y2 + canvas.mainRec_height;
						x2_rec2 = x2 + width2;
						y2_rec2 = y2;
					}
					else if(intersection.x <= x2){
						//left line
						x1_rec2 = x2;
						y1_rec2 = y2 + canvas.mainRec_height;
						x2_rec2 = x2;
						y2_rec2 = y2;
					}

				}//y1 < y2

				else if(y1 == y2){
					if(x1 >= x2){
						//right line
						x1_rec2 = x2  + width2;
						y1_rec2 = y2 + canvas.mainRec_height;
						x2_rec2 = x2 + width2;
						y2_rec2 = y2;


					}
					else {
						//left line
						x1_rec2 = x2;
						y1_rec2 = y2 + canvas.mainRec_height;
						x2_rec2 = x2;
						y2_rec2 = y2;

					}
				}//y1 == y2

				intersection = findIntersection(x1_link, y1_link, x2_link, y2_link, x1_rec2, y1_rec2, x2_rec2, y2_rec2);

				//for intersection margin
				let angle
				let offset

				let hasMargin = true;
				let complexIntersection = true;
				if(hasMargin){
					angle = findAngle(x1_link, y1_link,intersection.x, intersection.y)
					//console.log("angle: ", angle)

					let quarter =  Math.trunc(angle/(Math.PI/2))
					let add = 0;

					//offset length correction
					if(quarter % 2){
						add = angle  - quarter * Math.PI/2  < Math.PI/4 ? Math.abs(Math.sin(angle)) : Math.abs(Math.cos(angle))
					}
					else {
						add = angle  - quarter * Math.PI/2  < Math.PI/4 ? Math.abs(Math.cos(angle)) : Math.abs(Math.sin(angle))
					}

					let smallAngle = Math.abs(angle  - (quarter + 0) * Math.PI/2) < (Math.PI/180) * 30
						|| Math.abs(angle  - (quarter + 1) * Math.PI/2) < (Math.PI/180) * 30;
					if(smallAngle){
						//add = 0.5;
					}
					//offset length correction

					offset = 20 * (1 + add);

					let newX = intersection.x + offset * Math.cos(angle)
					let newY = intersection.y + offset * Math.sin(angle)

					intersection = Qt.point(newX, newY);

				}//for intersection margin

				if(complexIntersection && hasMargin){
					drawIntersectionExt(ctx, intersection,angle, false);
					drawIntersectionArc(ctx, intersection, angle + Math.PI, offset, false)

				}
				else {//simple
					drawIntersection(ctx, intersection, false);
				}
			}

			function drawIntersection(ctx, intersection, selected){
				let size = canvas.intersectionSize;

				ctx.lineWidth = 0.5;
				ctx.beginPath()
				ctx.roundedRect(intersection.x - size/2, intersection.y  - size/2, size, size, size, size);
				ctx.fill();
				ctx.stroke();
			}

			function drawIntersectionExt(ctx, intersection, angle, selected){
				let size = canvas.intersectionSize;
				let sizeSmall = size/2;

				ctx.beginPath()
				ctx.roundedRect(intersection.x - sizeSmall/2, intersection.y  - sizeSmall/2, sizeSmall, sizeSmall, sizeSmall, sizeSmall);
				ctx.fill();
				ctx.stroke();

				//draw tail

				ctx.beginPath();
				ctx.moveTo(intersection.x, intersection.y)
				ctx.lineWidth = 0.5;
				let offset = size/2;
				let newX = intersection.x + offset * Math.cos(angle)
				let newY = intersection.y + offset * Math.sin(angle)
				ctx.lineTo(newX, newY);
				ctx.stroke();


			}

			function drawIntersectionArc(ctx, point, angle, offset, selected){
				let rad = canvas.arcRadius;
				let endAngle = 0.6 * Math.PI;
				ctx.lineWidth = 2;
				ctx.beginPath()
				ctx.arc(point.x, point.y, rad, angle , angle - endAngle, true);
				ctx.stroke();
				ctx.beginPath()
				ctx.arc(point.x, point.y, rad, angle , angle + endAngle, false);
				ctx.stroke();

				ctx.beginPath();//draw tail
				let startX = point.x + rad * Math.cos(angle);
				let startY = point.y  + rad * Math.sin(angle);
				ctx.moveTo(startX, startY);
				ctx.lineWidth = 0.6;
				let finishX = startX + offset * Math.cos(angle)
				let finishY = startY + offset * Math.sin(angle)
				ctx.lineTo(finishX, finishY);
				ctx.stroke();

			}

			function findIntersection(x1, y1, x2, y2, x3, y3, x4, y4){

				let px = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) /
					((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4));

				let py = ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)) /
					((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4));


				return Qt.point(px, py);
			}

			function findAngle(x1, y1, x2, y2){
				if (x2 - x1 == 0){
					return 0
				}

				let angle;
				angle = Math.atan((y2 - y1) / (x2 - x1))

				if((x2 == x1) && (y1 == y2)){
					angle = 0;
				}
				else {
					let angleBase = Math.atan((y2 - y1) / (x2 - x1))

					if((y1 == y2) && (x1 > x2)){
						angle = 0
					}
					else if((y1 == y2) && (x1 < x2)){
						angle = Math.PI;
					}
					else if((y1 < y2) && (x1 == x2)){
						angle = 1.5 * Math.PI;
					}
					else if((y1 > y2) && (x1 == x2)){
						angle = Math.PI/2;
					}
					//
					else if((y1 > y2) && (x1 < x2)){//boottom left
						angle = angleBase + Math.PI;
					}
					else if((y1 < y2) && (x1 < x2)){//top left
						angle = angleBase + Math.PI;
					}
					else if((y1 < y2) && (x1 > x2)){//top right
						angle = angleBase + 2 * Math.PI;
					}
				}
				return angle;
			}
		}//canvas
	}//mainContainer


	Shortcut {
		sequence: "Ctrl+C";
		enabled: true;
		onActivated: {
			console.log("Ctrl+C");
			canvasPage.copySignal(canvas.selectedIndex);
		}
	}

	Shortcut {
		id: pasteShortcut;

		sequence: "Ctrl+V";
		enabled: true;
		onActivated: {
			console.log("Ctrl+V");
			canvasPage.pasteSignal(canvas.selectedIndex);
		}
	}


	Shortcut {
		sequence: "Delete";
		enabled: true;
		onActivated: {
			console.log("Delete");
			canvasPage.deleteSignal(canvas.selectedIndex);
		}
	}

	Shortcut {
		sequence: "Ctrl+Z";
		enabled: true;
		onActivated: {
			console.log("Ctrl+Z");
			canvasPage.revertSignal();
		}
	}

	Shortcut {
		sequence: "F2";
		enabled: true;
		onActivated: {
			console.log("F2");
			canvasPage.renameSignal(canvas.selectedIndex);
		}
	}


	function copyObjectFunc(index){
		if(index >= 0){
			// bufferModel.clear();
			// bufferModel.copyItemDataFromModel(0, canvasPage.objectsModel, index);
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
			// let index = canvasPage.objectsModel.insertNewItem();
			// canvasPage.objectsModel.copyItemDataFromModel(index, bufferModel,0);

			canvas.selectedIndex = 100//index;

			canvas.requestPaint();
		}
	}

	function deleteObjectFunc(index){
		console.log("DELETE OBJECT: ", index);
		let id = canvasPage.objectsModel.get(index).item.m_id;

		//remove links
		for(let i = 0; i < canvasPage.objectsModel.count; i++){
			let item = canvasPage.objectsModel.get(i).item;
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
		canvasPage.objectsModel.removeItem(index);

		canvas.selectedIndex = -1;

		canvas.requestPaint();
	}


	CustomScrollbar{
		id: scrollVert;

		anchors.right: parent.right;
		anchors.rightMargin: 1;
		anchors.bottom: parent.bottom;

		secondSize: 12;
		visible: (backgroundRec.topY >=0 && backgroundRec.bottomY <= canvasPage.height) ? false : true;
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

		visible: (backgroundRec.leftX >=0 && backgroundRec.rightX <= canvasPage.width) ? false : true;
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

}
