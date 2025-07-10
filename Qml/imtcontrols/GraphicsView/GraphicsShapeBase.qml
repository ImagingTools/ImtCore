import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0


QtObject {

	property Item viewItem: null;

	property bool isTouched: false;
	property bool isSelected: false;
	property bool showNodes: false;
	property int touchedNodeIndex: -1;
	property int selectedNodeIndex: -1;

	property var selectedNodeCoordinate
	property var touchedNodeCoordinate

	property CanvasMatrix layerMatrix: CanvasMatrix{};
	property CanvasMatrix shapeMatrix: CanvasMatrix{};
	property CanvasMatrix tempMatrix: CanvasMatrix{};

	property var viewMode;
	property var points: [];

	signal shapeChanged();
	signal shapeInfo(var info);
	signal pointInfo(var info);

	onShapeChanged: {
		viewItem.requestPaint();
	}

	function draw (ctx, layerMatrixArg){
	}

	function getParams(){
		return [];
	}

	function setPoints(pointList){
	}

	function isInside(x_, y_, matrix){
		return false;
	}

	function getScreenPosition(logPosition){
		let screenPosition = layerMatrix.transformPoint(logPosition)
		screenPosition = shapeMatrix.transformPoint(logPosition)

		return screenPosition
	}

	function getLogPosition(screenPosition){
		let invShapeMatrix = shapeMatrix.getInvertedMatrix();
		let invLayerMatrix = layerMatrix.getInvertedMatrix();

		let logPosition = tempMatrix.transformPoint(screenPosition, invShapeMatrix)
		logPosition = tempMatrix.transformPoint(screenPosition, invLayerMatrix)

		return logPosition
	}

	function setCoordinateShift(deltaX, deltaY){
		let pointsListNew = [];

		for(let i = 0; i < points.length; i++){
			let point = points[i];
			let pointNew = Qt.point(0,0);
			pointNew.x = point.x + deltaX;
			pointNew.y = point.y + deltaY;
			pointsListNew.push(pointNew);
		}
		setPoints(pointsListNew);
	}

	function getPointDescription(pointIndex){
		let obj = ({})
		return obj
	}

	function getShapeDescription(){
		let obj = ({})
		return obj
	}

}

