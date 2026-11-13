import QtQuick
import "listed" as L

Item {
    L.Unlisted { id: unlisted }

    Component.onCompleted: {
        console.log("unlisted", unlisted.name)
        Qt.quit()
    }
}
