import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0


BoundingBox {
	id: polygonShape;

	property string color: "transparent";

	function draw(ctx, transformMatrixArg){

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
		if(isSelected){
			ctx.globalAlpha = 0.7
		}
		ctx.fill();
		ctx.globalAlpha = 1

	}

	function isInside(xArg, yArg){

		let mousePoint = Qt.point(xArg, yArg)
		if(findNodeIndex(mousePoint) > -1){
			return true;
		}

		return AnalyticGeometry.pointInPolygon(mousePoint, points)
	}

	function addPoint(point){
		if(isSelected && viewItem.isPointsAdditionMode){
			let logPosition = getLogPosition(point)
			let firstIndex = -1;
			let secondIndex = -1;
			let minDist = 1000000
			for(let i = 0; i < points.length; i++){
				let nextIndex = (i == points.length -1) ? 0 : i+1
				let dist = AnalyticGeometry.distanceBetweenPointAndLineSection2d(point, getScreenPosition(points[i]), getScreenPosition(points[nextIndex]))
				if(dist > -1 && dist < minDist){
					minDist = dist;
					firstIndex = i;
					secondIndex = nextIndex;
				}
			}
			if(firstIndex < 0 || secondIndex < 0){
				return;
			}
			if(secondIndex == 0){
				points.splice(0, 0, logPosition);
			}
			else {
				points.splice(secondIndex, 0, logPosition);
			}
			console.log("First_INdex:: ", firstIndex, "Second_Index::", secondIndex)

		}
	}
}

