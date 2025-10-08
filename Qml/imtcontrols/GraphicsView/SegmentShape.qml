import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0


SegmentBaseShape {
	id: sector;

	property real radius: Style.sizeHintBXS;

	function draw(ctx, transformMatrixArg){

		ctx.fillStyle = sector.color
		ctx.strokeStyle = isSelected ? DesignScheme.selectionColor : sector.borderColor
		ctx.lineWidth = isSelected ? DesignScheme.selectionLineWidth : sector.borderWidth

		let centerScreen = getScreenPosition(center)
		let radiusScreen = radius * transformMatrixArg.xScale()

		let startRad = Functions.getRadians(startAngle)
		let endRad = Functions.getRadians(endAngle)
		let startX = centerScreen.x + radiusScreen * Math.cos(startRad)
		let startY =  centerScreen.y + radiusScreen * Math.sin(startRad)
		let endX = centerScreen.x + radiusScreen * Math.cos(endRad)
		let endY =  centerScreen.y + radiusScreen * Math.sin(endRad)

		ctx.beginPath()
		if(Math.abs(startAngle - endAngle) > precision){
			ctx.arc(centerScreen.x, centerScreen.y, radiusScreen,  startRad, endRad, anticlockwise)
			ctx.lineTo(centerScreen.x, centerScreen.y)
			ctx.lineTo(startX, startY)
		}
		else {
			ctx.arc(centerScreen.x, centerScreen.y, radiusScreen,  0, 2 * Math.PI, anticlockwise)
		}

		ctx.stroke();
		if(isSelected){
			ctx.globalAlpha = 0.7
		}
		ctx.fill();
		ctx.globalAlpha = 1

		// ctx.beginPath()
		// DesignScheme.drawNode(ctx, center, 4)
		// ctx.fill();
		// ctx.closePath();

	}

	function isInsideRadius(dist){
		return (dist <= radius)
	}

	function getBoundingBoxPoints(){

		let pointsObj = ({});

		if(equalAngles){
			pointsObj.topLeftPoint = Qt.point(center.x - radius, center.y - radius)
			pointsObj.topRightPoint = Qt.point(center.x + radius, center.y - radius)
			pointsObj.bottomLeftPoint = Qt.point(center.x - radius, center.y + radius)
			pointsObj.bottomRightPoint = Qt.point(center.x + radius, center.y + radius)
		}

		return pointsObj;
	}

	function drawControlPoints(ctx, transformMatrixArg){
		let pointsObj = getBoundingBoxMidPoints();
		ctx.strokeStyle = DesignScheme.boundingBoxBorderColor
		ctx.fillStyle = DesignScheme.boundingBoxBorderColor
		ctx.lineWidth = DesignScheme.boundingBoxLineWidth
		ctx.beginPath()

		DesignScheme.drawBoundingBoxControlPoint(ctx, pointsObj.topPoint)
		DesignScheme.drawBoundingBoxControlPoint(ctx, pointsObj.bottomPoint)
		DesignScheme.drawBoundingBoxControlPoint(ctx, pointsObj.leftPoint)
		DesignScheme.drawBoundingBoxControlPoint(ctx, pointsObj.rightPoint)

		ctx.stroke()
		ctx.fill()
	}

}

