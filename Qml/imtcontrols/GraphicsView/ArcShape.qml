import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

GraphicsShapeBase {
	function draw(ctx, layerMatrix){
		let params = getParams()
		ctx.strokeStyle = params.color !== undefined ? params.color : "transparent";
		ctx.lineWidth = params.lineWidth !== undefined ? params.lineWidth : 0
		let radius = params.radius !== undefined ? params.radius : 0
		let startAngle = params.startAngle;
		let endAngle = params.endAngle;
		let anticlockwise = params.anticlockwise
		ctx.beginPath()
		ctx.arc(params.point.x, params.point.y, radius,  startAngle, endAngle, anticlockwise)
		ctx.stroke();
		ctx.closePath();
	}
}

