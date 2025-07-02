import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

QtObject {
	id: layer;

	property string layerId: "";
	property var shapeModel: [];

	property bool enabled: true;
	property bool visible: true;
	property bool canApplyViewTransform: true;

	signal loadImageSignal(string source)

	property CanvasMatrix layerMatrix: CanvasMatrix{};

	function addShape(shape){
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

	function draw(ctx, canvasMatrix){
		canvasMatrix.matrix = canvasMatrix.multiplyByMatrix(canvasMatrix.matrix, layerMatrix.matrix)

		canvasMatrix.setContextTransform(ctx)

		for(let i = 0; i < shapeModel.length; i++){
			ctx.globalAlpha = 1
			let shape = shapeModel[i]
			if(!shape.isSelected){
				shape.draw(ctx);
			}
		}
		for(let i = 0; i < shapeModel.length; i++){
			ctx.globalAlpha = 1
			let shape = shapeModel[i]
			if(shape.isSelected){
				shape.draw(ctx);
			}
		}
	}

}

