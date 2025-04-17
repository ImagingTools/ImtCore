import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

QtObject {

	property string layerId: "";
	property var shapeModel: [];

	property bool enabled: true;
	property bool visible: true;

	function addShape(shape){
		shapeModel.push(shape)
		let index = shapeModel.length -1;

		return index;
	}

	function clear(){
		shapeModel = []
	}

	function removeShape(index){
		shapeModel.splice(index, 1);
	}

	function draw(ctx){
		for(let i = 0; i < shapeModel.length; i++){
			shapeModel[i].draw(ctx, layerId, i);
		}
	}

	function drawSelection(ctx, selectedIndexes){
		for(let i = 0; i < shapeModel.length; i++){
			shapeModel[i].drawSelection(ctx, layerId, selectedIndexes[i]);
		}
	}

	function drawBorderPoints(ctx, selectedIndexes){
		for(let i = 0; i < shapeModel.length; i++){
			shapeModel[i].drawBorderPoints(ctx, layerId, selectedIndexes[i]);
		}
	}


}

