import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0


QtObject {
	id: layer;

	property Item viewItem: null;
	property string layerId: "";
	property var shapeModel: [];

	property bool enabled: true;
	property bool visible: true;
	property bool canApplyViewTransform: true;
	property bool isActive: false;

	signal loadImageSignal(string source)

	property CanvasMatrix viewMatrix: CanvasMatrix{};
	property CanvasMatrix layerMatrix: CanvasMatrix{};
	property CanvasMatrix tempMatrix: CanvasMatrix{};
	property CanvasMatrix identityMatrix: CanvasMatrix{};

	property var viewMode;

	property rect clipRect: Qt.rect(0,0,0,0)
	property rect clipRectBackup: Qt.rect(0,0,0,0)

	signal layerChanded()

	onClipRectChanged: {
		if(clipRect.width > 0 && clipRect.height > 0){
			clipRectBackup = Qt.rect(clipRect.x, clipRect.y, clipRect.width, clipRect.height)
		}
	}

	onLayerChanded: {
		viewItem.requestPaint();
	}

	function addShape(shape){
		if(shape.viewItem !==undefined){
			shape.viewItem = viewItem;
		}
		shape.layer = layer;

		shapeModel.push(shape)
		let index = shapeModel.length -1;

		if(shape.imageSource !== undefined){
			layer.loadImageSignal(shape.imageSource)
		}

		return index;
	}

	function clear(){
		shapeModel = []
	}

	function removeShape(index){
		shapeModel.splice(index, 1);
	}

	function draw(ctx, viewMatrixArg){
		viewMatrix.copyFrom(viewMatrixArg.matrix)
		ctx.save();

		if(clipRect.width !== 0 && clipRect.height !== 0){
			ctx.beginPath()
			ctx.moveTo(clipRect.x, clipRect.y)
			ctx.lineTo(clipRect.x + clipRect.width, clipRect.y)
			ctx.lineTo(clipRect.x + clipRect.width, clipRect.y + clipRect.height)
			ctx.lineTo(clipRect.x, clipRect.y + clipRect.height)
			ctx.closePath()
			ctx.clip()
		}

		tempMatrix.matrix = LinearAlgebra.multiplyByMatrix3x3(viewMatrix.matrix, layerMatrix.matrix)

		for(let i = 0; i < shapeModel.length; i++){
			ctx.globalAlpha = 1
			let shape = shapeModel[i]
			if(!shape.isSelected){
				drawShape(ctx, shape)
			}
		}
		for(let i = 0; i < shapeModel.length; i++){
			ctx.globalAlpha = 1
			let shape = shapeModel[i]
			if(shape.isSelected){
				drawShape(ctx, shape)
			}
		}

		ctx.restore();
	}

	function drawShape(ctx, shape){
		if(shape.viewMatrix !==undefined){
			shape.viewMatrix = viewMatrix
		}
		if(shape.layerMatrix !==undefined){
			shape.layerMatrix = layerMatrix
		}

		shape.drawComplex(ctx, tempMatrix);
	}

	function getScreenPosition(logPosition){
		let matrix = LinearAlgebra.multiplyByMatrix3x3(viewItem.viewMatrix.matrix, layer.layerMatrix.matrix)
		let screenPosition = LinearAlgebra.transformPoint2d(logPosition, matrix)
		return screenPosition
	}

	function getLogPosition(screenPosition){
		let matrix = LinearAlgebra.multiplyByMatrix3x3(viewItem.viewMatrix.matrix, layer.layerMatrix.matrix)
		matrix = LinearAlgebra.getInvertedMatrix3x3(matrix)
		let logPosition = LinearAlgebra.transformPoint2d(screenPosition, matrix)

		return logPosition
	}

	function getLayerBoundingBox(isScreenPosition){
		let allPoints = []
		let bbPoints
		for(let i = 0; i < shapeModel.length; i++){
			let shape = shapeModel[i]
			let currBBPoints = shape.getBoundingBoxCornerPoints(true)
			allPoints.push(currBBPoints.topLeftPoint)
			allPoints.push(currBBPoints.topRightPoint)
			allPoints.push(currBBPoints.bottomLeftPoint)
			allPoints.push(currBBPoints.bottomRightPoint)
		}

		bbPoints = getLimitsObject(allPoints)

		return bbPoints
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

}

