import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0


BoundingBox {
	id: sector;

	property var center: Qt.point(0,0);

	property real startAngle: 0;//degrees
	property real endAngle: 30;//degrees

	property string color: Style.borderColor;
	property string borderColor: color
	property int borderWidth: 1;
	property real opacity: 1;
	property real precision: 0.0001

	readonly property bool anticlockwise: true;

	function isInside(xArg, yArg){
		let margin_ = Style.marginM
		let point = getLogPosition(Qt.point(xArg, yArg))
		let dist = AnalyticGeometry.distanceBetweenPoints2d(point, center)
		let angle = getAngle(point.x, point.y, center)

		let radiusOk = isInsideRadius(dist)

		let angleOk = isInsideAngles(angle)

		return radiusOk && angleOk;
	}

	function isInsideRadius(dist){//to override
		return false;
	}

	function isInsideAngles(angle){
		if(Math.abs(startAngle - endAngle) < precision){
			return true;
		}

		let startAngle_ = startAngle >= 0 ? startAngle : startAngle + 360
		let endAngle_ = endAngle >= 0 ? endAngle : endAngle + 360

		let angleOk = angle >= Math.min(startAngle_, endAngle_) && angle <= Math.max(startAngle_, endAngle_);
		if(endAngle_  > startAngle_){
			angleOk = !angleOk
		}
		return angleOk;
	}

	function getAngle(xArg, yArg, center){
		let x0 = center.x
		let y0 = center.y
		let x_ = (xArg- x0)
		let y_ = (yArg - y0)
		let r = Math.sqrt(x_*x_+y_*y_)
		let angle = 0

		if(x_ >= 0 && y_ <= 0){
			angle = Math.asin(Math.abs(x_)/r)*180/Math.PI - 90 + 360
		}
		else if(x_ > 0 && y_ > 0){
			angle = Math.acos(Math.abs(x_)/r)*180/Math.PI
		}
		else if(x_ <= 0 && y_ > 0){
			angle = Math.asin(Math.abs(x_)/r)*180/Math.PI + 90
		}
		else if(x_ <= 0 && y_ <= 0){
			angle = Math.acos(Math.abs(x_)/r)*180/Math.PI - 180 + 360
		}

		return angle;
	}

	function setCoordinateShift(deltaX, deltaY){
		let centerNew = Qt.point(0,0);
		let centerScreen = getScreenPosition(center)
		centerNew.x = centerScreen.x + deltaX;
		centerNew.y = centerScreen.y + deltaY;
		center = getLogPosition(centerNew)
	}

}

