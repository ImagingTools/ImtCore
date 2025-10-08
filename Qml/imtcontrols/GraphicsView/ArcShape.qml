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
		ctx.beginPath()

		let centerScreen = getScreenPosition(center)
		let radiusScreen = radius * transformMatrixArg.xScale()

		let startRad = Functions.getRadians(startAngle)
		let endRad = Functions.getRadians(endAngle)

		ctx.arc(centerScreen.x, centerScreen.y, radiusScreen,  startRad, endRad, anticlockwise)
		ctx.stroke();
	}

	function isInsideRadius(dist){
		let margin_ = Style.marginM;
		return (dist < radius + margin_ && dist > radius - margin_)
	}

}

