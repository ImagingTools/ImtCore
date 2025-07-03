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
	property CanvasMatrix tempMatrix: CanvasMatrix{};

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

		tempMatrix.copy(canvasMatrix.matrix);
		tempMatrix.matrix = tempMatrix.multiplyByMatrix(tempMatrix.matrix, layerMatrix.matrix)

		tempMatrix.setContextTransform(ctx)

		for(let i = 0; i < shapeModel.length; i++){
			ctx.globalAlpha = 1
			let shape = shapeModel[i]
			if(!shape.isSelected){
				shape.draw(ctx, tempMatrix);
			}
		}
		for(let i = 0; i < shapeModel.length; i++){
			ctx.globalAlpha = 1
			let shape = shapeModel[i]
			if(shape.isSelected){
				shape.draw(ctx, tempMatrix);
			}
		}
	}

}

