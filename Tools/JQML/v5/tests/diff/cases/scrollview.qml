import QtQuick
import QtQuick.Controls
import QtTest

// ScrollView: the Flickable it makes for an item (its content size, the bars, scrolling both ways, the wheel), a ListView
// in it as its content item, a TextArea in it
Item {
    width: 500; height: 400
    ScrollView { id: sv; width: 200; height: 150; Rectangle { id: big; implicitWidth: 400; implicitHeight: 600 } }
    ScrollView { id: lv; x: 250; width: 200; height: 150; ListView { id: list; model: 50; delegate: Rectangle { width: 180; height: 30 } } }
    ScrollView { id: ta; y: 200; width: 200; height: 100; TextArea { id: area; text: "a\nb\nc\nd\ne\nf\ng\nh" } }
    TestCase { name: "SV"; when: windowShown
        function test_sv() {
            let f = sv.contentItem
            console.log("sv", f.contentItem !== undefined, f.width, f.height, f.contentWidth, f.contentHeight, sv.contentWidth, sv.contentHeight, sv.implicitWidth, sv.implicitHeight, big.parent === f.contentItem, sv.padding, sv.effectiveScrollBarWidth, sv.effectiveScrollBarHeight)
            let vb = sv.ScrollBar.vertical, hb = sv.ScrollBar.horizontal
            console.log("bars", vb.parent === sv, vb.x, vb.y, vb.width, vb.height, vb.size, hb.x, hb.y, hb.width, hb.height, hb.size, vb.policy, vb.active)
            f.contentY = 100; console.log("scrolled", vb.position)
            vb.position = 0.5; console.log("bar moved", f.contentY)
            console.log("list", lv.contentItem === list, list.parent === lv, lv.contentWidth, lv.contentHeight, lv.ScrollBar.vertical.size > 0)
            console.log("textarea", ta.contentItem === area, area.width, area.height === area.implicitHeight, ta.contentHeight > 0)
            mouseWheel(sv, 50, 50, 0, -120); wait(400); console.log("wheel", f.contentY)
        }
    }
}
