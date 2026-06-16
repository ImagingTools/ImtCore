import QtQuick 2.12
import imtcontrols 1.0

Item {
    width: 640
    height: 480

    Component {
        id: dynamicItemComponent
        MenuItem {}
    }

    QtObject {
        id: saveAction
        property string text: "Save"
        property bool enabled: true
        property bool checkable: false
        property bool checked: false
        function trigger() {}
        function toggle() {}
    }

    Menu {
        id: apiMenu
        title: "Dynamic"
    }

    function rowLabelFromRows(index) {
        var row = apiMenu._rows[index]
        if (!row) return "<null>"
        if (row.isMenuSeparator) return "<separator>"
        return row.text
    }

    Timer {
        interval: 0
        running: true
        repeat: false
        onTriggered: {
            var first = dynamicItemComponent.createObject(apiMenu)
            first.text = "First"

            var second = dynamicItemComponent.createObject(apiMenu)
            second.text = "Second"

            apiMenu.addItem(first)
            apiMenu.addItem(second)
            apiMenu.insertItem(1, dynamicItemComponent.createObject(apiMenu, { text: "Inserted" }))
            apiMenu.addSeparator()
            apiMenu.addAction(saveAction)

            console.log("menu002-count-after-add:" + apiMenu.count)

            Qt.quit()
        }
    }
}
