import QtQuick

// One GridView in a state: how many delegates were made, how many exist.
Item {
    id: c

    property string label: ""
    property int made: 0
    property int viewWidth: 200
    property int viewHeight: 200
    property bool viewVisible: true
    property bool holderVisible: true
    property real viewOpacity: 1

    Item {
        visible: c.holderVisible
        width: 200
        height: 200

        GridView {
            id: view
            width: c.viewWidth
            height: c.viewHeight
            visible: c.viewVisible
            opacity: c.viewOpacity
            model: 100
            cellWidth: 100
            cellHeight: 50
            cacheBuffer: 0
            delegate: Rectangle {
                width: 100
                height: 50
                Component.onCompleted: c.made++
            }
        }
    }

    function built() {
        var n = 0
        for (var i = 0; i < 100; i++) {
            if (view.itemAtIndex(i) !== null) n++
        }
        return n
    }

    function report() {
        console.log(label, "made", made, "built", built(), "count", view.count)
    }
}
