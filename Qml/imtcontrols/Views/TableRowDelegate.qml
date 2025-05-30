import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

TableRowDelegateBase {
	id: tableDelegateContainer

	width: tableRow.width //+ tableRow.anchors.leftMargin;
	property alias tableRowViewer: tableRow

	TableRowViewer {
		id: tableRow

		height: parent.height;

		anchors.left: parent.left;
		anchors.leftMargin: tableDelegateContainer.tableItem
							&& tableDelegateContainer.tableItem.checkable ? 3 * Style.marginM : 0

		rowDelegate: tableDelegateContainer
		model: tableDelegateContainer.columnCount
	}
}

