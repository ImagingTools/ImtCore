import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

BoundingBox {

	id: gridShape;

	property int labelXWidth: 36;
	property int labelYHeight: 40;
	property int gridStepMajor: 40;
	property int gridStepMinor: 10;
	property real thinningZoomLevel: 0.8;
	property int fontSize: Style.fontSizeM;
	property string fontColor: Style.borderColor2;
	property string backgroundColor: Style.baseColor;

	property string labelX: "X";
	property string labelY: "Y";

	property var axesOrigin: Qt.point(0, 0)

	property CanvasMatrix identityMatrix: CanvasMatrix{};
	property CanvasMatrix labelMatrix: CanvasMatrix{};

	function draw(ctx, layerMatrixArg){
		let deltaX = layerMatrixArg.xTranslation();
		let deltaY = layerMatrixArg.yTranslation();
		let scaleCoeff = layerMatrixArg.xScale();
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

		ctx.fillRect(labelXWidth,0, gridShape.viewItem.drawingAreaWidth - labelXWidth, gridShape.viewItem.drawingAreaHeight - labelYHeight)
		// ctx.fillRect(0,0, gridShape.viewItem.drawingAreaWidth, gridShape.viewItem.drawingAreaHeight)

		layerMatrix.setContextTransform(ctx);

		//GRID
		let verticalLineTopY = (gridShape.viewItem.drawingAreaHeight - labelYHeight) - (Math.trunc((gridShape.viewItem.drawingAreaHeight - labelYHeight)/ step) * step) - deltaAddY;
		verticalLineTopY /= scaleMax1
		//MINOR GRID
		if(scaleCoeff >= 2){
			ctx.strokeStyle = Style.imagingToolsGradient0;
			let step = gridShape.gridStepMinor
			for(let i = 0; i * step < gridShape.viewItem.drawingAreaWidth - labelXWidth + deltaAddX; i++){//vertical lines
				let x1 = labelXWidth + i * step;
				let y1 =  verticalLineTopY ;
				let x2 = x1;
				let y2 = gridShape.viewItem.drawingAreaHeight - labelYHeight ;

				ctx.beginPath()
				ctx.moveTo(x1, y1);
				ctx.lineTo(x2, y2);
				ctx.closePath()
				ctx.stroke();
			}

			for(let i = 0; i * step < gridShape.viewItem.drawingAreaHeight - labelYHeight  + deltaAddY; i++){//horizontal lines

				let x1 = labelXWidth ;
				let y1 =  gridShape.viewItem.drawingAreaHeight - i * step - labelYHeight;
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
		for(let i = 0; i * step  <= (gridShape.viewItem.drawingAreaWidth  /*- labelXWidth*/ + deltaAddX)/scaleCoeff; i++){//vertical lines
			if(gridShape.thinningCheck(scaleCoeff, i)){
				continue
			}
			let x1 = labelXWidth + i * step;
			let y1 =  verticalLineTopY ;
			let x2 = x1;
			let y2 = gridShape.viewItem.drawingAreaHeight  - labelYHeight ;

			ctx.beginPath()
			ctx.moveTo(x1, y1);
			ctx.lineTo(x2, y2);
			ctx.closePath()
			ctx.stroke();
		}
		//horizontal lines
		for(let i = 0; i * step <= (gridShape.viewItem.drawingAreaHeight - labelYHeight + deltaAddY)/ scaleMax1 ; i++){//horizontal lines
			if(gridShape.thinningCheck(scaleCoeff, i)){
				continue
			}
			let x1 = labelXWidth ;
			let y1 =  gridShape.viewItem.drawingAreaHeight - i * step - labelYHeight;
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
		ctx.fillRect(0,0, labelXWidth, gridShape.viewItem.drawingAreaHeight)
		ctx.fillRect(labelXWidth,gridShape.viewItem.drawingAreaHeight - labelYHeight,
					 gridShape.viewItem.drawingAreaWidth, gridShape.viewItem.drawingAreaHeight)


		//LABELS FOR COORDINATE AXIS

		ctx.font = String(fontSize) + "px sans-serif"//"14px sans-serif"

		ctx.strokeStyle = Style.borderColor;
		ctx.fillStyle = gridShape.fontColor;

		//label x
		labelMatrix.setXTranslation(deltaX);
		labelMatrix.setYTranslation(0);
		labelMatrix.setContextTransform(ctx);

		for(let i = 1; i * step  <= (gridShape.viewItem.drawingAreaWidth - labelXWidth + deltaAddX)/scaleCoeff; i++){
			if(gridShape.thinningCheck(scaleCoeff, i)){
				continue
			}
			let x_ = (labelXWidth + i * step) * scaleCoeff;
			let y_ = gridShape.viewItem.drawingAreaHeight  - labelYHeight + fontSize + Style.marginXS
			ctx.beginPath()
			let str = String(i * step - gridShape.axesOrigin.x)

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

		for(let i = 1; i * step * scaleCoeff < (gridShape.viewItem.drawingAreaHeight - labelYHeight + deltaAddY) * scaleCoeff / scaleMax1; i++){
			if(gridShape.thinningCheck(scaleCoeff, i)){
				continue
			}
			let x_ = labelXWidth;
			let y_ = (gridShape.viewItem.drawingAreaHeight - i * step  - labelYHeight)* scaleCoeff
			//console.log("deltaAddY:: ", y_, deltaAddY, (gridShape.viewItem.drawingAreaHeight - labelYHeight + deltaAddY * scaleCoeff))

			ctx.beginPath()
			let str = String(i * step - gridShape.axesOrigin.y)

			let textLength = ctx.measureText(str).width
			x_ = x_ - textLength - Style.marginXS

			ctx.fillText(str,x_, y_);
			ctx.closePath()
		}

		//coordinate axis

		identityMatrix.setContextTransform(ctx);

		//axes background
		ctx.fillStyle = backgroundColor;
		ctx.fillRect(0,gridShape.viewItem.drawingAreaHeight - labelYHeight, labelXWidth, gridShape.viewItem.drawingAreaHeight)

		//Label names
		let labelFontSize = fontSize + 2
		ctx.font = String(labelFontSize) + "px sans-serif"
		let labelXLength = ctx.measureText(gridShape.labelX).width
		let labelYLength = ctx.measureText(gridShape.labelY).width
		let labelXWidth_ = Math.max(labelXLength + 10, 20)
		let labelYHeight_ = labelFontSize + 4

		ctx.fillRect(gridShape.viewItem.drawingAreaWidth - labelXWidth_ ,gridShape.viewItem.drawingAreaHeight - labelYHeight, labelXWidth, gridShape.labelYHeight)
		ctx.fillRect(0 ,0, gridShape.labelXWidth, labelYHeight_)

		ctx.fillStyle = gridShape.fontColor;
		ctx.fillText(gridShape.labelX, gridShape.viewItem.drawingAreaWidth - labelXLength, gridShape.viewItem.drawingAreaHeight  - labelYHeight + fontSize + Style.marginXS);
		ctx.fillText(gridShape.labelY, Style.marginXS, Style.marginXS + labelFontSize/2);

		//horizontal axe
		ctx.lineWidth = 2;
		ctx.strokeStyle = Style.borderColor;
		ctx.beginPath()
		let strartX = gridShape.axesOrigin.x !== 0 && gridShape.axesOrigin.y !== 0? labelXWidth : 0
		ctx.moveTo(strartX, gridShape.viewItem.drawingAreaHeight - labelYHeight - gridShape.axesOrigin.y);
		ctx.lineTo(gridShape.viewItem.drawingAreaWidth, gridShape.viewItem.drawingAreaHeight - labelYHeight - gridShape.axesOrigin.y);
		ctx.closePath()
		ctx.stroke();

		//vertical axe
		ctx.beginPath()
		let finishY = gridShape.axesOrigin.x !== 0 && gridShape.axesOrigin.y !== 0? labelYHeight : 0;
		ctx.moveTo(labelXWidth + gridShape.axesOrigin.x, 0);
		ctx.lineTo(labelXWidth + gridShape.axesOrigin.x, gridShape.viewItem.drawingAreaHeight - finishY);
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

