import QtQuick
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0


Rectangle{
	id: graph;

	color: Style.baseColor;

	width: Style.sizeHintM
	height: width;

	clip: true

	property string title: "Curve"
	property string legendX: "Index"
	property string legendY: "Value"

	property string lineColor: Style.imaginToolsAccentColor;

	property real gridStepMajorX: 40
	property real gridStepMajorY: 40
	property int labelFontSize: Style.fontSizeM//Style.fontSizeXXS

	property var linePoints: [];
	property int pointCount: linePoints.length
	property bool hasData: true;
	property bool canDraw: true;

	property var tooltipValues: []
	property var labelXValues: []
	property var labelYValues: []

	property int lableXOriginMargin: 20
	property int legendMargin: 0

	property bool alwaysShowOrigin: false;
	property bool hasTooltip: true;
	property bool hasMinorGrid: true;
	property bool isMultiGraph: false;

	property var rightLimit;

	property real xScale: 1
	property real yScale: 1

	property alias graphicsViewAlias: graphicsView;
	property alias gridShapeAlias: gridShape;
	property bool ready: graphicsView.ready

	property bool fitToWidth: false;

	property bool wasFitToWidth: false;

	property int legendFontSize: Style.fontSizeM

	property alias hasLeftButtonMenu: graphicsView.hasLeftButtonMenu;
	property alias hasRightButtonMenu: graphicsView.hasRightButtonMenu;
	property alias restrictMove: graphicsView.restrictMove;
	property alias restrictZoom: graphicsView.restrictZoom;
	property alias restrictSelect: graphicsView.restrictSelect;
	property alias hideScrollbars: graphicsView.hideScrollbars;


	function requestPaint() {
		graphicsView.resize()
	}

	function fitToView(){
		graphicsView.fitToInactivAndActiveLayer();
	}

	function setEditMode(mode){
		graphicsView.isPointsEditMode = mode
		graphicsView.isSelectionMode = !mode
		graphicsView.restrictSelect = !mode
		if(!mode){
			let wasClearSelection = false;
			let activeLayer  = graphicsView.getActiveLayer()
			let shapeModel = activeLayer.shapeModel;
			for(let j = 0; j < shapeModel.length; j++){
				let shape = shapeModel[j];
				if(shape.isSelected){
					shape.isSelected = false;
					wasClearSelection = true;
				}
			}
			if(wasClearSelection){
				graphicsView.requestPaint()
			}
		}
	}

	function createLine(){
		let lineObj = polylineComp.createObject(this);
		lineObj.color = graph.lineColor

		return lineObj
	}

	function getLabelX(xArg){
		let index = Math.round(Number(xArg) / gridStepMajorX)
		if(!graph.labelXValues.length){
			return gridShape.getLabelXBase(xArg)
		}
		if(index < 0 || index > (graph.labelXValues.length -1)){
			let str = ""
			return str
		}
		else {
			return String(labelXValues[index])
		}
	}

	function getLabelY(yArg){
		let index = Math.round(Number(yArg) / gridStepMajorY)
		if(!graph.labelYValues.length){
			return gridShape.getLabelXBase(yArg)
		}
		if(index < 0 || index > (graph.labelYValues.length -1)){
			let str = ""
			return str
		}
		else {
			return String(labelYValues[index])
		}
	}

	function getTooltipText(pointIndex, lineIndex){
		let ok = checkTooltips(pointIndex, lineIndex)
		if(!ok){
			return ""
		}
		let val = graph.tooltipValues[pointIndex]
		let str = "(" + val.x + ", " + val.y + ")"
		return str
	}

	function checkTooltips(pointIndex, lineIndex){
		return pointIndex <= (graph.tooltipValues.length -1)
	}

	function reset(){
		wasFitToWidth = false
		xScale = xScaleBackup
	}

	onLinePointsChanged: {
		reset()

		let activeLayer = graphicsView.getActiveLayer()
		if(!activeLayer || !activeLayer.shapeModel.length){
			return
		}
		let line = activeLayer.shapeModel[0]
		line.points = linePoints
		requestPaint()
	}

	// onPointCountChanged: {
	// 	reset()
	// 	requestPaint()
	// }

	BaseText{
		id: curveTitle;

		anchors.horizontalCenter: parent.horizontalCenter;
		text: graph.title
	}

	Item{
		id: clipItem

		anchors.top: curveTitle.bottom;
		anchors.bottom: curveLegendX.top;
		anchors.left: parent.left;
		anchors.right: parent.right

		anchors.leftMargin: Style.marginL
		anchors.bottomMargin: Style.marginM
		anchors.topMargin: 0

		width: parent.width

		GraphicsView{
			id: graphicsView

			height: parent.height

			width: graph.hasData && graph.canDraw ? parent.width : 0;

			hideScrollbars: true;
			restrictMove: true;
			restrictZoom: true;
			restrictSelect: true;
			propagateWheelEvents: true;
			propagateMouseEvents: true
			hasHoverReaction: true;

			//renderStrategy: Canvas.Immediate

			property bool compl: false;
			property bool ready: compl && width > 0 && height > 0
			property bool lineCreated: false;


			Component.onCompleted: {
				compl = true
			}
			onReadyChanged: {
				if(ready){
					graphicsView.startFunc()
				}
			}

			onPainted: {
				if(graph.fitToWidth && !graph.wasFitToWidth){
					fitToWidthFunction()
				}
			}

			function fitToActiveLayer(){
				fitToInactivAndActiveLayer()
			}

			function fitToWidthFunction(){
				if(graph.wasFitToWidth){
					return;
				}

				let activeLayer = getActiveLayer()
				if(!activeLayer){
					return;
				}

				let ok = !graph.isMultiGraph ? graph.linePoints.length : activeLayer.shapeModel.length
				if(!ok){
					return;
				}

				let maxX = 0
				let shapeModel = activeLayer.shapeModel
				for(let i = 0; i < shapeModel.length; i++){
					let shape = shapeModel[i]
					let points = shape.points
					for(let j = 0; j < points.length; j++){

						let currX = shape.getScreenPosition(points[j]).x
						if(currX > maxX){
							maxX = currX
						}
					}
				}

				if(graph.rightLimit !== undefined){
					let rightLimitScreen = rightLimitShape.getScreenPosition(rightLimitShape.points[0]).x
					if(rightLimitScreen > maxX){
						maxX = rightLimitScreen
					}
				}

				let margin  = Style.marginXXL
				let gridWidth = width - gridShape.labelYWidth - gridShape.legendMargin - margin
				if (gridWidth < 0){
					return
				}

				maxX = maxX - (gridShape.labelYWidth + gridShape.legendMargin)
				if (gridWidth <= 0 || maxX <= 0){
					return
				}

				let scale_ = gridWidth/ maxX
				graph.xScale = (graph.xScale * scale_)
				setLayersParams()
				graph.wasFitToWidth = true
				requestPaint()
			}

			function resize(){
				if(width > 0 && height > 0){
					setLayersParams()
					let activeLayer = getActiveLayer()
					let shapeModel = activeLayer ? activeLayer.shapeModel : null
					let ok =!graph.isMultiGraph ? graph.linePoints.length : !shapeModel ? false: shapeModel.length
					if(graph.fitToWidth){
						graph.wasFitToWidth = false
					}
					if(ok){
						fitToInactivAndActiveLayer()
					}
					else {
						requestPaint()
					}
				}
			}

			function startFunc(){
				if(!graph.hasData){
					return
				}

				let background = getBackgroundLayer()
				background.addShape(gridShape);

				let activeLayer = getActiveLayer()
				let inactiveLayer = getInactiveLayer()


				activeLayer.layerMatrix.setXScale(graph.xScale)
				activeLayer.layerMatrix.setYScale(-graph.yScale);

				inactiveLayer.layerMatrix.setXScale(graph.xScale)
				inactiveLayer.layerMatrix.setYScale(-graph.yScale);

				if(!graph.isMultiGraph){
					let lineObj =graph.createLine();
					activeLayer.addShape(lineObj);
					if(graph.linePoints.length){
						graphicsView.lineCreated = true;
					}
				}

				if(graph.alwaysShowOrigin){
					inactiveLayer.addShape(originShape);
				}
				if(graph.rightLimit !== undefined){
					rightLimitShape.points = [graph.rightLimit]
					inactiveLayer.addShape(rightLimitShape);
				}

			}

			function setLayersParams(){
				if(!graph.hasData){
					return
				}

				let activeLayer = getActiveLayer()
				let inactiveLayer = getInactiveLayer()

				if(!width || !height || !activeLayer||!inactiveLayer){
					return
				}

				let clipRect = Qt.rect(gridShape.labelYWidth, gridShape.legendMargin, width - gridShape.labelYWidth - gridShape.legendMargin, height - gridShape.labelXHeight - gridShape.legendMargin)

				activeLayer.clipRect = clipRect

				activeLayer.layerMatrix.setXScale(graph.xScale)
				activeLayer.layerMatrix.setYScale(-graph.yScale)
				activeLayer.layerMatrix.setXTranslation(gridShape.labelYWidth + gridShape.axesOrigin.x)
				activeLayer.layerMatrix.setYTranslation(height - gridShape.labelXHeight - gridShape.axesOrigin.y)

				inactiveLayer.clipRect = clipRect

				inactiveLayer.layerMatrix.setXScale(graph.xScale)
				inactiveLayer.layerMatrix.setYScale(-graph.yScale)
				inactiveLayer.layerMatrix.setXTranslation(gridShape.labelYWidth + gridShape.axesOrigin.x)
				inactiveLayer.layerMatrix.setYTranslation(height - gridShape.labelXHeight - gridShape.axesOrigin.y)

			}
		}
	}

	BaseText{
		id: curveLegendX;

		anchors.bottom: parent.bottom;
		anchors.horizontalCenter: parent.horizontalCenter;
		text: graph.legendX
		font.pixelSize: graph.legendFontSize
	}
	BaseText{
		id: curveLegendY;

		anchors.left: parent.left;
		anchors.leftMargin: -width/2 + height/2
		anchors.verticalCenter: parent.verticalCenter;
		rotation: -90;
		text: graph.legendY
		font.pixelSize: graph.legendFontSize
	}


	CoordinateGridShape{
		id: gridShape;

		labelXHeight: 24
		cutAxesEnds: true
		legendMargin: graph.legendMargin;
		gridStepMajorX: graph.gridStepMajorX * graph.xScale
		gridStepMajorY: graph.gridStepMajorY * graph.yScale
		xScale: graph.xScale
		yScale: graph.yScale
		canDrawText: true;
		labelX: ""
		labelY: ""
		fontSize: graph.labelFontSize
		lableXOriginMargin: graph.lableXOriginMargin
		hasMinorGrid: graph.hasMinorGrid

		function getLabelX(xArg){//override
			return graph.getLabelX(xArg)
		}

		function getLabelY(yArg){//override
			return graph.getLabelY(yArg)
		}

	}

	BoundingBox{
		id: originShape

		points: [Qt.point(0,0)]
	}

	BoundingBox{
		id: rightLimitShape
	}

	Component{
		id: polylineComp

		PolyLineShape{
			showNodes: true;
			points: graph.linePoints;

			isHidden: graph.isMultiGraph ? false : !graph.linePoints.length
			hasHoverReaction: graph.hasTooltip

			property int lineIndex: 0;

			function getTooltipText(){
				let str = graph.getTooltipText(highlightedNodeIndex, lineIndex)
				if(str == ""){
					return getTooltipTextBase()
				}
				else {
					return str
				}
			}

			function getTooltipTextBase(){
				let str = "(" + Number(highlightedNodeCoordinate.x).toFixed(2)  + ", " + Number(highlightedNodeCoordinate.y).toFixed(2) + ")";
				return str
			}

			onHighlightedNodeIndexChanged: {
				if(!graph.hasTooltip){
					return;
				}
				if(highlightedNodeIndex > -1){
					toolTip.text = getTooltipText()//"(" + Number(highlightedNodeCoordinate.x).toFixed(2)  + ", " + Number(highlightedNodeCoordinate.y).toFixed(2) + ")";
					let screenPoint = getScreenPosition(Qt.point(highlightedNodeCoordinate.x, highlightedNodeCoordinate.y))
					toolTip.closeTooltip()
					toolTip.show(screenPoint.x, screenPoint.y)
				}
				else {
					toolTip.closeTooltip()
				}
			}
		}
	}

	CustomTooltip{
		id: toolTip;
		timeout: 3000
		targetItem: graphicsView
		componentMargin: Style.marginS
	}

	property real xScaleBackup: xScale
	property bool xScaleBackupWasSet: false
	onXScaleChanged: {
		if(!xScaleBackupWasSet){
			xScaleBackup = xScale
			xScaleBackupWasSet = true

		}
	}
}


