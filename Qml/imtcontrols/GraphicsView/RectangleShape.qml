import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

BoundingBox {

	//Reimplemented functions
	function draw (ctx, layerMatrixArg){
		drawBase(ctx)
		if(isSelected){
			drawSelection(ctx, layerMatrixArg)
		}
		if(showNodes){
			drawNodes(ctx, layerMatrixArg)
		}
	}
	function drawBase(ctx, layerMatrixArg){
		//console.log("drawBase")
		let params_;
		params_ = getParams()

		if(isSelected){
			ctx.globalAlpha = 0.7
		}

		ctx.fillStyle = params_.color !== undefined ? params_.color : "transparent";
		ctx.strokeStyle = params_.borderColor !== undefined ? params_.borderColor : "transparent";
		ctx.lineWidth = params_.lineWidth !== undefined ? params_.lineWidth : 0
		let radius = params_.radius !== undefined ? params_.radius : 0
		let point_ = points[0];
		ctx.beginPath()
		ctx.roundedRect(point_.x , point_.y, params_.width, params_.height, radius, radius)
		ctx.stroke();
		ctx.fill();
		ctx.closePath();
	}

	function drawSelection(ctx, layerMatrixArg){
		//console.log("drawSelected")
		let params_;
		params_ = getParams()

		ctx.strokeStyle = "yellow";
		ctx.lineWidth = 4
		let radius = params_.radius !== undefined ? params_.radius : 0

		let point_ = points[0];

		ctx.beginPath()
		ctx.roundedRect(point_.x , point_.y, params_.width, params_.height, radius, radius)
		ctx.stroke();
		ctx.closePath();
	}

	function drawNodes(ctx, layerMatrixArg){
	}


	function getBoundingBoxCoordinate(){
			return getParams().point
	}


	function getBoundingBoxWidth(){
		return getParams().width;
	}

	function getBoundingBoxHeight(){
		return getParams().height;
	}

	function isInside(x_, y_, matrix){
		let topLeftPoint = getBoundingBoxCoordinate();

		let scaleCoeff = matrix.xScale()
		let deltaX = matrix.xTranslation()
		let deltaY = matrix.yTranslation()

		let width_ = getBoundingBoxWidth()
		let height_ = getBoundingBoxHeight()

		x_ = (x_ - deltaX) / scaleCoeff
		y_ = (y_ - deltaY) / scaleCoeff

		return (
			x_ > topLeftPoint.x
			&& x_ < topLeftPoint.x + width_
			&& y_ > topLeftPoint.y
			&& y_ < topLeftPoint.y + height_
					)
	}
}

