import QtQuick 2.12
import Acf 1.0

TableCellDelegateBase {
	id: delegateContainer

	property alias cellText: tableCellText

	onRowDelegateChanged: {
		if(rowDelegate){
			rowDelegate.tableItem.modelRefresh.connect(delegateContainer.refreshModelData)
		}
	}

	onReused: {
		delegateContainer.cellText.text = delegateContainer.getValue()
	}

	function refreshModelData(rowIndex_, modelRole_){
		if(rowIndex_ == delegateContainer.rowIndex && modelRole_ == delegateContainer.cellHeaderId){
			reused();
		}
	}

	TableCellText {
		id: tableCellText

		anchors.left: parent.left
		anchors.leftMargin: delegateContainer && delegateContainer.rowDelegate ? delegateContainer.rowDelegate.textLeftMargin : 0
		anchors.right: parent.right
		anchors.rightMargin: delegateContainer && delegateContainer.rowDelegate ? delegateContainer.rowDelegate.textRightMargin : 0
		anchors.verticalCenter: parent.verticalCenter
		rowDelegate: delegateContainer ? delegateContainer.rowDelegate : null;
		columnIndex: delegateContainer ? delegateContainer.columnIndex : -1
	}
}
