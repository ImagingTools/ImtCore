import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0


GraphicsShapeBase {

	id: root;

	property bool editMode: false;
	property string controlPointColor: DesignScheme.boundingBoxBorderColor


	function drawBoundingBox(ctx){
		ctx.globalAlpha = 1
		let cornerPointsObj = getBoundingBoxCornerPoints(true);
		if(!Object.keys(cornerPointsObj).length){
			return;
		}
		if(!_isInsideRotationArea){
			drawBoundingBoxBorder(ctx, cornerPointsObj);
			drawBoundingBoxRotationControl(ctx, cornerPointsObj);
			drawBoundingBoxControlPoints(ctx, cornerPointsObj);
		}
		else {//rotation
			let transformMatrix = LinearAlgebra.multiplyByMatrix3x3(viewItem.viewMatrix.matrix, layer.layerMatrix.matrix)
			transformMatrix = LinearAlgebra.multiplyByMatrix3x3(transformMatrix, shapeMatrixBackup.matrix)
			let rotationMatrix = LinearAlgebra.multiplyByMatrix3x3(transformMatrix, rotationTempMatrix.matrix)

			drawBoundingBoxBorder(ctx, bbPointsBackupScreen, rotationTempMatrixScreen.matrix);
			drawBoundingBoxRotationControl(ctx, bbPointsBackupScreen, rotationTempMatrixScreen.matrix);
			drawBoundingBoxControlPoints(ctx, bbPointsBackupScreen, rotationTempMatrixScreen.matrix);
		}
	}

	function drawBoundingBoxBorder(ctx, cornerPointsObj, transformMatrix){
		DesignScheme.drawBoundingBoxBorder(ctx, cornerPointsObj, transformMatrix)
	}

	function drawBoundingBoxControlPoints(ctx, cornerPointsObj, transformMatrix){
		let midPoints;
		if(!_isInsideRotationArea){
			midPoints  = getBoundingBoxMidPoints(true, cornerPointsObj)
		}
		else {
			midPoints  = getBoundingBoxMidPoints(true, bbPointsBackupScreen)
		}

		//corner points
		DesignScheme.drawBoundingBoxControlPoint(ctx, cornerPointsObj.topLeftPoint, transformMatrix);
		DesignScheme.drawBoundingBoxControlPoint(ctx, cornerPointsObj.topRightPoint, transformMatrix);
		DesignScheme.drawBoundingBoxControlPoint(ctx, cornerPointsObj.bottomRightPoint, transformMatrix);
		DesignScheme.drawBoundingBoxControlPoint(ctx, cornerPointsObj.bottomLeftPoint, transformMatrix);

		//midPoints
		DesignScheme.drawBoundingBoxControlPoint(ctx, midPoints.leftPoint , transformMatrix);
		DesignScheme.drawBoundingBoxControlPoint(ctx, midPoints.rightPoint , transformMatrix);
		DesignScheme.drawBoundingBoxControlPoint(ctx, midPoints.topPoint, transformMatrix);
		DesignScheme.drawBoundingBoxControlPoint(ctx, midPoints.bottomPoint , transformMatrix);

	}

	function drawBoundingBoxRotationControl(ctx, cornerPointsObj, transformMatrix){
		DesignScheme.drawBoundingBoxRotationControl(ctx, cornerPointsObj, transformMatrix);
	}

	function getBoundingBoxCenter(cornerPointsObj){
		if(cornerPointsObj == undefined){
			cornerPointsObj = getBoundingBoxCornerPoints();
		}

		return Qt.point((cornerPointsObj.topLeftPoint.x + cornerPointsObj.topRightPoint.x)/2, (cornerPointsObj.topRightPoint.y + cornerPointsObj.bottomRightPoint.y)/2);
	}

	function getLimitsObject(pointsArg){
		let pointCount = 0
		if(pointsArg.length){
			pointCount = pointsArg.length
		}
		let pointsObj = ({});

		let minX = 0;
		let minY = 0;
		let maxX = 0
		let maxY = 0;

		for(let i = 0; i < pointCount; i++){
			let point = pointsArg[i]
			if (i == 0){
				minX = maxX = point.x
				minY = maxY = point.y

				continue
			}

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


	function getBoundingBoxPoints(){
		return getLimitsObject(points)
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
		let pointsBB = getBoundingBoxPoints()
		return pointsBB.topLeftPoint
	}

	function getBoundingBoxWidth(pointsBB){
		if(!points.length){
			return 0;
		}

		if(pointsBB == undefined){
			pointsBB = getBoundingBoxPoints()
		}
		return (pointsBB.topRightPoint.x - pointsBB.topLeftPoint.x)
	}

	function getBoundingBoxHeight(pointsBB){
		if(!points.length){
			return 0;
		}

		if(pointsBB == undefined){
			pointsBB = getBoundingBoxPoints()
		}
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

	function boundingBoxPointsTransform(pointsObjLog, matrixArg){
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
			point = LinearAlgebra.transformPoint2d(point, matrixArg);

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

	function editFunction(position){
		//console.log("editFunction:::::")

		if(!viewItem.isEditMode || !isSelected){
			return;
		}

		let cornerPoints = bbPointsBackupScreen;

		let deltaXArg = position.x - mousePressedCoord.x
		let deltaYArg = position.y - mousePressedCoord.y

		let xArg = mousePressedCoord.x
		let yArg = mousePressedCoord.y

		if(_isInsideRotationArea){//rotation
			//console.log("ROTATE")
			let center = getBoundingBoxCenter(cornerPoints)
			let startPoint = Qt.point(mousePressedCoord.x, mousePressedCoord.y)
			let newPoint = Qt.point(position.x, position.y)
			let angle0 = getAngle(startPoint.x, startPoint.y , center)
			let angle  = getAngle(newPoint.x, newPoint.y, center)

			let angleRad = (Math.PI / 180) * (angle - angle0);

			let matrixLayer = LinearAlgebra.multiplyByMatrix3x3(viewItem.viewMatrix.matrix, layer.layerMatrix.matrix)
			let matrixShape = LinearAlgebra.multiplyByMatrix3x3(matrixLayer, shapeMatrixBackup.matrix)
			let matrixShapeInv = LinearAlgebra.getInvertedMatrix3x3(matrixShape)
			let logPositionCenter = LinearAlgebra.transformPoint2d(center, matrixShapeInv)
			let startPointLog = LinearAlgebra.transformPoint2d(startPoint, matrixShapeInv)
			let newPointLog = LinearAlgebra.transformPoint2d(newPoint, matrixShapeInv)
			let angle0Log = getAngle(startPointLog.x, startPointLog.y , logPositionCenter)
			let angleLog  = getAngle(newPointLog.x, newPointLog.y, logPositionCenter)
			let angleRadLog = (Math.PI / 180) * (angleLog - angle0Log);

			rotationTempMatrix.matrix = rotationTempMatrix.getRotationMatrix(identityMatrix.matrix, angleRadLog, logPositionCenter)
			rotationTempMatrixScreen.matrix = rotationTempMatrixScreen.getRotationMatrix(identityMatrix.matrix, angleRad, center)
			shapeMatrix.matrix = LinearAlgebra.multiplyByMatrix3x3(shapeMatrixBackup.matrix, rotationTempMatrix.matrix)
		}
		else {//Bounding box edges moving
			let margin = 10;

			let midPoints  = getBoundingBoxMidPoints(false, cornerPoints);

			let bb_width = bbWidthBackupScreen
			let bb_height = bbHeightBackupScreen

			let scale_xLeft = (bb_width - deltaXArg) / bb_width
			let scale_xRight = (bb_width + deltaXArg) / bb_width
			let scale_yTop = (bb_height - deltaYArg) / bb_height
			let scale_yBottom = (bb_height + deltaYArg) / bb_height
			let marginXLeft = bb_width - deltaXArg
			let marginXRight = bb_width + deltaXArg
			let marginYTop = bb_height - deltaYArg
			let marginYBottom = bb_height + deltaYArg

			if(xArg >= cornerPoints.topLeftPoint.x - margin && xArg <= cornerPoints.topLeftPoint.x + margin
					&& yArg >= cornerPoints.topLeftPoint.y - margin && yArg <= cornerPoints.topLeftPoint.y + margin){

				//console.log("topLeftMoving::::::::::::::")
				topLeftMoving(scale_xLeft, scale_yTop, deltaXArg, deltaYArg, marginXLeft, marginYTop);
			}
			else if(xArg >= cornerPoints.topRightPoint.x - margin && xArg <= cornerPoints.topRightPoint.x + margin
					&& yArg >= cornerPoints.topRightPoint.y - margin && yArg <= cornerPoints.topRightPoint.y + margin){

				//console.log("topRightMoving::::::::::::::")
				topRightMoving(scale_xRight, scale_yTop, deltaXArg, deltaYArg, marginXRight, marginYTop);

			}
			else if(xArg >= cornerPoints.bottomLeftPoint.x - margin && xArg <= cornerPoints.bottomLeftPoint.x + margin
					&& yArg >= cornerPoints.bottomLeftPoint.y - margin && yArg <= cornerPoints.bottomLeftPoint.y + margin){

				//console.log("bottomLeftMoving::::::::::::::")
				bottomLeftMoving(scale_xLeft, scale_yBottom, deltaXArg, deltaYArg, marginXLeft, marginYBottom);
			}
			else if(xArg >= cornerPoints.bottomRightPoint.x - margin && xArg <= cornerPoints.bottomRightPoint.x + margin
					&& yArg >= cornerPoints.bottomRightPoint.y - margin && yArg <= cornerPoints.bottomRightPoint.y + margin){

				//console.log("bottomRightMoving::::::::::::::")
				bottomRightMoving(scale_xRight, scale_yBottom, deltaXArg, deltaYArg, marginXRight, marginYBottom);
			}
			else if(xArg >= cornerPoints.topLeftPoint.x - margin && xArg <= cornerPoints.topLeftPoint.x + margin
					&& yArg >= cornerPoints.topLeftPoint.y - margin && yArg <= cornerPoints.bottomLeftPoint.y + margin){

				//console.log("leftMoving::::::::::::::")
				leftMoving(scale_xLeft, deltaXArg, deltaYArg, marginXLeft)
			}
			else if(xArg >= cornerPoints.topRightPoint.x - margin && xArg <= cornerPoints.topRightPoint.x + margin
					&& yArg >= cornerPoints.topRightPoint.y - margin && yArg <= cornerPoints.bottomRightPoint.y + margin){

				//console.log("rightMoving::::::::::::::")
				rightMoving(scale_xRight, deltaXArg, deltaYArg, marginXRight)
			}
			else if(xArg >= cornerPoints.topLeftPoint.x - margin && xArg <= cornerPoints.topRightPoint.x + margin
					&& yArg >= cornerPoints.topRightPoint.y - margin && yArg <= cornerPoints.topRightPoint.y + margin){

				//console.log("topMoving::::::::::::::")
				topMoving(scale_yTop, deltaXArg, deltaYArg, marginYTop)
			}
			else if(xArg >= cornerPoints.bottomLeftPoint.x - margin && xArg <= cornerPoints.bottomRightPoint.x + margin
					&& yArg >= cornerPoints.bottomRightPoint.y - margin && yArg <= cornerPoints.bottomRightPoint.y + margin){

				//console.log("bottomMoving::::::::::::::")
				bottomMoving(scale_yBottom, deltaXArg, deltaYArg, marginYBottom)
			}
			else {
				//console.log("Moving ALL::::::::::::::")
				transformPoints(1,1,deltaXArg, deltaYArg)
			}

		}
	}

	function isInsideRotationArea(xArg, yArg, cornerPoints){
		let ok = false;

		if(cornerPoints == undefined){
			cornerPoints = getBoundingBoxCornerPoints();
		}

		let topLeftX = cornerPoints.topLeftPoint.x + (cornerPoints.topRightPoint.x - cornerPoints.topLeftPoint.x)/3
		let topLeftY = cornerPoints.topLeftPoint.y + (cornerPoints.bottomRightPoint.y - cornerPoints.topRightPoint.y)/3

		let bottomRightX = cornerPoints.topLeftPoint.x + 2 * (cornerPoints.topRightPoint.x - cornerPoints.topLeftPoint.x)/3
		let bottomRightY = cornerPoints.topLeftPoint.y + 2 *(cornerPoints.bottomRightPoint.y - cornerPoints.topRightPoint.y)/3

		ok = xArg >= topLeftX && xArg <= bottomRightX && yArg >= topLeftY && yArg <= bottomRightY

		//console.log("isInsideRotationArea", ok)
		return ok;
	}

	function isInside(xArg, yArg){
		return false;
	}

	function isInsideBoundingBox(xArg, yArg){
		let margin = 10
		let point = getLogPosition(Qt.point(xArg, yArg))
		let cornerPointsObj = getBoundingBoxCornerPoints();
		if(!Object.keys(cornerPointsObj).length){
			return false;
		}

		xArg = point.x
		yArg = point.y

		return (
			xArg > cornerPointsObj.topLeftPoint.x - margin
			&& xArg < cornerPointsObj.topRightPoint.x + margin
			&& yArg > cornerPointsObj.topLeftPoint.y - margin
			&& yArg < cornerPointsObj.bottomRightPoint.y + margin
					)
	}

	function rotate(angle, center){
		for(let i = 0; i < points.length; i++){
			points[i] = LinearAlgebra.rotatePoint(angle, center, points[i])
		}
		shapeChanged();
	}

	function leftMoving(scaleX, deltaXArg, deltaYArg, marginXArg){
		if(marginXArg < Style.marginXXL){
			return
		}
		transformPoints(scaleX, 1, deltaXArg, 0)
	}

	function rightMoving(scaleX, deltaXArg, deltaYArg, marginXArg){
		if(marginXArg < Style.marginXXL){
			return
		}
		transformPoints(scaleX, 1,0,0)
	}

	function topMoving(scaleY, deltaXArg, deltaYArg, marginYArg){
		if(marginYArg < Style.marginXXL){
			return
		}
		transformPoints(1, scaleY, 0, deltaYArg)
	}

	function bottomMoving(scaleY, deltaXArg, deltaYArg, marginYArg){
		if(marginYArg < Style.marginXXL){
			return
		}
		transformPoints(1, scaleY,0,0)
	}

	function topLeftMoving(scaleX, scaleY, deltaXArg, deltaYArg, marginXArg, marginYArg){
		if(marginXArg < Style.marginXXL || marginYArg < Style.marginXXL){
			return
		}
		transformPoints(scaleX, scaleY, deltaXArg, deltaYArg)
	}

	function topRightMoving(scaleX, scaleY, deltaXArg, deltaYArg, marginXArg, marginYArg){
		if(marginXArg < Style.marginXXL || marginYArg < Style.marginXXL){
			return
		}
		transformPoints(scaleX, scaleY, 0, deltaYArg)
	}

	function bottomLeftMoving(scaleX, scaleY, deltaXArg, deltaYArg, marginXArg, marginYArg){
		if(marginXArg < Style.marginXXL || marginYArg < Style.marginXXL){
			return
		}
		transformPoints(scaleX, scaleY, deltaXArg, 0)
	}

	function bottomRightMoving(scaleX, scaleY, deltaXArg, deltaYArg, marginXArg, marginYArg){
		if(marginXArg < Style.marginXXL || marginYArg < Style.marginXXL){
			return
		}
		transformPoints(scaleX, scaleY,0,0)
	}

	function transformPoints(scaleX, scaleY, translationX, translationY){

		let matrix = [
				[scaleX, 0, translationX],
				[0, scaleY, translationY],
				[0, 0, 1]
			  ];

		for(let i = 0; i < pointsBackupScreen.length; i++){
			let pointB = pointsBackupScreen[i]
			let topLeftPoint = bbPointsBackupScreen.topLeftPoint
			let newPoint = LinearAlgebra.transformPoint2d(Qt.point(pointB.x - topLeftPoint.x, pointB.y -topLeftPoint.y), matrix)
			points[i] = getLogPosition(Qt.point(newPoint.x + topLeftPoint.x, newPoint.y + topLeftPoint.y))
		}
	}

	function getAngle(xArg, yArg, center){
		let x0 = center.x
		let y0 = center.y
		let x_ = (xArg- x0)
		let y_ = (yArg - y0)
		let r = Math.sqrt(x_*x_+y_*y_)
		let angle = 0
		if(x_ >= 0 && y_ <= 0)
			angle = Math.asin(Math.abs(x_)/r)*180/Math.PI
		else if(x_ > 0 && y_ > 0)
			angle = Math.acos(Math.abs(x_)/r)*180/Math.PI + 90
		else if(x_ <= 0 && y_ > 0)
			angle = Math.asin(Math.abs(x_)/r)*180/Math.PI + 180
		else if(x_ <= 0 && y_ <= 0)
			angle = Math.acos(Math.abs(x_)/r)*180/Math.PI + 270

		return angle;
	}

	onMousePositionChanged: {
		if(viewItem.isEditMode && isSelected){
			if(mouseIsPressed){
				//console.log("onPositionChanged:::::")
				let position = Qt.point(mouseEvent.x, mouseEvent.y)
				editFunction(position)
			}
		}
		else if(viewItem.isPointsEditMode && isSelected){
			//console.log("mouseIsPressed:: ", mouseIsPressed)
			if(mouseIsPressed){
				if(editNodeIndex > -1){
					points[editNodeIndex] = getLogPosition(Qt.point(mouseEvent.x, mouseEvent.y))
				}
				shapeChanged()

			}

		}

	}

	property var pointsBackupScreen: []
	property real bbWidthBackupScreen: 1
	property real bbHeightBackupScreen: 1
	property var bbPointsBackupScreen
	property var bbPointsBackup
	property CanvasMatrix shapeMatrixBackup : CanvasMatrix{}
	property CanvasMatrix rotationTempMatrix: CanvasMatrix{};
	property CanvasMatrix rotationTempMatrixScreen: CanvasMatrix{};

	property bool _isInsideRotationArea: false;


	onIsSelectedChanged: {
		setBackup()
	}

	onMousePressed: {
		setBackup()
		if(viewItem.isEditMode && isSelected){
			_isInsideRotationArea = isInsideRotationArea(mouseEvent.x, mouseEvent.y, bbPointsBackupScreen)
			//console.log("_isInsideRotationArea::", _isInsideRotationArea)
			if(_isInsideRotationArea){
				bbPointsBackup = getBoundingBoxCornerPoints(false)
				shapeMatrixBackup.copyFrom(shapeMatrix.matrix)
			}
		}
	}

	onMouseReleased: {
		if(_isInsideRotationArea){
			_isInsideRotationArea = false;
			let newPoints = []
			for(let i = 0; i < points.length; i++){
				newPoints.push(LinearAlgebra.transformPoint2d(points[i], rotationTempMatrix.matrix))
			}
			points = newPoints
			shapeMatrix.copyFrom(shapeMatrixBackup.matrix)
			rotationTempMatrix.reset()
			rotationTempMatrixScreen.reset()
		}
	}

	function setBackup(){
		if(isSelected && viewItem && viewItem.isEditMode){
			pointsBackupScreen = []
			for(let i = 0; i < points.length; i++){
				pointsBackupScreen.push(getScreenPosition(points[i]))
			}

			bbPointsBackupScreen = getBoundingBoxCornerPoints(true)
			bbWidthBackupScreen = getBoundingBoxWidth(bbPointsBackupScreen)
			bbHeightBackupScreen = getBoundingBoxHeight(bbPointsBackupScreen)
		}
	}

}

