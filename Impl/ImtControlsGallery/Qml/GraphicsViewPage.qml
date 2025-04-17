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

		layer.addShape(rec);
		layer.addShape(rec);
		layer.addShape(rec);

		// layer.addShape(polygon);

		// layer.addShape(circle);
		// layer.addShape(circle);
		// layer.addShape(circle);

		 //layer.addShape(line);

		//layer.addShape(arc);

		// layer.addShape(text);


		scheme.requestPaint();
		//TEST
	}

	RectangleParams{
		id: recParams;
	}

	Component{
		id: selectedRecComp;
		SelectionShape{
		}
	}
	RectangleShape{
		id: rec;
		selectionComp: selectedRecComp;

		Component.onCompleted: {
			drawModel.push({"point": Qt.point(300, 300), "width": 100, "height": 100, "color": "red"})
			drawModel.push({"point": Qt.point(600, 300), "width": 100, "height": 100, "color": "green"})
			drawModel.push({"point": Qt.point(600, 600), "width": 100, "height": 100, "color": "blue"})
		}

		function getSelectedObject(){
			return selectedRec;
		}


		function getParams(layerId, index){
			let item = drawModel[index];

			recParams.point = item.point;
			recParams.width = item.width;
			recParams.height = item.height;
			recParams.radius = item.radius !== undefined ? item.radius : 0;
			recParams.color = item.color;

			return recParams;
			// return item;
		}
	}


	PolygonParams{
		id: polygonParams
	}
	PolygonShape{
		id: polygon;

		property var drawModel: []


		Component.onCompleted: {
			drawModel.push({"points": [Qt.point(300, 300), Qt.point(600, 300),  Qt.point(600, 600)], "color": "red"})
		}

		function getParams(layerId, index){
			return drawModel[index];
		}
	}

	CircleParams{
		id: circleParams
	}
	CircleShape{
		id: circle;

		property var drawModel: []


		Component.onCompleted: {
			drawModel.push({"point": Qt.point(300, 300), "color": "red", "radius": 100})
			drawModel.push({"point": Qt.point(600, 300), "color": "green", "radius": 100})
			drawModel.push({"point": Qt.point(600, 600), "color": "violet", "radius": 100})
		}

		function getParams(layerId, index){
			return drawModel[index];
		}
	}

	LineParams{
		id: lineParams;
	}
	LineShape{
		id: line;

		property var drawModel: []


		Component.onCompleted: {
			drawModel.push({"points": [Qt.point(300, 300), Qt.point(600, 300),  Qt.point(600, 600)], "color": "red"})

		}

		function getParams(layerId, index){
			return drawModel[index];
		}
	}

	ArcParams{
	}
	ArcShape{
		id: arc;

		property var drawModel: []


		Component.onCompleted: {
			drawModel.push({"point": Qt.point(300, 300), "color": "red", "radius": 100, "startAngle": 0, "endAngle": Math.PI, "anticlockwise": true})

		}

		function getParams(layerId, index){
			return drawModel[index];
		}
	}

	TextParams{
		id: textParams;
	}
	TextShape{
		id: text;

		property var drawModel: []


		Component.onCompleted: {
			drawModel.push({"point": Qt.point(300, 300), "color": "red", "fontSize": 20, "text": "TEXT"})

		}

		function getParams(layerId, index){
			return drawModel[index];
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


