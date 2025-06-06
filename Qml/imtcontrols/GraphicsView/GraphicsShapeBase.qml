import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

QtObject {

	property bool isTouched: false;
	property bool isSelected: false;

	function draw (ctx, layerId){
		drawBase(ctx, layerId)
		if(isSelected){
			drawSelection(ctx, layerId)
		}
	}

	function drawBase(ctx, layerId){
	}

	function drawSelection(ctx, layerId){
	}

	function drawEditPoints(ctx, layerId){
	}

	function drawBoundingBox(ctx, layerId){
	}

	function getParams(layerId){
		return [];
	}

	function getPoints(layerId){
		return []
	}

	function setPoints(pointList){
	}

	function getBoundingBoxCoordinate(){
			return Qt.point(0,0);
	}

	function getBoundingBoxWidth(){
		return 0;
	}

	function getBoundingBoxHeight(){
		return 0;
	}

	function isInside(x_, y_, matrix/*deltaX, deltaY, scaleCoeff*/){
		return false;
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

