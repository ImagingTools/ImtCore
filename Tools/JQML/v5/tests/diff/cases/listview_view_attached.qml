import QtQuick

// ListView.view in the first evaluation of the bindings of a delegate
Item {
    width: 200; height: 200
    ListView { id: lv; width: 150; height: 200; model: 3
        delegate: Rectangle { width: ListView.view.width; height: 20; property string name: ListView.view === lv ? "view" : "none" } }
    Component.onCompleted: { console.log(lv.itemAtIndex(0).width, lv.itemAtIndex(0).name); Qt.quit() }
}
