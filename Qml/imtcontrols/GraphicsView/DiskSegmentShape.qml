import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0


SegmentBaseShape {
	id: sector;

	property real innerRadius: Style.sizeHintBXS;
	property real outerRadius: Style.sizeHintBXS;

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

		ctx.globalAlpha = opacity

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

	function getBoundingBoxPoints(){
		let pointsObj = ({});

		if(equalAngles){
			pointsObj.topLeftPoint = Qt.point(center.x - outerRadius, center.y - outerRadius)
			pointsObj.topRightPoint = Qt.point(center.x + outerRadius, center.y - outerRadius)
			pointsObj.bottomLeftPoint = Qt.point(center.x - outerRadius, center.y + outerRadius)
			pointsObj.bottomRightPoint = Qt.point(center.x + outerRadius, center.y + outerRadius)
		}
		else {
			let pointsList = []
			let startRad = Functions.getRadians(startAngle)
			let endRad = Functions.getRadians(endAngle)

			if(isInsideAngles(0)){
				pointsList.push(Qt.point(center.x + outerRadius, center.y))
				pointsList.push(Qt.point(center.x + innerRadius, center.y))
			}
			if(isInsideAngles(90)){
				pointsList.push(Qt.point(center.x, center.y + outerRadius))
				pointsList.push(Qt.point(center.x, center.y + innerRadius))
			}
			if(isInsideAngles(180)){
				pointsList.push(Qt.point(center.x - outerRadius, center.y))
				pointsList.push(Qt.point(center.x - innerRadius, center.y))
			}
			if(isInsideAngles(270)){
				pointsList.push(Qt.point(center.x, center.y - outerRadius))
				pointsList.push(Qt.point(center.x, center.y - innerRadius))
			}

			pointsList.push(Qt.point(center.x + outerRadius * Math.cos(startRad), center.y + outerRadius * Math.sin(startRad)))
			pointsList.push(Qt.point(center.x + outerRadius * Math.cos(endRad), center.y + outerRadius * Math.sin(endRad)))

			pointsList.push(Qt.point(center.x + innerRadius * Math.cos(startRad), center.y + innerRadius * Math.sin(startRad)))
			pointsList.push(Qt.point(center.x + innerRadius * Math.cos(endRad), center.y + innerRadius * Math.sin(endRad)))

			return getLimitsObject(pointsList)
		}

		return pointsObj;
	}

	function drawControlPoints(ctx, transformMatrixArg){
		let pointsObj = getBoundingBoxMidPoints(true);
		ctx.strokeStyle = DesignScheme.boundingBoxBorderColor
		ctx.fillStyle = DesignScheme.boundingBoxBorderColor
		ctx.lineWidth = DesignScheme.boundingBoxLineWidth
		ctx.beginPath()

		if(equalAngles){
			DesignScheme.drawBoundingBoxControlPoint(ctx, pointsObj.topPoint, identityMatrix.matrix, controlPointColor)
			DesignScheme.drawBoundingBoxControlPoint(ctx, pointsObj.bottomPoint, identityMatrix.matrix, controlPointColor)
			DesignScheme.drawBoundingBoxControlPoint(ctx, pointsObj.leftPoint, identityMatrix.matrix, controlPointColor)
			DesignScheme.drawBoundingBoxControlPoint(ctx, pointsObj.rightPoint, identityMatrix.matrix, controlPointColor)

			DesignScheme.drawBoundingBoxControlPoint(ctx, getScreenPosition(Qt.point(center.x + innerRadius, center.y)), identityMatrix.matrix, controlPointColor)
			DesignScheme.drawBoundingBoxControlPoint(ctx, getScreenPosition(Qt.point(center.x, center.y + innerRadius)), identityMatrix.matrix, controlPointColor)
			DesignScheme.drawBoundingBoxControlPoint(ctx, getScreenPosition(Qt.point(center.x - innerRadius, center.y)), identityMatrix.matrix, controlPointColor)
			DesignScheme.drawBoundingBoxControlPoint(ctx, getScreenPosition(Qt.point(center.x, center.y - innerRadius)), identityMatrix.matrix, controlPointColor)
		}
		else {
			let centerScreen = getScreenPosition(center)
			let outerRadiusScreen = outerRadius * transformMatrixArg.xScale()
			let innerRadiusScreen = innerRadius * transformMatrixArg.xScale()

			let startRad = Functions.getRadians(startAngle)
			let endRad = Functions.getRadians(endAngle)

			// let startX_inner = centerScreen.x + innerRadiusScreen * Math.cos(startRad)
			// let startY_inner =  centerScreen.y + innerRadiusScreen * Math.sin(startRad)
			// let endX_inner = centerScreen.x + innerRadiusScreen * Math.cos(endRad)
			// let endY_inner =  centerScreen.y + innerRadiusScreen * Math.sin(endRad)

			// let startX_outer = centerScreen.x + outerRadiusScreen * Math.cos(startRad)
			// let startY_outer =  centerScreen.y + outerRadiusScreen * Math.sin(startRad)
			// let endX_outer = centerScreen.x + outerRadiusScreen * Math.cos(endRad)
			// let endY_outer =  centerScreen.y + outerRadiusScreen * Math.sin(endRad)

			let middleEdgeStartX  = centerScreen.x + 0.5 * (innerRadiusScreen + outerRadiusScreen) * Math.cos(startRad)
			let middleEdgeStartY  = centerScreen.y + 0.5 * (innerRadiusScreen + outerRadiusScreen) * Math.sin(startRad)
			let middleEdgeEndX  = centerScreen.x + 0.5 * (innerRadiusScreen + outerRadiusScreen) * Math.cos(endRad)
			let middleEdgeEndY  = centerScreen.y + 0.5 * (innerRadiusScreen + outerRadiusScreen) * Math.sin(endRad)

			let arcCenterX_inner = centerScreen.x + innerRadiusScreen * Math.cos((startRad + endRad)/2)
			let arcCenterY_inner = centerScreen.y + innerRadiusScreen * Math.sin((startRad + endRad)/2)
			let arcCenterX_outer = centerScreen.x + outerRadiusScreen * Math.cos((startRad + endRad)/2)
			let arcCenterY_outer = centerScreen.y + outerRadiusScreen * Math.sin((startRad + endRad)/2)

			//start and end points
			// DesignScheme.drawBoundingBoxControlPoint(ctx, Qt.point(startX_inner, startY_inner), identityMatrix.matrix, controlPointColor)
			// DesignScheme.drawBoundingBoxControlPoint(ctx,  Qt.point(endX_inner, endY_inner), identityMatrix.matrix, controlPointColor)
			// DesignScheme.drawBoundingBoxControlPoint(ctx, Qt.point(startX_outer, startY_outer), identityMatrix.matrix, controlPointColor)
			// DesignScheme.drawBoundingBoxControlPoint(ctx,  Qt.point(endX_outer, endY_outer), identityMatrix.matrix, controlPointColor)

			//middel edge points
			DesignScheme.drawBoundingBoxControlPoint(ctx, Qt.point(middleEdgeStartX, middleEdgeStartY), identityMatrix.matrix, controlPointColor)
			DesignScheme.drawBoundingBoxControlPoint(ctx,  Qt.point(middleEdgeEndX, middleEdgeEndY), identityMatrix.matrix, controlPointColor)

			//arc center point
			DesignScheme.drawBoundingBoxControlPoint(ctx, Qt.point(arcCenterX_inner, arcCenterY_inner), identityMatrix.matrix, controlPointColor)
			DesignScheme.drawBoundingBoxControlPoint(ctx, Qt.point(arcCenterX_outer, arcCenterY_outer), identityMatrix.matrix, controlPointColor)
		}

		ctx.stroke()
		ctx.fill()
	}

}

