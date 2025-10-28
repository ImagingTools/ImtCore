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

	property string lineColor: Style.greenColor;

	property var linePoints: [];
	property bool hasData: false;
	property bool canDraw: true;


	function requestPaint() {
		graphicsView.requestPaint()
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

		// onPaintTime: {
		// 	console.log("PAINT_TIME::", time, model.index)
		// }

		function resize(){
			if(width > 0 && height > 0){
				setLayersParams()
				fitToActiveLayer()
			}
		}

		function startFunc(){
			if(!graph.hasData){
				return
			}

			let background = getBackgroundLayer()
			background.addShape(gridShape);

			let activeLayer = getActiveLayer()

			activeLayer.layerMatrix.invertY();

			let lineObj = polylineComp.createObject(this);
			lineObj.color = graph.lineColor
			activeLayer.addShape(lineObj);
		}

		function setLayersParams(){
			if(!graph.hasData){
				return
			}

			let activeLayer = getActiveLayer()

			if(!width || !height || !activeLayer){
				return
			}

			let clipRect = Qt.rect(gridShape.labelXWidth, gridShape.legendMargin, width - gridShape.labelXWidth - gridShape.legendMargin, height - gridShape.labelYHeight - gridShape.legendMargin)
			activeLayer.clipRect = clipRect

			activeLayer.layerMatrix.setXTranslation(gridShape.labelXWidth + gridShape.axesOrigin.x)
			activeLayer.layerMatrix.setYTranslation(height - gridShape.labelYHeight - gridShape.axesOrigin.y)

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

		labelYHeight: 20
		cutAxesEnds: true
		legendMargin: 20;
		gridStepMajorX: 10
		gridStepMajorY: 10
		canDrawText: true;
		labelX: ""
		labelY: ""
		fontSize: Style.fontSizeXXS

	}

	Component{
		id: polylineComp

		PolyLineShape{
			showNodes: true;
			points: graph.linePoints;

			onHighlightedNodeIndexChanged: {
				if(highlightedNodeIndex > -1){
					toolTip.text = "(" + Number(highlightedNodeCoordinate.x).toFixed(2)  + ", " + Number(highlightedNodeCoordinate.y).toFixed(2) + ")";
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


