import QtQuick
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0


Rectangle{
	id: graph;

	color: Style.baseColor;

	width: Style.sizeHintM
	height: width;

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

	property bool alwaysShowOrigin: false;
	property bool hasTooltip: true;
	property bool hasMinorGrid: true;

	property real xScale: 1
	property real yScale: 1


	function requestPaint() {
		graphicsView.requestPaint()
	}

	function getLabelX(xArg){
		let index = Math.round(Number(xArg) / gridStepMajorX/ xScale)
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
		let index = Math.round(Number(yArg) / gridStepMajorY / yScale)
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

	onLinePointsChanged: {
		requestPaint()
	}
	onPointCountChanged: {
		requestPaint()
	}

	BaseText{
		id: curveTitle;

		anchors.horizontalCenter: parent.horizontalCenter;
		text: graph.title
	}


	GraphicsView{
		id: graphicsView

		anchors.top: curveTitle.bottom;
		anchors.bottom: curveLegendX.top;
		anchors.left: parent.left;

		anchors.leftMargin: Style.marginL
		anchors.bottomMargin: Style.marginM
		anchors.topMargin: 0

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

		Component.onCompleted: {
			compl = true
		}
		onReadyChanged: {
			if(ready){
				graphicsView.startFunc()
			}
		}

		function resize(){
			if(width > 0 && height > 0){
				setLayersParams()
				if(graph.linePoints.length){
					fitToActiveLayer()
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

			activeLayer.layerMatrix.setXScale(graph.xScale)
			activeLayer.layerMatrix.setYScale(-graph.yScale);


			let lineObj = polylineComp.createObject(this);
			lineObj.color = graph.lineColor
			activeLayer.addShape(lineObj);

			if(graph.alwaysShowOrigin){
				activeLayer.addShape(originShape);
			}
		}

		function setLayersParams(){
			if(!graph.hasData){
				return
			}

			let activeLayer = getActiveLayer()

			if(!width || !height || !activeLayer){
				return
			}

			let clipRect = Qt.rect(gridShape.labelYWidth, gridShape.legendMargin, width - gridShape.labelYWidth - gridShape.legendMargin, height - gridShape.labelXHeight - gridShape.legendMargin)
			activeLayer.clipRect = clipRect

			activeLayer.layerMatrix.setXScale(graph.xScale)
			activeLayer.layerMatrix.setYScale(-graph.yScale)
			activeLayer.layerMatrix.setXTranslation(gridShape.labelYWidth + gridShape.axesOrigin.x)
			activeLayer.layerMatrix.setYTranslation(height - gridShape.labelXHeight - gridShape.axesOrigin.y)

		}
	}

	BaseText{
		id: curveLegendX;

		anchors.bottom: parent.bottom;
		anchors.horizontalCenter: parent.horizontalCenter;
		text: graph.legendX
	}
	BaseText{
		id: curveLegendY;

		anchors.left: parent.left;
		anchors.leftMargin: -width/2 + height/2
		anchors.verticalCenter: parent.verticalCenter;
		rotation: -90;
		text: graph.legendY
	}


	CoordinateGridShape{
		id: gridShape;

		labelXHeight: 20
		cutAxesEnds: true
		legendMargin: 20;
		gridStepMajorX: graph.gridStepMajorX * graph.xScale
		gridStepMajorY: graph.gridStepMajorY * graph.yScale
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

	Component{
		id: polylineComp

		PolyLineShape{
			showNodes: true;
			points: graph.linePoints;

			isHidden: !graph.linePoints.length
			hasHoverReaction: graph.hasTooltip

			function getTooltipText(){
				if(highlightedNodeIndex > (graph.tooltipValues.length -1)){
					return getTooltipTextBase()
				}
				else {
					let val = graph.tooltipValues[highlightedNodeIndex]
					let str = "(" + val.x + ", " + val.y + ")"
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
}


