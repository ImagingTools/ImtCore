import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0


SegmentBaseShape {
	id: sector;

	property real radius: Style.sizeHintBXS;
	property int lineWidth: 1;

	function draw(ctx, transformMatrixArg){
		if(lineDashArray.length && !isSelected){
			ctx.setLineDash(lineDashArray)
		}

		ctx.strokeStyle = isSelected ? DesignScheme.selectionColor : color
		ctx.lineWidth = isSelected ? DesignScheme.selectionLineWidth : lineWidth

		ctx.globalAlpha = opacity

		ctx.beginPath()

		let centerScreen = getScreenPosition(center)
		let radiusScreen = radius * transformMatrixArg.xScale()

		let startRad = Functions.getRadians(startAngle)
		let endRad = Functions.getRadians(endAngle)

		if(!equalAngles){
			ctx.arc(centerScreen.x, centerScreen.y, radiusScreen,  startRad, endRad, anticlockwise)
		}
		else {
			ctx.arc(centerScreen.x, centerScreen.y, radiusScreen,  0, 2*Math.PI, anticlockwise)
		}
		ctx.stroke();

		ctx.globalAlpha = 1
		if(lineDashArray.length && !isSelected){
			ctx.setLineDash([])
		}

	}

	function isInsideRadius(dist){
		let margin_ = Style.marginM;
		return (dist < radius + margin_ && dist > radius - margin_)
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

			pointsList.push(Qt.point(center.x + radius * Math.cos(startRad), center.y + radius * Math.sin(startRad)))
			pointsList.push(Qt.point(center.x + radius * Math.cos(endRad), center.y + radius * Math.sin(endRad)))

			return getLimitsObject(pointsList)
		}

		return pointsObj;
	}
}

