import QtQuick
import "sub"

Item {


    Component.onCompleted: {
        console.log("palette", Theme.name)
        Qt.quit()
    }
}
