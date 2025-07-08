import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

GraphicsShapeBase {

	id: gridShape;

	property Item viewItem: null
	property int originX: 36;
	property int originY: 40;
	property int gridStepMajor: 40;
	property int gridStepMinor: 10;
	property real thinningZoomLevel: 0.8;
	property int fontSize: Style.fontSizeM
	property string backgroundColor: Style.baseColor;

	property string labelX: "X";
	property string labelY: "Y";

	property CanvasMatrix identityMatrix: CanvasMatrix{};
	property CanvasMatrix labelMatrix: CanvasMatrix{};

	function draw(ctx, layerMatrix){
		let deltaX = layerMatrix.xTranslation();
		let deltaY = layerMatrix.yTranslation();
		let scaleCoeff = layerMatrix.xScale();
		let scaleMax1 = Math.min(1, scaleCoeff)

		let deltaAddX = deltaX > 0 ? 0 : -deltaX
		let deltaAddY = deltaY <= 0 ? 0 : deltaY

		let step = gridShape.gridStepMajor;

		ctx.lineCap = "round"
		ctx.lineJoin = "round"
		ctx.lineWidth = 1;

		ctx.fillStyle = backgroundColor;
		ctx.strokeStyle = Style.imagingToolsGradient1;

		//background
		identityMatrix.setContextTransform(ctx);

		ctx.fillRect(originX,0, gridShape.viewItem.drawingAreaWidth - originX, gridShape.viewItem.drawingAreaHeight - originY)
		// ctx.fillRect(0,0, gridShape.viewItem.drawingAreaWidth, gridShape.viewItem.drawingAreaHeight)

		layerMatrix.setContextTransform(ctx);

		//GRID
		let verticalLineTopY = (gridShape.viewItem.drawingAreaHeight - originY) - (Math.trunc((gridShape.viewItem.drawingAreaHeight - originY)/ step) * step) - deltaAddY;
		verticalLineTopY /= scaleMax1
		//MINOR GRID
		if(scaleCoeff >= 2){
			ctx.strokeStyle = Style.imagingToolsGradient0;
			let step = gridShape.gridStepMinor
			for(let i = 0; i * step < gridShape.viewItem.drawingAreaWidth - originX + deltaAddX; i++){//vertical lines
				let x1 = originX + i * step;
				let y1 =  verticalLineTopY ;
				let x2 = x1;
				let y2 = gridShape.viewItem.drawingAreaHeight - originY ;

				ctx.beginPath()
				ctx.moveTo(x1, y1);
				ctx.lineTo(x2, y2);
				ctx.closePath()
				ctx.stroke();
			}

			for(let i = 0; i * step < gridShape.viewItem.drawingAreaHeight - originY  + deltaAddY; i++){//horizontal lines

				let x1 = originX ;
				let y1 =  gridShape.viewItem.drawingAreaHeight - i * step - originY;
				let x2 =  gridShape.viewItem.drawingAreaWidth + deltaAddX ;
				let y2 =  y1;

				ctx.beginPath()
				ctx.moveTo(x1, y1);
				ctx.lineTo(x2, y2);
				ctx.closePath()
				ctx.stroke();
			}
		}

		//MAJOR GRID
		ctx.strokeStyle = Style.imagingToolsGradient1;
		//vertical lines
		for(let i = 0; i * step  <= (gridShape.viewItem.drawingAreaWidth  /*- originX*/ + deltaAddX)/scaleCoeff; i++){//vertical lines
			if(gridShape.thinningCheck(scaleCoeff, i)){
				continue
			}
			let x1 = originX + i * step;
			let y1 =  verticalLineTopY ;
			let x2 = x1;
			let y2 = gridShape.viewItem.drawingAreaHeight  - originY ;

			ctx.beginPath()
			ctx.moveTo(x1, y1);
			ctx.lineTo(x2, y2);
			ctx.closePath()
			ctx.stroke();
		}
		//horizontal lines
		for(let i = 0; i * step <= (gridShape.viewItem.drawingAreaHeight - originY + deltaAddY)/ scaleMax1 ; i++){//horizontal lines
			if(gridShape.thinningCheck(scaleCoeff, i)){
				continue
			}
			let x1 = originX ;
			let y1 =  gridShape.viewItem.drawingAreaHeight - i * step - originY;
			let x2 =  (gridShape.viewItem.drawingAreaWidth  + deltaAddX)/scaleCoeff ;
			let y2 =  y1;

			ctx.beginPath()
			ctx.moveTo(x1, y1);
			ctx.lineTo(x2, y2);
			ctx.closePath()
			ctx.stroke();
		}


		//AXES BACKGROUND
		identityMatrix.setContextTransform(ctx);

		ctx.fillStyle = backgroundColor;
		ctx.fillRect(0,0, originX, gridShape.viewItem.drawingAreaHeight)
		ctx.fillRect(originX,gridShape.viewItem.drawingAreaHeight - originY,
					 gridShape.viewItem.drawingAreaWidth, gridShape.viewItem.drawingAreaHeight)


		//LABELS FOR COORDINATE AXIS

		ctx.font = String(fontSize) + "px sans-serif"//"14px sans-serif"

		ctx.strokeStyle = Style.borderColor;
		ctx.fillStyle = Style.borderColor2;

		//label x
		labelMatrix.setXTranslation(deltaX);
		labelMatrix.setYTranslation(0);
		labelMatrix.setContextTransform(ctx);

		for(let i = 1; i * step  <= (gridShape.viewItem.drawingAreaWidth - originX + deltaAddX)/scaleCoeff; i++){
			if(gridShape.thinningCheck(scaleCoeff, i)){
				continue
			}
			let x_ = (originX + i * step) * scaleCoeff;
			let y_ = gridShape.viewItem.drawingAreaHeight  - originY + fontSize + Style.marginXS
			ctx.beginPath()
			let str = String(i * step)

			let textLength = ctx.measureText(str).width
			x_ = x_ - textLength/2

			ctx.fillText(str,x_, y_);
			ctx.closePath()
		}


		//LABEL Y
		labelMatrix.reset()
		//labelMatrix.setXTranslation(0);
		labelMatrix.setYTranslation(deltaY);
		labelMatrix.setContextTransform(ctx);

		for(let i = 1; i * step * scaleCoeff < (gridShape.viewItem.drawingAreaHeight - originY + deltaAddY) * scaleCoeff / scaleMax1; i++){
			if(gridShape.thinningCheck(scaleCoeff, i)){
				continue
			}
			let x_ = originX;
			let y_ = (gridShape.viewItem.drawingAreaHeight - i * step  - originY)* scaleCoeff
			//console.log("deltaAddY:: ", y_, deltaAddY, (gridShape.viewItem.drawingAreaHeight - originY + deltaAddY * scaleCoeff))

			ctx.beginPath()
			let str = String(i * step)

			let textLength = ctx.measureText(str).width
			x_ = x_ - textLength - Style.marginXS

			ctx.fillText(str,x_, y_);
			ctx.closePath()
		}


		//coordinate axis

		identityMatrix.setContextTransform(ctx);

		//axes background
		ctx.fillStyle = backgroundColor;
		ctx.fillRect(0,gridShape.viewItem.drawingAreaHeight - originY, originX, gridShape.viewItem.drawingAreaHeight)

		//Label names
		let labelFontSize = fontSize + 2
		ctx.font = String(labelFontSize) + "px sans-serif"
		let labelXLength = ctx.measureText(gridShape.labelX).width
		let labelYLength = ctx.measureText(gridShape.labelY).width
		let labelXWidth = Math.max(labelXLength + 10, 20)
		let labelYHeight = labelFontSize + 4

		ctx.fillRect(gridShape.viewItem.drawingAreaWidth - labelXWidth ,gridShape.viewItem.drawingAreaHeight - originY, labelXWidth, gridShape.originY)
		ctx.fillRect(0 ,0, gridShape.originX, labelYHeight)

		ctx.fillStyle = Style.borderColor2;
		ctx.fillText(gridShape.labelX, gridShape.viewItem.drawingAreaWidth - labelXLength, gridShape.viewItem.drawingAreaHeight  - originY + fontSize + Style.marginXS);
		ctx.fillText(gridShape.labelY, Style.marginXS, Style.marginXS + labelFontSize/2);

		//horizontal axe
		ctx.lineWidth = 2;
		ctx.strokeStyle = Style.borderColor;
		ctx.beginPath()
		ctx.moveTo(0, gridShape.viewItem.drawingAreaHeight - originY);
		ctx.lineTo(gridShape.viewItem.drawingAreaWidth, gridShape.viewItem.drawingAreaHeight - originY);
		ctx.closePath()
		ctx.stroke();

		//vertical axe
		ctx.beginPath()
		ctx.moveTo(originX, 0);
		ctx.lineTo(originX, gridShape.viewItem.drawingAreaHeight);
		ctx.closePath()
		ctx.stroke();
	}

	function thinningCheck(scaleCoeff, i){
		let ok = false;
		if(scaleCoeff < gridShape.thinningZoomLevel && i%2 !== 0){
			ok = true
		}
		if(scaleCoeff < gridShape.thinningZoomLevel/2 && i%(2*2) !== 0){
			ok = true;
		}

		return ok;
	}
}

