import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

GraphicsShapeBase {
	function draw(ctx, layerId){
		let params = getParams(layerId)
		ctx.fillStyle = params.color !== undefined ? params.color : "transparent";
		ctx.strokeStyle = params.color !== undefined ? params.color : "transparent";
		ctx.lineWidth = params.lineWidth !== undefined ? params.lineWidth : 1;
		let fontSize = params.fontSize !== undefined ? params.fontSize : 10;
		let fontBold = params.fontBold !== undefined ? params.fontBold : false;
		let fontFamily = params.fontFamily !== undefined ? params.fontFamily : "sans-serif";
		let fontStr = String(fontSize) + "px" + " " + fontFamily;
		if(fontBold){
			fontStr = "bold " + fontStr;
		}
		let text = params.text !== undefined ? params.text : "";
		ctx.font = fontStr; //"20px sans-serif";
		let textWidth = ctx.measureText(text).width
		let text_x = params.point.x;
		let text_y = params.point.y;
		ctx.beginPath()
		ctx.fillText(text, text_x, text_y);
		ctx.closePath();

	}

}

