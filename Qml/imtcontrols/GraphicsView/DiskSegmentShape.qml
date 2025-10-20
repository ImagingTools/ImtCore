import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0


SegmentBaseShape {
	id: sector;

	property real innerRadius: Style.sizeHintBXS;
	property real outerRadius: Style.sizeHintBXS;

	function draw(ctx, transformMatrixArg){
		if(lineDashArray.length && !isSelected){
			ctx.setLineDash(lineDashArray)
		}

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
		if(innerRadius >= 0){
			ctx.arc(centerScreen.x, centerScreen.y, innerRadiusScreen, endRad, startRad, !anticlockwise)
		}
		//ctx.lineTo(outerStartX, outerStartY)
		ctx.closePath()

		if(isSelected){
			ctx.globalAlpha = 0.7
		}
		ctx.fill();
		ctx.globalAlpha = 1
		if(!equalAngles){
			ctx.stroke();
		}

		if(equalAngles){
			ctx.beginPath()
			ctx.arc(centerScreen.x, centerScreen.y, outerRadiusScreen,  startRad, endRad, anticlockwise)
			ctx.stroke();
			ctx.beginPath()
			ctx.arc(centerScreen.x, centerScreen.y, innerRadiusScreen, endRad, startRad, !anticlockwise)
			ctx.stroke();
		}

		if(lineDashArray.length && !isSelected){
			ctx.setLineDash([])
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

		let controlPointsList = []

		if(equalAngles){
			DesignScheme.drawPoint(ctx, pointsObj.topPoint, controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)
			DesignScheme.drawPoint(ctx, pointsObj.bottomPoint, controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)
			DesignScheme.drawPoint(ctx, pointsObj.leftPoint, controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)
			DesignScheme.drawPoint(ctx, pointsObj.rightPoint, controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)

			// DesignScheme.drawPoint(ctx, getScreenPosition(Qt.point(center.x + innerRadius, center.y)), controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)
			// DesignScheme.drawPoint(ctx, getScreenPosition(Qt.point(center.x, center.y + innerRadius)), controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)
			// DesignScheme.drawPoint(ctx, getScreenPosition(Qt.point(center.x - innerRadius, center.y)), controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)
			// DesignScheme.drawPoint(ctx, getScreenPosition(Qt.point(center.x, center.y - innerRadius)), controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)

			let topRightInnerPoint = getScreenPosition(Qt.point(center.x + innerRadius * Math.cos(Math.PI/4), center.y + innerRadius * Math.sin(Math.PI/4)))
			let topLeftInnerPoint = getScreenPosition(Qt.point(center.x + innerRadius * Math.cos(Math.PI/4 + Math.PI/2), center.y + innerRadius * Math.sin(Math.PI/4 + Math.PI/2)))
			let bottomLeftInnerPoint = getScreenPosition(Qt.point(center.x + innerRadius * Math.cos(Math.PI/4 + Math.PI), center.y + innerRadius * Math.sin(Math.PI/4 + Math.PI)))
			let bottomRightInnerPoint = getScreenPosition(Qt.point(center.x + innerRadius * Math.cos(Math.PI/4 + 3*Math.PI/2), center.y + innerRadius * Math.sin(Math.PI/4 + 3*Math.PI/2)))

			DesignScheme.drawPoint(ctx, topRightInnerPoint, controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)
			DesignScheme.drawPoint(ctx, topLeftInnerPoint, controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)
			DesignScheme.drawPoint(ctx, bottomLeftInnerPoint, controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)
			DesignScheme.drawPoint(ctx, bottomRightInnerPoint, controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)

			controlPointsList.push(pointsObj.topPoint)
			controlPointsList.push(pointsObj.bottomPoint)
			controlPointsList.push(pointsObj.leftPoint)
			controlPointsList.push(pointsObj.rightPoint)

			controlPointsList.push(topRightInnerPoint)
			controlPointsList.push(topLeftInnerPoint)
			controlPointsList.push(bottomLeftInnerPoint)
			controlPointsList.push(bottomRightInnerPoint)
		}
		else {
			let centerScreen = getScreenPosition(center)
			let outerRadiusScreen = outerRadius * transformMatrixArg.xScale()
			let innerRadiusScreen = innerRadius * transformMatrixArg.xScale()

			let middleAngle = isInsideAngles((startAngle + endAngle)/2) ? (startAngle + endAngle)/2 : (startAngle + endAngle)/2 + 180

			let startRad = Functions.getRadians(startAngle)
			let endRad = Functions.getRadians(endAngle)
			let middleRad = Functions.getRadians(middleAngle)

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

			let arcCenterX_inner = centerScreen.x + innerRadiusScreen * Math.cos(middleRad)
			let arcCenterY_inner = centerScreen.y + innerRadiusScreen * Math.sin(middleRad)
			let arcCenterX_outer = centerScreen.x + outerRadiusScreen * Math.cos(middleRad)
			let arcCenterY_outer = centerScreen.y + outerRadiusScreen * Math.sin(middleRad)

			//start and end points
			// DesignScheme.drawPoint(ctx, Qt.point(startX_inner, startY_inner), controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)
			// DesignScheme.drawPoint(ctx,  Qt.point(endX_inner, endY_inner), controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)
			// DesignScheme.drawPoint(ctx, Qt.point(startX_outer, startY_outer), controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)
			// DesignScheme.drawPoint(ctx,  Qt.point(endX_outer, endY_outer), controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)

			//middel edge points
			DesignScheme.drawPoint(ctx, Qt.point(middleEdgeStartX, middleEdgeStartY), controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)
			DesignScheme.drawPoint(ctx,  Qt.point(middleEdgeEndX, middleEdgeEndY), controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)

			//arc center point
			DesignScheme.drawPoint(ctx, Qt.point(arcCenterX_inner, arcCenterY_inner), controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)
			DesignScheme.drawPoint(ctx, Qt.point(arcCenterX_outer, arcCenterY_outer), controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)

			controlPointsList.push(Qt.point(middleEdgeStartX, middleEdgeStartY))
			controlPointsList.push(Qt.point(middleEdgeEndX, middleEdgeEndY))
			controlPointsList.push(Qt.point(arcCenterX_inner, arcCenterY_inner))
			controlPointsList.push(Qt.point(arcCenterX_outer, arcCenterY_outer))
		}

		controlPointsScreen = controlPointsList

		ctx.stroke()
		ctx.fill()
	}

}

