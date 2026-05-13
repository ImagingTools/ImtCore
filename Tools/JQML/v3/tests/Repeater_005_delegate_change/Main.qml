import QtQuick

Item {
    Component {
        id: delegate1
        Item {
            property string val: "D1-" + modelData
        }
    }

    Component {
        id: delegate2
        Item {
            property string val: "D2-" + modelData
        }
    }

    Repeater {
        id: rep
        model: ["A", "B"]
        delegate: delegate1
    }

    Component.onCompleted: {
        console.log("d1", rep.count, rep.itemAt(0).val, rep.itemAt(1).val)
        rep.delegate = delegate2
        console.log("d2", rep.count, rep.itemAt(0).val, rep.itemAt(1).val)
        Qt.quit()
    }
}
