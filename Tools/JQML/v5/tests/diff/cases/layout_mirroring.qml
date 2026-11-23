import QtQuick
import QtQuick.Layouts

// LayoutMirroring: the anchors (left and right swapped with their margins, the center offset negated), Row and
// RowLayout from the right, the alignment of a Text; childrenInherit passes it to the items below
Item {
    id: root
    width: 300; height: 300
    Item {
        id: box
        width: 200; height: 300
        LayoutMirroring.enabled: true
        LayoutMirroring.childrenInherit: true
        Rectangle { id: l; anchors.left: parent.left; anchors.leftMargin: 10; anchors.rightMargin: 3; width: 20; height: 5 }
        Rectangle { id: r; anchors.right: parent.right; anchors.rightMargin: 7; width: 20; height: 5 }
        Rectangle { id: lr; anchors.left: parent.left; anchors.right: parent.right; anchors.leftMargin: 4; anchors.rightMargin: 9; height: 5 }
        Rectangle { id: hc; anchors.horizontalCenter: parent.horizontalCenter; anchors.horizontalCenterOffset: 15; width: 20; height: 5 }
        Rectangle { id: f; anchors.fill: parent; anchors.leftMargin: 6; anchors.rightMargin: 2 }
        Row { id: row; y: 50; spacing: 5; Rectangle { id: r1; width: 30; height: 5 } Rectangle { id: r2; width: 40; height: 5 } }
        RowLayout { id: rl; y: 60; width: 200; spacing: 5; Rectangle { id: q1; implicitWidth: 30; implicitHeight: 5 } Rectangle { id: q2; implicitWidth: 40; implicitHeight: 5 } }
        Text { id: t; y: 80; width: 200; text: "x" }
        Item {
            id: inner
            LayoutMirroring.enabled: false
            width: 100; height: 10
            Rectangle { id: il; anchors.left: parent.left; anchors.leftMargin: 10; width: 20; height: 5 }
        }
    }
    Item {
        id: noInherit
        x: 0; y: 0; width: 100; height: 20
        LayoutMirroring.enabled: true
        Rectangle { id: ni; anchors.left: parent.left; width: 20; height: 5 }
        Item { id: niChild; width: 100; height: 10; Rectangle { id: nic; anchors.left: parent.left; width: 20; height: 5 } }
    }
    Component.onCompleted: {
        console.log(l.x, r.x, lr.x, lr.width, hc.x, f.x, f.width)
        console.log(row.effectiveLayoutDirection, row.width, r1.x, r2.x, rl.layoutDirection, q1.x, q2.x, t.horizontalAlignment, t.effectiveHorizontalAlignment)
        console.log(il.x, inner.LayoutMirroring.enabled, ni.x, nic.x, niChild.LayoutMirroring.enabled, box.LayoutMirroring.enabled)
        box.LayoutMirroring.enabled = false
        console.log(l.x, r.x, lr.x, hc.x, row.effectiveLayoutDirection, r1.x, q1.x, t.effectiveHorizontalAlignment)
        Qt.quit()
    }
}
