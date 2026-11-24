// every member of TableView and TreeView in Qt 6.12 (for-in over the objects): its type and its default value
import QtQuick
import QtQml.Models
Item {
    TableView { id: tv }
    TreeView { id: trv }
    Component.onCompleted: {
        let tvNames = ["alternatingRows", "alternatingRowsChanged", "animate", "animateChanged", "bottomRow", "bottomRowChanged", "cellAtIndex", "cellAtPos", "cellAtPosition", "clearColumnReordering", "clearColumnWidths", "clearRowHeights", "clearRowReordering", "closeEditor", "columnAtIndex", "columnMoved", "columnSpacing", "columnSpacingChanged", "columnWidth", "columnWidthProvider", "columnWidthProviderChanged", "columns", "columnsChanged", "currentColumn", "currentColumnChanged", "currentRow", "currentRowChanged", "delegate", "delegateChanged", "delegateModelAccess", "delegateModelAccessChanged", "edit", "editTriggers", "editTriggersChanged", "explicitColumnWidth", "explicitRowHeight", "forceLayout", "implicitColumnWidth", "implicitRowHeight", "index", "isColumnLoaded", "isRowLoaded", "itemAtCell", "itemAtIndex", "keyNavigationEnabled", "keyNavigationEnabledChanged", "layoutChanged", "leftColumn", "leftColumnChanged", "model", "modelChanged", "modelIndex", "moveColumn", "moveRow", "pointerNavigationEnabled", "pointerNavigationEnabledChanged", "positionViewAtCell", "positionViewAtColumn", "positionViewAtIndex", "positionViewAtRow", "resizableColumns", "resizableColumnsChanged", "resizableRows", "resizableRowsChanged", "reuseItems", "reuseItemsChanged", "rightColumn", "rightColumnChanged", "rowAtIndex", "rowHeight", "rowHeightProvider", "rowHeightProviderChanged", "rowMoved", "rowSpacing", "rowSpacingChanged", "rows", "rowsChanged", "selectionBehavior", "selectionBehaviorChanged", "selectionMode", "selectionModeChanged", "selectionModel", "selectionModelChanged", "setColumnWidth", "setRowHeight", "syncDirection", "syncDirectionChanged", "syncView", "syncViewChanged", "topRow", "topRowChanged"]
        let trvNames = ["collapse", "collapseRecursively", "collapsed", "depth", "expand", "expandRecursively", "expandToIndex", "expanded", "isExpanded", "rootIndex", "rootIndexChanged", "toggleExpanded"]
        for (let n of tvNames) console.log("TableView " + n + " " + typeof tv[n] + " " + (typeof tv[n] === "function" ? "" : String(tv[n])))
        for (let n of trvNames) console.log("TreeView " + n + " " + typeof trv[n] + " " + (typeof trv[n] === "function" ? "" : String(trv[n])))
        Qt.quit()
    }
}
