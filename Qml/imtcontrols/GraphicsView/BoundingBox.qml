import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

QtObject {

	id: root;

	property bool editMode: false;

	function drawBoundingBox(ctx){
		drawBoundingBoxBorder(ctx);
		drawBoundingBoxControlPoints(ctx);
		drawBoundingBoxRotationControl(ctx);
	}

	function drawBoundingBoxBorder(ctx){
		let cornerPointList = getBoundingBoxCornerPoints();

		ctx.strokeStyle = "black";
		ctx.lineWidth = 2

		ctx.beginPath()
		ctx.moveTo(cornerPointList.topLeftPoint.x, cornerPointList.topLeftPoint.y)

		ctx.lineTo(cornerPointList.topRightPoint.x, cornerPointList.topRightPoint.y)
		ctx.lineTo(cornerPointList.bottomRightPoint.x, cornerPointList.bottomRightPoint.y)
		ctx.lineTo(cornerPointList.bottomLeftPoint.x, cornerPointList.bottomLeftPoint.y)
		ctx.lineTo(cornerPointList.topLeftPoint.x, cornerPointList.topLeftPoint.y)

		ctx.stroke();
		ctx.closePath();
	}

	function drawBoundingBoxControlPoints(ctx){
		let cornerPoints = getBoundingBoxCornerPoints()
		let midPoints  = getBoundingBoxMidPoints()

		ctx.strokeStyle = "black";
		ctx.lineWidth = 2
		let radius = 6;

		ctx.beginPath()

		//corner points
		ctx.moveTo(cornerPoints.topLeftPoint.x, cornerPoints.topLeftPoint.y)
		ctx.arc(cornerPoints.topLeftPoint.x, cornerPoints.topLeftPoint.y, radius, 0, 2*Math.PI, true);

		ctx.moveTo(cornerPoints.topRightPoint.x, cornerPoints.topRightPoint.y)
		ctx.arc(cornerPoints.topRightPoint.x, cornerPoints.topRightPoint.y, radius, 0, 2*Math.PI, true);

		ctx.moveTo(cornerPoints.bottomRightPoint.x, cornerPoints.bottomRightPoint.y)
		ctx.arc(cornerPoints.bottomRightPoint.x, cornerPoints.bottomRightPoint.y, radius, 0, 2*Math.PI, true);

		ctx.moveTo(cornerPoints.bottomLeftPoint.x, cornerPoints.bottomLeftPoint.y)
		ctx.arc(cornerPoints.bottomLeftPoint.x, cornerPoints.bottomLeftPoint.y, radius, 0, 2*Math.PI, true);

		//midPoints
		ctx.moveTo(midPoints.leftPoint.x, midPoints.leftPoint.y)
		ctx.arc(midPoints.leftPoint.x, midPoints.leftPoint.y, radius, 0, 2*Math.PI, true);

		ctx.moveTo(midPoints.rightPoint.x, midPoints.rightPoint.y)
		ctx.arc(midPoints.rightPoint.x, midPoints.rightPoint.y, radius, 0, 2*Math.PI, true);

		ctx.moveTo(midPoints.topPoint.x, midPoints.topPoint.y)
		ctx.arc(midPoints.topPoint.x, midPoints.topPoint.y, radius, 0, 2*Math.PI, true);

		ctx.moveTo(midPoints.bottomPoint.x, midPoints.bottomPoint.y)
		ctx.arc(midPoints.bottomPoint.x, midPoints.bottomPoint.y, radius, 0, 2*Math.PI, true);

		ctx.stroke();
		ctx.closePath();
	}

	function drawBoundingBoxRotationControl(ctx){
		let cornerPoints = getBoundingBoxCornerPoints();

		ctx.strokeStyle = "black";
		ctx.lineWidth = 2

		let bottomLeftX = cornerPoints.topLeftPoint.x + (cornerPoints.topRightPoint.x - cornerPoints.topLeftPoint.x)/3
		let bottomLeftY = cornerPoints.topLeftPoint.y + 2 *(cornerPoints.bottomRightPoint.y - cornerPoints.topRightPoint.y)/3
		let topLeftX = cornerPoints.topLeftPoint.x + (cornerPoints.topRightPoint.x - cornerPoints.topLeftPoint.x)/3
		let topLeftY = cornerPoints.topLeftPoint.y + (cornerPoints.bottomRightPoint.y - cornerPoints.topRightPoint.y)/3
		let topRightX = cornerPoints.topLeftPoint.x + 2 * (cornerPoints.topRightPoint.x - cornerPoints.topLeftPoint.x)/3
		let topRightY = cornerPoints.topLeftPoint.y + (cornerPoints.bottomRightPoint.y - cornerPoints.topRightPoint.y)/3

		ctx.beginPath()
		ctx.moveTo(bottomLeftX, bottomLeftY);
		ctx.lineTo(topLeftX, topLeftY);
		ctx.lineTo(topRightX, topRightY);

		ctx.stroke();
		ctx.closePath();
	}

	function getBoundingBoxCenter(){
		let cornerPointList = getBoundingBoxCornerPoints();

		return Qt.point((cornerPointList.topLeftPoint.x + cornerPointList.topRightPoint.x)/2,0, (cornerPointList.topRightPoint.y + cornerPointList.bottomRightPoint.y)/2);
	}

	function getBoundingBoxPoints(){
		return [];
	}

	function getBoundingBoxCornerPoints(){
		let points = {};

		points.topLeftPoint = Qt.point(0,0)
		points.topRightPoint = Qt.point(0, 0)
		points.bottomRightPoint = Qt.point(0,0)
		points.bottomLeftPoint = Qt.point(0, 0)

		return points;
	}

	function getBoundingBoxMidPoints(){
		let midpoints = []
		let cornerPoints = getBoundingBoxCornerPoints();

		midpoints.topPoint = Qt.point((cornerPoints.topLeftPoint.x + cornerPoints.topRightPoint.x)/2, cornerPoints.topLeftPoint.y)
		midpoints.rightPoint = Qt.point(cornerPoints.topRightPoint.x, (cornerPoints.topRightPoint.y + cornerPoints.bottomRightPoint.y)/2)
		midpoints.bottomPoint = Qt.point((cornerPoints.bottomRightPoint.x + cornerPoints.bottomLeftPoint.x)/2, cornerPoints.bottomRightPoint.y)
		midpoints.leftPoint = Qt.point(cornerPoints.bottomLeftPoint.x, (cornerPoints.topLeftPoint.y + cornerPoints.bottomLeftPoint.y)/2)

		return midpoints;
	}

	function getBoundingBoxRotation(){
		return 0;
	}

	function getBoundingBoxCoordinate(){
		return Qt.point(0,0);
	}

	function getBoundingBoxWidth(){
		return 0;
	}

	function getBoundingBoxHeight(){
		return 0;
	}

	function draw(ctx, layerMatrix){
		drawBoundingBox(ctx)
	}

	function editFunction(xArg, yArg, deltaXArg, deltaYArg, matrix){
		if(!editMode){
			return;
		}

		let scaleCoeff = matrix.xScale()
		let shiftX = matrix.xTranslation()
		let shiftY = matrix.yTranslation()

		xArg = (xArg - shiftX) / scaleCoeff
		yArg = (yArg - shiftY) / scaleCoeff

		if(isInsideRotationArea(xArg, yArg)){//rotation
			let angle = 0//TODO
			rotate(angle);
		}
		else {//control points moving
			let cornerPoints = getBoundingBoxCornerPoints();
			let midPoints  = getBoundingBoxMidPoints();

			let margin = 10;

			if(xArg >= cornerPoints.topLeftPoint.x - margin && xArg <= cornerPoints.topLeftPoint.x + margin
					&& yxArg >= cornerPoints.topLeftPoint.y - margin && yArg <= cornerPoints.topLeftPoint.y + margin){

				topLeftMoving(deltaXArg, deltaYArg);

			}
			else if(xArg >= cornerPoints.topRightPoint.x - margin && xArg <= cornerPoints.topRightPoint.x + margin
					  && yxArg >= cornerPoints.topRightPoint.y - margin && yArg <= cornerPoints.topRightPoint.y + margin){

				topRightMoving(deltaXArg, deltaYArg);

			}
			else if(xArg >= cornerPoints.bottomLeftPoint.x - margin && xArg <= cornerPoints.bottomLeftPoint.x + margin
					  && yxArg >= cornerPoints.bottomLeftPoint.y - margin && yArg <= cornerPoints.bottomLeftPoint.y + margin){

				bottomLeftMoving(deltaXArg, deltaYArg);

			}
			else if(xArg >= cornerPoints.bottomRightPoint.x - margin && xArg <= cornerPoints.bottomRightPoint.x + margin
					&& yxArg >= cornerPoints.bottomRightPoint.y - margin && yArg <= cornerPoints.bottomRightPoint.y + margin){

					bottomRightMoving(deltaXArg, deltaYArg);
			}
			else if(xArg >= midPoints.leftPoint.x - margin && xArg <= midPoints.leftPoint.x + margin
					&& yxArg >= midPoints.leftPoint.y - margin && yArg <= midPoints.leftPoint.y + margin){

				leftMoving(deltaXArg, deltaYArg);
			}
			else if(xArg >= midPoints.rightPoint.x - margin && xArg <= midPoints.rightPoint.x + margin
					&& yxArg >= midPoints.rightPoint.y - margin && yArg <= midPoints.rightPoint.y + margin){

				rightMoving(deltaXArg, deltaYArg);
			}
			else if(xArg >= midPoints.topPoint.x - margin && xArg <= midPoints.topPoint.x + margin
					&& yxArg >= midPoints.topPoint.y - margin && yArg <= midPoints.topPoint.y + margin){

				topMoving(deltaXArg, deltaYArg);
			}
			else if(xArg >= midPoints.bottomPoint.x - margin && xArg <= midPoints.bottomPoint.x + margin
					&& yxArg >= midPoints.bottomPoint.y - margin && yArg <= midPoints.bottomPoint.y + margin){

				bottomMoving(deltaXArg, deltaYArg);
			}
		}
	}

	function isInsideRotationArea(xArg, yArg){
		let ok = false;

		let cornerPoints = getBoundingBoxCornerPoints();

		let topLeftX = cornerPoints.topLeftPoint.x + (cornerPoints.topRightPoint.x - cornerPoints.topLeftPoint.x)/3
		let topLeftY = cornerPoints.topLeftPoint.y + (cornerPoints.bottomRightPoint.y - cornerPoints.topRightPoint.y)/3

		let bottomRightX = cornerPoints.topLeftPoint.x + 2 * (cornerPoints.topRightPoint.x - cornerPoints.topLeftPoint.x)/3
		let bottomRightY = cornerPoints.topLeftPoint.y + 2 *(cornerPoints.bottomRightPoint.y - cornerPoints.topRightPoint.y)/3

		ok = xArg >= topLeftX && xArg <= bottomRightX && yArg >= topLeftY && yArg <= bottomRightY

		return ok;
	}

	function rotate(angle){
	}

	function leftMoving(deltaX, deltaY){
	}

	function rightMoving(deltaX, deltaY){
	}

	function topMoving(deltaX, deltaY){
	}

	function bottomMoving(deltaX, deltaY){
	}

	function topLeftMoving(deltaX, deltaY){
	}

	function bottomLeftMoving(deltaX, deltaY){
	}

	function topRightMoving(deltaX, deltaY){
	}

	function bottomRightMoving(deltaX, deltaY){
	}


}

