import QtQuick

Item {
    property var data: []

    Component.onCompleted: {
        console.log("data =", data)
        console.log("data is Array?", Array.isArray(data))
    }
}