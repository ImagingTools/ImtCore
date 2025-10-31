import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0


BoundingBox {

	id: gridShape;

	property int labelYWidth: 36;
	property int labelXHeight: 40;
	property real gridStepMajorX: 40;
	property real gridStepMinorX: 10;
	property real gridStepMajorY: 40;
	property real gridStepMinorY: 10;
	property real thinningZoomLevel: 0.8;
	property int fontSize: Style.fontSizeM;
	property string fontColor: Style.borderColor2;
	property string backgroundColor: Style.baseColor;
	property string majorGridColor: Style.borderColor2;
	property string minorGridColor: Style.borderColor;
	property real majorGridOpacity: 0.5
	property string axesColor: Style.borderColor2;

	property string labelX: "X";
	property string labelY: "Y";

	property var axesOrigin: Qt.point(0, 0)
	property bool isFixedOrigin: true;
	property bool cutAxesEnds: false;
	property int legendMargin: -1;

	property bool  canDrawText: true;
	property bool showGridLines: true;
	property bool showGridFrame: !showGridLines;
	property bool hasMinorGrid: true;

	property int labelPrecision : 0;
	property real lableXOriginMargin: 0

	property CanvasMatrix labelMatrix: CanvasMatrix{};


	function getLogicalMajorLineX(lineX){
		return lineX
	}
	function getLogicalMajorLineY(lineY){
		return lineY
	}

	function getLabelX(xArg){//for overriding
		return getLabelXBase(xArg)
	}
	function getLabelY(yArg){//for overriding
		return getLabelYBase(yArg)
	}
	function getLabelXBase(xArg){
		return xArg
	}
	function getLabelYBase(yArg){
		return yArg
	}

	function draw(ctx, transformMatrixArg){
		let deltaX = transformMatrixArg.xTranslation();
		let deltaY = transformMatrixArg.yTranslation();
		let scaleCoeff = transformMatrixArg.xScale();
		let scaleMax1 = Math.min(1, scaleCoeff)

		let deltaAddX = deltaX > 0 ? 0 : -deltaX
		let deltaAddY = deltaY <= 0 ? 0 : deltaY

		let deltaMinusX = deltaX > 0 ? deltaX : 0
		let deltaMinusY = deltaY < 0 ? -deltaY : 0

		let stepX = gridShape.gridStepMajorX;
		let stepY = gridShape.gridStepMajorY;

		let step_thinningX = scaleCoeff > gridShape.thinningZoomLevel ? stepX : scaleCoeff > gridShape.thinningZoomLevel/2 ? stepX * 2  : stepX * 4
		let step_thinningY = scaleCoeff > gridShape.thinningZoomLevel ? stepY : scaleCoeff > gridShape.thinningZoomLevel/2 ? stepY * 2  : stepY * 4
		let firstVertLineX = axesOrigin.x / step_thinningX < 1 ? axesOrigin.x : axesOrigin.x % step_thinningX
		let firstHorizLineY = axesOrigin.y / step_thinningY < 1 ? axesOrigin.y : axesOrigin.y % step_thinningY

		//Label names params
		let labelNameFontSize = fontSize + 2
		ctx.font = String(labelNameFontSize) + "px sans-serif"
		let labelXNameLength = ctx.measureText(gridShape.labelX).width
		let labelYNameLength = ctx.measureText(gridShape.labelY).width
		// let labelXNameWidth_ = Math.max(labelXNameLength + 10, 20)
		// let labelYNameHeight_ = labelNameFontSize + 4

		let marginForXNameLabel = gridShape.legendMargin > -1 ? gridShape.legendMargin : (labelXNameLength + Style.marginS)
		let marginForYNameLabel = gridShape.legendMargin > -1 ? gridShape.legendMargin : (labelNameFontSize + Style.marginS)

		ctx.lineCap = "round"
		ctx.lineJoin = "round"
		ctx.lineWidth = 1;

		ctx.fillStyle = backgroundColor;
		ctx.strokeStyle = gridShape.majorGridColor;

		//background

		ctx.fillRect(labelYWidth,0, gridShape.viewItem.drawingAreaWidth - labelYWidth, gridShape.viewItem.drawingAreaHeight - labelXHeight)
		// ctx.fillRect(0,0, gridShape.viewItem.drawingAreaWidth, gridShape.viewItem.drawingAreaHeight)


		//GRID
		let verticalLineTopY = (gridShape.viewItem.drawingAreaHeight - labelXHeight) - (Math.trunc((gridShape.viewItem.drawingAreaHeight - labelXHeight)/ stepY) * stepY) - deltaAddY;
		verticalLineTopY /= scaleMax1

		//MINOR GRID

		//let hasMinorGrid = scaleCoeff >= 2
		if(gridShape.showGridLines){
			let hasMinorGrid = gridShape.gridStepMajorX * scaleCoeff > 40 && gridShape.hasMinorGrid
			if(hasMinorGrid){
				ctx.strokeStyle = gridShape.minorGridColor;

				let stepMinorX = gridShape.gridStepMinorX
				let stepMinorY = gridShape.gridStepMinorY
				//vertical minor lines
				for(let i = 0; i * stepMinorX < gridShape.viewItem.drawingAreaWidth - labelYWidth + deltaAddX; i++){//vertical lines
					let x1 = firstVertLineX + labelYWidth + i * stepMinorX;
					let y1 =  verticalLineTopY ;
					let x2 = x1;
					let y2 = gridShape.viewItem.drawingAreaHeight - labelXHeight ;

					let point1 = getScreenPosition(Qt.point(x1, y1))
					let point2 = getScreenPosition(Qt.point(x2, y2))
					x1 = point1.x;
					y1 = point1.y
					x2 = point2.x;
					//y2 = point2.y

					ctx.beginPath()
					ctx.moveTo(x1, y1);
					ctx.lineTo(x2, y2);
					ctx.closePath()
					ctx.stroke();
				}
				//horizontal minor lines
				for(let i = 0; i * stepMinorY < gridShape.viewItem.drawingAreaHeight - labelXHeight  + deltaAddY; i++){//horizontal lines
					let x1 = labelYWidth ;
					let y1 =  gridShape.viewItem.drawingAreaHeight - i * stepMinorY - labelXHeight - firstHorizLineY;
					let x2 =  gridShape.viewItem.drawingAreaWidth + deltaAddX ;
					let y2 =  y1;

					let point1 = getScreenPosition(Qt.point(x1, y1))
					let point2 = getScreenPosition(Qt.point(x2, y2))
					//x1 = point1.x;
					y1 = point1.y
					x2 = point2.x;
					y2 = point2.y

					ctx.beginPath()
					ctx.moveTo(x1, y1);
					ctx.lineTo(x2, y2);
					ctx.closePath()
					ctx.stroke();
				}
			}

			//MAJOR GRID
			ctx.strokeStyle = gridShape.majorGridColor;
			ctx.globalAlpha = gridShape.majorGridOpacity
			//vertical major lines
			for(let i = 0; i * stepX  <= (gridShape.viewItem.drawingAreaWidth  /*- labelYWidth*/ + deltaAddX)/scaleCoeff; i++){//vertical lines
				if(gridShape.thinningCheck(scaleCoeff, i)){
					continue
				}
				let x1 = firstVertLineX + labelYWidth + i * stepX;
				let y1 =  verticalLineTopY ;
				let x2 = x1;
				let y2 = gridShape.viewItem.drawingAreaHeight  - labelXHeight ;

				let point1 = getScreenPosition(Qt.point(x1, y1))
				let point2 = getScreenPosition(Qt.point(x2, y2))
				x1 = point1.x;
				y1 = point1.y
				x2 = point2.x;
				//y2 = point2.y

				ctx.beginPath()
				ctx.moveTo(x1, y1);
				ctx.lineTo(x2, y2);
				ctx.closePath()
				ctx.stroke();
			}
			//horizontal major lines
			//ctx.strokeStyle = gridShape.majorGridColor;
			for(let i = 0; i * stepY <= (gridShape.viewItem.drawingAreaHeight - labelXHeight + deltaAddY)/ scaleMax1 ; i++){//horizontal lines
				if(gridShape.thinningCheck(scaleCoeff, i)){
					continue
				}
				let x1 = labelYWidth ;
				let y1 =  gridShape.viewItem.drawingAreaHeight - i * stepY - labelXHeight - firstHorizLineY;
				let x2 =  (gridShape.viewItem.drawingAreaWidth  + deltaAddX)/scaleCoeff ;
				let y2 =  y1;

				let point1 = getScreenPosition(Qt.point(x1, y1))
				let point2 = getScreenPosition(Qt.point(x2, y2))
				//x1 = point1.x;
				y1 = point1.y
				x2 = point2.x;
				y2 = point2.y

				ctx.beginPath()
				ctx.moveTo(x1, y1);
				ctx.lineTo(x2, y2);
				ctx.closePath()
				ctx.stroke();
			}

			ctx.globalAlpha = 1

			// completing the grid to zero
			//MINOR GRID
			if(hasMinorGrid){
				//ctx.strokeStyle = "violet"
				ctx.strokeStyle = gridShape.minorGridColor;

				let stepMinorX = gridShape.gridStepMinorX
				let stepMinorY = gridShape.gridStepMinorY
				//vertical minor lines
				for(let i = 1; i * stepMinorX <= (deltaMinusX)/scaleCoeff + stepX; i++){//vertical lines
					let x1 = firstVertLineX + labelYWidth - i * stepMinorX;
					let y1 =  verticalLineTopY ;
					let x2 = x1;
					let y2 = gridShape.viewItem.drawingAreaHeight - labelXHeight ;

					let point1 = getScreenPosition(Qt.point(x1, y1))
					let point2 = getScreenPosition(Qt.point(x2, y2))
					x1 = point1.x;
					y1 = point1.y
					x2 = point2.x;
					//y2 = point2.y

					ctx.beginPath()
					ctx.moveTo(x1, y1);
					ctx.lineTo(x2, y2);
					ctx.closePath()
					ctx.stroke();
				}
				//horizontal minor lines
				//ctx.strokeStyle = ctx.strokeStyle = "blue"
				for(let i = 1; i * stepMinorY <= (deltaMinusY + gridShape.viewItem.drawingAreaHeight) / (scaleCoeff); i++){//horizontal lines

					let x1 = labelYWidth ;
					let y1 =  gridShape.viewItem.drawingAreaHeight + i * stepMinorY - labelXHeight - firstHorizLineY;
					let x2 =  gridShape.viewItem.drawingAreaWidth + deltaAddX ;
					let y2 =  y1;

					let point1 = getScreenPosition(Qt.point(x1, y1))
					let point2 = getScreenPosition(Qt.point(x2, y2))
					//x1 = point1.x;
					y1 = point1.y
					x2 = point2.x;
					y2 = point2.y

					ctx.beginPath()
					ctx.moveTo(x1, y1);
					ctx.lineTo(x2, y2);
					ctx.closePath()
					ctx.stroke();
				}
			}

			ctx.globalAlpha = gridShape.majorGridOpacity

			//MAJOR GRID
			//ctx.strokeStyle = "red"
			ctx.strokeStyle = gridShape.majorGridColor;
			//vertical major lines
			for(let i = 1; i * stepX  <= deltaMinusX/scaleCoeff + stepX; i++){//vertical lines
				if(gridShape.thinningCheck(scaleCoeff, i)){
					continue
				}
				let x1 = firstVertLineX + labelYWidth -i * stepX;
				let y1 =  verticalLineTopY ;
				let x2 = x1;
				let y2 = gridShape.viewItem.drawingAreaHeight  - labelXHeight;

				let point1 = getScreenPosition(Qt.point(x1, y1))
				let point2 = getScreenPosition(Qt.point(x2, y2))
				x1 = point1.x;
				y1 = point1.y
				x2 = point2.x;
				//y2 = point2.y

				ctx.beginPath()
				ctx.moveTo(x1, y1);
				ctx.lineTo(x2, y2);
				ctx.closePath()
				ctx.stroke();

			}
			//horizontal major lines
			//ctx.strokeStyle = "green"
			for(let i = 1; i * stepY <= (deltaMinusY + gridShape.viewItem.drawingAreaHeight) / (scaleCoeff) ; i++){//horizontal lines
				if(gridShape.thinningCheck(scaleCoeff, i)){
					continue
				}
				let x1 = labelYWidth ;
				let y1 =  gridShape.viewItem.drawingAreaHeight + i * stepY - labelXHeight - firstHorizLineY;
				let x2 =  (gridShape.viewItem.drawingAreaWidth  + deltaAddX)/scaleCoeff ;
				let y2 =  y1;

				let point1 = getScreenPosition(Qt.point(x1, y1))
				let point2 = getScreenPosition(Qt.point(x2, y2))
				//x1 = point1.x;
				y1 = point1.y
				x2 = point2.x;
				y2 = point2.y

				ctx.beginPath()
				ctx.moveTo(x1, y1);
				ctx.lineTo(x2, y2);
				ctx.closePath()
				ctx.stroke();
			}

			ctx.globalAlpha = 1
		}

		//AXES BACKGROUND

		ctx.fillStyle = backgroundColor;
		ctx.fillRect(0,0, labelYWidth, gridShape.viewItem.drawingAreaHeight)
		ctx.fillRect(labelYWidth,gridShape.viewItem.drawingAreaHeight - labelXHeight,
					 gridShape.viewItem.drawingAreaWidth, gridShape.viewItem.drawingAreaHeight)


		//LABELS FOR COORDINATE AXIS

		if(gridShape.canDrawText){
			ctx.font = String(fontSize) + "px sans-serif"//"14px sans-serif"

			ctx.strokeStyle = gridShape.axesColor;
			ctx.fillStyle = gridShape.fontColor;

			// let stepXLog = getLogicalMajorLineX(stepX)
			// let stepYLog = getLogicalMajorLineY(stepY)

			//LABEL Y
			labelMatrix.reset()
			labelMatrix.setYTranslation(deltaY);

			for(let i = 1; i * stepY * scaleCoeff < (gridShape.viewItem.drawingAreaHeight - labelXHeight + deltaAddY) * scaleCoeff / scaleMax1; i++){
				if(gridShape.thinningCheck(scaleCoeff, i)){
					continue
				}
				let x_ = labelYWidth;
				let y_ = (gridShape.viewItem.drawingAreaHeight - i * stepY - labelXHeight - firstHorizLineY)* scaleCoeff
				y_ = LinearAlgebra.transformPoint2d(Qt.point(x_, y_), labelMatrix.matrix).y

				//console.log("deltaAddY:: ", y_, deltaAddY, (gridShape.viewItem.drawingAreaHeight - labelXHeight + deltaAddY * scaleCoeff))

				let str = gridShape.getLabelY(String(getLogicalMajorLineY(Number(i * stepY - gridShape.axesOrigin.y + firstHorizLineY).toFixed(gridShape.labelPrecision))))
				if(str == ""){
					continue
				}
				ctx.beginPath()

				let textLength = ctx.measureText(str).width
				x_ = x_ - textLength - Style.marginXS

				ctx.fillText(str,x_, y_);
				ctx.closePath()
			}
			// completing label y to zero
			for(let i = 0; i * stepY  <= (deltaMinusY + gridShape.viewItem.drawingAreaHeight) / (scaleCoeff); i++){
				if(gridShape.thinningCheck(scaleCoeff, i)){
					continue
				}
				let x_ = labelYWidth;
				let y_ = (gridShape.viewItem.drawingAreaHeight + i * stepY  - labelXHeight - firstHorizLineY)* scaleCoeff
				y_ = LinearAlgebra.transformPoint2d(Qt.point(x_, y_), labelMatrix.matrix).y

				let str = gridShape.getLabelY(String(getLogicalMajorLineY(Number(firstHorizLineY -i * stepY - gridShape.axesOrigin.y).toFixed(gridShape.labelPrecision))))
				if(str == ""){
					continue
				}

				ctx.beginPath()

				let textLength = ctx.measureText(str).width
				x_ = x_ - textLength - Style.marginXS

				ctx.fillText(str,x_, y_);
				ctx.closePath()
			}

			//labeles background
			ctx.fillStyle = backgroundColor;
			ctx.fillRect(0, gridShape.viewItem.drawingAreaHeight - labelXHeight, labelYWidth, gridShape.viewItem.drawingAreaHeight)

			//label x
			ctx.fillStyle = gridShape.fontColor;
			labelMatrix.reset()
			labelMatrix.setXTranslation(deltaX);

			for(let i = 1; i * stepX  <= (gridShape.viewItem.drawingAreaWidth - labelYWidth + deltaAddX)/scaleCoeff; i++){
				if(gridShape.thinningCheck(scaleCoeff, i)){
					continue
				}
				let x_ = (firstVertLineX + labelYWidth + i * stepX) * scaleCoeff;
				let y_ = gridShape.viewItem.drawingAreaHeight  - labelXHeight + fontSize + Style.marginXS
				x_ = LinearAlgebra.transformPoint2d(Qt.point(x_, y_), labelMatrix.matrix).x

				let str = gridShape.getLabelX(String(getLogicalMajorLineX(Number(firstVertLineX + i * stepX - gridShape.axesOrigin.x).toFixed(gridShape.labelPrecision))))
				if(str == ""){
					continue
				}

				ctx.beginPath()

				let textLength = ctx.measureText(str).width
				x_ = x_ - textLength/2

				ctx.fillText(str,x_, y_);
				ctx.closePath()
			}
			// completing label x to zero
			for(let i = 0; i * stepX  <= deltaMinusX/scaleCoeff + stepX; i++){
				if(gridShape.thinningCheck(scaleCoeff, i)){
					continue
				}
				let x_ = (firstVertLineX + labelYWidth - i * stepX) * scaleCoeff;
				let y_ = gridShape.viewItem.drawingAreaHeight  - labelXHeight + fontSize + Style.marginXS
				x_ = LinearAlgebra.transformPoint2d(Qt.point(x_, y_), labelMatrix.matrix).x

				let str = gridShape.getLabelX(String(getLogicalMajorLineX(Number(firstVertLineX -i * stepX - gridShape.axesOrigin.x).toFixed(gridShape.labelPrecision))))
				if(str == ""){
					continue
				}

				ctx.beginPath()

				let textLength = ctx.measureText(str).width
				x_ = x_ - textLength/2

				ctx.fillText(str,x_, y_);
				ctx.closePath()
			}


		}

		//coordinate axis


		//labeles background
		ctx.fillStyle = backgroundColor;
		ctx.fillRect(0, gridShape.viewItem.drawingAreaHeight - labelXHeight, labelYWidth - lableXOriginMargin, gridShape.viewItem.drawingAreaHeight)

		//legend background
		ctx.fillStyle = backgroundColor;
		ctx.fillRect(0, 0, gridShape.viewItem.drawingAreaWidth, marginForYNameLabel)
		ctx.fillRect(gridShape.viewItem.drawingAreaWidth - marginForXNameLabel, 0, marginForXNameLabel, gridShape.viewItem.drawingAreaHeight)

		//Legend
		let startX = (gridShape.axesOrigin.x !== 0 && gridShape.axesOrigin.y !== 0) || gridShape.cutAxesEnds ? labelYWidth : 0
		let finishY = (gridShape.axesOrigin.x !== 0 && gridShape.axesOrigin.y !== 0) || gridShape.cutAxesEnds ? labelXHeight : 0;

		let topYAxePoint = Qt.point(labelYWidth + gridShape.axesOrigin.x,0);
		let bottomYAxePoint = Qt.point(labelYWidth + gridShape.axesOrigin.x,gridShape.viewItem.drawingAreaHeight - finishY);
		let leftXAxePoint = Qt.point(startX,gridShape.viewItem.drawingAreaHeight - labelXHeight - gridShape.axesOrigin.y);
		let rightXAxePoint = Qt.point(gridShape.viewItem.drawingAreaWidth, gridShape.viewItem.drawingAreaHeight - labelXHeight - gridShape.axesOrigin.y);

		let topYAxePointTr = Qt.point(topYAxePoint.x,topYAxePoint.y);
		let bottomYAxePointTr = Qt.point(bottomYAxePoint.x, bottomYAxePoint.y);
		let leftXAxePointTr = Qt.point(leftXAxePoint.x,leftXAxePoint.y);
		let rightXAxePointTr = Qt.point(rightXAxePoint.x, rightXAxePoint.y);

		if(!gridShape.isFixedOrigin){
			topYAxePointTr = getScreenPosition(topYAxePoint)
			bottomYAxePointTr = getScreenPosition(bottomYAxePoint)
			leftXAxePointTr = getScreenPosition(leftXAxePoint)
			rightXAxePointTr = getScreenPosition(rightXAxePoint)
		}

		//horizontal axe
		ctx.globalAlpha = gridShape.majorGridOpacity

		ctx.lineWidth = 2;
		ctx.strokeStyle = gridShape.axesColor;
		ctx.beginPath()
		ctx.moveTo(leftXAxePoint.x, leftXAxePointTr.y);
		ctx.lineTo(rightXAxePoint.x - marginForXNameLabel, rightXAxePointTr.y);
		ctx.closePath()
		ctx.stroke();

		//vertical axe
		ctx.beginPath()
		ctx.moveTo(topYAxePointTr.x, topYAxePoint.y + marginForYNameLabel);
		ctx.lineTo(bottomYAxePointTr.x, bottomYAxePoint.y);
		ctx.closePath()
		ctx.stroke();


		//frame
		if(gridShape.showGridFrame){

			let leftBottomFrame = Qt.point(labelYWidth, gridShape.viewItem.drawingAreaHeight - labelXHeight)
			let leftTopFrame = Qt.point(labelYWidth, marginForYNameLabel)
			let rightTopFrame = Qt.point(gridShape.viewItem.drawingAreaWidth - marginForXNameLabel, marginForYNameLabel)
			let rightBottomFrame = Qt.point(gridShape.viewItem.drawingAreaWidth - marginForXNameLabel, gridShape.viewItem.drawingAreaHeight - labelXHeight)

			ctx.beginPath()
			ctx.moveTo(leftBottomFrame.x, leftBottomFrame.y);
			ctx.lineTo(leftTopFrame.x, leftTopFrame.y);
			ctx.lineTo(rightTopFrame.x, rightTopFrame.y);
			ctx.lineTo(rightBottomFrame.x, rightBottomFrame.y);
			ctx.closePath()
			ctx.stroke();

		}


		ctx.globalAlpha = 1



		//Legend
		ctx.font = String(labelNameFontSize) + "px sans-serif"
		ctx.fillStyle = gridShape.fontColor;
		//ctx.fillText(gridShape.labelX, gridShape.viewItem.drawingAreaWidth - labelXNameLength, gridShape.viewItem.drawingAreaHeight  - labelXHeight + fontSize + Style.marginXS);
		// ctx.fillText(gridShape.labelY, Style.marginXS, Style.marginXS + labelNameFontSize/2);

		ctx.fillText(gridShape.labelX, rightXAxePoint.x - labelXNameLength -4, rightXAxePointTr.y + labelNameFontSize/2 - 1);
		ctx.fillText(gridShape.labelY, topYAxePointTr.x - labelYNameLength/2 -1, topYAxePoint.y + labelNameFontSize);

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

