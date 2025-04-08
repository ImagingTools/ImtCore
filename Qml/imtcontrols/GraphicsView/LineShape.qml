import QtQuick 2.12
import Acf 1.0

GraphicsShape {
	function draw(ctx, layerId, index){
		let params = getParams(layerId, index)
		ctx.strokeStyle = params.color !== undefined ? params.color : "transparent";
		ctx.lineWidth = params.lineWidth !== undefined ? params.lineWidth : 0
		ctx.beginPath()
		let lastPoint;
		for(let i = 0;i < params.points.length; i++){
			let point = params.points[i];
			if(i == 0){
				ctx.moveTo(point.x, point.y);
			}
			else {
				ctx.lineTo(point.x, point.y);
			}
			if(i == params.points.length -1){
				lastPoint = point
			}
		}
		ctx.stroke();
		ctx.closePath();
	}
}

