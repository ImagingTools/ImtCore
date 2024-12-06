import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

TableRowDelegateBase {
	id: tableRowDelegateBase

	TableRowCanvasViewer {
		id: canvasViewer;
		rowDelegate: tableRowDelegateBase;
	}

    TableRowViewer {
        z: parent.z + 1;
        anchors.fill: parent;
        rowDelegate: root.rowDelegate;
        model: root.rowDelegate.columnCount;
        loadDefaultCellDelegate: false;
    }
}
