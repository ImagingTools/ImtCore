import QtQuick
import "Models"

// the import that comes last gives the type: the SortFilterProxyModel of the project, not that of QtQuick
Item {
    SortFilterProxyModel {
        id: proxy
        textFilter: "abc"
        filterableFields: ["name"]
        onAccepted: console.log("accepted", textFilter, filterableFields)
    }
    Component.onCompleted: {
        console.log("has apply", typeof proxy.apply, "has sourceModel", proxy.sourceModel === undefined)
        proxy.apply()
        Qt.quit()
    }
}
