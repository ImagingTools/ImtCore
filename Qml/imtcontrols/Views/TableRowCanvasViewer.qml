import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Item {
	id: root;

	anchors.fill: parent;

	property TableRowDelegateBase rowDelegate: null;

	Connections {
		target: root.rowDelegate.tableItem;

		function onWidthRecalc(){
			canvas.requestPaint();
		}
	}

	Canvas {
		id: canvas;
		anchors.fill: parent

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
			let key = root.rowDelegate.tableItem.headers.getData("Id", columnIndex)
			let contents = root.rowDelegate.tableItem.columnContentComps;

			if (contents[key]){
				return;
			}

			let val = getDefaultCellValue(key);

			if (drawCustomCell(ctx, x, y, cellWidth, cellHeight, key)){
				return;
			}

			if (val !== undefined){
				let text = String(val);
				let maxTextWidth = cellWidth - Style.size_mainMargin;
				let trimmedText = trimTextToFit(ctx, text, maxTextWidth);

				ctx.textBaseline = "middle";
				ctx.textAlign = "left";

				ctx.fillText(trimmedText, x + 2, cellHeight / 2);
			}
		}

		function getDefaultCellValue(key){
			let val = ''

			if(root.rowDelegate.tableItem !==null && root.rowDelegate.dataModel != null){
				if ("item" in root.rowDelegate.dataModel){
					val = root.rowDelegate.dataModel.item[key]
				}
				else {
					val = root.rowDelegate.dataModel[key];
				}
			}

			return val;
		}

		function drawCustomCell(ctx, x, y, cellWidth, cellHeight, columnId){
			return false;
		}

		onPaint: {
			if (!root.rowDelegate.tableItem){
				return
			}

			var ctx = canvas.getContext('2d');
			ctx.reset();
			ctx.clearRect(0, 0, root.rowDelegate.width, root.rowDelegate.height);

			ctx.stroke();
			let xCell = Style.size_mainMargin;
			let fontPixelSize = root.rowDelegate.tableItem.emptyDecorCell ? Style.fontSize_common : root.rowDelegate.tableItem.cellDecorator.isValidData("FontSize", columnIndex) ? root.rowDelegate.tableItem.cellDecorator.getData("FontSize", columnIndex) : Style.fontSize_common
			ctx.font = '' + fontPixelSize + "px " + Style.fontFamily

			for (let columnIndex = 0; columnIndex < root.rowDelegate.columnCount; columnIndex++){
				let defaultWidth = root.rowDelegate.columnCount == 0 ? 0 : root.rowDelegate.width/root.rowDelegate.columnCount;
				let widthFromModel = root.rowDelegate.tableItem.widthDecoratorDynamic.isValidData("Width", columnIndex) ?
						root.rowDelegate.tableItem.widthDecoratorDynamic.getData("Width", columnIndex) : -1;

				let cellWidth = 0;
				if(!root.rowDelegate.tableItem.widthDecoratorDynamic.getItemsCount()){
					cellWidth = defaultWidth;
				}
				else if(widthFromModel >= 0){
					cellWidth = widthFromModel;
				}
				else{
					cellWidth = defaultWidth;
				}

				ctx.fillStyle = root.rowDelegate.tableItem.emptyDecorCell ? Style.textColor : root.rowDelegate.tableItem.cellDecorator.isValidData("FontColor", columnIndex) ? root.rowDelegate.tableItem.cellDecorator.getData("FontColor", columnIndex) : Style.textColor

				drawCell(ctx, xCell, 0, cellWidth, root.rowDelegate.height, columnIndex)

				xCell += cellWidth;
			}

			ctx.stroke();

			ctx.fill();
		}
	}

	TableRowViewer {
		z: parent.z + 1;
		anchors.fill: parent;
		rowDelegate: root.rowDelegate;
		model: root.rowDelegate.columnCount;
		loadDefaultCellDelegate: false;
	}
}


