import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

BoundingBox {
	id: imageShape;

	property real width: 0;
	property real height: 0;

	property string imageSource: "";

	/*!
	 * Optional reference to a CQmlImageShape C++ object.
	 * When set, the image is loaded from the IBitmap-backed image provider.
	 */
	property var bitmapShape: null;

	onBitmapShapeChanged: {
		if(bitmapShape && bitmapShape.imageUrl){
			imageShape.imageSource = bitmapShape.imageUrl;
			if(viewItem){
				viewItem.loadImage(imageShape.imageSource);
				viewItem.requestPaint();
			}
		}
	}

	Connections {
		target: bitmapShape
		enabled: bitmapShape !== null
		function onImageChanged(){
			if(bitmapShape.imageUrl){
				imageShape.imageSource = bitmapShape.imageUrl;
				if(viewItem){
					viewItem.loadImage(imageShape.imageSource);
					viewItem.requestPaint();
				}
			}
		}
	}

	function draw(ctx, transformMatrixArg) {
		let params_ = getParams();
		if(params_.source !== undefined){
			imageShape.imageSource = params_.source
		}
		let x1 = params_.point !== undefined ? params_.point.x : points[0].x;
		let y1 = params_.point !== undefined ? params_.point.y : points[0].y;
		let width_ = params_.width !== undefined ? params_.width : width;
		let height_ = params_.height !== undefined ? params_.height : height;
		let x2 = x1 + width_;
		let y2 = y1 + height_;

		let point1 = getScreenPosition(Qt.point(x1,y1))
		let point2 = getScreenPosition(Qt.point(x2,y2))
		x1 = point1.x;
		y1 =  point1.y ;
		x2 = point2.x;
		y2 = point2.y;

		width_ = x2 - x1
		height_ = y2 - y1

		if(width_ > 0 && height_ > 0){
			ctx.drawImage(imageShape.imageSource, x1,y1, width_, height_)
		}
		else {
			ctx.drawImage(imageShape.imageSource, x1,y1)
		}
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

