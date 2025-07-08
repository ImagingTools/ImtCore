import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0


BoundingBox {

	property bool isTouched: false;
	property bool isSelected: false;
	property bool showNodes: false;
	property bool hasNodeSelection: false;

	property var selectedNodeCoordinate

	property CanvasMatrix shapeMatrix: CanvasMatrix{};
	property CanvasMatrix tempMatrix: CanvasMatrix{};

	signal shapeChanged();

	function draw (ctx, layerMatrix){
	}

	// function drawBase(ctx){
	// }

	// function drawSelection(ctx){
	// }

	// function drawNodes(ctx){
	// }

	function drawSelectedNode(ctx, point){
	}

	function getParams(){
		return [];
	}

	function getPoints(){
		return []
	}

	function setPoints(pointList){
	}


	function isInside(x_, y_, matrix){
		return false;
	}

	function getLogPosition(screenPosition){
		return screenPosition
	}

	function getScreenPosition(logPosition){
		return logPosition
	}

	function setCoordinateShift(deltaX, deltaY){
		let pointsListNew = [];
		let pointList = getPoints();
		for(let i = 0; i < pointList.length; i++){
			let point = pointList[i];
			let pointNew = Qt.point(0,0);
			pointNew.x = point.x + deltaX;
			pointNew.y = point.y + deltaY;
			pointsListNew.push(pointNew);
		}
		setPoints(pointsListNew);
	}
}

