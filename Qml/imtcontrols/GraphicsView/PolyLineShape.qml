import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0


BoundingBox {
	id: polylineShape;

	property string color: "#000000";

	function draw(ctx, layerMatrixArg){

		drawBase(ctx, layerMatrixArg);
		if(showNodes){
			drawNodes(ctx, layerMatrixArg)
		}
		if(selectedNodeIndex >-1  && selectedNodeCoordinate !==undefined){
			drawSelectedNode(ctx, selectedNodeCoordinate, layerMatrixArg)
		}
	}

	function drawBase(ctx, layerMatrixArg){

		let params = getParams()
		ctx.strokeStyle = params.color !== undefined ? params.color : polylineShape.color;
		ctx.lineWidth = params.lineWidth !== undefined ? params.lineWidth : 1
		ctx.beginPath()
		for(let i = 0;i < points.length; i++){
			let point = points[i];
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

	function drawNodes(ctx, layerMatrixArg){
		let params = getParams()
		ctx.strokeStyle = params.color !== undefined ? params.color : polylineShape.color;
		ctx.fillStyle = params.color !== undefined ? params.color : polylineShape.color;
		ctx.lineWidth = params.lineWidth !== undefined ? params.lineWidth : 2
		ctx.beginPath()
		for(let i = 0;i < points.length; i++){
			DesignScheme.drawNode(ctx, points[i])
		}
		ctx.closePath();
	}

	function drawSelectedNode(ctx, point, layerMatrixArg){
		let params = getParams()
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
}

