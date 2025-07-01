import QtQuick 2.12

import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtguigql 1.0

Rectangle {
	id: testPage;

	anchors.fill: parent;
	clip: true;
	
	property string json: "{\"type\": \"data\",\"id\": \"70e4d6b5-7afe-4490-9b80-09636fc825e2\",\"payload\": {\"data\": {\"OnAccountsCollectionChanged\": {\"itemIds\":[\"dce1f9da-8a1b-40c7-b903-49076ab7e550\",\"0722c437-dbaf-4a09-ac64-411ca5bcb61c\"],\"operationContext\":{\"ownerId\":\"su\",\"ownerName\":\"superuser\"},\"typeOperation\":\"removed\"}}}}"
	
	TreeItemModel {
		id: treeItemModel
	}
	
	TreeItemModel {
		id: treeItemModel2
	}

	// Row {
	// 	height: 30;

	// 	Button {
	// 		width: 100;
	// 		height: 30;
	// 		text: "Test";
	// 		onClicked: {
	// 			// treeItemModel.createFromJson(testPage.json)
	// 			// console.log("treeItemModel", treeItemModel.toJson())
	// 			// treeItemModel2.copy(treeItemModel)
	// 			// console.log("treeItemModel2", treeItemModel2.toJson())


	// 			//console.log("requestPaint!!!!!!")
	// 			canvas.isReverse = !canvas.isReverse;
	// 			canvas.requestPaint()
	// 		}
	// 	}
	// }


	// Canvas{
	// 	id: canvas;
	// 	anchors.centerIn: parent;

	// 	width:  parent.width;
	// 	height:  parent.height;

	// 	property real imageX: (canvas.width/3 + canvas.width/8)
	// 	property real imageY: (canvas.height/3 + canvas.height/8)

	// 	Component.onCompleted: {
	// 		loadImage("/Images/testImage")
	// 	}

	// 	onImageLoaded: {
	// 		requestPaint()
	// 	}

	// 	onWidthChanged: {
	// 		requestPaint()
	// 	}

	// 	onHeightChanged: {
	// 		requestPaint()
	// 	}

	// 	property bool isReverse: false;

	// 	onPaint: {
	// 		//console.log("ON_PAINT")
	// 		var ctx = getContext('2d');

	// 		ctx.clearRect(0, 0, canvas.width, canvas.height);

	// 		canvas.imageX = (canvas.width/3 + canvas.width/8);
	// 		canvas.imageY = (canvas.height/3 + canvas.height/8);

	// 		if(!isReverse){
	// 			canvasMatrix.rotateContext(ctx, Math.PI/2, Qt.point(canvas.imageX, canvas.imageY))
	// 			//canvasMatrix.translateContext(ctx, 100, 100)
	// 		}
	// 		else {
	// 			let invertedMatrix = canvasMatrix.getInvertedMatrix(canvasMatrix.matrix)
	// 			canvasMatrix.matrix = canvasMatrix.multiplyByMatrix(canvasMatrix.matrix, invertedMatrix);
	// 			canvasMatrix.setContextTransform(ctx)
	// 		}

	// 		ctx.drawImage("/Images/testImage", canvas.width/3, canvas.height/3, canvas.width/4, canvas.height/4)

	// 	}

	// 	CanvasMatrix{
	// 		id: canvasMatrix;
	// 	}
	// }


	// Rectangle{
	// 	x:canvas.width/3
	// 	y: canvas.height/3;

	// 	width: canvas.width/4
	// 	height:  canvas.height/4

	// 	color: "transparent"
	// 	border.color: "green"
	// }

	// Rectangle{
	// 	anchors.fill: canvas;
	// 	color: "transparent"
	// 	border.color: "red"
	// }

	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////

	GraphicsView{
		id: view;

		anchors.left: parent.left;
		anchors.top: parent.top;
		anchors.leftMargin: 50;
		anchors.topMargin: 50;

		width: parent.width/2
		height: parent.height/2

		minZoomLevel: 1;
		restrictMove: true;

		Component.onCompleted: {
			//TEST

			let background = getBackgroundLayer()
			background.addShape(gridShape);

			let activeLayer = getActiveLayer()
			let lineObj = polylineComp.createObject(this);
			activeLayer.addShape(lineObj);
		}
	}

	Component{
		id: polylineComp

		PolyLineShape{
			id: line;

			color: "blue";

			function draw(ctx, layerId){
				drawBase(ctx, layerId)
				drawNodes(ctx, layerId)
			}

			function translateCoordToGrid(point){
				let newPoint = Qt.point(0,0);
				newPoint.x = point.x + gridShape.labelSize;
				newPoint.y = gridShape.viewItem.height - point.y - gridShape.labelSize
				return newPoint;
			}

			function getPoints(layerId){
				let pointList = []

				let point1 = translateCoordToGrid(Qt.point(40, 40))
				let point2 = translateCoordToGrid(Qt.point(120,80))
				let point3 = translateCoordToGrid(Qt.point(200,300))
				let point4 = translateCoordToGrid(Qt.point(400 ,400))

				pointList.push(point1)
				pointList.push(point2)
				pointList.push(point3)
				pointList.push(point4)

				return pointList
			}
		}
	}

	CoordinateGridShape{
		id: gridShape;

		viewItem: view;
	}

}
