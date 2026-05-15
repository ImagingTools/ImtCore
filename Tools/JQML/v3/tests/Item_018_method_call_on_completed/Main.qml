import QtQuick

Item {
    function init() {
        console.log("init called")
    }

    Component.onCompleted: {
        init()
        Qt.quit()
    }
}
