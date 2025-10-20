import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0


QtObject {
	id: shapeBase

	property Item viewItem: null;
	property var layer;

	property bool hasCalibration: false
	property bool isTouched: false;
	property bool isSelected: false;
	property bool canSelect: false;
	property bool showNodes: false;
	property bool showControlPoints: false;
	property int touchedNodeIndex: -1;
	property int highlightedNodeIndex: -1;
	property int editNodeIndex: -1;

	property var highlightedNodeCoordinate
	property var touchedNodeCoordinate

	property CanvasMatrix viewMatrix: CanvasMatrix{};
	property CanvasMatrix layerMatrix: CanvasMatrix{};
	property CanvasMatrix shapeMatrix: CanvasMatrix{};
	property CanvasMatrix tempMatrix: CanvasMatrix{};
	property CanvasMatrix identityMatrix: CanvasMatrix{};

	property var viewMode;
	property var points: [];

	property bool mouseIsPressed: false;
	property var mousePressedCoord: Qt.point(0,0);

	property bool isHidden: false;
	property var lineDashArray: [];

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

	onMousePressed: {
		mouseIsPressed = true;
		//mousePressedCoord = getLogPosition(Qt.point(mouseEvent.x, mouseEvent.y))
		mousePressedCoord = Qt.point(mouseEvent.x, mouseEvent.y)
		if(viewItem.isPointsEditMode){
			editNodeIndex = findNodeIndex(getLogPosition(mousePressedCoord))
		}
	}
	onMouseReleased: {
		mouseIsPressed = false;
		editNodeIndex = -1
	}

	onMouseClicked: {
	}

	function draw (ctx, transformMatrixArg){
	}

	function getParams(){
		return [];
	}

	function setPoints(pointList){
		points = pointList
	}


	function drawComplex(ctx, transformMatrixArg){
		if(isHidden){
			return;
		}
		//console.log("!!!!!_____", LinearAlgebra.isIdentityMatrix(shapeMatrix.matrix))
		if(!LinearAlgebra.isIdentityMatrix(shapeMatrix.matrix)){
			transformMatrixArg.matrix = LinearAlgebra.multiplyByMatrix3x3(transformMatrixArg.matrix, shapeMatrix.matrix)
		}
		draw (ctx, transformMatrixArg)
		if(isSelected && viewItem.isEditMode){
			drawBoundingBox(ctx)
		}

		if(isSelected && (viewItem.isPointsEditMode || viewItem.isPointsAdditionMode || viewItem.isPointsDeletionMode)){
			drawEditPoints(ctx)
		}

		if(showControlPoints){
			drawControlPoints(ctx, transformMatrixArg)
		}
	}

	function drawControlPoints(ctx, transformMatrixArg){

	}

	function drawEditPoints(ctx){
		ctx.globalAlpha = 1
		for(let i = 0; i < points.length; i++){
			let point = getScreenPosition(points[i]);
			DesignScheme.drawEditPoint(ctx, point)
		}
	}

	function drawBoundingBox(ctx){}

	function getScreenPosition(logPosition){
		let matrix = LinearAlgebra.multiplyByMatrix3x3(viewItem.viewMatrix.matrix, layer.layerMatrix.matrix)
		matrix = LinearAlgebra.multiplyByMatrix3x3(matrix, shapeMatrix.matrix)
		if(hasCalibration){
			logPosition = getCalibratedPosition(logPosition)
		}
		let screenPosition = LinearAlgebra.transformPoint2d(logPosition, matrix)
		return screenPosition
	}

	function getLogPosition(screenPosition){
		let matrix = LinearAlgebra.multiplyByMatrix3x3(viewItem.viewMatrix.matrix, layer.layerMatrix.matrix)
		matrix = LinearAlgebra.multiplyByMatrix3x3(matrix, shapeMatrix.matrix)
		matrix = LinearAlgebra.getInvertedMatrix3x3(matrix)
		let logPosition = LinearAlgebra.transformPoint2d(screenPosition, matrix)
		if(hasCalibration){
			logPosition = getReverseCalibratedPosition(logPosition)
		}
		return logPosition
	}

	function getCalibratedPosition(position){
		return position;
	}
	function getReverseCalibratedPosition(position){
		return position;
	}

	function findNodeIndex(position){
		let foundIndex = -1
		for(let i = 0; i < points.length; i++){
			let point = points[i];
			let pointSize = (DesignScheme.shapePointSize + Style.marginS)/viewItem.viewMatrix.xScale();
			// if(point.x >= position.x - pointSize
			// 	&& point.x <= position.x + pointSize
			// 	&& point.y >= position.y - pointSize
			// 	&& point.y <= position.y + pointSize
			// 	){
			// 	foundIndex = i;
			// }
			let dist = AnalyticGeometry.distanceBetweenPoints2d(point, position)

			if(dist <= pointSize){
				//console.log("DIST:: ", dist, i)
				return i
			}
		}
		return foundIndex;
	}



	function setCoordinateShift(deltaX, deltaY){
		//console.log("setCoordinateShift", deltaX, deltaY)
		let pointsListNew = [];

		for(let i = 0; i < points.length; i++){
			let point = points[i];
			let pointNew = Qt.point(0,0);
			let pointScreen = getScreenPosition(point)
			pointNew.x = pointScreen.x + deltaX;
			pointNew.y = pointScreen.y + deltaY;
			pointNew = getLogPosition(pointNew)
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

	function deletePoint(point){
		if(points.length < 3){
			return;
		}
		if(isSelected && viewItem.isPointsDeletionMode){
			let minDistance = Style.marginL
			for(let i = 0; i < points.length; i++){
				let shapePoint = getScreenPosition(points[i]);
				let dist = AnalyticGeometry.distanceBetweenPoints2d(point, shapePoint);
				if(dist <= minDistance){
					points.splice(i, 1);
					break
				}
			}
		}

	}
	function addPoint(point){
		if(isSelected && viewItem.isPointsAdditionMode){
			let minDistancePointIndex = getNearestPointIndex(point);

			let logPosition = getLogPosition(point)
			if(minDistancePointIndex == points.length -1){
				//points.push(logPosition)
				points.splice(minDistancePointIndex, 0, logPosition);
			}
			else if(minDistancePointIndex == 0){
				//points.splice(0, 0, logPosition);
				points.splice(minDistancePointIndex + 1, 0, logPosition);
			}
			else {
				let prevDist = AnalyticGeometry.distanceBetweenPoints2d(point, getScreenPosition(points[minDistancePointIndex - 1]))
				let nextDist = AnalyticGeometry.distanceBetweenPoints2d(point, getScreenPosition(points[minDistancePointIndex + 1]))
				if(prevDist < nextDist){
					points.splice(minDistancePointIndex, 0, logPosition);
				}
				else {
					points.splice(minDistancePointIndex + 1, 0, logPosition);
				}
			}
		}
	}

	function getNearestPointIndex(point){
		let minDistance = 1000000
		let minDistancePointIndex = -1;
		for(let i = 0; i < points.length; i++){
			let shapePoint = getScreenPosition(points[i]);
			let dist = AnalyticGeometry.distanceBetweenPoints2d(point, shapePoint);
			if(dist <= minDistance){
				minDistance  = dist;
				minDistancePointIndex = i;

			}
		}
		return minDistancePointIndex
	}

}

