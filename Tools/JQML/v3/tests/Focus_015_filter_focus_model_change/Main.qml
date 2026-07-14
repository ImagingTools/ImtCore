import QtQuick

Item {
    id: root
    width: 520
    height: 280

    property var sourceItems: [
        { id: "a1", title: "Alpha" },
        { id: "a2", title: "Alpine" },
        { id: "b1", title: "Beta" },
        { id: "g1", title: "Gamma" },
        { id: "d1", title: "Delta" }
    ]

    property var filteredItems: sourceItems

    function logFocus(name, obj) {
        console.log(name, "focus", obj.focus, "activeFocus", obj.activeFocus)
    }

    function dump(tag) {
        console.log(tag,
            "text", filterInput.text,
            "filter", filterInput.focus, filterInput.activeFocus,
            "count", listView.count
        )
    }

    function applyFilter(text) {
        var term = (text || "").toLowerCase()
        var result = []
        for (var i = 0; i < sourceItems.length; i++) {
            var item = sourceItems[i]
            if (!term || item.title.toLowerCase().indexOf(term) >= 0)
                result.push(item)
        }

        // Replace array reference to emulate backend refresh and delegate rebuild.
        filteredItems = result
        dump("focus015-filter-applied")
    }

    TextInput {
        id: filterInput
        x: 16
        y: 16
        width: 220
        height: 36
        text: ""

        onFocusChanged: root.logFocus("filter", filterInput)
        onActiveFocusChanged: root.logFocus("filter", filterInput)

        onTextChanged: {
            filterDebounce.restart()
        }
    }

    Timer {
        id: filterDebounce
        interval: 100
        repeat: false
        onTriggered: {
            root.applyFilter(filterInput.text)
        }
    }

    Rectangle {
        id: listFrame
        x: 16
        y: 64
        width: 488
        height: 200
        border.width: 1
        border.color: "#999"


        ListView {
            id: listView
            anchors.fill: parent
            clip: true
            model: root.filteredItems

            delegate: Item {
                id: row
                width: listView.width
                height: 28

                onFocusChanged: root.logFocus("row-" + index, row)
                onActiveFocusChanged: root.logFocus("row-" + index, row)

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    x: 8
                    text: modelData.title
                }
            }
        }
    }

    Timer {
        id: step1
        interval: 0
        repeat: false
        onTriggered: {
            filterInput.forceActiveFocus()
            root.dump("focus015-start")
            filterInput.text = "a"
            root.dump("focus015-after-text-a")
            step2.start()
        }
    }

    Timer {
        id: step2
        interval: 180
        repeat: false
        onTriggered: {
            root.dump("focus015-after-filter-a")
            filterInput.text = "al"
            root.dump("focus015-after-text-al")
            step3.start()
        }
    }

    Timer {
        id: step3
        interval: 180
        repeat: false
        onTriggered: {
            root.dump("focus015-after-filter-al")
            filterInput.text = ""
            root.dump("focus015-after-text-empty")
            step4.start()
        }
    }

    Timer {
        id: step4
        interval: 180
        repeat: false
        onTriggered: {
            root.dump("focus015-after-filter-empty")
            Qt.quit()
        }
    }

    Component.onCompleted: {
        step1.start()
    }
}
