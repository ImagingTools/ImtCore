import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0


SegmentBaseShape {
	id: sector;

	property real innerRadius: Style.sizeHintBXS;
	property real outerRadius: Style.sizeHintBXS/2;

	function draw(ctx, transformMatrixArg){
		let startAngle_ = startAngle
		let endAngle_ = endAngle
		let equalAngles = Math.abs(startAngle - endAngle) < precision
		if(equalAngles){
			startAngle_ = 0
			endAngle_ = 360
		}

		ctx.fillStyle = sector.color
		ctx.strokeStyle = isSelected ? DesignScheme.selectionColor : sector.borderColor
		ctx.lineWidth = isSelected ? DesignScheme.selectionLineWidth : sector.borderWidth

		let centerScreen = getScreenPosition(center)

		let innerRadiusScreen = innerRadius * transformMatrixArg.xScale()
		let outerRadiusScreen = outerRadius * transformMatrixArg.xScale()

		let startRad = Functions.getRadians(startAngle_)
		let endRad = Functions.getRadians(endAngle_)

		let innerStartX = centerScreen.x + innerRadiusScreen * Math.cos(startRad)
		let innerStartY =  centerScreen.y + innerRadiusScreen * Math.sin(startRad)
		let innerEndX = centerScreen.x + innerRadiusScreen * Math.cos(endRad)
		let innerEndY =  centerScreen.y + innerRadiusScreen * Math.sin(endRad)

		let outerStartX = centerScreen.x + outerRadiusScreen * Math.cos(startRad)
		let outerStartY =  centerScreen.y + outerRadiusScreen * Math.sin(startRad)
		let outerEndX = centerScreen.x + outerRadiusScreen * Math.cos(endRad)
		let outerEndY =  centerScreen.y + outerRadiusScreen * Math.sin(endRad)

		ctx.beginPath()
		ctx.arc(centerScreen.x, centerScreen.y, outerRadiusScreen,  startRad, endRad, anticlockwise)

		//ctx.lineTo(innerEndX, innerEndY)
		if(innerRadius > 0){
			ctx.arc(centerScreen.x, centerScreen.y, innerRadiusScreen, endRad, startRad, !anticlockwise)
		}
		//ctx.lineTo(outerStartX, outerStartY)
		ctx.closePath()

		if(!equalAngles){
			ctx.stroke();
		}
		if(isSelected){
			ctx.globalAlpha = 0.7
		}
		ctx.fill();
		ctx.globalAlpha = 1

		if(equalAngles){
			ctx.beginPath()
			ctx.arc(centerScreen.x, centerScreen.y, outerRadiusScreen,  startRad, endRad, anticlockwise)
			ctx.stroke();
			ctx.beginPath()
			ctx.arc(centerScreen.x, centerScreen.y, innerRadiusScreen, endRad, startRad, !anticlockwise)
			ctx.stroke();
		}
	}

	function isInsideRadius(dist){
		return (dist <= outerRadius && dist >= innerRadius)
	}


}

