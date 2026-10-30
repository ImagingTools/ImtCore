import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

// One canvas for the whole table.
// Visible rows, cells and headers are painted by calling draw() on the delegates.
Item {
	id: canvasTable;
	objectName: "CanvasTable";

	clip: true;

	property var elements: null;
	property var headers: null;
	property var widthDecorator: null;

	property int columnCount: 0;
	property int rowCount: 0;
	property var columnWidthList: [];

	property int itemHeight: Style.tableRowHeight;
	property int headerHeight: Style.tableHeaderHeight;
	property bool showHeaders: true;
	readonly property real headerSpace: canvasTable.showHeaders ? canvasTable.headerHeight : 0;

	property int selectedIndex: -1;
	property var selectedIndexes: [];
	property int hoverIndex: -1;
	property bool selectable: true;
	property bool isMultiSelect: true;
	property bool hoverEnabled: (Style.enableHoverEffect !== undefined && Style.enableHoverEffect !== null) ? Style.enableHoverEffect : true;

	property bool enableAlternating: (Style.enableAlternating === true) || (Style.enableTableAlternating !== undefined ? Style.enableTableAlternating : false);
	property color alternatingColor: Style.tableAlternateRowColor !== undefined ? Style.tableAlternateRowColor : Style.backgroundColor2;
	property color rowColor: "transparent";
	property color selectedColor: Style.selectedColor;
	property color hoverColor: Style.hover;
	property color focusRingColor: Style.focusRingColor;
	property color backgroundColor: Style.baseColor;
	property color headerColor: Style.baseColor;
	property color textColor: Style.textColor;
	property color headerTextColor: Style.textColor;
	property color borderColor: Style.borderColor;
	property color borderColorHorizontal: "transparent";
	property color borderColorVertical: Style.borderColor;

	property int horizontalBorderSize: 0;
	property int verticalBorderSize: 0;
	property bool visibleLeftBorderFirst: true;
	property bool visibleRightBorderLast: false;
	property bool visibleTopBorderFirst: false;
	property bool visibleBottomBorderLast: true;

	property int textMarginHor: Style.marginM;
	property int fontPixelSize: Style.fontSizeM;
	property string fontFamily: Style.fontFamily;

	property bool isFlickable: false;
	property int minCellWidth: 30;
	property int defaultColumnWidth: Style.sizeHintXXS;

	property bool hasSort: false;
	property string currentHeaderId: "";
	property string currentSortOrder: "ASC";
	property var nonSortableColumns: [];

	property CanvasTableRowDelegate rowDelegate: defaultRowDelegate;
	property CanvasTableCellDelegate cellDelegate: defaultCellDelegate;
	property CanvasTableHeaderDelegate headerDelegate: defaultHeaderDelegate;
	property var columnCellDelegates: null;
	property var cellDelegatesById: null;

	property real contentWidth: 0;
	property real contentHeight: canvasTable.rowCount * canvasTable.itemHeight;
	property alias contentX: flick.contentX;
	property alias contentY: flick.contentY;

	property bool widthUpdate: false;
	property bool updatingSelection: false;
	property bool dragMoved: false;
	property real pressX: 0;
	property real pressY: 0;
	property real pressContentX: 0;
	property real pressContentY: 0;

	property bool canResizeColumns: true;
	property int resizeColumn: -1;
	property int hoverResizeColumn: -1;
	property real resizeAnchorX: 0;
	property real resizeAnchorWidth: 0;
	property real resizeHandleWidth: Style.marginS;
	property color resizeHandleColor: Style.titleColor;
	property var userColumnWidths: null;

	property var watchedElements: null;
	property var watchedHeaders: null;
	property var watchedWidths: null;
	property bool elementsBound: false;
	property bool headersBound: false;
	property bool widthsBound: false;

	signal clicked(int index);
	signal doubleClicked(string id, int index);
	signal rightButtonMouseClicked(int mouseX, int mouseY);
	signal selectionChanged(var selection);
	signal headerClicked(string headerId);
	signal headerRightMouseClicked(string headerId);
	signal sortingChanged(string headerId, string sortOrder);

	function requestPaint() {
		if (!tableCanvas)
			return;
		canvasTable.syncCounts();
		canvasTable.bindVisibleItems();
		tableCanvas.requestPaint();
	}

	function textFont() {
		return canvasTable.fontPixelSize + "px " + canvasTable.fontFamily;
	}

	function trimText(ctx, text, maxWidth) {
		text = String(text);
		if (maxWidth <= 0)
			return "";
		if (ctx.measureText(text).width <= maxWidth)
			return text;

		let ellipsis = "...";
		let ellipsisWidth = ctx.measureText(ellipsis).width;
		if (ellipsisWidth >= maxWidth)
			return "";

		let low = 0;
		let high = text.length;
		while (low < high){
			let mid = Math.floor((low + high + 1) / 2);
			let part = text.slice(0, mid);
			if (ctx.measureText(part).width + ellipsisWidth <= maxWidth)
				low = mid;
			else
				high = mid - 1;
		}

		if (low <= 0)
			return ellipsis;
		return text.slice(0, low) + ellipsis;
	}

	function readCount(model) {
		if (!model)
			return 0;
		if (model.getItemsCount)
			return model.getItemsCount();
		if (model.count !== undefined && model.count !== null)
			return model.count;
		if (model.length !== undefined && model.length !== null)
			return model.length;
		return 0;
	}

	function sdlItem(model, index) {
		if (!model || !model.get || index < 0)
			return null;
		let row = model.get(index);
		if (!row)
			return null;
		if (row.item)
			return row.item;
		return row;
	}

	function modelField(model, index, key) {
		if (!model || index < 0 || key === "")
			return undefined;
		if (model.getData){
			if (!model.containsKey || model.containsKey(key, index))
				return model.getData(key, index);
			return undefined;
		}

		let row = null;
		if (model.get)
			row = model.get(index);
		else
			row = model[index];
		if (!row)
			return undefined;
		if (row.item && row.item[key] !== undefined && row.item[key] !== null)
			return row.item[key];
		return row[key];
	}

	function syncCounts() {
		canvasTable.columnCount = canvasTable.readCount(canvasTable.headers);
		canvasTable.rowCount = canvasTable.readCount(canvasTable.elements);
	}

	function headerId(columnIndex) {
		let value = canvasTable.modelField(canvasTable.headers, columnIndex, "id");
		if (value === undefined || value === null || value === "")
			value = canvasTable.modelField(canvasTable.headers, columnIndex, "m_id");
		if (value === undefined || value === null)
			return "";
		return String(value);
	}

	function headerTitle(columnIndex) {
		let value = canvasTable.modelField(canvasTable.headers, columnIndex, "name");
		if (value === undefined || value === null || value === "")
			value = canvasTable.modelField(canvasTable.headers, columnIndex, "m_name");
		if (value === undefined || value === null || value === "")
			value = canvasTable.modelField(canvasTable.headers, columnIndex, "title");
		if (value === undefined || value === null)
			return "";
		return String(value);
	}

	function cellValue(rowIndex, columnIndex) {
		let key = canvasTable.headerId(columnIndex);
		if (key === "")
			return "";
		let value = canvasTable.modelField(canvasTable.elements, rowIndex, key);
		if (value === undefined || value === null || value === "")
			value = canvasTable.modelField(canvasTable.elements, rowIndex, "m_" + key);
		if (value === undefined || value === null)
			return "";
		return value;
	}

	function cellText(rowIndex, columnIndex) {
		let value = canvasTable.cellValue(rowIndex, columnIndex);
		if (value === undefined || value === null)
			return "";
		return String(value);
	}

	function rowId(rowIndex) {
		let value = canvasTable.modelField(canvasTable.elements, rowIndex, "id");
		if (value === undefined || value === null || value === "")
			value = canvasTable.modelField(canvasTable.elements, rowIndex, "Id");
		if (value === undefined || value === null || value === "")
			value = canvasTable.modelField(canvasTable.elements, rowIndex, "m_id");
		if (value === undefined || value === null)
			return "";
		return String(value);
	}

	function columnWidthAt(columnIndex) {
		let list = canvasTable.columnWidthList;
		if (list && columnIndex >= 0 && columnIndex < list.length)
			return list[columnIndex];
		if (canvasTable.columnCount <= 0 || flick.width <= 0)
			return 0;
		return flick.width / canvasTable.columnCount;
	}

	function decorNumber(key, index) {
		let decor = canvasTable.widthDecorator;
		if (!decor || !decor.getData || index < 0)
			return -1;
		if (decor.getItemsCount && index >= decor.getItemsCount())
			return -1;
		if (decor.isValidData && !decor.isValidData(key, index))
			return -1;
		let value = decor.getData(key, index);
		if (value === undefined || value === null)
			return -1;
		return value;
	}

	function buildColumnWidths(count, viewWidth) {
		let widths = [];
		let decor = canvasTable.widthDecorator;
		let decorCount = 0;
		if (decor && decor.getItemsCount)
			decorCount = decor.getItemsCount();

		if (decorCount <= 0){
			let each = canvasTable.isFlickable ? canvasTable.defaultColumnWidth : viewWidth / count;
			for (let i = 0; i < count; i++)
				widths.push(each);
			return widths;
		}

		let fixed = 0;
		let autoCount = 0;
		let visibleCount = 0;
		for (let i = 0; i < count; i++){
			let widthValue = canvasTable.decorNumber("Width", i);
			let percentValue = canvasTable.decorNumber("WidthPercent", i);
			if (widthValue !== 0 && percentValue !== 0)
				visibleCount = visibleCount + 1;
			if (widthValue === -1 && percentValue === -1)
				autoCount = autoCount + 1;
			else if (percentValue >= 0)
				fixed = fixed + viewWidth * (percentValue / 100);
			else if (widthValue > 0)
				fixed = fixed + widthValue;
		}

		let stretch = visibleCount > 0 && (viewWidth < visibleCount * canvasTable.minCellWidth || autoCount === decorCount);
		for (let column = 0; column < count; column++){
			let widthValue = canvasTable.decorNumber("Width", column);
			let percentValue = canvasTable.decorNumber("WidthPercent", column);
			let cellWidth = 0;
			if (widthValue === 0 || percentValue === 0)
				cellWidth = 0;
			else if (stretch)
				cellWidth = canvasTable.isFlickable ? canvasTable.defaultColumnWidth : viewWidth / visibleCount;
			else if (percentValue >= 0)
				cellWidth = viewWidth * (percentValue / 100);
			else if (widthValue >= 0)
				cellWidth = widthValue;
			else if (!canvasTable.isFlickable && autoCount > 0)
				cellWidth = (viewWidth - fixed) / autoCount;
			else
				cellWidth = canvasTable.defaultColumnWidth;

			if (cellWidth < 0)
				cellWidth = 0;
			widths.push(cellWidth);
		}

		return widths;
	}

	function updateColumnWidths() {
		if (!flick || canvasTable.widthUpdate)
			return;

		canvasTable.widthUpdate = true;
		canvasTable.syncCounts();

		let count = canvasTable.columnCount;
		let viewWidth = flick.width;
		if (count <= 0 || viewWidth <= 0){
			if (count <= 0){
				canvasTable.columnWidthList = [];
				canvasTable.contentWidth = 0;
			}
			canvasTable.widthUpdate = false;
			canvasTable.requestPaint();
			return;
		}

		if (canvasTable.userColumnWidths && canvasTable.userColumnWidths.length !== count)
			canvasTable.userColumnWidths = null;

		if (canvasTable.userColumnWidths && canvasTable.userColumnWidths.length === count){
			let kept = canvasTable.userColumnWidths.slice();
			let keptTotal = 0;
			for (let keptIndex = 0; keptIndex < kept.length; keptIndex++)
				keptTotal = keptTotal + kept[keptIndex];
			canvasTable.columnWidthList = kept;
			canvasTable.contentWidth = keptTotal;
			canvasTable.widthUpdate = false;
			canvasTable.requestPaint();
			return;
		}

		let widths = canvasTable.buildColumnWidths(count, viewWidth);
		let total = 0;
		for (let i = 0; i < widths.length; i++)
			total = total + widths[i];

		canvasTable.columnWidthList = widths;
		canvasTable.contentWidth = total;
		canvasTable.widthUpdate = false;
		canvasTable.requestPaint();
	}

	function scheduleWidths() {
		if (!widthTimer)
			return;
		widthTimer.restart();
	}

	property var visibleBoundItems: [];

	function bindModel(model, previous, bound) {
		if (bound && previous && previous.countChanged && previous.countChanged.disconnect)
			previous.countChanged.disconnect(canvasTable.onSdlListChanged);
		if (model && model.countChanged && model.countChanged.connect){
			model.countChanged.connect(canvasTable.onSdlListChanged);
			return true;
		}
		return false;
	}

	function onSdlListChanged() {
		canvasTable.updateColumnWidths();
	}

	function bindVisibleItems() {
		if (!flick)
			return;

		let next = [];
		let rowHeight = canvasTable.itemHeight;
		let rows = canvasTable.rowCount;
		if (rowHeight > 0 && rows > 0){
			let first = Math.floor(flick.contentY / rowHeight);
			if (first < 0)
				first = 0;
			let viewHeight = tableCanvas ? tableCanvas.height : 0;
			let last = Math.ceil((flick.contentY + Math.max(0, viewHeight - canvasTable.headerSpace)) / rowHeight);
			if (last > rows)
				last = rows;
			for (let rowIndex = first; rowIndex < last; rowIndex++){
				let item = canvasTable.sdlItem(canvasTable.elements, rowIndex);
				if (item)
					next.push(item);
			}
		}

		for (let columnIndex = 0; columnIndex < canvasTable.columnCount; columnIndex++){
			let item = canvasTable.sdlItem(canvasTable.headers, columnIndex);
			if (item)
				next.push(item);
		}

		let previous = canvasTable.visibleBoundItems;
		if (!previous)
			previous = [];
		for (let i = 0; i < previous.length; i++){
			if (next.indexOf(previous[i]) < 0 && previous[i].modelChanged && previous[i].modelChanged.disconnect)
				previous[i].modelChanged.disconnect(canvasTable.requestPaint);
		}
		for (let i = 0; i < next.length; i++){
			if (previous.indexOf(next[i]) < 0 && next[i].modelChanged && next[i].modelChanged.connect)
				next[i].modelChanged.connect(canvasTable.requestPaint);
		}
		canvasTable.visibleBoundItems = next;
	}

	function cellDelegateFor(columnIndex) {
		let byId = canvasTable.cellDelegatesById;
		if (byId){
			let id = canvasTable.headerId(columnIndex);
			if (id !== "" && byId[id])
				return byId[id];
		}

		let list = canvasTable.columnCellDelegates;
		if (list && list[columnIndex])
			return list[columnIndex];
		return canvasTable.cellDelegate;
	}

	function setCellDelegate(headerId, delegate) {
		if (!canvasTable.cellDelegatesById)
			canvasTable.cellDelegatesById = {};
		canvasTable.cellDelegatesById[headerId] = delegate;
		if (delegate)
			delegate.table = canvasTable;
		canvasTable.requestPaint();
	}

	function attachDelegate(delegate) {
		if (delegate)
			delegate.table = canvasTable;
	}

	function isRowSelected(rowIndex) {
		if (rowIndex === canvasTable.selectedIndex)
			return true;
		let list = canvasTable.selectedIndexes;
		if (!list || !list.indexOf)
			return false;
		return list.indexOf(rowIndex) >= 0;
	}

	function selectRow(rowIndex) {
		canvasTable.updatingSelection = true;
		canvasTable.selectedIndexes = [rowIndex];
		canvasTable.selectedIndex = rowIndex;
		canvasTable.updatingSelection = false;
		canvasTable.selectionChanged(canvasTable.selectedIndexes);
		canvasTable.requestPaint();
	}

	function toggleSelected(rowIndex) {
		let list = [];
		if (canvasTable.selectedIndexes)
			list = canvasTable.selectedIndexes.slice();
		let pos = list.indexOf(rowIndex);
		if (pos >= 0)
			list.splice(pos, 1);
		else
			list.push(rowIndex);

		canvasTable.updatingSelection = true;
		canvasTable.selectedIndexes = list;
		if (list.length === 0)
			canvasTable.selectedIndex = -1;
		else
			canvasTable.selectedIndex = list[list.length - 1];
		canvasTable.updatingSelection = false;
		canvasTable.selectionChanged(list);
		canvasTable.requestPaint();
	}

	function rowAt(y) {
		if (y < canvasTable.headerSpace)
			return -1;
		let rowHeight = canvasTable.itemHeight;
		if (rowHeight <= 0)
			return -1;
		let row = Math.floor((y - canvasTable.headerSpace + flick.contentY) / rowHeight);
		if (row < 0 || row >= canvasTable.rowCount)
			return -1;
		return row;
	}

	function setColumnWidth(columnIndex, width) {
		if (columnIndex < 0 || columnIndex >= canvasTable.columnCount)
			return;
		if (width < canvasTable.minCellWidth)
			width = canvasTable.minCellWidth;

		let list = [];
		if (canvasTable.columnWidthList)
			list = canvasTable.columnWidthList.slice();
		while (list.length <= columnIndex)
			list.push(canvasTable.defaultColumnWidth);

		list[columnIndex] = width;
		canvasTable.userColumnWidths = list.slice();
		canvasTable.columnWidthList = list;

		let total = 0;
		for (let i = 0; i < list.length; i++)
			total = total + list[i];
		canvasTable.contentWidth = total;
		canvasTable.requestPaint();
	}

	function resizeColumnAt(x, y) {
		if (!canvasTable.canResizeColumns)
			return -1;
		if (y < 0 || y >= canvasTable.headerSpace)
			return -1;

		let handle = canvasTable.resizeHandleWidth;
		if (handle < 4)
			handle = 4;
		let edge = -flick.contentX;
		for (let columnIndex = 0; columnIndex < canvasTable.columnCount; columnIndex++){
			let cellWidth = canvasTable.columnWidthAt(columnIndex);
			edge = edge + cellWidth;
			if (cellWidth > 0 && Math.abs(x - edge) <= handle)
				return columnIndex;
		}
		return -1;
	}

	function updateResizeHover(x, y) {
		let edge = -1;
		if (!tableMouse.pressed)
			edge = canvasTable.resizeColumnAt(x, y);
		tableMouse.cursorShape = edge >= 0 || canvasTable.resizeColumn >= 0 ? Qt.SplitHCursor : Qt.ArrowCursor;
		if (edge === canvasTable.hoverResizeColumn)
			return;
		canvasTable.hoverResizeColumn = edge;
		canvasTable.requestPaint();
	}

	function columnAt(x) {
		let cursor = -flick.contentX;
		for (let i = 0; i < canvasTable.columnCount; i++){
			let cellWidth = canvasTable.columnWidthAt(i);
			if (cellWidth > 0 && x >= cursor && x < cursor + cellWidth)
				return i;
			cursor = cursor + cellWidth;
		}
		return -1;
	}

	function setContentX(value) {
		let maxValue = flick.contentWidth - flick.width;
		if (maxValue < 0)
			maxValue = 0;
		if (value < 0)
			value = 0;
		if (value > maxValue)
			value = maxValue;
		if (flick.contentX !== value)
			flick.contentX = value;
	}

	function setContentY(value) {
		let maxValue = flick.contentHeight - flick.height;
		if (maxValue < 0)
			maxValue = 0;
		if (value < 0)
			value = 0;
		if (value > maxValue)
			value = maxValue;
		if (flick.contentY !== value)
			flick.contentY = value;
	}

	function revealRow(rowIndex) {
		let rowHeight = canvasTable.itemHeight;
		let top = rowIndex * rowHeight;
		let bottom = top + rowHeight;
		if (top < flick.contentY)
			canvasTable.setContentY(top);
		else if (bottom > flick.contentY + flick.height)
			canvasTable.setContentY(bottom - flick.height);
	}

	function moveSelection(step) {
		if (canvasTable.rowCount <= 0)
			return;
		let row = canvasTable.selectedIndex;
		if (row < 0)
			row = 0;
		else
			row = row + step;
		if (row < 0)
			row = 0;
		if (row >= canvasTable.rowCount)
			row = canvasTable.rowCount - 1;
		canvasTable.selectRow(row);
		canvasTable.revealRow(row);
	}

	function updateHover(y) {
		let row = -1;
		if (canvasTable.hoverEnabled)
			row = canvasTable.rowAt(y);
		if (row === canvasTable.hoverIndex)
			return;
		canvasTable.hoverIndex = row;
		canvasTable.requestPaint();
	}

	function activateHeader(columnIndex, button) {
		let id = canvasTable.headerId(columnIndex);
		if (button === Qt.RightButton){
			canvasTable.headerRightMouseClicked(id);
			return;
		}

		canvasTable.headerClicked(id);
		if (!canvasTable.hasSort)
			return;
		if (canvasTable.nonSortableColumns && canvasTable.nonSortableColumns.indexOf(id) >= 0)
			return;

		if (canvasTable.currentHeaderId === id){
			if (canvasTable.currentSortOrder === "ASC")
				canvasTable.currentSortOrder = "DESC";
			else
				canvasTable.currentSortOrder = "ASC";
		}
		else{
			canvasTable.currentHeaderId = id;
			canvasTable.currentSortOrder = "ASC";
		}

		canvasTable.sortingChanged(id, canvasTable.currentSortOrder);
		canvasTable.requestPaint();
	}

	function handleClick(x, y, button, modifiers) {
		if (y < canvasTable.headerSpace){
			let column = canvasTable.columnAt(x);
			if (column >= 0)
				canvasTable.activateHeader(column, button);
			return;
		}

		let row = canvasTable.rowAt(y);
		if (row < 0)
			return;
		if (canvasTable.selectable){
			if (canvasTable.isMultiSelect && (modifiers & Qt.ControlModifier))
				canvasTable.toggleSelected(row);
			else
				canvasTable.selectRow(row);
		}
		canvasTable.clicked(row);
	}

	function paintAll(ctx) {
		let viewWidth = tableCanvas.width;
		let viewHeight = tableCanvas.height;
		if (viewWidth <= 0 || viewHeight <= 0)
			return;

		ctx.clearRect(0, 0, viewWidth, viewHeight);
		ctx.fillStyle = canvasTable.backgroundColor;
		ctx.fillRect(0, 0, viewWidth, viewHeight);

		let headerSpace = canvasTable.headerSpace;
		if (headerSpace < viewHeight){
			ctx.save();
			ctx.beginPath();
			ctx.rect(0, headerSpace, viewWidth, viewHeight - headerSpace);
			ctx.clip();
			canvasTable.paintRows(ctx, viewWidth, viewHeight, headerSpace);
			ctx.restore();
		}

		if (headerSpace > 0)
			canvasTable.paintHeaders(ctx, viewWidth, headerSpace);
	}

	function paintRows(ctx, viewWidth, viewHeight, headerSpace) {
		let rowPainter = canvasTable.rowDelegate;
		if (!rowPainter || !rowPainter.draw)
			return;

		let rows = canvasTable.rowCount;
		let rowHeight = canvasTable.itemHeight;
		if (rows <= 0 || rowHeight <= 0)
			return;

		let first = Math.floor(flick.contentY / rowHeight);
		if (first < 0)
			first = 0;
		let last = Math.ceil((flick.contentY + (viewHeight - headerSpace)) / rowHeight);
		if (last > rows)
			last = rows;

		for (let rowIndex = first; rowIndex < last; rowIndex++){
			let y = headerSpace + rowIndex * rowHeight - flick.contentY;
			ctx.save();
			rowPainter.draw(ctx, 0, y, viewWidth, rowHeight, rowIndex);
			ctx.restore();
			canvasTable.paintRowCells(ctx, y, rowHeight, rowIndex, viewWidth);
		}
	}

	function paintRowCells(ctx, y, rowHeight, rowIndex, viewWidth) {
		let x = -flick.contentX;
		for (let columnIndex = 0; columnIndex < canvasTable.columnCount; columnIndex++){
			let cellWidth = canvasTable.columnWidthAt(columnIndex);
			if (cellWidth > 0 && x + cellWidth > 0 && x < viewWidth){
				let painter = canvasTable.cellDelegateFor(columnIndex);
				if (painter && painter.draw){
					if (!painter.table)
						painter.table = canvasTable;
					ctx.save();
					painter.draw(ctx, x, y, cellWidth, rowHeight, rowIndex, columnIndex);
					ctx.restore();
				}
			}
			x = x + cellWidth;
		}
	}

	function paintHeaders(ctx, viewWidth, headerSpace) {
		ctx.fillStyle = canvasTable.headerColor;
		ctx.fillRect(0, 0, viewWidth, headerSpace);

		let painter = canvasTable.headerDelegate;
		let x = -flick.contentX;
		for (let columnIndex = 0; columnIndex < canvasTable.columnCount; columnIndex++){
			let cellWidth = canvasTable.columnWidthAt(columnIndex);
			if (painter && painter.draw && cellWidth > 0 && x + cellWidth > 0 && x < viewWidth){
				if (!painter.table)
					painter.table = canvasTable;
				ctx.save();
				painter.draw(ctx, x, 0, cellWidth, headerSpace, columnIndex);
				ctx.restore();
			}
			x = x + cellWidth;
		}

		ctx.fillStyle = canvasTable.borderColor;
		ctx.fillRect(0, headerSpace - 1, viewWidth, 1);

		canvasTable.paintResizeHandle(ctx, canvasTable.hoverResizeColumn, headerSpace);
		if (canvasTable.resizeColumn !== canvasTable.hoverResizeColumn)
			canvasTable.paintResizeHandle(ctx, canvasTable.resizeColumn, headerSpace);
	}

	function paintResizeHandle(ctx, columnIndex, headerSpace) {
		if (columnIndex < 0)
			return;

		let edge = -flick.contentX;
		for (let i = 0; i <= columnIndex; i++)
			edge = edge + canvasTable.columnWidthAt(i);

		ctx.fillStyle = canvasTable.resizeHandleColor;
		ctx.fillRect(edge - 1, 0, 2, headerSpace);
	}

	onElementsChanged: {
		canvasTable.elementsBound = canvasTable.bindModel(canvasTable.elements, canvasTable.watchedElements, canvasTable.elementsBound);
		canvasTable.watchedElements = canvasTable.elements;
		canvasTable.updateColumnWidths();
	}

	onHeadersChanged: {
		canvasTable.headersBound = canvasTable.bindModel(canvasTable.headers, canvasTable.watchedHeaders, canvasTable.headersBound);
		canvasTable.watchedHeaders = canvasTable.headers;
		canvasTable.updateColumnWidths();
	}

	onWidthDecoratorChanged: {
		canvasTable.widthsBound = canvasTable.bindModel(canvasTable.widthDecorator, canvasTable.watchedWidths, canvasTable.widthsBound);
		canvasTable.watchedWidths = canvasTable.widthDecorator;
		canvasTable.scheduleWidths();
	}

	onRowDelegateChanged: {
		canvasTable.attachDelegate(canvasTable.rowDelegate);
		canvasTable.requestPaint();
	}

	onCellDelegateChanged: {
		canvasTable.attachDelegate(canvasTable.cellDelegate);
		canvasTable.requestPaint();
	}

	onHeaderDelegateChanged: {
		canvasTable.attachDelegate(canvasTable.headerDelegate);
		canvasTable.requestPaint();
	}

	onColumnCellDelegatesChanged: {
		let list = canvasTable.columnCellDelegates;
		if (list){
			for (let i = 0; i < list.length; i++)
				canvasTable.attachDelegate(list[i]);
		}
		canvasTable.requestPaint();
	}

	onSelectedIndexChanged: {
		if (canvasTable.updatingSelection)
			return;
		if (canvasTable.selectedIndex < 0){
			canvasTable.selectedIndexes = [];
			canvasTable.selectionChanged(canvasTable.selectedIndexes);
			canvasTable.requestPaint();
			return;
		}
		canvasTable.selectRow(canvasTable.selectedIndex);
	}

	onSelectedColorChanged: canvasTable.requestPaint();
	onHoverColorChanged: canvasTable.requestPaint();
	onAlternatingColorChanged: canvasTable.requestPaint();
	onRowColorChanged: canvasTable.requestPaint();
	onBackgroundColorChanged: canvasTable.requestPaint();
	onHeaderColorChanged: canvasTable.requestPaint();
	onTextColorChanged: canvasTable.requestPaint();
	onHeaderTextColorChanged: canvasTable.requestPaint();
	onBorderColorChanged: canvasTable.requestPaint();
	onBorderColorHorizontalChanged: canvasTable.requestPaint();
	onBorderColorVerticalChanged: canvasTable.requestPaint();
	onEnableAlternatingChanged: canvasTable.requestPaint();
	onShowHeadersChanged: canvasTable.requestPaint();
	onItemHeightChanged: canvasTable.requestPaint();
	onHeaderHeightChanged: canvasTable.requestPaint();
	onFontPixelSizeChanged: canvasTable.requestPaint();
	onFontFamilyChanged: canvasTable.requestPaint();
	onHorizontalBorderSizeChanged: canvasTable.requestPaint();
	onVerticalBorderSizeChanged: canvasTable.requestPaint();
	onCurrentHeaderIdChanged: canvasTable.requestPaint();
	onCurrentSortOrderChanged: canvasTable.requestPaint();
	onHoverEnabledChanged: canvasTable.requestPaint();
	onIsFlickableChanged: canvasTable.scheduleWidths();

	Component.onCompleted: {
		canvasTable.attachDelegate(canvasTable.rowDelegate);
		canvasTable.attachDelegate(canvasTable.cellDelegate);
		canvasTable.attachDelegate(canvasTable.headerDelegate);
		canvasTable.scheduleWidths();
	}

	focus: true;

	Keys.onUpPressed: canvasTable.moveSelection(-1);
	Keys.onDownPressed: canvasTable.moveSelection(1);

	CanvasTableRowDelegate {
		id: defaultRowDelegate;
		table: canvasTable;
	}

	CanvasTableCellDelegate {
		id: defaultCellDelegate;
		table: canvasTable;
	}

	CanvasTableHeaderDelegate {
		id: defaultHeaderDelegate;
		table: canvasTable;
	}

	Flickable {
		id: flick;

		anchors.left: parent.left;
		anchors.right: vbar.visible ? vbar.left : parent.right;
		anchors.top: parent.top;
		anchors.topMargin: canvasTable.headerSpace;
		anchors.bottom: parent.bottom;
		anchors.bottomMargin: hbar.visible ? hbar.secondSize + hbar.anchors.topMargin : 0;

		clip: true;
		interactive: false;
		boundsBehavior: Flickable.StopAtBounds;

		contentWidth: canvasTable.contentWidth;
		contentHeight: canvasTable.contentHeight;

		onWidthChanged: canvasTable.scheduleWidths();
		onHeightChanged: canvasTable.requestPaint();
		onContentXChanged: canvasTable.requestPaint();
		onContentYChanged: canvasTable.requestPaint();
	}

	Canvas {
		id: tableCanvas;

		anchors.left: flick.left;
		anchors.right: flick.right;
		anchors.top: parent.top;
		anchors.bottom: flick.bottom;
		z: 1;

		antialiasing: true;
		renderStrategy: Canvas.Immediate;

		onAvailableChanged: {
			if (tableCanvas.available)
				canvasTable.requestPaint();
		}

		onPaint: {
			let ctx = tableCanvas.getContext("2d");
			if (!ctx)
				return;
			canvasTable.paintAll(ctx);
		}
	}

	MouseArea {
		id: tableMouse;

		anchors.left: flick.left;
		anchors.right: flick.right;
		anchors.top: parent.top;
		anchors.bottom: flick.bottom;
		z: 2;

		hoverEnabled: true;
		acceptedButtons: Qt.LeftButton | Qt.RightButton;
		cursorShape: Qt.ArrowCursor;

		onPressed: {
			canvasTable.forceActiveFocus();
			canvasTable.pressX = mouse.x;
			canvasTable.pressY = mouse.y;
			canvasTable.pressContentX = flick.contentX;
			canvasTable.pressContentY = flick.contentY;
			canvasTable.dragMoved = false;
			canvasTable.resizeColumn = -1;

			if (mouse.button === Qt.LeftButton){
				let edge = canvasTable.resizeColumnAt(mouse.x, mouse.y);
				if (edge >= 0){
					canvasTable.resizeColumn = edge;
					canvasTable.resizeAnchorX = mouse.x;
					canvasTable.resizeAnchorWidth = canvasTable.columnWidthAt(edge);
					canvasTable.dragMoved = true;
					tableMouse.cursorShape = Qt.SplitHCursor;
				}
			}
		}

		onPositionChanged: {
			if (canvasTable.resizeColumn >= 0){
				let width = canvasTable.resizeAnchorWidth + (mouse.x - canvasTable.resizeAnchorX);
				canvasTable.setColumnWidth(canvasTable.resizeColumn, width);
				return;
			}

			if (!tableMouse.pressed){
				canvasTable.updateResizeHover(mouse.x, mouse.y);
				canvasTable.updateHover(mouse.y);
				return;
			}

			let dx = mouse.x - canvasTable.pressX;
			let dy = mouse.y - canvasTable.pressY;
			if (!canvasTable.dragMoved){
				if (Math.abs(dx) < 4 && Math.abs(dy) < 4)
					return;
				canvasTable.dragMoved = true;
			}

			canvasTable.setContentX(canvasTable.pressContentX - dx);
			canvasTable.setContentY(canvasTable.pressContentY - dy);
		}

		onReleased: {
			canvasTable.resizeColumn = -1;
			canvasTable.updateResizeHover(mouse.x, mouse.y);
			canvasTable.updateHover(mouse.y);
		}

		onExited: {
			tableMouse.cursorShape = Qt.ArrowCursor;
			if (canvasTable.hoverResizeColumn !== -1){
				canvasTable.hoverResizeColumn = -1;
				canvasTable.requestPaint();
			}
			if (canvasTable.hoverIndex !== -1){
				canvasTable.hoverIndex = -1;
				canvasTable.requestPaint();
			}
		}

		onClicked: {
			if (canvasTable.dragMoved)
				return;
			if (mouse.button === Qt.RightButton)
				canvasTable.rightButtonMouseClicked(mouse.x, mouse.y);
			canvasTable.handleClick(mouse.x, mouse.y, mouse.button, mouse.modifiers);
		}

		onDoubleClicked: {
			if (canvasTable.dragMoved)
				return;
			if (mouse.button !== Qt.LeftButton)
				return;
			let row = canvasTable.rowAt(mouse.y);
			if (row < 0)
				return;
			canvasTable.doubleClicked(canvasTable.rowId(row), row);
		}

		onWheel: {
			let dx = 0;
			let dy = 0;
			if (wheel.pixelDelta){
				dx = wheel.pixelDelta.x;
				dy = wheel.pixelDelta.y;
			}
			if (dx === 0 && dy === 0 && wheel.angleDelta){
				dx = wheel.angleDelta.x;
				dy = wheel.angleDelta.y;
			}
			if (dx !== 0)
				canvasTable.setContentX(flick.contentX - dx);
			if (dy !== 0)
				canvasTable.setContentY(flick.contentY - dy);
			wheel.accepted = true;
		}
	}

	Rectangle {
		id: headerCorner;

		anchors.top: parent.top;
		anchors.right: parent.right;
		anchors.left: flick.right;
		height: canvasTable.headerSpace;
		color: canvasTable.headerColor;
		visible: vbar.visible && canvasTable.headerSpace > 0;
		z: 3;

		Rectangle {
			anchors.left: parent.left;
			anchors.right: parent.right;
			anchors.bottom: parent.bottom;
			height: 1;
			color: canvasTable.borderColor;
		}
	}

	CustomScrollbar {
		id: vbar;

		z: 100;

		anchors.right: parent.right;
		anchors.top: flick.top;
		anchors.bottom: flick.bottom;

		secondSize: 10;
		targetItem: flick;
	}

	CustomScrollbar {
		id: hbar;

		z: 101;

		anchors.right: flick.right;
		anchors.top: flick.bottom;
		anchors.topMargin: 1;

		secondSize: 10;
		vertical: false;
		targetItem: flick;
	}

	Timer {
		id: widthTimer;
		interval: 0;
		onTriggered: canvasTable.updateColumnWidths();
	}
}
