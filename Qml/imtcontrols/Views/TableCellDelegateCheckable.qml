import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import com.imtcore.imtqml 1.0

TableCellDelegateBase {
	id: delegateContainerCheckBox

	signal clicked();

	property Item tableItem:  delegateContainerCheckBox.rowDelegate && delegateContainerCheckBox.rowDelegate.tableItem ? delegateContainerCheckBox.rowDelegate.tableItem : null;

	Connections{
		target: delegateContainerCheckBox.tableItem

		function onSelectionChanged(selection){
			if(selection.indexOf(delegateContainerCheckBox.rowIndex) > -1){
				selectionIndicator.checkState = Qt.Checked
			}
			else {
				selectionIndicator.checkState = Qt.Unchecked
			}
		}
	}

	onRowDelegateChanged: {
		if(rowDelegate){
			rowDelegate.reused.connect(delegateContainerCheckBox.onRedraw)
			delegateContainerCheckBox.onRedraw()
		}
	}

	function onRedraw(){
		nameText.text = getValue()
		if(!tableItem){
			return;
		}

	}

	CheckBox{
		id: selectionIndicator;

		anchors.left: parent.left
		anchors.leftMargin: Style.marginM
		anchors.verticalCenter: parent.verticalCenter;

		function nextCheckState(){}
		mouseArea.enabled: false
		mouseArea.hoverEnabled: false
	}

	TableCellText {
		id: nameText;

		anchors.left: selectionIndicator.right
		anchors.leftMargin: Style.marginM

		anchors.verticalCenter: parent.verticalCenter

		rowDelegate: delegateContainerCheckBox.rowDelegate
		columnIndex: delegateContainerCheckBox.columnIndex
	}
}
