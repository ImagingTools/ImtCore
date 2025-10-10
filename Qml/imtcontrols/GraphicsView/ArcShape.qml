import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0


SegmentBaseShape {
	id: sector;

	property real radius: Style.sizeHintBXS;
	property int lineWidth: 1;

	function draw(ctx, transformMatrixArg){

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
	}

	function isInsideRadius(dist){
		let margin_ = Style.marginM;
		return (dist < radius + margin_ && dist > radius - margin_)
	}

	function getBoundingBoxPoints(){
		let pointsObj = ({});

		//if(equalAngles){
			pointsObj.topLeftPoint = Qt.point(center.x - radius, center.y - radius)
			pointsObj.topRightPoint = Qt.point(center.x + radius, center.y - radius)
			pointsObj.bottomLeftPoint = Qt.point(center.x - radius, center.y + radius)
			pointsObj.bottomRightPoint = Qt.point(center.x + radius, center.y + radius)
		//}

		return pointsObj;
	}
}

