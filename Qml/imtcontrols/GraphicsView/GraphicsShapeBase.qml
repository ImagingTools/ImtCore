import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0


QtObject {

	property Item viewItem: null;
	property var layer;

	property bool isTouched: false;
	property bool isSelected: false;
	property bool showNodes: false;
	property int touchedNodeIndex: -1;
	property int highlightedNodeIndex: -1;

	property var highlightedNodeCoordinate
	property var touchedNodeCoordinate

	property CanvasMatrix viewMatrix: CanvasMatrix{};
	property CanvasMatrix layerMatrix: CanvasMatrix{};
	property CanvasMatrix shapeMatrix: CanvasMatrix{};
	property CanvasMatrix tempMatrix: CanvasMatrix{};
	property CanvasMatrix identityMatrix: CanvasMatrix{};

	property var viewMode;
	property var points: [];
	property var pointsBackup: [];

	signal shapeChanged();
	signal shapeInfo(var info);
	signal pointInfo(var info);

	signal mouseClicked(var mouseEvent)
	signal mousePressed(var mouseEvent)
	signal mouseReleased(var mouseEvent)
	signal mouseDoubleClicked(var mouseEvent)
	signal mousePositionChanged(var mouseEvent)
	signal mousePositionShift(real deltaX, real deltaY)

	onShapeChanged: {
		viewItem.requestPaint();
	}

	function draw (ctx, transformMatrixArg){
	}

	function getParams(){
		return [];
	}

	function setPoints(pointList){
	}



	function getScreenPosition(logPosition){
		let matrix = tempMatrix.multiplyByMatrix(viewMatrix.matrix, layerMatrix.matrix)
		let screenPosition = tempMatrix.transformPoint(logPosition, matrix)
		return screenPosition
	}

	function getLogPosition(screenPosition){
		let invViewMatrix = viewMatrix.getInvertedMatrix();
		let invLayerMatrix = layerMatrix.getInvertedMatrix();

		let matrix = tempMatrix.multiplyByMatrix(invViewMatrix, invLayerMatrix)

		let logPosition = tempMatrix.transformPoint(screenPosition, matrix)

		return logPosition
	}

	function findNodeIndex(position){
		let foundIndex = -1
		for(let i = 0; i < points.length; i++){
			let point = points[i];
			let pointSize = DesignScheme.shapePointSize + Style.marginXS;
			if(point.x >= position.x - pointSize
				&& point.x <= position.x + pointSize
				&& point.y >= position.y - pointSize
				&& point.y <= position.y + pointSize
				){
				foundIndex = i;
			}
		}
		return foundIndex;
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

