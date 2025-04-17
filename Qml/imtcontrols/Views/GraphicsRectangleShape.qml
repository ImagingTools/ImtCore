import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

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

	function drawSelection(ctx, layerId, index){
		let params_;
		params_ = getParams(layerId, index)

		ctx.fillStyle = "transparent";
		ctx.strokeStyle = "yellow";
		ctx.lineWidth = 4
		let radius = params_.radius !== undefined ? params_.radius : 0
		ctx.beginPath()
		ctx.roundedRect(params_.point.x , params_.point.y, params_.width, params_.height, radius, radius)
		ctx.stroke();
		ctx.fill();
		ctx.closePath();
	}

	function drawBorderPoints(ctx, layerId, index){
		let params_;
		params_ = getParams(layerId, index)
		let point = params_.point;
		let width = params_.width;
		let height = params_.height;
		ctx.fillStyle = "yellow";
		ctx.strokeStyle = "yellow";
		let radius = 8;

		ctx.beginPath()
		ctx.arc(point.x, point.y, radius,  0, 2 * Math.PI, true)
		ctx.stroke();
		ctx.fill();
		ctx.closePath();

		ctx.beginPath()
		ctx.arc(point.x + width, point.y, radius,  0, 2 * Math.PI, true)
		ctx.stroke();
		ctx.fill();
		ctx.closePath();

		ctx.beginPath()
		ctx.arc(point.x + width, point.y + height, radius,  0, 2 * Math.PI, true)
		ctx.stroke();
		ctx.fill();
		ctx.closePath();

		ctx.beginPath()
		ctx.arc(point.x, point.y + height , radius,  0, 2 * Math.PI, true)
		ctx.stroke();
		ctx.fill();
		ctx.closePath();

	}

}

