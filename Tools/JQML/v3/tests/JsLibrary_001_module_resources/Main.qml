import QtQuick
import JsModule 1.0

// A JavaScript resource of a module is reached through its qualifier, whatever the number of its top-level names
Item {
    Component.onCompleted: {
        console.log("single-function", typeof SingleFunction.twice, SingleFunction.twice(21))
        console.log("several-functions", SeveralFunctions.first(), SeveralFunctions.second())
        console.log("single-object", typeof SingleObject.Table, SingleObject.Table.size)
        Qt.quit()
    }
}
