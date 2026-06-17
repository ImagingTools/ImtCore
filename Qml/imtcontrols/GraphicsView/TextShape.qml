import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

BoundingBox {
	id: textShape

	property string fontFamily: Style.fontFamily
	property int fontSize: 10
	property bool fontBold: false
	property string fillColor: Style.textColor
	property string strokeColor: Style.textColor
	property string text: ""
	property bool canZoomText: false
	property real scaleCoeff: viewItem.scaleCoeff

	function draw(ctx, layerMatrix){
		let params = getParams()
		ctx.fillStyle = params.color !== undefined ? params.color : textShape.fillColor;
		ctx.strokeStyle = params.color !== undefined ? params.color : textShape.strokeColor;
		ctx.lineWidth = params.lineWidth !== undefined ? params.lineWidth : 1;
		let fontSize = params.fontSize !== undefined ? params.fontSize : textShape.fontSize;
		if(viewItem && canZoomText){
			fontSize =  Math.ceil(fontSize * textShape.scaleCoeff)
		}
		let fontBold = params.fontBold !== undefined ? params.fontBold : textShape.fontBold;
		let fontFamily = params.fontFamily !== undefined ? params.fontFamily : textShape.fontFamily;
		let fontStr = String(fontSize) + "px" + " " + fontFamily;
		if(fontBold){
			fontStr = "bold " + fontStr;
		}
		let text = params.text !== undefined ? params.text : textShape.text;
		ctx.font = fontStr; //"20px sans-serif";
		let textWidth = ctx.measureText(text).width
		let point = points[0]
		let screenPoint = getScreenPosition(Qt.point(point.x, point.y))
		let text_x = screenPoint.x;
		let text_y = screenPoint.y;
		if(points.length == 2){
			let point2 = points[1]
			let screenPoint2 = getScreenPosition(Qt.point(point2.x, point2.y))
			let metrics = ctx.measureText(text)
			let textW = metrics.width
			let textH = fontSize
			text_x = screenPoint.x + ((screenPoint2.x - screenPoint.x)  - textW) / 2
			text_y = screenPoint.y + ((screenPoint2.y - screenPoint.y) + textH) / 2
		}
		ctx.beginPath()
		ctx.fillText(text, text_x, text_y);
		ctx.closePath();

	}

}
