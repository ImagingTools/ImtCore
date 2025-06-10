import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

QtObject {

	id: root;

	function drawBoundingBox(ctx, layerId){
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

}

