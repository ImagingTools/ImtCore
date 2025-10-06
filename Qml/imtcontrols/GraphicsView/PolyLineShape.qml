import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0


BoundingBox {
	id: polylineShape;

	property string color: "#000000";
	property var colorList: [];
	property int lineWidth: 1;
	property real shapePointSize: DesignScheme.shapePointSize;
	property real shapeSelectedPointSize: DesignScheme.shapeSelectedPointSize;
	property bool showLine: true
	property bool isMultiColor: false;
	property bool isHidden: false;


	function draw(ctx, transformMatrixArg){
		if(isHidden){
			return;
		}

		drawBase(ctx, transformMatrixArg);
		if(showNodes){
			drawNodes(ctx, transformMatrixArg)
		}
	}

	function drawBase(ctx, transformMatrixArg){

		if(!showLine || points.length < 2){
			return;
		}

		let params = getParams()
		ctx.strokeStyle = isSelected ? DesignScheme.selectionColor : params.color !== undefined ? params.color : polylineShape.color;
		ctx.lineWidth = params.lineWidth !== undefined ? params.lineWidth : lineWidth
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
		ctx.closePath();
	}

	function drawNodes(ctx, transformMatrixArg){
		let params = getParams()
		ctx.strokeStyle = isSelected ? DesignScheme.selectionColor : params.color !== undefined ? params.color : polylineShape.color;
		ctx.fillStyle = isSelected ? DesignScheme.selectionColor : params.color !== undefined ? params.color : polylineShape.color;
		ctx.lineWidth = params.lineWidth !== undefined ? params.lineWidth : 2

		let drawMultiColor = isMultiColor && colorList.length == points.length
		if(!drawMultiColor){
			ctx.beginPath()
		}
		for(let i = 0;i < points.length; i++){
			if(drawMultiColor){
				ctx.beginPath()
				ctx.strokeStyle = colorList[i]
				ctx.fillStyle = colorList[i]
			}
			let point = getScreenPosition(points[i]);
			if(i == highlightedNodeIndex){
				DesignScheme.drawSelectedNode(ctx, point, shapeSelectedPointSize);
			}
			else {
				DesignScheme.drawNode(ctx, point, shapePointSize)
			}
			if(drawMultiColor){
				ctx.stroke();
				ctx.fill();
				ctx.closePath();
			}
		}

		if(!drawMultiColor){
			ctx.stroke();
			ctx.fill();

			ctx.closePath();

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
		let point  = getLogPosition(Qt.point(mouseEvent.x, mouseEvent.y))
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
		let margin = Style.marginL / viewItem.viewMatrix.xScale();
		let mousePoint = getLogPosition(Qt.point(xArg, yArg))
		for(let i = 0; i < points.length-1; i++){
			let nearestPoint = AnalyticGeometry.nearestPointOnLine2d(mousePoint, points[i], points[i+1], true);
			let dist = AnalyticGeometry.distanceBetweenPoints2d(mousePoint, nearestPoint)

			if(dist < margin){
				return true;
			}
		}
		return false;

	}
}

