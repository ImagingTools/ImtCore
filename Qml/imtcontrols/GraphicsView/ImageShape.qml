import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

BoundingBox {

	property real width: 0;
	property real height: 0;

	property string imageSource: "";

	function draw(ctx, transformMatrixArg) {
		let params_ = getParams();
		if(params_.source !== undefined){
			imageSource = params_.source
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
			ctx.drawImage(imageSource, x1,y1, width_, height_)
		}
		else {
			ctx.drawImage(imageSource, x1,y1)
		}
	}
}

