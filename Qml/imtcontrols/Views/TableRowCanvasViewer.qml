import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Canvas {
	id: canvas;

	property TableRowDelegateBase rowDelegate: null;

	Connections {
		target: canvas.rowDelegate.tableItem;
		function onWidthRecalc(){
			canvas.requestPaint();
		}
	}

	function trimTextToFit(ctx, text, maxWidth){
		if (ctx.measureText(text).width <= maxWidth){
			return text;
		}

		let ellipsis = "...";
		let ellipsisWidth = ctx.measureText(ellipsis).width;

		while (text.length > 0 && ctx.measureText(text).width + ellipsisWidth > maxWidth){
			text = text.slice(0, -1);
		}

		return text + ellipsis;
	}

	function drawCell(ctx, x, y, cellWidth, cellHeight, columnIndex){
		let key = rowDelegate.tableItem.headers.getData("Id", columnIndex)
		let registeredDrawCellFunctions = rowDelegate.tableItem.registeredDrawCellFunctions;

		if (registeredDrawCellFunctions[key]){
			registeredDrawCellFunctions[key](ctx, x, y, cellWidth, cellHeight, columnIndex, canvas);
			return;
		}


		let val = getCellValue(key);
		if (val !== undefined && val !== ""){
			let text = String(val);
			drawText(ctx, x + 2 + Style.sizeSmallMargin, cellHeight / 2, cellWidth, text)
		}
	}

	function drawText(ctx, x, y, cellWidth, text){
		let maxTextWidth = cellWidth - Style.sizeSmallMargin * 2;
		let trimmedText;
		if (text !== ""){
			trimmedText = trimTextToFit(ctx, text, maxTextWidth);
		}

		ctx.textBaseline = "middle";
		ctx.textAlign = "left";

		ctx.fillText(trimmedText, x, y);
	}

	function getCellValue(key){
		let val = ''

		if(rowDelegate.tableItem !==null && rowDelegate.dataModel != null){
			if ("item" in rowDelegate.dataModel){
				val = rowDelegate.dataModel.item[key]
			}
			else {
				val = rowDelegate.dataModel[key];
			}
		}

		return val;
	}

	onPaint: {
		if (!rowDelegate.tableItem){
			return
		}

		var ctx = canvas.getContext('2d');
		ctx.reset();
		ctx.clearRect(0, 0, rowDelegate.width, rowDelegate.height);

		let color = (containsMouse || rowDelegate.selected) ? Style.selectedColor :
				rowDelegate.tableItem.enableAlternating ? rowDelegate.tableItem.alternatingColor : 'white';
		ctx.fillStyle = color
		ctx.fillRect(x, y, rowDelegate.width, rowDelegate.height)

		// ctx.beginPath()
		ctx.strokeStyle =  rowDelegate.tableItem.borderColorHorizontal
		// ctx.fillStyle = "red" //rowDelegate.selected ? Style.selectedColor : "transparent";

		ctx.moveTo(0, 0);
		ctx.lineWidth = (rowDelegate.rowIndex > 0 && rowDelegate.rowIndex < rowCount) ? rowDelegate.horizontalBorderSize :
																																	  (rowDelegate.rowIndex == 0 && rowDelegate.visibleTopBorderFirst) ? rowDelegate.horizontalBorderSize : 0;
		ctx.lineTo(rowDelegate.width, 0);

		ctx.lineWidth =  rowDelegate.visibleBottomBorderLast  ? rowDelegate.horizontalBorderSize : 0;

		ctx.moveTo(0, rowDelegate.height);
		ctx.lineTo(rowDelegate.width, rowDelegate.height);

		// ctx.stroke();
		ctx.strokeStyle =  rowDelegate.tableItem.borderColorVertical
		let fontPixelSize = rowDelegate.tableItem.emptyDecorCell ? Style.fontSizeNormal : rowDelegate.tableItem.cellDecorator.isValidData("FontSize", columnIndex) ? rowDelegate.tableItem.cellDecorator.getData("FontSize", columnIndex) : Style.fontSizeNormal
		ctx.font = '' + fontPixelSize + "px " + Style.fontFamily


		let xCell = 0; // Style.sizeMainMargin;

		for (let columnIndex = 0; columnIndex < rowDelegate.columnCount; columnIndex++){
			let defaultWidth = rowDelegate.columnCount == 0 ? 0 : rowDelegate.width/rowDelegate.columnCount;
			let widthFromModel = rowDelegate.tableItem.widthDecoratorDynamic.isValidData("Width", columnIndex) ?
					rowDelegate.tableItem.widthDecoratorDynamic.getData("Width", columnIndex) : -1;

			let cellWidth = 0;
			if(!rowDelegate.tableItem.widthDecoratorDynamic.getItemsCount()){
				cellWidth = defaultWidth;
			}
			else if(widthFromModel >= 0){
				cellWidth = widthFromModel;
			}
			else{
				cellWidth = defaultWidth;
			}

			if (cellWidth > 0){

				let lineWidth = rowDelegate.isRightBorder ? rowDelegate.verticalBorderSize * rowDelegate.visibleLeftBorderFirst * (columnIndex == 0)
														  : rowDelegate.visibleLeftBorderFirst ? rowDelegate.verticalBorderSize : columnIndex > 0 ? rowDelegate.verticalBorderSize : 0;

				if (lineWidth > 0){
					ctx.lineWidth = lineWidth
					ctx.moveTo(xCell, 0);
					ctx.lineTo(xCell, rowDelegate.height)
				}

				lineWidth = !rowDelegate.isRightBorder ?
							rowDelegate.verticalBorderSize * rowDelegate.visibleRightBorderLast  * (columnIndex == (rowDelegate.columnCount -1)) * (rowDelegate.columnCount > 0) :
							rowDelegate.visibleRightBorderLast ? rowDelegate.verticalBorderSize  :
																 rowDelegate.verticalBorderSize * (columnIndex < (rowDelegate.columnCount -1));


				if (lineWidth > 0){
					ctx.lineWidth = lineWidth
					ctx.moveTo(xCell, 0);
					ctx.lineTo(xCell, rowDelegate.height)
				}
				// ctx.moveTo(0, 0);

				ctx.fillStyle = rowDelegate.tableItem.emptyDecorCell ? Style.textColor : rowDelegate.tableItem.cellDecorator.isValidData("FontColor", columnIndex) ? rowDelegate.tableItem.cellDecorator.getData("FontColor", columnIndex) : Style.textColor

				drawCell(ctx, xCell, 0, cellWidth, rowDelegate.height, columnIndex)

				xCell += cellWidth;
			}
		}

		ctx.stroke();

		ctx.fill();
	}
}


