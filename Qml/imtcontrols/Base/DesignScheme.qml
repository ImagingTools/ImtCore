pragma Singleton
import QtQuick 2.12


QtObject {
	id: scheme;

	property real shapePointSize: 4;
	property real shapeSelectedPointSize: 6;
	property int boundingBoxLineWidth: 2;
	property int boundingBoxControlPointRadius: 6;
	property string boundingBoxBorderColor: "#000000"

	property int selectionLineWidth: 4;
	property string selectionColor: "orange"

	function drawNode(ctx, point){
		let radius = shapePointSize;
		ctx.moveTo(point.x, point.y);
		ctx.arc(point.x, point.y, radius,  0, 2 * Math.PI, true);
		ctx.stroke();
		ctx.fill();
	}

	function drawSelectedNode(ctx, point){
		let radius = shapeSelectedPointSize;
		ctx.beginPath();
		ctx.moveTo(point.x, point.y);
		ctx.arc(point.x, point.y, radius,  0, 2 * Math.PI, true);
		ctx.stroke();
		ctx.fill();
		ctx.closePath();
	}


	function drawBoundingBoxBorder(ctx, cornerPointList){
		ctx.strokeStyle = boundingBoxBorderColor;
		ctx.lineWidth = boundingBoxLineWidth

		ctx.beginPath()
		ctx.moveTo(cornerPointList.topLeftPoint.x, cornerPointList.topLeftPoint.y)

		ctx.lineTo(cornerPointList.topRightPoint.x, cornerPointList.topRightPoint.y)
		ctx.lineTo(cornerPointList.bottomRightPoint.x, cornerPointList.bottomRightPoint.y)
		ctx.lineTo(cornerPointList.bottomLeftPoint.x, cornerPointList.bottomLeftPoint.y)
		ctx.lineTo(cornerPointList.topLeftPoint.x, cornerPointList.topLeftPoint.y)

		ctx.stroke();
		ctx.closePath();
	}

	function drawBoundingBoxRotationControl(ctx, cornerPointList){
		ctx.strokeStyle = boundingBoxBorderColor;
		ctx.lineWidth = boundingBoxLineWidth

		let bottomLeftX = cornerPointList.topLeftPoint.x + (cornerPointList.topRightPoint.x - cornerPointList.topLeftPoint.x)/3
		let bottomLeftY = cornerPointList.topLeftPoint.y + 2 *(cornerPointList.bottomRightPoint.y - cornerPointList.topRightPoint.y)/3
		let topLeftX = cornerPointList.topLeftPoint.x + (cornerPointList.topRightPoint.x - cornerPointList.topLeftPoint.x)/3
		let topLeftY = cornerPointList.topLeftPoint.y + (cornerPointList.bottomRightPoint.y - cornerPointList.topRightPoint.y)/3
		let topRightX = cornerPointList.topLeftPoint.x + 2 * (cornerPointList.topRightPoint.x - cornerPointList.topLeftPoint.x)/3
		let topRightY = cornerPointList.topLeftPoint.y + (cornerPointList.bottomRightPoint.y - cornerPointList.topRightPoint.y)/3

		ctx.beginPath()
		ctx.moveTo(bottomLeftX, bottomLeftY);
		ctx.lineTo(topLeftX, topLeftY);
		ctx.lineTo(topRightX, topRightY);

		ctx.stroke();
		ctx.closePath();
	}

	function drawBoundingBoxControlPoint(ctx, point){
		let radius = boundingBoxControlPointRadius;
		ctx.strokeStyle = boundingBoxBorderColor;
		ctx.lineWidth = boundingBoxLineWidth

		ctx.moveTo(point.x, point.y)
		ctx.arc(point.x, point.y, radius, 0, 2*Math.PI, true);
	}
}
