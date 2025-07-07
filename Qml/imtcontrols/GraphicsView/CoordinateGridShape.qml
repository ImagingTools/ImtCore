import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

GraphicsShapeBase {

	id: gridShape;

	property Item viewItem: null
	property int axeMarginHorizontal: 30;
	property int axeMarginVertical: 40;
	property int gridStepMajor: 40;
	property int gridStepMinor: 10;
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

		ctx.fillRect(axeMarginHorizontal,0, gridShape.viewItem.drawingAreaWidth - axeMarginHorizontal, gridShape.viewItem.drawingAreaHeight - axeMarginVertical)
		// ctx.fillRect(0,0, gridShape.viewItem.drawingAreaWidth, gridShape.viewItem.drawingAreaHeight)

		layerMatrix.setContextTransform(ctx);

		//GRID
		let verticalLineTopY = (gridShape.viewItem.drawingAreaHeight - axeMarginVertical) - Math.trunc((gridShape.viewItem.drawingAreaHeight - axeMarginVertical)/ step) * step - deltaAddY;

		//MINOR GRID
		if(scaleCoeff >= 2){
			ctx.strokeStyle = Style.imagingToolsGradient0;
			let step = gridShape.gridStepMinor
			for(let i = 0; i * step < gridShape.viewItem.drawingAreaWidth - axeMarginHorizontal + deltaAddX; i++){//vertical lines
				let x1 = axeMarginHorizontal + i * step;
				let y1 =  verticalLineTopY ;
				let x2 = x1;
				let y2 = gridShape.viewItem.drawingAreaHeight - axeMarginVertical ;

				ctx.beginPath()
				ctx.moveTo(x1, y1);
				ctx.lineTo(x2, y2);
				ctx.closePath()
				ctx.stroke();
			}

			for(let i = 0; i * step < gridShape.viewItem.drawingAreaHeight - axeMarginVertical  + deltaAddY; i++){//horizontal lines

				let x1 = axeMarginHorizontal ;
				let y1 =  gridShape.viewItem.drawingAreaHeight - i * step - axeMarginVertical;
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
		for(let i = 0; i * step < gridShape.viewItem.drawingAreaWidth - axeMarginHorizontal + deltaAddX; i++){//vertical lines
			let x1 = axeMarginHorizontal + i * step;
			let y1 =  verticalLineTopY ;
			let x2 = x1;
			let y2 = gridShape.viewItem.drawingAreaHeight - axeMarginVertical ;

			ctx.beginPath()
			ctx.moveTo(x1, y1);
			ctx.lineTo(x2, y2);
			ctx.closePath()
			ctx.stroke();
		}

		for(let i = 0; i * step < gridShape.viewItem.drawingAreaHeight - axeMarginVertical  + deltaAddY; i++){//horizontal lines
			let x1 = axeMarginHorizontal ;
			let y1 =  gridShape.viewItem.drawingAreaHeight - i * step - axeMarginVertical;
			let x2 =  gridShape.viewItem.drawingAreaWidth + deltaAddX ;
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
		ctx.fillRect(0,0, axeMarginHorizontal, gridShape.viewItem.drawingAreaHeight)
		ctx.fillRect(axeMarginHorizontal,gridShape.viewItem.drawingAreaHeight - axeMarginVertical,
					 gridShape.viewItem.drawingAreaWidth, gridShape.viewItem.drawingAreaHeight)


		//LABELS FOR COORDINATE AXIS

		ctx.font = String(fontSize) + "px sans-serif"//"14px sans-serif"

		ctx.strokeStyle = Style.borderColor;
		ctx.fillStyle = Style.borderColor2;

		//label x
		labelMatrix.setXTranslation(deltaX);
		labelMatrix.setYTranslation(0);
		labelMatrix.setContextTransform(ctx);

		for(let i = 1; i * step * scaleCoeff < (gridShape.viewItem.drawingAreaWidth - axeMarginHorizontal + deltaAddX); i++){
			let x_ = (axeMarginHorizontal + i * step) * scaleCoeff;
			let y_ = gridShape.viewItem.drawingAreaHeight  - axeMarginVertical + fontSize + Style.marginXS
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

		for(let i = 1; i * step * scaleCoeff < (gridShape.viewItem.drawingAreaHeight - axeMarginVertical + deltaAddY) * scaleCoeff; i++){
			let x_ = axeMarginHorizontal;
			let y_ = (gridShape.viewItem.drawingAreaHeight - i * step  - axeMarginVertical)* scaleCoeff
			//console.log("deltaAddY:: ", y_, deltaAddY, (gridShape.viewItem.drawingAreaHeight - axeMarginVertical + deltaAddY * scaleCoeff))

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
		ctx.fillRect(0,gridShape.viewItem.drawingAreaHeight - axeMarginVertical, axeMarginHorizontal, gridShape.viewItem.drawingAreaHeight)

		//Label names
		let labelFontSize = fontSize + 2
		ctx.font = String(labelFontSize) + "px sans-serif"
		let labelXLength = ctx.measureText(gridShape.labelX).width
		let labelYLength = ctx.measureText(gridShape.labelY).width
		let labelXWidth = Math.max(labelXLength + 10, 30)
		let labelYHeight = 30

		ctx.fillRect(gridShape.viewItem.drawingAreaWidth - labelXWidth ,gridShape.viewItem.drawingAreaHeight - axeMarginVertical, labelXWidth, gridShape.axeMarginVertical)
		ctx.fillRect(0 ,0, gridShape.axeMarginHorizontal, labelYHeight)

		ctx.fillStyle = Style.borderColor2;
		ctx.fillText(gridShape.labelX, gridShape.viewItem.drawingAreaWidth - labelXLength, gridShape.viewItem.drawingAreaHeight  - axeMarginVertical + fontSize + Style.marginXS);
		ctx.fillText(gridShape.labelY, Style.marginXS, Style.marginXS + labelFontSize/2);

		//horizontal axe
		ctx.lineWidth = 2;
		ctx.strokeStyle = Style.borderColor;
		ctx.beginPath()
		ctx.moveTo(0, gridShape.viewItem.drawingAreaHeight - axeMarginVertical);
		ctx.lineTo(gridShape.viewItem.drawingAreaWidth, gridShape.viewItem.drawingAreaHeight - axeMarginVertical);
		ctx.closePath()
		ctx.stroke();

		//vertical axe
		ctx.beginPath()
		ctx.moveTo(axeMarginHorizontal, 0);
		ctx.lineTo(axeMarginHorizontal, gridShape.viewItem.drawingAreaHeight);
		ctx.closePath()
		ctx.stroke();
	}
}

