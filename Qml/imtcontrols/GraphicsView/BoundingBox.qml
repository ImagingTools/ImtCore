import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0


GraphicsShapeBase {

	id: root;

	property bool editMode: false;

	function drawBoundingBox(ctx){
		ctx.globalAlpha = 1
		let cornerPointsObj = getBoundingBoxCornerPoints(true);
		if(!Object.keys(cornerPointsObj).length){
			return;
		}
		drawBoundingBoxBorder(ctx, cornerPointsObj);
		drawBoundingBoxRotationControl(ctx, cornerPointsObj);
		drawBoundingBoxControlPoints(ctx, cornerPointsObj);
	}

	function drawBoundingBoxBorder(ctx, cornerPointsObj){
		DesignScheme.drawBoundingBoxBorder(ctx, cornerPointsObj)
	}

	function drawBoundingBoxControlPoints(ctx, cornerPointsObj){
		let midPoints  = getBoundingBoxMidPoints(true, cornerPointsObj)

		//corner points
		DesignScheme.drawBoundingBoxControlPoint(ctx, cornerPointsObj.topLeftPoint);
		DesignScheme.drawBoundingBoxControlPoint(ctx, cornerPointsObj.topRightPoint);
		DesignScheme.drawBoundingBoxControlPoint(ctx, cornerPointsObj.bottomRightPoint);
		DesignScheme.drawBoundingBoxControlPoint(ctx, cornerPointsObj.bottomLeftPoint);

		//midPoints
		DesignScheme.drawBoundingBoxControlPoint(ctx, midPoints.leftPoint);
		DesignScheme.drawBoundingBoxControlPoint(ctx, midPoints.rightPoint);
		DesignScheme.drawBoundingBoxControlPoint(ctx, midPoints.topPoint);
		DesignScheme.drawBoundingBoxControlPoint(ctx, midPoints.bottomPoint);

	}

	function drawBoundingBoxRotationControl(ctx, cornerPointsObj){
		DesignScheme.drawBoundingBoxRotationControl(ctx, cornerPointsObj);
	}

	function getBoundingBoxCenter(){
		let cornerPointsObj = getBoundingBoxCornerPoints();

		return Qt.point((cornerPointsObj.topLeftPoint.x + cornerPointsObj.topRightPoint.x)/2,0, (cornerPointsObj.topRightPoint.y + cornerPointsObj.bottomRightPoint.y)/2);
	}

	function getBoundingBoxPoints(){
		if(!points.length){
			return ({});
		}
		let pointsObj = ({});

		let minX = 1000000;
		let minY = 1000000;
		let maxX  = -1000000
		let maxY = -1000000;

		if(!points.length){
			minX = 0;
			minY = 0;
			maxX = 0;
			maxY = 0;
		}

		for(let i = 0; i < points.length; i++){
			let point = points[i]

			let x_ = point.x
			let y_ = point.y
			if(x_ < minX){
				minX = x_
			}
			if(y_ < minY){
				minY = y_
			}
			if(x_ > maxX){
				maxX = x_
			}
			if(y_ > maxY){
				maxY = y_
			}
		}

		pointsObj.topLeftPoint = Qt.point(minX, minY)
		pointsObj.topRightPoint = Qt.point(maxX, minY)
		pointsObj.bottomLeftPoint = Qt.point(minX, maxY)
		pointsObj.bottomRightPoint = Qt.point(maxX, maxY)

		return pointsObj;
	}

	function getBoundingBoxCornerPoints(isScreenPosition){
		if(isScreenPosition){
			return boundingBoxPointsToScreen(getBoundingBoxPoints())
		}
		return getBoundingBoxPoints();
	}

	function getBoundingBoxMidPoints(isScreenPosition, cornerPointsObj){

		let midpoints = ({})
		if(cornerPointsObj == undefined){
			cornerPointsObj = getBoundingBoxCornerPoints(isScreenPosition);
		}
		if(!Object.keys(cornerPointsObj).length){
			return ({});
		}

		midpoints.topPoint = Qt.point((cornerPointsObj.topLeftPoint.x + cornerPointsObj.topRightPoint.x)/2, cornerPointsObj.topLeftPoint.y)
		midpoints.rightPoint = Qt.point(cornerPointsObj.topRightPoint.x, (cornerPointsObj.topRightPoint.y + cornerPointsObj.bottomRightPoint.y)/2)
		midpoints.bottomPoint = Qt.point((cornerPointsObj.bottomRightPoint.x + cornerPointsObj.bottomLeftPoint.x)/2, cornerPointsObj.bottomRightPoint.y)
		midpoints.leftPoint = Qt.point(cornerPointsObj.bottomLeftPoint.x, (cornerPointsObj.topLeftPoint.y + cornerPointsObj.bottomLeftPoint.y)/2)

		return midpoints;
	}

	function getBoundingBoxRotation(){
		return 0;
	}

	function getBoundingBoxCoordinate(){
		if(!points.length){
			return null;
		}
		let point = getBoundingBoxPoints()
		return point.topLeftPoint
	}

	function getBoundingBoxWidth(){
		if(!points.length){
			return 0;
		}

		let pointsBB = getBoundingBoxPoints()
		return (pointsBB.topRightPoint.x - pointsBB.topLeftPoint.x)
	}

	function getBoundingBoxHeight(){
		if(!points.length){
			return 0;
		}

		let pointsBB = getBoundingBoxPoints()
		return (pointsBB.bottomLeftPoint.y - pointsBB.topLeftPoint.y)
	}

	function boundingBoxPointsToScreen(pointsObjLog){
		if(!Object.keys(pointsObjLog).length){
			return ({});
		}

		let pointsObj = ({});

		let minX = 1000000;
		let minY = 1000000;
		let maxX  = -1000000
		let maxY = -1000000;

		let points = []

		points.push(Qt.point(pointsObjLog.topLeftPoint.x, pointsObjLog.topLeftPoint.y))
		points.push(Qt.point(pointsObjLog.topRightPoint.x, pointsObjLog.topRightPoint.y))
		points.push(Qt.point(pointsObjLog.bottomLeftPoint.x, pointsObjLog.bottomLeftPoint.y))
		points.push(Qt.point(pointsObjLog.bottomRightPoint.x, pointsObjLog.bottomRightPoint.y))

		for(let i = 0; i < points.length; i++){
			let point = points[i]
			point = getScreenPosition(point);

			let x_ = point.x
			let y_ = point.y
			if(x_ < minX){
				minX = x_
			}
			if(y_ < minY){
				minY = y_
			}
			if(x_ > maxX){
				maxX = x_
			}
			if(y_ > maxY){
				maxY = y_
			}
		}

		pointsObj.topLeftPoint = Qt.point(minX, minY)
		pointsObj.topRightPoint = Qt.point(maxX, minY)
		pointsObj.bottomLeftPoint = Qt.point(minX, maxY)
		pointsObj.bottomRightPoint = Qt.point(maxX, maxY)

		return pointsObj;
	}

	function editFunction(xArg, yArg, deltaXArg, deltaYArg){
		if(!viewItem.editMode){
			return;
		}

		if(isInsideRotationArea(xArg, yArg)){//rotation
			let angle = 0//TODO
			rotate(angle);
		}
		else {//Bounding box edges moving
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

	function isInside(xArg, yArg){
		return false;
	}

	function isInsideBoundingBox(xArg, yArg){
		let point = getLogPosition(Qt.point(xArg, yArg))
		let cornerPointsObj = getBoundingBoxCornerPoints();
		if(!Object.keys(cornerPointsObj).length){
			return false;
		}

		xArg = point.x
		yArg = point.y

		return (
			xArg > cornerPointsObj.topLeftPoint.x
			&& xArg < cornerPointsObj.topRightPoint.x
			&& yArg > cornerPointsObj.topLeftPoint.y
			&& yArg < cornerPointsObj.bottomRightPoint.y
					)
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

	onMousePositionChanged: {
		if(viewItem.isEditMode){

			if(mouseIsPressed){


			}
		}

	}

}

