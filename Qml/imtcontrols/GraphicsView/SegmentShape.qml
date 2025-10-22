import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0


SegmentBaseShape {
	id: sector;

	property real radius: Style.sizeHintBXS;


	function draw(ctx, transformMatrixArg){

		if(lineDashArray.length && !isSelected){
			ctx.setLineDash(lineDashArray)
		}
		ctx.fillStyle = getFillStyle(ctx, transformMatrixArg)
		ctx.strokeStyle = isSelected ? DesignScheme.selectionColor : sector.borderColor
		ctx.lineWidth = isSelected ? DesignScheme.selectionLineWidth : sector.borderWidth

		ctx.globalAlpha = opacity

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


		if(isSelected){
			ctx.globalAlpha = 0.7
		}
		ctx.fill();
		ctx.globalAlpha = 1
		ctx.stroke();

		if(lineDashArray.length && !isSelected){
			ctx.setLineDash([])
		}


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
		else {
			let pointsList = []
			let startRad = Functions.getRadians(startAngle)
			let endRad = Functions.getRadians(endAngle)

			if(isInsideAngles(0)){
				pointsList.push(Qt.point(center.x + radius, center.y))
			}
			if(isInsideAngles(90)){
				pointsList.push(Qt.point(center.x, center.y + radius))
			}
			if(isInsideAngles(180)){
				pointsList.push(Qt.point(center.x - radius, center.y))
			}
			if(isInsideAngles(270)){
				pointsList.push(Qt.point(center.x, center.y - radius))
			}

			pointsList.push(center)

			pointsList.push(Qt.point(center.x + radius * Math.cos(startRad), center.y + radius * Math.sin(startRad)))
			pointsList.push(Qt.point(center.x + radius * Math.cos(endRad), center.y + radius * Math.sin(endRad)))

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

			controlPointsList.push(pointsObj.topPoint)
			controlPointsList.push(pointsObj.bottomPoint)
			controlPointsList.push(pointsObj.leftPoint)
			controlPointsList.push(pointsObj.rightPoint)

		}
		else {
			let centerScreen = getScreenPosition(center)
			let radiusScreen = radius * transformMatrixArg.xScale()

			let middleAngle = isInsideAngles((startAngle + endAngle)/2) ? (startAngle + endAngle)/2 : (startAngle + endAngle)/2 + 180

			let startRad = Functions.getRadians(startAngle)
			let endRad = Functions.getRadians(endAngle)
			let middleRad = Functions.getRadians(middleAngle)

			// let startX = centerScreen.x + radiusScreen * Math.cos(startRad)
			// let startY =  centerScreen.y + radiusScreen * Math.sin(startRad)
			// let endX = centerScreen.x + radiusScreen * Math.cos(endRad)
			// let endY =  centerScreen.y + radiusScreen * Math.sin(endRad)

			let middleEdgeStartX  = centerScreen.x + 0.5*radiusScreen * Math.cos(startRad)
			let middleEdgeStartY  = centerScreen.y + 0.5*radiusScreen * Math.sin(startRad)
			let middleEdgeEndX  = centerScreen.x + 0.5*radiusScreen * Math.cos(endRad)
			let middleEdgeEndY  = centerScreen.y + 0.5*radiusScreen * Math.sin(endRad)

			let arcCenterX = centerScreen.x + radiusScreen * Math.cos(middleRad)
			let arcCenterY = centerScreen.y + radiusScreen * Math.sin(middleRad)

			//start and end points
			// DesignScheme.drawPoint(ctx, Qt.point(startX, startY), controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)
			// DesignScheme.drawPoint(ctx,  Qt.point(endX, endY), controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)

			//middel edge points
			DesignScheme.drawPoint(ctx, Qt.point(middleEdgeStartX, middleEdgeStartY), controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)
			DesignScheme.drawPoint(ctx,  Qt.point(middleEdgeEndX, middleEdgeEndY), controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)

			//arc center point
			DesignScheme.drawPoint(ctx, Qt.point(arcCenterX, arcCenterY), controlPointRadius, controlPointColor, controlPointBorderColor, controlPointBorderWidth)

			controlPointsList.push(Qt.point(middleEdgeStartX, middleEdgeStartY))
			controlPointsList.push(Qt.point(middleEdgeEndX, middleEdgeEndY))
			controlPointsList.push(Qt.point(arcCenterX, arcCenterY))
		}

		controlPointsScreen = controlPointsList

		ctx.stroke()
		ctx.fill()
	}

}

