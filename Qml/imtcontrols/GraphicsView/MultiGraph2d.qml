import QtQuick
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0


Graph2d{
	id: graph;

	property var tooltipValuesArray: []

	function addLine(pointsArg, colorArg, tooltipValuesArg){
		if(!pointsArg || !pointsArg.length){
			return;
		}
		let actioveLayer = graphicsViewAlias.getActiveLayer()
		let line = createLine()
		line.points = pointsArg
		if(colorArg !==undefined){
			line.color = colorArg;
		}

		let empty = []
		let ttv = tooltipValuesArg !==undefined ? tooltipValuesArg : empty
		tooltipValuesArray.push(ttv)

		actioveLayer.addShape(line)

		requestPaint();
	}

	function getPoints(index){
		let actioveLayer = graphicsViewAlias.getActiveLayer()
		let shapeModel = actioveLayer.shapeModel[index]
		if(!shapeModel.length || index < 0 || index > shapeModel.length -1){
			return []
		}
		let line = actioveLayer.shapeModel[index]

		return line.points
	}

	function setPoints(points, index){
		let actioveLayer = graphicsViewAlias.getActiveLayer()
		let shapeModel = actioveLayer.shapeModel[index]
		if(!shapeModel.length || index < 0 || index > shapeModel.length -1){
			return false
		}
		let line = actioveLayer.shapeModel[index]

		line.points = points;

		return true
	}
}


