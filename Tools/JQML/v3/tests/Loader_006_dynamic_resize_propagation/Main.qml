import QtQuick

Item {
    Component {
        id: comp
        Item {
            width: 40
            height: 30
        }
    }

    Loader {
        id: loader
        width: 100
        height: 60
        sourceComponent: comp

        onLoaded: {
            console.log("step1", width, height, item.width, item.height)
            width = 130
            height = 70
            console.log("step2", width, height, item.width, item.height)
            Qt.quit()
        }
    }
}
