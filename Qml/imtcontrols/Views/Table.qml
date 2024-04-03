import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

TableBase {
    id: tableContainer;

    rowDelegate: Component {
            TableRowDelegate {
            table: tableContainer
            width: tableContainer.width
            minHeight: tableContainer.itemHeight
        }
    }
    cellDelegate: Component {
        TableCellDelegateSimple {}
    }

    headerDelegate: Component {
        TableHeaderDelegate {
            height: tableContainer.headerHeight
            table: tableContainer
        }
    }
}
