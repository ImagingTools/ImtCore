import QtQuick 2.12

import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0
import imtcolgui 1.0




GraphicsView{
	id: view;

	hasLeftButtonMenu: true;
	hasRightButtonMenu: true;
	property var drawModel: []

	color: Style.baseColor

	Component.onCompleted: {
		//TEST

		//Rectangle
		drawModel.push({"point": Qt.point(600, 300), "width": 100, "height": 100, "color": "red"})
		drawModel.push({"point": Qt.point(1200, 300), "width": 100, "height": 100, "color": "green"})
		drawModel.push({"point": Qt.point(1200, 600), "width": 100, "height": 100, "color": "blue"})

		//Polygon
		// drawModel.push({"points": [Qt.point(300, 300), Qt.point(600, 300),  Qt.point(600, 600)], "color": "red"})

		//Circle
		// drawModel.push({"point": Qt.point(300, 300), "color": "red", "radius": 100})
		// drawModel.push({"point": Qt.point(600, 300), "color": "green", "radius": 100})
		// drawModel.push({"point": Qt.point(600, 600), "color": "violet", "radius": 100})

		//line
		// drawModel.push({"points": [Qt.point(300, 300), Qt.point(600, 300),  Qt.point(600, 600)], "color": "red"})

		//Arc
		//drawModel.push({"point": Qt.point(300, 300), "color": "red", "radius": 100, "startAngle": 0, "endAngle": Math.PI, "anticlockwise": true})

		//Text
		//drawModel.push({"point": Qt.point(300, 300), "color": "red", "fontSize": 20, "text": "TEXT"})


		let background = view.getBackgroundLayer()
		background.addShape(imageShape);
		background.addShape(gridShape);

		background.addShape(boundingBox);//remove

		let layer = view.getActiveLayer();

		let rec1 = rec1Comp.createObject(this);
		let rec2 = rec2Comp.createObject(this);
		let rec3 = rec3Comp.createObject(this);

		rec1.points.push(view.drawModel[0].point);
		rec2.points.push(view.drawModel[1].point);
		rec3.points.push(view.drawModel[2].point);

		layer.addShape(rec1);
		layer.addShape(rec2);
		layer.addShape(rec3);

		let polygonObj = polygonComp.createObject(this);
		layer.addShape(polygonObj);


		let toolsLayer = view.getToolsLayer()
		toolsLayer.addShape(rulerShape)


		// layer.addShape(polygon);

		// layer.addShape(circle);
		// layer.addShape(circle);
		// layer.addShape(circle);

		 //layer.addShape(line);

		//layer.addShape(arc);

		// layer.addShape(text);



		view.requestPaint();

		//TEST
	}

	GraphicsShapeBase{
		id: gridShape;

		function draw(ctx, transformMatrixArg){
			let step = 30;

			ctx.lineCap = "round"
			ctx.lineJoin = "round"
			ctx.lineWidth = 1;

			ctx.fillStyle = Style.imagingToolsGradient1;
			ctx.strokeStyle = Style.imagingToolsGradient1;


			for(let i = 0; i * step < view.drawingAreaWidth; i++){//vertical lines
				let x1 = i * step;
				let y1 =  0 ;
				let x2 = i * step;
				let y2 = view.drawingAreaHeight ;

				let point1 = getScreenPosition(Qt.point(x1,y1))
				let point2 = getScreenPosition(Qt.point(x2,y2))
				x1 = point1.x;
				y1 =  point1.y ;
				x2 = point2.x;
				y2 = point2.y;

				ctx.beginPath()
				ctx.moveTo(x1, y1);
				ctx.lineTo(x2, y2);
				ctx.stroke();
			}

			for(let i = 0; i * step < view.drawingAreaHeight; i++){//horizontal lines
				let x1 = 0 ;
				let y1 =  i * step ;
				let x2 =  view.drawingAreaWidth;
				let y2 =  i * step;

				let point1 = getScreenPosition(Qt.point(x1,y1))
				let point2 = getScreenPosition(Qt.point(x2,y2))
				x1 = point1.x;
				y1 =  point1.y ;
				x2 = point2.x;
				y2 = point2.y;

				ctx.beginPath()
				ctx.moveTo(x1, y1);
				ctx.lineTo(x2, y2);
				ctx.stroke();
			}

			ctx.strokeStyle = Style.borderColor;
			ctx.lineWidth = 2;
			ctx.beginPath()
			ctx.moveTo(1, 1);
			ctx.lineTo(view.drawingAreaWidth, 1);
			ctx.lineTo(view.drawingAreaWidth, view.drawingAreaHeight - 1);
			ctx.lineTo(1, view.drawingAreaHeight - 1);
			ctx.lineTo(1, 1);
			ctx.stroke();
		}
	}

	GraphicsShapeBase{
		id: rulerShape;

		function draw(ctx, transformMatrixArg){

			ctx.lineCap = "round"
			ctx.lineJoin = "round"
			ctx.lineWidth = 2;

			ctx.strokeStyle = "green";


			let width_ = 500
			let height_ =  50
			let step = 10;
			let x0= 500//view.drawingAreaWidth/3
			let y0 = 800//view.drawingAreaHeight - 150

			for(let i = 0; i * step <= width_; i++){//vertical lines
				let x1 = x0 + i * step;
				let y1 =  y0 + height_ ;
				let x2 = x0 + i * step;
				let coeff_ = i % 10 ? 1 : 0
				let y2 = y0 + coeff_ * height_/2 ;

				ctx.beginPath()
				ctx.moveTo(x1, y1);
				ctx.lineTo(x2, y2);
				ctx.stroke();
			}

			ctx.moveTo(x0, y0 + 7 * height_/8);
			ctx.lineTo(x0 + width_, y0 + 7 * height_/8);

			ctx.stroke();
		}
	}

	ImageParams{
		id: imageParams

		source: "/Images/testImage"
		width: view.width;
		height: view.height;
		point: Qt.point(0,0)
	}

	ImageShape{
		id: imageShape;

		imageSource: "/Images/testImage";
		function getParams(){
			return imageParams;
		}
	}

	BoundingBox{
		id: boundingBox;

		function getBoundingBoxCornerPoints(){
			let points = {};
			points.topLeftPoint = getScreenPosition(Qt.point(200,200))
			points.topRightPoint = getScreenPosition(Qt.point(300, 200))
			points.bottomRightPoint = getScreenPosition(Qt.point(300,400))
			points.bottomLeftPoint = getScreenPosition(Qt.point(200, 400))

			return points;
		}
	}

	Component{
		id: selectedRecComp;
		SelectionShape{
		}
	}

	RectangleParams{
		id: recParams1;
	}

	Component{
		id: rec1Comp;
		RectangleShape{
			Component.onCompleted: {
				let item = view.drawModel[0]
				points.push(item.point);
				width = item.width;
				height = item.height;
				radius = item.radius !== undefined ? item.radius : 0
			}

			function setPoints(pointList){
				let item = view.drawModel[0];
				item.point.x = pointList[0].x;
				item.point.y = pointList[0].y;
			}

			function getParams(){
				let item = view.drawModel[0];

				recParams1.point = item.point;
				recParams1.width = item.width;
				recParams1.height = item.height;
				recParams1.radius = item.radius !== undefined ? item.radius : 0;
				recParams1.color = item.color;

				return recParams1;
			}
		}
	}

	RectangleParams{
		id: recParams2;
	}
	Component{
		id: rec2Comp;
		RectangleShape{

			Component.onCompleted: {
				let item = view.drawModel[1]
				points.push(item.point);
				width = item.width;
				height = item.height;
				radius = item.radius !== undefined ? item.radius : 0
			}

			function setPoints(pointList){
				let item = view.drawModel[1];
				item.point.x = pointList[0].x;
				item.point.y = pointList[0].y;
			}

			function getParams(){
				let item = view.drawModel[1];

				recParams2.point = item.point;
				recParams2.width = item.width;
				recParams2.height = item.height;
				recParams2.radius = item.radius !== undefined ? item.radius : 0;
				recParams2.color = item.color;

				return recParams2;
			}
		}
	}


	RectangleParams{
		id: recParams3;
	}
	Component{
		id: rec3Comp;
		RectangleShape{

			Component.onCompleted: {
				let item = view.drawModel[2]
				points.push(item.point);
				width = item.width;
				height = item.height;
				radius = item.radius !== undefined ? item.radius : 0
			}

			function setPoints(pointList){
				let item = view.drawModel[2];
				item.point.x = pointList[0].x;
				item.point.y = pointList[0].y;
			}

			function getParams(){
				let item = view.drawModel[2];

				recParams3.point = item.point;
				recParams3.width = item.width;
				recParams3.height = item.height;
				recParams3.radius = item.radius !== undefined ? item.radius : 0;
				recParams3.color = item.color;

				return recParams3;
			}
		}
	}

	PolygonParams{
		id: polygonParams
	}
	PolygonShape{
		id: polygon;

		Component.onCompleted: {
		}

		function getParams(){
			return view.drawModel[0];
		}
	}

	CircleParams{
		id: circleParams
	}
	CircleShape{
		id: circle;

		Component.onCompleted: {
		}

		function getParams(){
			return view.drawModel[0];
		}
	}

	LineParams{
		id: lineParams;
	}
	PolyLineShape{
		id: line;

		Component.onCompleted: {
		}

		function getParams(){
			return view.drawModel[0];
		}
	}

	ArcParams{
	}
	ArcShape{
		id: arc;

		Component.onCompleted: {
		}

		function getParams(){
			return view.drawModel[0];
		}
	}

	TextParams{
		id: textParams;
	}
	TextShape{
		id: text;

		Component.onCompleted: {
		}

		function getParams(){
			return view.drawModel[0];
		}
	}



	ComplexShape{

		PolygonShape{
		}

		TextShape{
		}

	}

	Component{
		id: polygonComp;

		PolygonShape{
			color: "violet"
			Component.onCompleted: {
				let points_ = []
				points_.push(Qt.point(40, 200))
				points_.push(Qt.point(120, 40))
				points_.push(Qt.point(280, 80))
				points_.push(Qt.point(360, 240))
				points_.push(Qt.point(80, 320))

				points = points_
			}
		}
	}


    onCopySignal: {
		if(view.selectedIndex >=0){
			view.copyObjectFunc(view.selectedIndex);
        }
    }
    onPasteSignal: {
		if(view.selectedIndex >=0){
			view.pasteObjectFunc();
        }
    }
    onDeleteSignal: {
		if(view.selectedIndex >=0){
			view.deleteObjectFunc(view.selectedIndex);
        }
    }
}


