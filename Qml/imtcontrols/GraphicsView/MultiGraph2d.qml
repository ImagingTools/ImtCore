import QtQuick
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0


Graph2d{
	id: graph;

	isMultiGraph: true;
	property var tooltipValuesArray: []

	function addLine(pointsArg, colorArg, showNodesArg, isEditableArg, lineDashArrayArg, tooltipValuesArg){
		if(!pointsArg || !pointsArg.length){
			return;
		}
		let activeLayer = graphicsViewAlias.getActiveLayer()
		let lineIndex  = activeLayer.shapeModel.length
		let line = createLine()
		line.points = pointsArg
		line.lineIndex = lineIndex;
		if(colorArg !==undefined){
			line.color = colorArg;
		}
		if(showNodesArg !==undefined){
			line.showNodes = showNodesArg
		}
		if(isEditableArg !==undefined){
			//line.isEditable = isEditableArg//??
			line.canSelect = isEditableArg
		}
		else {
			line.canSelect = true;
		}
		if(lineDashArrayArg !==undefined){
			line.lineDashArray = lineDashArrayArg
		}

		let empty = []
		let ttv = tooltipValuesArg !==undefined ? tooltipValuesArg : empty
		tooltipValuesArray.push(ttv)

		activeLayer.addShape(line)

		graphicsViewAlias.firstResize = true;

		requestPaint();
	}

	function getLine(index){
		let activeLayer = graphicsViewAlias.getActiveLayer()
		let shapeModel = activeLayer.shapeModel[index]
		if(!shapeModel.length || index < 0 || index > shapeModel.length -1){
			return null
		}

		return shapeModel[index]
	}

	function getPoints(index){
		let activeLayer = graphicsViewAlias.getActiveLayer()
		let shapeModel = activeLayer.shapeModel
		if(!shapeModel.length || index < 0 || index > shapeModel.length -1){
			return []
		}
		let line = activeLayer.shapeModel[index]

		return line.points
	}

	function setPoints(points, index){
		let activeLayer = graphicsViewAlias.getActiveLayer()
		let shapeModel = activeLayer.shapeModel
		if(!shapeModel.length || index < 0 || index > shapeModel.length -1){
			return false
		}
		let line = activeLayer.shapeModel[index]

		line.points = points;

		return true
	}

	function clear(){
		clearLines ()
		clearLabels()
	}

	function clearLines (){
		let activeLayer = graphicsViewAlias.getActiveLayer()
		activeLayer.shapeModel = []
		tooltipValuesArray = []
		requestPaint();
	}

	function clearLabels(){
		labelXValues = []
		labelYValues = []
	}

	function getTooltipText(pointIndex, lineIndex){//override
		if(lineIndex > (tooltipValuesArray.length -1)){
			return "";
		}
		let tooltips = tooltipValuesArray[lineIndex]
		if(pointIndex > (tooltips.length -1)){
			return "";
		}
		let val = tooltips[pointIndex]
		let str = "(" + val.x + ", " + val.y + ")"

		return str
	}

	function checkTooltips(pointIndex, lineIndex){
		if(lineIndex > (tooltipValuesArray.length -1)){
			return false;
		}
		let tooltips = tooltipValuesArray[lineIndex]
		if(pointIndex > (tooltips.length -1)){
			return false;
		}
		return true;
	}

	function getLineCount(){
		let activeLayer = graphicsViewAlias.getActiveLayer()
		if(!activeLayer){
			return 0
		}
		let shapeModel = activeLayer.shapeModel

		return shapeModel.length
	}
}


