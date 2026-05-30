import QtQuick

Item {
    property var data: []

    Component.onCompleted: {
        console.log("data length", data.length)
        console.log("data is array", Array.isArray(data))
        Qt.quit()
    }
}
