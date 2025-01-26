import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

TableBase {
    id: tableContainer;

    rowDelegate: Component {
        TableRowDelegate {
            tableItem: tableContainer
            minHeight: tableContainer.itemHeight

            readOnly: tableContainer.readOnly;
        }
    }

    cellDelegate: Component {
		TableCellTextDelegate {
			Component.onCompleted: {
				console.log("Cell onCompleted")
			}

			Component.onDestruction: {
				console.log("Cell onDestruction")
			}

		}
    }

    headerDelegate: Component {
        TableHeaderDelegate {
            height: tableContainer.headerHeight
            tableItem: tableContainer
        }
    }
}

