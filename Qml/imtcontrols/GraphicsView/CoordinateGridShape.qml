import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

GraphicsShapeBase {

	id: gridShape;

	property Item viewItem: null
	property int axeMarginHorizontal: 30;
	property int axeMarginVertical: 40;
	property int gridStep: 40;
	property int fontSize: Style.fontSizeM
	property string backgroundColor: Style.baseColor;

	property CanvasMatrix identityMatrix: CanvasMatrix{};
	property CanvasMatrix labelMatrix: CanvasMatrix{};

	function draw(ctx, layerMatrix){
		let deltaX = layerMatrix.xTranslation();
		let deltaY = layerMatrix.yTranslation();
		let scaleCoeff = layerMatrix.xScale();

		let step = gridShape.gridStep;

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

		//grid
		let verticalLineTopY = (gridShape.viewItem.drawingAreaHeight - axeMarginVertical) - Math.trunc((gridShape.viewItem.drawingAreaHeight - axeMarginVertical)/ step) * step;

		for(let i = 0; i * step < gridShape.viewItem.drawingAreaWidth - axeMarginHorizontal; i++){//vertical lines
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

		for(let i = 0; i * step < gridShape.viewItem.drawingAreaHeight - axeMarginVertical; i++){//horizontal lines
			let x1 = axeMarginHorizontal ;
			let y1 =  gridShape.viewItem.drawingAreaHeight - i * step - axeMarginVertical;
			let x2 =  gridShape.viewItem.drawingAreaWidth ;
			let y2 =  y1;

			ctx.beginPath()
			ctx.moveTo(x1, y1);
			ctx.lineTo(x2, y2);
			ctx.closePath()
			ctx.stroke();
		}

		//axes background
		identityMatrix.setContextTransform(ctx);

		ctx.fillStyle = backgroundColor;
		ctx.fillRect(0,0, axeMarginHorizontal, gridShape.viewItem.drawingAreaHeight)
		ctx.fillRect(axeMarginHorizontal,gridShape.viewItem.drawingAreaHeight - axeMarginVertical,
					 gridShape.viewItem.drawingAreaWidth, gridShape.viewItem.drawingAreaHeight)


		//labels for coordinate axis

		ctx.font = String(fontSize) + "px sans-serif"//"14px sans-serif"

		ctx.strokeStyle = Style.borderColor;
		ctx.fillStyle = Style.borderColor2;

		//label x
		labelMatrix.setXTranslation(deltaX);
		labelMatrix.setYTranslation(0);
		labelMatrix.setContextTransform(ctx);

		for(let i = 1; i * step * scaleCoeff < gridShape.viewItem.drawingAreaWidth - axeMarginHorizontal; i++){
			let x_ = (axeMarginHorizontal + i * step) * scaleCoeff;
			let y_ = gridShape.viewItem.drawingAreaHeight  - axeMarginVertical + fontSize + Style.marginXS

			ctx.beginPath()
			let str = String(i * step)

			let textLength = ctx.measureText(str).width
			x_ = x_ - textLength/2

			ctx.fillText(str,x_, y_);
			ctx.closePath()
		}


		//label y
		labelMatrix.reset()
		//labelMatrix.setXTranslation(0);
		labelMatrix.setYTranslation(deltaY);
		labelMatrix.setContextTransform(ctx);

		for(let i = 1; i * step * scaleCoeff < gridShape.viewItem.drawingAreaHeight - axeMarginVertical; i++){
			let x_ = axeMarginHorizontal;
			let y_ = (gridShape.viewItem.drawingAreaHeight - i * step  - axeMarginVertical)* scaleCoeff

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


		//horizontal axe
		ctx.strokeStyle = Style.borderColor;
		ctx.lineWidth = 2;
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

