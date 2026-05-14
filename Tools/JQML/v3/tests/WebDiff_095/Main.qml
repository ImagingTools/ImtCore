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
            property int calc: model.v + 95
        }
    }

    Component.onCompleted: {
        console.log("wd095", "repeater-calc", m.count, r.count)
        Qt.quit()
    }
}
