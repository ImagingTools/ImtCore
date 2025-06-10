import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

QtObject {

	id: root;


	function drawBoundingBox(ctx, layerId){
		drawBoundingBoxBorder(ctx, layerId);
		drawBoundingBoxControlPoints(ctx, layerId);
		drawBoundingBoxRotationControl(ctx, layerId);
	}

	function drawBoundingBoxBorder(ctx, layerId){
		let cornerPointList = getBoundingBoxCornerPoints();

		ctx.strokeStyle = "black";
		ctx.lineWidth = 2

		ctx.beginPath()
		ctx.moveTo(cornerPointList[0].x, cornerPointList[0].y)

		ctx.lineTo(cornerPointList[1].x, cornerPointList[1].y)
		ctx.lineTo(cornerPointList[2].x, cornerPointList[2].y)
		ctx.lineTo(cornerPointList[3].x, cornerPointList[3].y)
		ctx.lineTo(cornerPointList[0].x, cornerPointList[0].y)

		ctx.stroke();
		ctx.closePath();
	}

	function drawBoundingBoxControlPoints(ctx, layerId){
		let controlPointList = getBoundingBoxCornerPoints().concat(getBoundingBoxMidPoints());

		ctx.strokeStyle = "black";
		ctx.lineWidth = 2
		let radius = 6;

		ctx.beginPath()
		for(let i = 0; i < controlPointList.length; i++){
			ctx.moveTo(controlPointList[i].x, controlPointList[i].y)
			ctx.arc(controlPointList[i].x, controlPointList[i].y, radius, 0, 2*Math.PI, true);
		}
		ctx.stroke();
		ctx.closePath();
	}

	function drawBoundingBoxRotationControl(ctx, layerId){
		let cornerPointList = getBoundingBoxCornerPoints();

		ctx.strokeStyle = "black";
		ctx.lineWidth = 2

		let bottomLeftX = cornerPointList[0].x + (cornerPointList[1].x - cornerPointList[0].x)/3
		let bottomLeftY = cornerPointList[0].y + 2 *(cornerPointList[2].y - cornerPointList[1].y)/3
		let topLeftX = cornerPointList[0].x + (cornerPointList[1].x - cornerPointList[0].x)/3
		let topLeftY = cornerPointList[0].y + (cornerPointList[2].y - cornerPointList[1].y)/3
		let topRightX = cornerPointList[0].x + 2 * (cornerPointList[1].x - cornerPointList[0].x)/3
		let topRightY = cornerPointList[0].y + (cornerPointList[2].y - cornerPointList[1].y)/3

		ctx.beginPath()
		ctx.moveTo(bottomLeftX, bottomLeftY);
		ctx.lineTo(topLeftX, topLeftY);
		ctx.lineTo(topRightX, topRightY);

		ctx.stroke();
		ctx.closePath();
	}

	function getBoundingBoxCenter(){
		let cornerPointList = getBoundingBoxCornerPoints();

		return Qt.point((cornerPointList[0].x + cornerPointList[1].x)/2,0, (cornerPointList[1].y + cornerPointList[2].y)/2);
	}

	function getBoundingBoxPoints(){
		return [];
	}

	function getBoundingBoxCornerPoints(){
		return [];
	}

	function getBoundingBoxMidPoints(){
		let midpointList = []
		let cornerPointList = getBoundingBoxCornerPoints();
		if(cornerPointList.length < 4){
			return;
		}
		midpointList.push(Qt.point((cornerPointList[0].x + cornerPointList[1].x)/2, cornerPointList[0].y))
		midpointList.push(Qt.point(cornerPointList[1].x, (cornerPointList[1].y + cornerPointList[2].y)/2))
		midpointList.push(Qt.point((cornerPointList[2].x + cornerPointList[3].x)/2, cornerPointList[2].y))
		midpointList.push(Qt.point(cornerPointList[3].x, (cornerPointList[0].y + cornerPointList[3].y)/2))

		return midpointList;
	}

	function getBoundingBoxRotation(){
		return 0;
	}

	function rotateBoundingBox(angle){
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

	function draw(ctx, layerId){
		drawBoundingBox(ctx, layerId)
	}

}

