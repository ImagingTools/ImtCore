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

	color: Style.baseColor
	
	property string json: "{\"type\": \"data\",\"id\": \"70e4d6b5-7afe-4490-9b80-09636fc825e2\",\"payload\": {\"data\": {\"OnAccountsCollectionChanged\": {\"itemIds\":[\"dce1f9da-8a1b-40c7-b903-49076ab7e550\",\"0722c437-dbaf-4a09-ac64-411ca5bcb61c\"],\"operationContext\":{\"ownerId\":\"su\",\"ownerName\":\"superuser\"},\"typeOperation\":\"removed\"}}}}"
	

	Column{
		visible: true;

		anchors.top: parent.top;
		anchors.right: parent.right;
		anchors.topMargin: Style.marginL
		anchors.rightMargin: Style.marginL

		width: 200;

		spacing: 20;

		BaseText{
			font.pixelSize: Style.fontSizeL

			text: "Zoom: " + view.scaleCoeff.toFixed(2)
		}
		// BaseText{
		// 	font.pixelSize: Style.fontSizeL

		// 	text: "DeltaX: " + view.deltaX
		// }
		// BaseText{
		// 	font.pixelSize: Style.fontSizeL

		// 	text: "DeltaY: " + view.deltaY
		// }

		// Button{
		// 	text: "test"
		// 	onClicked: {
		// 		// let shapeList  = []
		// 		// let activeLayer = view.getActiveLayer()
		// 		// shapeList.push(view.shape1) //blue
		// 		// shapeList.push(view.shape2) //green
		// 		// shapeList.push(view.shape3) //red

		// 		// view.fitToShapeList(shapeList)


		// 	}
		// }
	}

	GraphicsView{
		id: view;

		anchors.left: parent.left;
		anchors.top: parent.top;
		anchors.leftMargin: 50;
		anchors.topMargin: 50;

		width: 0.8 * parent.width
		height: 0.8 * parent.height

		hideScrollbars: true;
		hasRightButtonMenu: true;
		hasLeftButtonMenu: true;
		leftMenuCoordinates: Qt.point(gridShape.labelXWidth + Style.marginM, Style.marginS)
		//minZoomLevel: 1;
		//restrictMove: true;
		//translateXPositiveLimit: gridShape.labelXWidth
		//translateYNegativeLimit: gridShape.labelYHeight
		hasHoverReaction: true;

		property var shape1
		property var shape2
		property var shape3

		Component.onCompleted: {
			//TEST

			let background = getBackgroundLayer()
			background.addShape(gridShape);

			let inactiveLayer = getInactiveLayer()
			let activeLayer = getActiveLayer()

			//transformation for the layers
			inactiveLayer.layerMatrix.invertY();
			activeLayer.layerMatrix.invertY();
			setLayersParams()

			//adding shapes to layers
			let lineObjRed = polylineCompRed.createObject(this);
			inactiveLayer.addShape(lineObjRed);

			let lineObjBlue = polylineCompBlue.createObject(this);
			activeLayer.addShape(lineObjBlue);
			let lineObjGreen = polylineCompGreen.createObject(this);
			activeLayer.addShape(lineObjGreen);

			let polygonObj = polygon.createObject(this);
			activeLayer.addShape(polygonObj);



			//activeLayer.addShape(lineObjRed);

			// view.shape1 = lineObjBlue;

			// view.shape2 = lineObjGreen;

			// view.shape3 = lineObjRed;

			//view.fitToShape(lineObjRed)

			//PERFORMANCE TEST

			// let lineObj10000 = polylineComp10000.createObject(this);
			// activeLayer.addShape(lineObj10000);

			// let count = 1000
			// for(let i = 0; i < count; i++){
			// 	let testLineObj = testLine.createObject(this);
			// 	let pointList = []

			// 	for(let j = 0; j < 10; j++){
			// 		let point = Qt.point(j*70, i)
			// 		pointList.push(point)
			// 	}

			// 	testLineObj.points = pointList
			// 	activeLayer.addShape(testLineObj);
			// }

		}

		function setLayersParams(){
			let inactiveLayer = getInactiveLayer()
			let activeLayer = getActiveLayer()

			if(!view.width || !view.height || !activeLayer || !inactiveLayer){
				return
			}

			gridShape.axesOrigin = Qt.point(0.5 * view.width, 0.5 * view.height);

			let clipRect = Qt.rect(gridShape.labelXWidth, gridShape.legendMargin, view.width - gridShape.labelXWidth - gridShape.legendMargin, view.height - gridShape.labelYHeight - gridShape.legendMargin)
			activeLayer.clipRect = clipRect

			inactiveLayer.layerMatrix.setXTranslation(gridShape.labelXWidth + gridShape.axesOrigin.x - 100)
			inactiveLayer.layerMatrix.setYTranslation(view.height - gridShape.labelYHeight - gridShape.axesOrigin.y)

			activeLayer.layerMatrix.setXTranslation(gridShape.labelXWidth + gridShape.axesOrigin.x)
			activeLayer.layerMatrix.setYTranslation(view.height - gridShape.labelYHeight - gridShape.axesOrigin.y)
		}


		onHeightChanged: {
			setLayersParams()

		}
		onWidthChanged: {
			setLayersParams()
		}
	}

	Component{
		id: polylineCompBlue

		PolyLineShape{

			color: "blue";

			showNodes: true;

			Component.onCompleted: {
				let pointList = []

				let point0 = Qt.point(-20, 40)
				let point1 = Qt.point(40, 40)
				let point2 = Qt.point(120,80)
				let point3 = Qt.point(240,280)
				let point4 = Qt.point(400 ,400)

				pointList.push(point0)
				pointList.push(point1)
				pointList.push(point2)
				pointList.push(point3)
				pointList.push(point4)

				points =  pointList
			}
		}
	}

	Component{
		id: polylineCompGreen

		PolyLineShape{

			color: "green";

			showNodes: true;

			Component.onCompleted: {
				let pointList = []

				let point0 = Qt.point(40, 10)
				let point1 = Qt.point(120, 40)
				let point2 = Qt.point(240,240)
				let point3 = Qt.point(400,320)


				pointList.push(point0)
				pointList.push(point1)
				pointList.push(point2)
				pointList.push(point3)

				points =  pointList
			}
		}
	}

	Component{
		id: polylineCompRed

		PolyLineShape{

			color: "red";

			showNodes: true;

			Component.onCompleted: {
				let pointList = []

				let point0 = Qt.point(-60, 60)
				let point1 = Qt.point(40, 60)
				let point2 = Qt.point(120,100)
				let point3 = Qt.point(200,320)
				let point4 = Qt.point(400 ,480)

				pointList.push(point0)
				pointList.push(point1)
				pointList.push(point2)
				pointList.push(point3)
				pointList.push(point4)

				points = pointList
			}
		}
	}

	//FOR PERFORMANCE TEST
	Component{
		id: polylineComp10000

		PolyLineShape{

			color: "violet";

			showNodes: true;

			property int count: 1000
			Component.onCompleted: {
				let pointList = []
				let date = new Date();

				for(let i = 0; i < count; i++){
					let point = Qt.point(i*1, 160)
					pointList.push(point)
				}

				points = pointList

				let date2 = new Date();

				console.log("TIME:: ", date2.valueOf() - date.valueOf())

			}

		}
	}

	Component{
		id: testLine

		PolyLineShape{

			color: "orange";

			showNodes: true;

			property int count: 10

		}
	}

	Component{
		id: polygon;

		PolygonShape{
			color: "violet"
			Component.onCompleted: {
				let points_ = []
				points_.push(Qt.point(40, -200))
				points_.push(Qt.point(120, -40))
				points_.push(Qt.point(280, -80))
				points_.push(Qt.point(360, -240))
				points_.push(Qt.point(80, -320))

				points = points_
			}
		}
	}

	CoordinateGridShape{
		id: gridShape;

		isFixedOrigin: false;
		legendMargin: 20;
		//gridStepMajorY: 20
		//gridStepMinorY: 5
		function getLogicalMajorLineY (lineY){
			return lineY/10
		}
	}

}
