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
	property bool isFixedOrigin: true;

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

		//Label names params
		let labelNameFontSize = fontSize + 2
		ctx.font = String(labelNameFontSize) + "px sans-serif"
		let labelXNameLength = ctx.measureText(gridShape.labelX).width
		let labelYNameLength = ctx.measureText(gridShape.labelY).width
		// let labelXNameWidth_ = Math.max(labelXNameLength + 10, 20)
		// let labelYNameHeight_ = labelNameFontSize + 4

		let marginForXNameLabel = (labelXNameLength + Style.marginS)
		let marginForYNameLabel = (labelNameFontSize + Style.marginS)

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

		//labeles background
		ctx.fillStyle = backgroundColor;
		ctx.fillRect(0, gridShape.viewItem.drawingAreaHeight - labelYHeight, labelXWidth, gridShape.viewItem.drawingAreaHeight)

		//legend background
		ctx.fillRect(0, 0, gridShape.viewItem.drawingAreaWidth, marginForYNameLabel * scaleCoeff)
		ctx.fillRect(gridShape.viewItem.drawingAreaWidth - marginForXNameLabel* scaleCoeff, 0, marginForXNameLabel * scaleCoeff, gridShape.viewItem.drawingAreaHeight)

		//Legend
		let strartX = gridShape.axesOrigin.x !== 0 && gridShape.axesOrigin.y !== 0? labelXWidth : 0
		let finishY = gridShape.axesOrigin.x !== 0 && gridShape.axesOrigin.y !== 0? labelYHeight : 0;

		let topYAxePoint = Qt.point(0,0);
		let bottomYAxePoint = Qt.point(0,gridShape.viewItem.drawingAreaHeight - finishY);
		let leftXAxePoint = Qt.point(strartX,0);
		let rightXAxePoint = Qt.point(gridShape.viewItem.drawingAreaWidth,0);

		if(!gridShape.isFixedOrigin){
			layerMatrix.setContextTransform(ctx);
			let invMatrix = layerMatrix.getInvertedMatrix();

			topYAxePoint = layerMatrix.transformPoint(topYAxePoint, invMatrix)
			bottomYAxePoint = layerMatrix.transformPoint(bottomYAxePoint, invMatrix)
			leftXAxePoint = layerMatrix.transformPoint(leftXAxePoint, invMatrix)
			rightXAxePoint = layerMatrix.transformPoint(rightXAxePoint, invMatrix)

			// topYAxePoint = layerMatrix.transformPoint(topYAxePoint)
			// bottomYAxePoint = layerMatrix.transformPoint(bottomYAxePoint)
			// leftXAxePoint = layerMatrix.transformPoint(leftXAxePoint)
			// rightXAxePoint = layerMatrix.transformPoint(rightXAxePoint)
		}
		// ctx.fillRect(gridShape.viewItem.drawingAreaWidth - labelXNameWidth_ ,gridShape.viewItem.drawingAreaHeight - labelYHeight, labelXWidth, gridShape.labelYHeight)
		// ctx.fillRect(0 ,0, gridShape.labelXWidth, labelYNameHeight_)

		//horizontal axe
		ctx.lineWidth = 2;
		ctx.strokeStyle = Style.borderColor;
		ctx.beginPath()
		ctx.moveTo(leftXAxePoint.x, gridShape.viewItem.drawingAreaHeight - labelYHeight - gridShape.axesOrigin.y);
		ctx.lineTo(rightXAxePoint.x - marginForXNameLabel, gridShape.viewItem.drawingAreaHeight - labelYHeight - gridShape.axesOrigin.y);
		ctx.closePath()
		ctx.stroke();

		//vertical axe
		ctx.beginPath()
		ctx.moveTo(labelXWidth + gridShape.axesOrigin.x, topYAxePoint.y + marginForXNameLabel);
		ctx.lineTo(labelXWidth + gridShape.axesOrigin.x, bottomYAxePoint.y);
		ctx.closePath()
		ctx.stroke();


		//Legend
		ctx.font = String(labelNameFontSize) + "px sans-serif"
		ctx.fillStyle = gridShape.fontColor;
		//ctx.fillText(gridShape.labelX, gridShape.viewItem.drawingAreaWidth - labelXNameLength, gridShape.viewItem.drawingAreaHeight  - labelYHeight + fontSize + Style.marginXS);
		// ctx.fillText(gridShape.labelY, Style.marginXS, Style.marginXS + labelNameFontSize/2);
		ctx.fillText(gridShape.labelX, rightXAxePoint.x - labelXNameLength -4, gridShape.viewItem.drawingAreaHeight  - labelYHeight  - gridShape.axesOrigin.y + labelNameFontSize/2 - 1);
		ctx.fillText(gridShape.labelY, gridShape.labelXWidth + gridShape.axesOrigin.x - labelYNameLength/2 -1, topYAxePoint.y + labelNameFontSize);

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

