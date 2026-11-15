import QtQuick
import "util.js" as Util

Item {
    Component.onCompleted: {
        console.log("js", Util.add(2, 3), Util.constant)
        Qt.quit()
    }
}
