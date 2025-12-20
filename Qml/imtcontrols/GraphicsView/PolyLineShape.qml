import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0


BoundingBox {
	id: polylineShape;

	property string color: "#000000";
	property string selectionColor: DesignScheme.selectionColor;
	property var colorList: [];
	property int lineWidth: 1;
	property real shapePointSize: DesignScheme.shapePointSize;
	property real shapeSelectedPointSize: DesignScheme.shapeSelectedPointSize;
	property bool showLine: true
	property bool isMultiColor: false;
	property string shapePointBorderColor: color;
	property bool hasHoverReaction: true;

	property var invalidPointIndexArr: [];

	onInvalidPointIndexArrChanged: {
		shapeChanged()
	}

	function draw(ctx, transformMatrixArg){

		drawBase(ctx, transformMatrixArg);
		if(showNodes){
			drawNodes(ctx, transformMatrixArg)
		}
	}

	function drawBase(ctx, transformMatrixArg){

		if(!showLine || points.length < 2){
			return;
		}

		if(lineDashArray.length && !isSelected){
			ctx.setLineDash(lineDashArray)
		}


		ctx.strokeStyle = isSelected ? DesignScheme.selectionColor : polylineShape.color;
		ctx.lineWidth = lineWidth
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
		ctx.stroke();
		if(lineDashArray.length && !isSelected){
			ctx.setLineDash([])
		}
		ctx.closePath();
	}

	function drawNodes(ctx, transformMatrixArg){
		ctx.strokeStyle = isSelected ? DesignScheme.selectionColor : polylineShape.shapePointBorderColor;
		ctx.fillStyle = isSelected ? DesignScheme.selectionColor :  polylineShape.color;
		ctx.lineWidth = 2

		let drawMultiColor = isMultiColor && colorList.length == points.length
		if(!drawMultiColor || !invalidPointIndexArr.length){
			ctx.beginPath()
		}
		for(let i = 0;i < points.length; i++){
			if(drawMultiColor || (invalidPointIndexArr.length)){
				ctx.beginPath()

				if(invalidPointIndexArr.indexOf(i) > -1){
					ctx.strokeStyle = "#ff0000"
					ctx.fillStyle = "#ff0000"
				}
				else if(isSelected){
					ctx.strokeStyle = DesignScheme.selectionColor
					ctx.fillStyle = DesignScheme.selectionColor
				}
				else if(!drawMultiColor){
					ctx.strokeStyle = polylineShape.shapePointBorderColor
					ctx.fillStyle = polylineShape.color
				}
				else if(drawMultiColor){
					ctx.strokeStyle = colorList[i]
					ctx.fillStyle = colorList[i]
				}
			}

			let point = getScreenPosition(points[i]);
			if(i == highlightedNodeIndex){
				DesignScheme.drawSelectedNode(ctx, point, shapeSelectedPointSize);
			}
			else {
				DesignScheme.drawNode(ctx, point, shapePointSize)
			}
			if(drawMultiColor  || (invalidPointIndexArr.length)){
				ctx.stroke();
				ctx.fill();
				ctx.closePath();
			}
		}

		if(!drawMultiColor || !invalidPointIndexArr.length){
			ctx.stroke();
			ctx.fill();

			ctx.closePath();

		}
	}

	function drawEditPoints(ctx){
		ctx.globalAlpha = 1
		for(let i = 0; i < points.length; i++){
			let point = getScreenPosition(points[i]);
			if(invalidPointIndexArr.indexOf(i) < 0){
				DesignScheme.drawEditPoint(ctx, point)
			}
			else {
				DesignScheme.drawEditPoint(ctx, point, "#ff0000")
			}
		}
	}

	function getPointDescription(pointIndex){
		let obj = ({})
		let point = points[pointIndex]
		obj.x = point.x;
		obj.y = point.y;
		return obj
	}

	onMouseClicked: {
		//let point  = getLogPosition(Qt.point(mouseEvent.x, mouseEvent.y))
		//console.log("Mouse Clicked::: ", point.x, point.y)
	}

	onMousePositionChanged: {
		if(!hasHoverReaction){
			return;
		}
		let point  = Qt.point(mouseEvent.x, mouseEvent.y)
		let foundNodeIndex = findNodeIndex(point)
		if(foundNodeIndex > -1){
			highlightedNodeCoordinate = points[foundNodeIndex]
		}
		highlightedNodeIndex = foundNodeIndex;

	}

	onHighlightedNodeIndexChanged: {
		shapeChanged()
	}

	function isInside(xArg, yArg){
		let margin = Style.marginL
		let mousePoint = Qt.point(xArg, yArg)
		for(let i = 0; i < points.length-1; i++){
			let currentPoint = getScreenPosition(points[i])
			let nextPoint = getScreenPosition(points[i+1])
			let nearestPoint = AnalyticGeometry.nearestPointOnLine2d(mousePoint, currentPoint, nextPoint, true);
			let dist = AnalyticGeometry.distanceBetweenPoints2d(mousePoint, nearestPoint)

			if(dist < margin){
				return true;
			}
		}
		return false;

	}
}

