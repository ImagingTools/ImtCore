import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

QtObject {

	property Component selectionComp: null;

	function draw (ctx, layerId){
	}

	function getParams(layerId){
		return [];
	}

	function getPoints(){
		return []
	}

	function setCoordinate(coord){

	}

	function createSelection(){
		if(selectionComp){
			let selection = selectionComp.createObject(this)
			selection.source = this;
			return selection;
		}
	}


}

