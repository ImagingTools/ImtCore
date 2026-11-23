import QtQuick
import QtQuick.Controls
import QtQuick.Templates as T
import Qt.labs.qmlmodels

// HorizontalHeaderViewDelegate / VerticalHeaderViewDelegate of the Basic style declared as the delegate of a header, and
// the table controls of QtQuick.Templates (T.HorizontalHeaderView...)
Item {
    width: 400; height: 300
    TableView { id: tv; x: 60; y: 40; width: 300; height: 200; columnWidthProvider: function() { return 80 }
        model: TableModel { TableModelColumn { display: "a" } TableModelColumn { display: "b" } rows: [{ a: 1, b: 2 }, { a: 3, b: 4 }] }
        delegate: Text { text: display } }
    HorizontalHeaderView { id: h; x: 60; syncView: tv; model: ["First", "Second"]
        delegate: HorizontalHeaderViewDelegate { } }
    VerticalHeaderView { id: v; y: 40; syncView: tv
        delegate: VerticalHeaderViewDelegate { } }
    T.HorizontalHeaderView { id: th; y: 260; width: 100; height: 30 }
    Timer { running: true; interval: 100; onTriggered: {
        let c = h.itemAtCell(0, 0), r = v.itemAtCell(0, 1)
        console.log("h", c.padding, c.highlighted, c.contentItem.text, c.width, c.implicitHeight > 0, c.background.color, c.background.border.color)
        console.log("v", r.contentItem.text, r.padding, r.contentItem.color)
        console.log("templates", th.syncDirection, typeof th.textRole)
        Qt.quit()
    } }
}
