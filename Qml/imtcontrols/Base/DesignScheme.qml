pragma Singleton
import QtQuick 2.12


QtObject {
	id: scheme;

	property real shapePointSize: 4;
	property real shapeSelectedPointSize: 6;
	property int boundingBoxLineWidth: 2;
	property int boundingBoxControlPointRadius: 6;
	property string boundingBoxBorderColor: "#3a3b3b"

	property int selectionLineWidth: 4;
	property string selectionColor: "orange"

	function drawNode(ctx, point, radius){
		if(radius == 0 || radius == undefined){
			radius = shapePointSize;
		}
		ctx.moveTo(point.x, point.y);
		ctx.arc(point.x, point.y, radius,  0, 2 * Math.PI, true);
		// ctx.stroke();
		// ctx.fill();
	}

	function drawSelectedNode(ctx, point, radius){
		if(radius == 0 || radius == undefined){
			radius = shapeSelectedPointSize;
		}
		ctx.moveTo(point.x, point.y);
		ctx.arc(point.x, point.y, radius,  0, 2 * Math.PI, true);
		// ctx.stroke();
		// ctx.fill();
	}


	function drawBoundingBoxBorder(ctx, cornerPointList, matrix){
		ctx.strokeStyle = boundingBoxBorderColor;
		ctx.lineWidth = boundingBoxLineWidth

		ctx.beginPath()
		let topLeftPoint = cornerPointList.topLeftPoint
		let topRightPoint = cornerPointList.topRightPoint
		let bottomLeftPoint = cornerPointList.bottomLeftPoint
		let bottomRightPoint = cornerPointList.bottomRightPoint
		if(matrix !== undefined){
			topLeftPoint = LinearAlgebra.transformPoint2d(topLeftPoint, matrix)
			topRightPoint = LinearAlgebra.transformPoint2d(topRightPoint, matrix)
			bottomLeftPoint = LinearAlgebra.transformPoint2d(bottomLeftPoint, matrix)
			bottomRightPoint = LinearAlgebra.transformPoint2d(bottomRightPoint, matrix)
		}

		ctx.moveTo(topLeftPoint.x, topLeftPoint.y)

		ctx.lineTo(topRightPoint.x, topRightPoint.y)
		ctx.lineTo(bottomRightPoint.x, bottomRightPoint.y)
		ctx.lineTo(bottomLeftPoint.x, bottomLeftPoint.y)
		ctx.lineTo(topLeftPoint.x, topLeftPoint.y)

		ctx.stroke();
		ctx.closePath();
	}

	function drawBoundingBoxRotationControl(ctx, cornerPointList, matrix){
		ctx.strokeStyle = boundingBoxBorderColor;
		ctx.fillStyle = boundingBoxBorderColor;
		ctx.lineWidth = boundingBoxLineWidth

		let topLeftPoint = cornerPointList.topLeftPoint
		let topRightPoint = cornerPointList.topRightPoint
		let bottomLeftPoint = cornerPointList.bottomLeftPoint
		let bottomRightPoint = cornerPointList.bottomRightPoint


		let bottomLeftRotation =  Qt.point(topLeftPoint.x + (topRightPoint.x - topLeftPoint.x)/3, topLeftPoint.y + 2 * (bottomRightPoint.y - topRightPoint.y)/3)
		let bottomRightRotation = Qt.point(topLeftPoint.x + 2 * (topRightPoint.x - topLeftPoint.x)/3, topLeftPoint.y + 2 *(bottomRightPoint.y - topRightPoint.y)/3)
		let topLeftRotation = Qt.point(topLeftPoint.x + (topRightPoint.x - topLeftPoint.x)/3, topLeftPoint.y + (bottomRightPoint.y - topRightPoint.y)/3)
		let topRightRotation = Qt.point(topLeftPoint.x + 2 * (topRightPoint.x - topLeftPoint.x)/3, topLeftPoint.y + (bottomRightPoint.y - topRightPoint.y)/3)

		let center = Qt.point((topLeftRotation.x + topRightRotation.x)/2, (topRightRotation.y + bottomLeftRotation.y)/2)

		if(matrix !== undefined){
			bottomLeftRotation = LinearAlgebra.transformPoint2d(bottomLeftRotation, matrix)
			bottomRightRotation = LinearAlgebra.transformPoint2d(bottomRightRotation, matrix)
			topLeftRotation = LinearAlgebra.transformPoint2d(topLeftRotation, matrix)
			topRightRotation = LinearAlgebra.transformPoint2d(topRightRotation, matrix)

			center =  LinearAlgebra.transformPoint2d(center, matrix)
		}


		ctx.beginPath()
		ctx.moveTo(bottomLeftRotation.x, bottomLeftRotation.y);
		ctx.lineTo(topLeftRotation.x, topLeftRotation.y);
		ctx.lineTo(topRightRotation.x, topRightRotation.y);

		ctx.stroke();
		ctx.closePath();

		let centerX = (topLeftRotation.x + topRightRotation.x)/2
		let centerY = (topRightRotation.y + bottomLeftRotation.y)/2
		ctx.beginPath()
		ctx.moveTo(center.x, center.y);
		ctx.arc(center.x, center.y, shapePointSize,  0, 2 * Math.PI, true);
		ctx.stroke();
		ctx.fill()
		ctx.closePath();
	}

	function drawBoundingBoxControlPoint(ctx, point, matrix){
		let radius = boundingBoxControlPointRadius;
		if(matrix !== undefined){
			point = LinearAlgebra.transformPoint2d(point, matrix)
		}
		ctx.strokeStyle = boundingBoxBorderColor;
		ctx.fillStyle = boundingBoxBorderColor;
		ctx.lineWidth = boundingBoxLineWidth

		ctx.moveTo(point.x, point.y)
		ctx.beginPath()
		ctx.arc(point.x, point.y, radius, 0, 2*Math.PI, true);
		ctx.stroke()
		ctx.fill()
		ctx.closePath();
	}

	function drawEditPoint(ctx, point){
		let radius = boundingBoxControlPointRadius;
		ctx.strokeStyle = boundingBoxBorderColor;
		ctx.fillStyle = boundingBoxBorderColor;
		ctx.lineWidth = boundingBoxLineWidth

		ctx.moveTo(point.x, point.y)
		ctx.beginPath()
		ctx.arc(point.x, point.y, radius, 0, 2*Math.PI, true);
		ctx.stroke()
		ctx.fill()
		ctx.closePath();
	}

}
