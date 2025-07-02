import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

GraphicsShapeBase {

	function draw(ctx){
		let params = getParams()
		ctx.fillStyle = params.color !== undefined ? params.color : "transparent";
		ctx.strokeStyle = params.borderColor !== undefined ? params.borderColor : "transparent";
		ctx.lineWidth = params.lineWidth !== undefined ? params.lineWidth : 0
		ctx.beginPath()
		for(let i = 0;i < params.points.length; i++){
			let point = params.points[i];
			if(i == 0){
				ctx.moveTo(point.x, point.y);
			}
			else {
				ctx.lineTo(point.x, point.y);
			}

		}
		ctx.stroke();
		ctx.fill();
		ctx.closePath();
	}


}

