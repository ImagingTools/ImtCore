import QtQuick 2.12

// Paints one cell of a CanvasTable. Override draw() to replace the cell.
// drawText() is the default text pass and can be called from an override.
QtObject {
	id: cellDelegate;

	property var table: null;

	function draw(ctx, x, y, width, height, rowIndex, columnIndex) {
		if (!cellDelegate.table || width <= 0 || height <= 0)
			return;

		ctx.save();
		ctx.beginPath();
		ctx.rect(x, y, width, height);
		ctx.clip();

		cellDelegate.drawText(ctx, x, y, width, height, rowIndex, columnIndex);
		cellDelegate.drawBorder(ctx, x, y, width, height, columnIndex);

		ctx.restore();
	}

	function drawText(ctx, x, y, width, height, rowIndex, columnIndex) {
		let text = cellDelegate.table.cellText(rowIndex, columnIndex);
		if (text === "")
			return;

		let margin = cellDelegate.table.textMarginHor;
		let maxWidth = width - margin * 2;
		if (maxWidth <= 0)
			return;

		ctx.fillStyle = cellDelegate.table.textColor;
		ctx.font = cellDelegate.table.textFont();
		ctx.textBaseline = "middle";
		ctx.textAlign = "center";

		let shown = cellDelegate.table.trimText(ctx, text, maxWidth);
		ctx.fillText(shown, x + width / 2, y + height / 2);
	}

	function drawBorder(ctx, x, y, width, height, columnIndex) {
		if (cellDelegate.table.verticalBorderSize <= 0)
			return;

		ctx.strokeStyle = cellDelegate.table.borderColorVertical;
		ctx.lineWidth = cellDelegate.table.verticalBorderSize;

		let drawLeft = columnIndex === 0 && cellDelegate.table.visibleLeftBorderFirst;
		if (drawLeft){
			let left = x + cellDelegate.table.verticalBorderSize / 2;
			ctx.beginPath();
			ctx.moveTo(left, y);
			ctx.lineTo(left, y + height);
			ctx.stroke();
		}

		let lastColumn = columnIndex === cellDelegate.table.columnCount - 1;
		let drawRight = !lastColumn || cellDelegate.table.visibleRightBorderLast;
		if (drawRight){
			let right = x + width - cellDelegate.table.verticalBorderSize / 2;
			ctx.beginPath();
			ctx.moveTo(right, y);
			ctx.lineTo(right, y + height);
			ctx.stroke();
		}
	}
}
