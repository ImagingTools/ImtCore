import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

GraphicsShapeBase {

	property string imageSource: "";

	function drawBase (ctx, layerId) {
		let params_ = getParams();
		if(params_.source !== undefined){
			imageSource = params_.source
		}
		let x_ = params_.point !== undefined ? params_.point.x : 0;
		let y_ = params_.point !== undefined ? params_.point.y : 0;
		let width_ = params_.width !== undefined ? params_.width : -1;
		let height_ = params_.height !== undefined ? params_.height : -1;
		if(width_ > 0 && height_ > 0){
			ctx.drawImage(imageSource, x_,y_, width_, height_)
		}
		else {
			ctx.drawImage(imageSource, x_,y_)
		}
	}
}

