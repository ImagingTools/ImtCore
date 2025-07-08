import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0


GraphicsShapeBase {
	id: polylineShape;

	property string color: "#000000";

	function draw(ctx, layerMatrix){

		drawBase(ctx);
		if(showNodes){
			drawNodes(ctx)
		}
		if(hasNodeSelection && selectedNodeCoordinate !==undefined){
			drawSelectedNode(ctx, selectedNodeCoordinate)
		}
	}

	function drawBase(ctx, layerMatrix){

		let params = getParams()
		ctx.strokeStyle = params.color !== undefined ? params.color : polylineShape.color;
		ctx.lineWidth = params.lineWidth !== undefined ? params.lineWidth : 1
		ctx.beginPath()
		let pointList = getPoints();
		for(let i = 0;i < pointList.length; i++){
			let point = pointList[i];
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

	function drawNodes(ctx, layerMatrix){
		let params = getParams()
		ctx.strokeStyle = params.color !== undefined ? params.color : polylineShape.color;
		ctx.fillStyle = params.color !== undefined ? params.color : polylineShape.color;
		ctx.lineWidth = params.lineWidth !== undefined ? params.lineWidth : 2
		let radius = params.radius !== undefined ? params.radius : DesignScheme.shapePointSize;
		ctx.beginPath()
		let pointList = getPoints();
		for(let i = 0;i < pointList.length; i++){
			let point = pointList[i];
			ctx.moveTo(point.x, point.y);
			ctx.arc(point.x, point.y, radius,  0, 2 * Math.PI, true)
			ctx.stroke();
			ctx.fill();
		}
		ctx.closePath();
	}

	function drawSelectedNode(ctx, point){
		let params = getParams()
		ctx.strokeStyle = params.color !== undefined ? params.color : polylineShape.color;
		ctx.fillStyle = params.color !== undefined ? params.color : polylineShape.color;
		ctx.lineWidth = params.lineWidth !== undefined ? params.lineWidth : 2
		let radius = params.radius !== undefined ? params.radius : DesignScheme.shapeSelectedPointSize;
		ctx.beginPath()
		ctx.moveTo(point.x, point.y);
		ctx.arc(point.x, point.y, radius,  0, 2 * Math.PI, true)
		ctx.stroke();
		ctx.fill();
		ctx.closePath();
	}

	function getPoints(){
		let pointList = []
		let params_ = getParams();
		pointList = params_.points
		return pointList;
	}
}

