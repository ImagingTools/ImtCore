import QtQuick 2.12
import Acf 1.0

GraphicsShape {

	function draw(ctx, layerId, index){
		let params = getParams(layerId, index)
		ctx.fillStyle = params.color !== undefined ? params.color : "transparent";
		ctx.strokeStyle = params.borderColor !== undefined ? params.borderColor : "transparent";
		ctx.lineWidth = params.lineWidth !== undefined ? params.lineWidth : 0
		let radius = params.radius !== undefined ? params.radius : 0
		ctx.beginPath()
		ctx.arc(params.point.x, params.point.y, radius,  0, 2 * Math.PI, true)
		ctx.stroke();
		ctx.fill();
		ctx.closePath();
	}


}

