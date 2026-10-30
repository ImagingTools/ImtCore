import QtQuick 2.12

// Paints one row of a CanvasTable. Override draw() to replace the row chrome.
// The table still asks the cell delegate to draw each cell afterwards.
QtObject {
	id: rowDelegate;

	property var table: null;

	function draw(ctx, x, y, width, height, rowIndex) {
		if (!rowDelegate.table)
			return;

		ctx.fillStyle = rowDelegate.rowColor(rowIndex);
		ctx.fillRect(x, y, width, height);

		if (rowDelegate.table.horizontalBorderSize > 0 && rowIndex > 0){
			ctx.strokeStyle = rowDelegate.table.borderColorHorizontal;
			ctx.lineWidth = rowDelegate.table.horizontalBorderSize;
			ctx.beginPath();
			ctx.moveTo(x, y + rowDelegate.table.horizontalBorderSize / 2);
			ctx.lineTo(x + width, y + rowDelegate.table.horizontalBorderSize / 2);
			ctx.stroke();
		}

		if (rowDelegate.table.horizontalBorderSize > 0 && rowDelegate.table.visibleBottomBorderLast && rowIndex === rowDelegate.table.rowCount - 1){
			ctx.strokeStyle = rowDelegate.table.borderColorHorizontal;
			ctx.lineWidth = rowDelegate.table.horizontalBorderSize;
			ctx.beginPath();
			ctx.moveTo(x, y + height - rowDelegate.table.horizontalBorderSize / 2);
			ctx.lineTo(x + width, y + height - rowDelegate.table.horizontalBorderSize / 2);
			ctx.stroke();
		}

		if (rowDelegate.table.selectedIndex === rowIndex && rowDelegate.table.isRowSelected(rowIndex)){
			ctx.strokeStyle = rowDelegate.table.focusRingColor;
			ctx.lineWidth = 1;
			ctx.beginPath();
			ctx.rect(x + 0.5, y + 0.5, width - 1, height - 1);
			ctx.stroke();
		}
	}

	function rowColor(rowIndex) {
		if (!rowDelegate.table)
			return "transparent";
		if (rowDelegate.table.isRowSelected(rowIndex))
			return rowDelegate.table.selectedColor;
		if (rowDelegate.table.hoverEnabled && rowIndex === rowDelegate.table.hoverIndex)
			return rowDelegate.table.hoverColor;
		if (rowDelegate.table.enableAlternating && (rowIndex % 2 === 1))
			return rowDelegate.table.alternatingColor;
		return rowDelegate.table.rowColor;
	}
}
