import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

TableRowDelegateBase {
	id: tableDelegateContainer

	TableRowCanvasViewer {
		anchors.fill: parent
		anchors.leftMargin: tableDelegateContainer.tableItem
							&& tableDelegateContainer.tableItem.checkable ? 3 * Style.size_mainMargin : 0

		rowDelegate: tableDelegateContainer
	}
}
