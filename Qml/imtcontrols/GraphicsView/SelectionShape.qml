import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

BoundingBox {

	property QtObject source: null;

	function draw(ctx, layerMatrix){
		drawSelection(ctx);
		drawBorderPoints(ctx)
	}

	function drawSelection(ctx){
		let params_;
		params_ = source.getParams()

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


	function drawBorderPoints(ctx){
		let params_;
		params_ = source.getParams()
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

	// function getPoint(params_){
	// 	if(coordinate == undefined){
	// 		coordinate = {}
	// 		coordinate.x = params_.point.x
	// 		coordinate.y = params_.point.y
	// 		//coordinate = params_.point
	// 		return coordinate
	// 	}
	// 	else {
	// 		return coordinate
	// 	}

	// }
}

