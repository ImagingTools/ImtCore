import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0


BoundingBox {
	id: polylineShape;

	property string color: "#000000";

	function draw(ctx, transformMatrixArg){

		drawBase(ctx, transformMatrixArg);
		if(showNodes){
			drawNodes(ctx, transformMatrixArg)
		}
		if(highlightedNodeIndex >-1  && highlightedNodeCoordinate !==undefined){
			drawSelectedNode(ctx, highlightedNodeCoordinate, transformMatrixArg)
		}
	}

	function drawBase(ctx, transformMatrixArg){

		let params = getParams()
		ctx.strokeStyle = params.color !== undefined ? params.color : polylineShape.color;
		ctx.lineWidth = params.lineWidth !== undefined ? params.lineWidth : 1
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
		ctx.strokeStyle = params.color !== undefined ? params.color : polylineShape.color;
		ctx.fillStyle = params.color !== undefined ? params.color : polylineShape.color;
		ctx.lineWidth = params.lineWidth !== undefined ? params.lineWidth : 2
		ctx.beginPath()
		for(let i = 0;i < points.length; i++){
			let point = getScreenPosition(points[i]);
			DesignScheme.drawNode(ctx, point)
		}
		ctx.closePath();
	}

	function drawSelectedNode(ctx, point, transformMatrixArg){
		let params = getParams()
		point = getScreenPosition(point)
		ctx.strokeStyle = params.color !== undefined ? params.color : polylineShape.color;
		ctx.fillStyle = params.color !== undefined ? params.color : polylineShape.color;
		ctx.lineWidth = params.lineWidth !== undefined ? params.lineWidth : 2
		DesignScheme.drawSelectedNode(ctx, point);
	}

	function getPointDescription(pointIndex){
		let obj = ({})
		let point = points[pointIndex]
		obj.x = point.x;
		obj.y = point.y;
		return obj
	}

	onMousePositionChanged: {
		let foundNodeIndex = findNodeIndex(mouseEvent)
		if(foundNodeIndex > -1){
			highlightedNodeCoordinate = points[foundNodeIndex]
		}
		highlightedNodeIndex = foundNodeIndex;
	}
	onHighlightedNodeIndexChanged: {
		shapeChanged()
	}
}

