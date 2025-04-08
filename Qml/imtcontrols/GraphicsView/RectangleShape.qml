import QtQuick 2.12
import Acf 1.0

GraphicsShape {

	function draw(ctx, layerId, index){
		let params_;
		params_ = getParams(layerId, index)

		ctx.fillStyle = params_.color !== undefined ? params_.color : "transparent";
		ctx.strokeStyle = params_.borderColor !== undefined ? params_.borderColor : "transparent";
		ctx.lineWidth = params_.lineWidth !== undefined ? params_.lineWidth : 0
		let radius = params_.radius !== undefined ? params_.radius : 0
		ctx.beginPath()
		ctx.roundedRect(params_.point.x , params_.point.y, params_.width, params_.height, radius, radius)
		ctx.stroke();
		ctx.fill();
		ctx.closePath();
	}


}

