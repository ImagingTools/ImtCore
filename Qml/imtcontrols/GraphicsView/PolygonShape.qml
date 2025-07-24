import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0


BoundingBox {
	id: polygonShape;

	property string color: "transparent";

	function draw(ctx, transformMatrixArg){
		if(isSelected){
			ctx.globalAlpha = 0.7
		}

		let params = getParams()
		ctx.fillStyle = params.color !== undefined ? params.color : polygonShape.color;
		ctx.strokeStyle = isSelected ? DesignScheme.selectionColor : params.borderColor !== undefined ? params.borderColor :  polygonShape.color;
		ctx.lineWidth = isSelected ? DesignScheme.selectionLineWidth : params.lineWidth !== undefined ? params.lineWidth : 1
		ctx.beginPath()
		for(let i = 0;i < points.length; i++){
			let point = getScreenPosition(points[i]);
			if(i == 0){
				ctx.moveTo(point.x, point.y);
			}
			else {
				ctx.lineTo(point.x, point.y);
			}

		}

		ctx.closePath();
		ctx.stroke();
		ctx.fill();

	}

	function isInside(xArg, yArg){

		let mousePoint = getLogPosition(Qt.point(xArg, yArg))

		return AnalyticGeometry.pointInPolygon(mousePoint, points)
	}

}

