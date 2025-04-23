import QtQuick 2.12

import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0
import imtcolgui 1.0

GraphicsView{
    id: scheme;

	property var drawModel: []


	Component.onCompleted: {
		//TEST

		let layer = scheme.createLayer("test");

		layer.addShape(rec1);
		layer.addShape(rec2);
		layer.addShape(rec3);

		// layer.addShape(polygon);

		// layer.addShape(circle);
		// layer.addShape(circle);
		// layer.addShape(circle);

		 //layer.addShape(line);

		//layer.addShape(arc);

		// layer.addShape(text);

		//Rectangle
		drawModel.push({"point": Qt.point(300, 300), "width": 100, "height": 100, "color": "red"})
		drawModel.push({"point": Qt.point(600, 300), "width": 100, "height": 100, "color": "green"})
		drawModel.push({"point": Qt.point(600, 600), "width": 100, "height": 100, "color": "blue"})

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

		scheme.requestPaint();

		//TEST
	}


	Component{
		id: selectedRecComp;
		SelectionShape{
		}
	}

	RectangleParams{
		id: recParams1;
	}
	RectangleShape{
		id: rec1;
		selectionComp: selectedRecComp;

		Component.onCompleted: {
		}


		function getParams(layerId){
			let item = drawModel[0];

			recParams1.point = item.point;
			recParams1.width = item.width;
			recParams1.height = item.height;
			recParams1.radius = item.radius !== undefined ? item.radius : 0;
			recParams1.color = item.color;

			return recParams1;
		}
	}
	RectangleParams{
		id: recParams2;
	}
	RectangleShape{
		id: rec2;
		selectionComp: selectedRecComp;

		Component.onCompleted: {
		}

		function getParams(layerId){
			let item = drawModel[1];

			recParams2.point = item.point;
			recParams2.width = item.width;
			recParams2.height = item.height;
			recParams2.radius = item.radius !== undefined ? item.radius : 0;
			recParams2.color = item.color;

			return recParams2;
		}
	}
	RectangleParams{
		id: recParams3;
	}
	RectangleShape{
		id: rec3;
		selectionComp: selectedRecComp;

		Component.onCompleted: {
		}

		function getParams(layerId){
			let item = drawModel[2];

			recParams3.point = item.point;
			recParams3.width = item.width;
			recParams3.height = item.height;
			recParams3.radius = item.radius !== undefined ? item.radius : 0;
			recParams3.color = item.color;

			return recParams3;
		}
	}


	PolygonParams{
		id: polygonParams
	}
	PolygonShape{
		id: polygon;

		Component.onCompleted: {
		}

		function getParams(layerId){
			return drawModel[0];
		}
	}

	CircleParams{
		id: circleParams
	}
	CircleShape{
		id: circle;

		Component.onCompleted: {
		}

		function getParams(layerId){
			return drawModel[0];
		}
	}

	LineParams{
		id: lineParams;
	}
	LineShape{
		id: line;

		Component.onCompleted: {
		}

		function getParams(layerId){
			return drawModel[0];
		}
	}

	ArcParams{
	}
	ArcShape{
		id: arc;

		Component.onCompleted: {
		}

		function getParams(layerId){
			return drawModel[0];
		}
	}

	TextParams{
		id: textParams;
	}
	TextShape{
		id: text;

		Component.onCompleted: {
		}

		function getParams(layerId){
			return drawModel[0];
		}
	}


	ComplexShape{

		PolygonShape{
		}

		TextShape{
		}

	}


    onCopySignal: {
        if(scheme.selectedIndex >=0){
            scheme.copyObjectFunc(scheme.selectedIndex);
        }
    }
    onPasteSignal: {
        if(scheme.selectedIndex >=0){
            scheme.pasteObjectFunc();
        }
    }
    onDeleteSignal: {
        if(scheme.selectedIndex >=0){
            scheme.deleteObjectFunc(scheme.selectedIndex);
        }
    }
}


