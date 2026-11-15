import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

TableRowDelegateBase {
	id: tableDelegateContainer

	width: tableRow.width //+ tableRow.anchors.leftMargin;
	property alias tableRowViewer: tableRow

	// The cell of the checkbox column: it is looked after like the cells of the row.
	Rectangle {
		id: checkBoxCell;

		anchors.left: parent.left;
		anchors.top: parent.top;

		width: tableRow.anchors.leftMargin;
		height: tableDelegateContainer.contentHeight;

		visible: width > 0;
		color: tableDelegateContainer.hasCellColor ? tableDelegateContainer.cellBackgroundColor : "transparent";

		Rectangle {
			anchors.left: parent.left;
			anchors.right: parent.right;
			anchors.top: parent.top;

			height: tableDelegateContainer.topSeparatorHeight;
			color: tableDelegateContainer.borderColorHorizontal;
		}

		Rectangle {
			anchors.left: parent.left;
			anchors.right: parent.right;
			anchors.bottom: parent.bottom;

			height: tableDelegateContainer.bottomSeparatorHeight;
			color: tableDelegateContainer.borderColorHorizontal;
		}
	}

	TableRowViewer {
		id: tableRow

		height: parent.height;

		anchors.left: parent.left;
		anchors.leftMargin: tableDelegateContainer.tableItem ? tableDelegateContainer.tableItem.checkColumnWidth : 0

		rowDelegate: tableDelegateContainer
		model: tableDelegateContainer.columnCount
	}
}

