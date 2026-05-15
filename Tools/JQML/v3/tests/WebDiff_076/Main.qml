import QtQuick

Item {
    ListModel {
        id: m
        ListElement { label: "a" }
        ListElement { label: "b" }
        ListElement { label: "c" }
    }

    Repeater {
        id: r
        model: m
        delegate: Text { text: model.label }
    }

    Component.onCompleted: {
        console.log("wd076", "repeater-listmodel", m.count, r.count)
        Qt.quit()
    }
}
