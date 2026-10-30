import QtQuick 2.12

// Paints one header cell of a CanvasTable. Override draw() to replace it.
QtObject {
	id: headerDelegate;

	property var table: null;

	function draw(ctx, x, y, width, height, columnIndex) {
		if (!headerDelegate.table || width <= 0 || height <= 0)
			return;

		ctx.save();
		ctx.beginPath();
		ctx.rect(x, y, width, height);
		ctx.clip();

		ctx.fillStyle = headerDelegate.table.headerColor;
		ctx.fillRect(x, y, width, height);

		headerDelegate.drawTitle(ctx, x, y, width, height, columnIndex);

		ctx.restore();
	}

	function drawTitle(ctx, x, y, width, height, columnIndex) {
		let title = headerDelegate.table.headerTitle(columnIndex);
		let margin = headerDelegate.table.textMarginHor;
		let headerId = headerDelegate.table.headerId(columnIndex);
		let sorted = headerDelegate.table.hasSort && headerId !== "" && headerId === headerDelegate.table.currentHeaderId;
		let markWidth = sorted ? 14 : 0;
		let maxWidth = width - margin * 2 - markWidth;
		if (maxWidth < 0)
			maxWidth = 0;

		if (title !== "" && maxWidth > 0){
			ctx.fillStyle = headerDelegate.table.headerTextColor;
			ctx.font = headerDelegate.table.textFont();
			ctx.textBaseline = "middle";
			ctx.textAlign = "left";
			let shown = headerDelegate.table.trimText(ctx, title, maxWidth);
			ctx.fillText(shown, x + margin, y + height / 2);
		}

		if (sorted)
			headerDelegate.drawSortMark(ctx, x + width - margin, y + height / 2);
	}

	function drawSortMark(ctx, right, midY) {
		ctx.fillStyle = headerDelegate.table.headerTextColor;
		ctx.beginPath();
		if (headerDelegate.table.currentSortOrder === "DESC"){
			ctx.moveTo(right - 8, midY - 2);
			ctx.lineTo(right - 2, midY - 2);
			ctx.lineTo(right - 5, midY + 3);
		}
		else{
			ctx.moveTo(right - 8, midY + 2);
			ctx.lineTo(right - 2, midY + 2);
			ctx.lineTo(right - 5, midY - 3);
		}
		ctx.closePath();
		ctx.fill();
	}
}
