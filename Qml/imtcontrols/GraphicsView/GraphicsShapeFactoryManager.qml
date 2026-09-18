import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0


QtObject {
	id: factoryManager

	property Item graphicsViewItem: null
	property var sdlShapeModel: null

	property Component textShapeComp: Component{TextShape{}}
	property Component imageShapeComp: Component{ImageShape2d{}}
	property Component rectangleShapeComp: Component{RectangleShape{}}
	property Component polygonShapeComp: Component{PolygonShape{}}
	property Component polyLineShapeComp: Component{PolyLineShape{}}
	property Component segmentShapeComp: Component{SegmentShape{}}
	property Component diskSegmentShapeComp: Component{DiskSegmentShape{}}
	property Component circleShapeComp: Component{CircleShape{}}

	property var factoryList: []

	signal sdlShapesCreated();

	Component.onCompleted: {
		factoryList.push(textShapeSdlComp.createObject(factoryManager))
		factoryList.push(imageShapeSdlComp.createObject(factoryManager))
		factoryList.push(rectangleShapeSdlComp.createObject(factoryManager))
		factoryList.push(polygonShapeSdlComp.createObject(factoryManager))
		factoryList.push(polyLineShapeSdlComp.createObject(factoryManager))
		factoryList.push(lineShapeSdlComp.createObject(factoryManager))
		factoryList.push(segmentShapeSdlComp.createObject(factoryManager))
		factoryList.push(annulusShapeSdlComp.createObject(factoryManager))
		factoryList.push(circleShapeSdlComp.createObject(factoryManager))

	}

	onSdlShapeModelChanged: {
		if(!graphicsViewItem || !sdlShapeModel){
			return;
		}
		let activeLayer = graphicsViewItem.getActiveLayer()
		activeLayer.clear()
		createShapeList(activeLayer)
	}

	function createShapeList(layerArg){
		layerArg.clear()
		for(let i = 0; i < sdlShapeModel.count; i++){
			let shapeItem = sdlShapeModel.get(i).item
			createShape(shapeItem, layerArg);
		}
		sdlShapesCreated()
	}

	function createShape(shapeItem, layerArg) {
		let sdlShapeType = shapeItem.__typename;
		let found = false
		for(let i = 0; i < factoryList.length; i++){
			let factory = factoryList[i]
			let factoryType = factory.typeName

			if(sdlShapeType == factoryType){
				factory.createQmlShape(shapeItem, layerArg)
				found = true
				break;
			}
		}
	}

	function addFactory(factorySdlComp, shapeComp){
		let obj = factorySdlComp.createObject(factoryManager)
		if(shapeComp !==undefined){
			obj.qmlShapeComp = shapeComp
		}

		factoryList.push(obj)
	}

	function clearShapes(){
		let activeLayer = graphicsViewItem.getActiveLayer()
		activeLayer.clear()
		let inactiveLayer = graphicsViewItem.getInactiveLayer()
		inactiveLayer.clear()
		let backgroundLayer = graphicsViewItem.getBackgroundLayer()
		backgroundLayer.clear()
		let toolsLayer = graphicsViewItem.getToolsLayer()
		toolsLayer.clear()
	}


	//factories
	property Component textShapeSdlComp: Component{
		GraphicsShapeFactoryBase{
			typeName: "TextLabelShape"
			qmlShapeComp: factoryManager.textShapeComp

			function createQmlShape(shapeItemArg, layerArg) {
				let textObj = qmlShapeComp.createObject(factoryManager)

				let rectangle2d = shapeItemArg.m_rect
				let topLeft = rectangle2d.m_topLeft
				let bottomRight = rectangle2d.m_bottomRight

				let fontFamily = shapeItemArg.m_fontName
				let fontSize = shapeItemArg.m_fontSize
				let fillStyle = shapeItemArg.m_painter.m_fill.m_color.m_color
				let strokeStyle = shapeItemArg.m_painter.m_stroke.m_color.m_color
				let text_ = shapeItemArg.m_text

				let point1 = Qt.point(topLeft.m_x , topLeft.m_y )
				let point2 = Qt.point(bottomRight.m_x , bottomRight.m_y)

				if(fontFamily == undefined){
					 fontFamily = Style.fontFamily
				}
				if(fontSize == undefined){
					fontSize = Style.fontSizeM
				}
				if(fillStyle == undefined){
					fillStyle = Style.textColor
				}
				if(strokeStyle == undefined){
					strokeStyle = Style.textColor
				}

				textObj.points = [point1, point2]
				textObj.fontFamily = fontFamily
				textObj.fontSize = fontSize
				textObj.fillColor = fillStyle
				textObj.strokeColor = strokeStyle
				textObj.text = text_
				layerArg.addShape(textObj)
			}

		}
	}
	property Component imageShapeSdlComp: Component{
		GraphicsShapeFactoryBase{
			typeName: "ImageShape"
			qmlShapeComp: factoryManager.imageShapeComp

			function createQmlShape(shapeItemArg, layerArg) {
				let imageObj = qmlShapeComp.createObject(factoryManager)
				let imageData = shapeItemArg.m_imageData
				let imageFormat = shapeItemArg.m_imageFormat

				let rectangle2d = shapeItemArg.m_rect
				let topLeft = rectangle2d.m_topLeft
				let bottomRight = rectangle2d.m_bottomRight

				let point1 = Qt.point(topLeft.m_x , topLeft.m_y )
				let point2 = Qt.point(bottomRight.m_x , bottomRight.m_y )
				let width_  = point2.x - point1.x
				let height_  = point2.y - point1.y

				let point = Qt.point(topLeft.m_x , topLeft.m_y )
				let sourceStr = "data:image/" + imageFormat + ";base64," + imageData

				imageObj.points = [point]
				imageObj.width = width_
				imageObj.height = height_
				imageObj.imageSource = sourceStr

				layerArg.addShape(imageObj)
			}
		}
	}
	property Component rectangleShapeSdlComp: Component{
		GraphicsShapeFactoryBase{
			typeName: "Rectangle2dShape"
			qmlShapeComp: factoryManager.rectangleShapeComp

			function createQmlShape(shapeItemArg, layerArg) {
				let recObj = qmlShapeComp.createObject(factoryManager)

				let rectangle2d = shapeItemArg.m_rectangle2d
				let topLeft = rectangle2d.m_topLeft
				let bottomRight = rectangle2d.m_bottomRight

				recObj.points = [Qt.point(topLeft.m_x , topLeft.m_y )]
				recObj.width = (bottomRight.m_x  - topLeft.m_x)
				recObj.height = (bottomRight.m_y - topLeft.m_y)

				recObj.color = shapeItemArg.m_painter.m_fill.m_color.m_color
				recObj.borderColor = shapeItemArg.m_painter.m_stroke.m_color.m_color
				recObj.penAlpha = shapeItemArg.m_painter.m_stroke.m_alpha/255
				recObj.brashAlpha = shapeItemArg.m_painter.m_fill.m_alpha/255
				layerArg.addShape(recObj)
			}
		}
	}
	property Component polygonShapeSdlComp: Component{
		GraphicsShapeFactoryBase{
			typeName: "Polygon2dShape"
			qmlShapeComp: factoryManager.polygonShapeComp

			function createQmlShape(shapeItemArg, layerArg) {
				let polyObj = qmlShapeComp.createObject(factoryManager)

				let polygon2d = shapeItemArg.m_polygon2d
				let pointArr = polygon2d.m_points

				let arr = []

				for(let i = 0; i < pointArr.count; i++){
					let point = pointArr.get(i).item
					point = Qt.point(point.m_x , point.m_y )
					arr.push(point)
				}

				polyObj.points = arr

				polyObj.color = shapeItemArg.m_painter.m_fill.m_color.m_color
				polyObj.borderColor = shapeItemArg.m_painter.m_stroke.m_color.m_color
				polyObj.penAlpha = shapeItemArg.m_painter.m_stroke.m_alpha/255
				polyObj.brashAlpha = shapeItemArg.m_painter.m_fill.m_alpha/255

				layerArg.addShape(polyObj)
			}
		}
	}
	property Component polyLineShapeSdlComp: Component{
		GraphicsShapeFactoryBase{
			typeName: "Polyline2dShape"
			qmlShapeComp: factoryManager.polyLineShapeComp

			function createQmlShape(shapeItemArg, layerArg) {
				let lineObj = qmlShapeComp.createObject(factoryManager)

				let line = shapeItemArg.m_polyline2d
				let points__ = line.m_points
				let arr = []
				for(let i = 0; i < points__.count; i++){
					let point__ = points__.get(i).item
					let point = Qt.point(point__.m_x , point__.m_y)
					arr.push(point)
				}

				lineObj.points = arr;

				let stroke = shapeItemArg.m_painter.m_stroke
				lineObj.color = stroke.m_color && stroke.m_color.m_color ? stroke.m_color.m_color : 'black'
				lineObj.penAlpha = stroke.m_alpha ? stroke.m_alpha/255 : 1
				lineObj.lineWidth = stroke.m_width ? stroke.m_width : 1
				layerArg.addShape(lineObj)
			}
		}
	}
	property Component lineShapeSdlComp: Component{
		GraphicsShapeFactoryBase{
			typeName: "Line2dShape"
			qmlShapeComp: factoryManager.polyLineShapeComp

			function createQmlShape(shapeItemArg, layerArg) {
				let lineObj = qmlShapeComp.createObject(factoryManager)

				let line = shapeItemArg.m_line
				let point1 = line.m_point1
				let point2 = line.m_point2

				lineObj.points = [Qt.point(point1.m_x , point1.m_y), Qt.point(point2.m_x , point2.m_y)]

				lineObj.color = shapeItemArg.m_painter.m_fill.m_color.m_color
				lineObj.penAlpha = shapeItemArg.m_painter.m_stroke.m_alpha/255
				lineObj.lineWidth = shapeItemArg.m_painter.m_stroke.m_width
				layerArg.addShape(lineObj)
			}
		}
	}
	property Component segmentShapeSdlComp: Component{
		GraphicsShapeFactoryBase{
			typeName: "SegmentShape"
			qmlShapeComp: factoryManager.segmentShapeComp

			function createQmlShape(shapeItemArg, layerArg) {
			}
		}
	}

	property Component annulusShapeSdlComp: Component{
		GraphicsShapeFactoryBase{
			typeName: "AnnulusShape"
			qmlShapeComp: factoryManager.diskSegmentShapeComp

			function createQmlShape(shapeItemArg, layerArg) {
				let annulusObj = qmlShapeComp.createObject(factoryManager)

				let annulus = shapeItemArg.m_circle
				let center = annulus.m_center
				let innerRadius = annulus.m_innerRadius
				let outerRadius = annulus.m_outerRadius
				let beginAngle = annulus.m_beginAngle
				let endAngle = annulus.m_endAngle

				annulusObj.center = [Qt.point(center.m_x , center.m_y )]
				annulusObj.innerRadius = innerRadius
				annulusObj.outerRadius = outerRadius
				annulusObj.startAngle = beginAngle
				annulusObj.endAngle = endAngle

				annulusObj.color = shapeItemArg.m_painter.m_fill.m_color.m_color
				annulusObj.borderColor = shapeItemArg.m_painter.m_stroke.m_color.m_color
				annulusObj.borderWidth = shapeItemArg.m_painter.m_stroke.m_width
				annulusObj.penAlpha = shapeItemArg.m_painter.m_stroke.m_alpha/255
				annulusObj.brashAlpha = shapeItemArg.m_painter.m_fill.m_alpha/255
				layerArg.addShape(annulusObj)
			}
		}
	}
	property Component circleShapeSdlComp: Component{
		GraphicsShapeFactoryBase{
			typeName: "CircleShape"
			qmlShapeComp: factoryManager.circleShapeComp

			function createQmlShape(shapeItemArg, layerArg) {
				let circleObj = qmlShapeComp.createObject(factoryManager)

				let circle = shapeItemArg.m_circle
				let center = circle.m_center
				let radius_ = circle.m_radius

				circleObj.center = [Qt.point(center.m_x , center.m_y )]
				circleObj.radius = radius_

				circleObj.color = shapeItemArg.m_painter.m_fill.m_color.m_color
				circleObj.borderColor = shapeItemArg.m_painter.m_stroke.m_color.m_color
				circleObj.penAlpha = shapeItemArg.m_painter.m_stroke.m_alpha/255
				circleObj.brashAlpha = shapeItemArg.m_painter.m_fill.m_alpha/255
				layerArg.addShape(circleObj)
			}
		}
	}

}
