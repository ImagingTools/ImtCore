import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0

BoundingBox {

	property var center: Qt.point(0,0);
	property real radius: Style.sizeHintBXS;
	property string color: Style.borderColor;
	property string borderColor: color//Style.borderColor2;
	property int borderWidth: 1;
	property real opacity: 1;

	function draw(ctx, transformMatrixArg){
		ctx.fillStyle = color;
		ctx.strokeStyle = borderColor;
		ctx.lineWidth = borderWidth

		ctx.globalAlpha = opacity

		if(isSelected){
			ctx.globalAlpha = 0.7
		}

		let centerScreen = getScreenPosition(center)
		let radiusScreen = radius * transformMatrixArg.xScale()

		ctx.beginPath()

		ctx.arc(centerScreen.x, centerScreen.y, radiusScreen,  0, 2 * Math.PI, true)

		ctx.fill();
		ctx.globalAlpha = 1;
		ctx.stroke();
		ctx.closePath();


		if(isSelected){
			drawSelection(ctx, transformMatrixArg, centerScreen, radiusScreen)
		}

	}

	function drawSelection(ctx, transformMatrixArg, centerScreen, radiusScreen){
		//console.log("drawSelected")
		ctx.strokeStyle = DesignScheme.selectionColor
		ctx.lineWidth = DesignScheme.selectionLineWidth
		ctx.beginPath()
		ctx.arc(centerScreen.x, centerScreen.y, radiusScreen,  0, 2 * Math.PI, true)
		ctx.stroke();
		ctx.closePath();
	}

	function drawNodes(ctx, transformMatrixArg){
	}

	function getBoundingBoxCoordinate(){
		return Qt.point(center.x - radius, center.y - radius);
	}

	function getBoundingBoxWidth(){
		return radius;
	}

	function getBoundingBoxHeight(){
		return radius;
	}

	function isInside(xArg, yArg){
		let point = getLogPosition(Qt.point(xArg, yArg))
		return AnalyticGeometry.distanceBetweenPoints2d(point, center) <= radius;
	}

	function getBoundingBoxPoints(){

		let pointsObj = ({});

		pointsObj.topLeftPoint = Qt.point(center.x - radius, center.y - radius)
		pointsObj.topRightPoint = Qt.point(center.x + radius, center.y - radius)
		pointsObj.bottomLeftPoint = Qt.point(center.x - radius, center.y + radius)
		pointsObj.bottomRightPoint = Qt.point(center.x + radius, center.y + radius)

		return pointsObj;
	}

	function setCoordinateShift(deltaX, deltaY){
		let centerNew = Qt.point(0,0);
		let centerScreen = getScreenPosition(center)
		centerNew.x = centerScreen.x + deltaX;
		centerNew.y = centerScreen.y + deltaY;
		center = getLogPosition(centerNew)
	}

	function drawControlPoints(ctx, transformMatrixArg){
		let pointsObj = getBoundingBoxMidPoints(true);
		ctx.strokeStyle = controlPointBorderColor
		ctx.fillStyle = controlPointBorderColor
		ctx.lineWidth = controlPointBorderWidth
		ctx.beginPath()

		DesignScheme.drawPoint(ctx, pointsObj.topPoint, controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)
		DesignScheme.drawPoint(ctx, pointsObj.bottomPoint, controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)
		DesignScheme.drawPoint(ctx, pointsObj.leftPoint, controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)
		DesignScheme.drawPoint(ctx, pointsObj.rightPoint, controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)

		let controlPointsList = []
		controlPointsList.push(pointsObj.topPoint)
		controlPointsList.push(pointsObj.bottomPoint)
		controlPointsList.push(pointsObj.leftPoint)
		controlPointsList.push(pointsObj.rightPoint)
		controlPointsScreen = controlPointsList

		ctx.stroke()
		ctx.fill()
	}
}

