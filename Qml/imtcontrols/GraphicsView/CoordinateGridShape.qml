import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

GraphicsShapeBase {

	id: gridShape;

	property Item viewItem: null
	property int axesMargin: 30;
	property int gridStep: 40;

	function draw(ctx, layerMatrix){
		let step = gridShape.gridStep;
		let margin = gridShape.axesMargin;

		ctx.lineCap = "round"
		ctx.lineJoin = "round"
		ctx.lineWidth = 1;

		ctx.fillStyle = Style.imagingToolsGradient1;
		ctx.strokeStyle = Style.imagingToolsGradient1;


		let verticalLineTopY = (gridShape.viewItem.drawingAreaHeight - margin) - Math.trunc((gridShape.viewItem.drawingAreaHeight - margin)/ step) * step;

		for(let i = 0; i * step < gridShape.viewItem.drawingAreaWidth - margin; i++){//vertical lines
			let x1 = margin + i * step;
			let y1 =  verticalLineTopY ;
			let x2 = x1;
			let y2 = gridShape.viewItem.drawingAreaHeight - margin ;

			ctx.beginPath()
			ctx.moveTo(x1, y1);
			ctx.lineTo(x2, y2);
			ctx.closePath()
			ctx.stroke();
		}

		for(let i = 0; i * step < gridShape.viewItem.drawingAreaHeight - margin; i++){//horizontal lines
			let x1 = margin ;
			let y1 =  gridShape.viewItem.drawingAreaHeight - i * step - margin;
			let x2 =  gridShape.viewItem.drawingAreaWidth ;
			let y2 =  y1;

			ctx.beginPath()
			ctx.moveTo(x1, y1);
			ctx.lineTo(x2, y2);
			ctx.closePath()
			ctx.stroke();
		}


		//labels for coordinate axis

		//"10px sans-serif"
		let fontSize = 14;
		ctx.font = String(fontSize) + "px sans-serif"//"14px sans-serif"

		ctx.strokeStyle = Style.borderColor;
		ctx.fillStyle = Style.borderColor2;

		for(let i = 1; i * step < gridShape.viewItem.drawingAreaWidth - margin; i++){
			let x_ = margin + i * step;
			let y_ = gridShape.viewItem.drawingAreaHeight  - margin + fontSize + Style.marginXS

			ctx.beginPath()
			let str = String(i * step)

			let textLength = ctx.measureText(str).width
			x_ = x_ - textLength/2

			ctx.fillText(str,x_, y_);
			ctx.closePath()
		}

		for(let i = 1; i * step < gridShape.viewItem.drawingAreaHeight - margin; i++){
			let x_ = margin;
			let y_ = gridShape.viewItem.drawingAreaHeight - i * step  -margin

			ctx.beginPath()
			let str = String(i * step)

			let textLength = ctx.measureText(str).width
			x_ = x_ - textLength - Style.marginXS

			ctx.fillText(str,x_, y_);
			ctx.closePath()
		}


		//coordinate axis
		ctx.strokeStyle = Style.borderColor;
		ctx.lineWidth = 2;
		ctx.beginPath()
		ctx.moveTo(0, gridShape.viewItem.drawingAreaHeight - margin);
		ctx.lineTo(gridShape.viewItem.drawingAreaWidth, gridShape.viewItem.drawingAreaHeight - margin);
		ctx.closePath()
		ctx.stroke();

		ctx.beginPath()
		ctx.moveTo(margin, 0);
		ctx.lineTo(margin, gridShape.viewItem.drawingAreaHeight);
		ctx.closePath()
		ctx.stroke();
	}
}

