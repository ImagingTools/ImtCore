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

		if(isSelected){
			ctx.globalAlpha = 0.7
		}

		let centerScreen = getScreenPosition(center)
		let radiusScreen = radius * transformMatrixArg.xScale()

		ctx.beginPath()

		ctx.arc(centerScreen.x, centerScreen.y, radiusScreen,  0, 2 * Math.PI, true)

		ctx.stroke();
		ctx.fill();
		ctx.closePath();

		ctx.globalAlpha = 1;

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
		return Qt.point(center.x - radius/2, center.y - radius/2);
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

		pointsObj.topLeftPoint = Qt.point(center.x - radius/2, center.y - radius/2)
		pointsObj.topRightPoint = Qt.point(center.x + radius/2, center.y - radius/2)
		pointsObj.bottomLeftPoint = Qt.point(center.x - radius/2, center.y + radius/2)
		pointsObj.bottomRightPoint = Qt.point(center.x + radius/2, center.y + radius/2)

		return pointsObj;
	}

	function setCoordinateShift(deltaX, deltaY){
		let centerNew = Qt.point(0,0);
		let centerScreen = getScreenPosition(center)
		centerNew.x = centerScreen.x + deltaX;
		centerNew.y = centerScreen.y + deltaY;
		center = getLogPosition(centerNew)
	}

}

