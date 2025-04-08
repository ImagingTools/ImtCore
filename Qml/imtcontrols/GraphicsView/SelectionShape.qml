import QtQuick 2.12
import Acf 1.0

GraphicsShape {

	property QtObject source: null;
	property int sourceIndex: 0;
	property var coordinate

	function draw(ctx, layerId, index){
		drawSelection(ctx, layerId, index);
		drawBorderPoints(ctx, layerId, index)
	}

	function drawSelection(ctx, layerId, index){
		let params_;
		params_ = source.getParams(layerId, index)

		ctx.fillStyle = "transparent";
		ctx.strokeStyle = "yellow";
		ctx.lineWidth = 4
		let radius = params_.radius !== undefined ? params_.radius : 0

		let point_ = getPoint(params_);

		ctx.beginPath()
		ctx.roundedRect(point_.x , point_.y, params_.width, params_.height, radius, radius)
		ctx.stroke();
		ctx.fill();
		ctx.closePath();
	}

	function getPoint(params_){
		if(coordinate == undefined){
			coordinate = params_.point
			return coordinate
		}
		else {
			return coordinate
		}

	}

	function drawBorderPoints(ctx, layerId, index){
		let params_;
		params_ = source.getParams(layerId, index)
		let point = getPoint(params_);
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

