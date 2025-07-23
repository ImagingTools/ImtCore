import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0


BoundingBox {
	property real width: 0
	property real height: 0
	property real radius: 0

	function draw(ctx, transformMatrixArg){
		//console.log("drawBase")
		let params_;
		params_ = getParams()

		if(isSelected){
			ctx.globalAlpha = 0.7
		}

		ctx.fillStyle = params_.color !== undefined ? params_.color : "transparent";
		ctx.strokeStyle = params_.borderColor !== undefined ? params_.borderColor : "transparent";
		ctx.lineWidth = params_.lineWidth !== undefined ? params_.lineWidth : 0
		let radius_ = params_.radius !== undefined ? params_.radius : radius
		let width_ = params_.width !== undefined ? params_.width : width;
		let height_ = params_.height !== undefined ? params_.height : height;
		let point_ = points[0];
		let x1  = point_.x
		let y1  = point_.y
		let x2  = x1 + width_
		let y2  = y1 + height_

		let point1 = getScreenPosition(Qt.point(x1,y1))
		let point2 = getScreenPosition(Qt.point(x2,y2))
		x1 = point1.x;
		y1 =  point1.y ;
		x2 = point2.x;
		y2 = point2.y;

		width_ = x2 - x1
		height_ = y2 - y1

		ctx.beginPath()
		ctx.roundedRect(x1 , y1, width_, height_, radius_, radius_)
		ctx.stroke();
		ctx.fill();
		ctx.closePath();

		if(isSelected){
			drawSelection(ctx, transformMatrixArg, x1, y1, width_, height_, radius_)
		}
		if(showNodes){
			drawNodes(ctx, transformMatrixArg)
		}

	}

	function drawSelection(ctx, transformMatrixArg, xArg, yArg, widthArg, heightArg, radiusArg){
		//console.log("drawSelected")
		ctx.strokeStyle = DesignScheme.selectionColor
		ctx.lineWidth = DesignScheme.selectionLineWidth
		ctx.beginPath()
		ctx.roundedRect(xArg , yArg, widthArg, heightArg, radiusArg, radiusArg)
		ctx.stroke();
		ctx.closePath();
	}

	function drawNodes(ctx, transformMatrixArg){
	}

	function getBoundingBoxCoordinate(){
			return points[0]
	}

	function getBoundingBoxWidth(){
		return width;
	}

	function getBoundingBoxHeight(){
		return height;
	}

	function isInside(xArg, yArg){
		let topLeftPoint = getBoundingBoxCoordinate();

		let width_ = getBoundingBoxWidth()
		let height_ = getBoundingBoxHeight()

		let point = getLogPosition(Qt.point(xArg, yArg))
		xArg = point.x
		yArg = point.y

		return (
			xArg > topLeftPoint.x
			&& xArg < topLeftPoint.x + width_
			&& yArg > topLeftPoint.y
			&& yArg < topLeftPoint.y + height_
					)
	}

	function getBoundingBoxPoints(){

		if(!points.length){
			return ({});
		}

		let pointsObj = ({});

		let params_ = getParams();

		let width_ = params_.width !== undefined ? params_.width : width;
		let height_ = params_.height !== undefined ? params_.height : height;

		let point0 = points[0];

		pointsObj.topLeftPoint = Qt.point(point0.x, point0.y)
		pointsObj.topRightPoint = Qt.point(point0.x + width_, point0.y)
		pointsObj.bottomLeftPoint = Qt.point(point0.x, point0.y + height_)
		pointsObj.bottomRightPoint = Qt.point(point0.x + width_, point0.y + height_)

		return pointsObj;
	}
}

