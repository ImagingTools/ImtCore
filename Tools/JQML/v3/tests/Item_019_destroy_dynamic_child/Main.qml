import QtQuick

Item {
    property Item childObject: Item {}

    Component.onCompleted: {
        childObject.destroy()
        console.log("child destroyed")
        Qt.quit()
    }
}
