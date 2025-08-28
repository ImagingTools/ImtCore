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

	signal layerChanded()

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

}

