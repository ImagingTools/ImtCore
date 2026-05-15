import QtQuick

Item {
    ListModel {
        id: m
        ListElement { v: 10 }
        ListElement { v: 20 }
    }

    Repeater {
        id: r
        model: m
        delegate: Item {
            property int calc: model.v + 107
        }
    }

    Component.onCompleted: {
        console.log("wd107", "repeater-calc", m.count, r.count)
        Qt.quit()
    }
}
