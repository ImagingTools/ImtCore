import QtQuick
import QtQuick.Controls
import QtTest

// Pane (content item, content size), Frame, GroupBox (label, padding), Page (header, footer, content), ToolBar, ToolSeparator,
// MenuSeparator, TabBar (the widths of its buttons, currentIndex and a click, addItem), SwipeView (its pages, currentIndex)
Item {
    width: 800; height: 600
    Pane { id: p1; Rectangle { id: r1; implicitWidth: 100; implicitHeight: 50 } }
    Pane { id: p2; y: 100; Rectangle { implicitWidth: 100; implicitHeight: 50 } Rectangle { implicitWidth: 30; implicitHeight: 20 } }
    Pane { id: p3; y: 200; contentWidth: 70; contentHeight: 40 }
    Frame { id: fr; x: 200; Rectangle { implicitWidth: 60; implicitHeight: 30 } }
    GroupBox { id: gb; x: 200; y: 100; Rectangle { implicitWidth: 60; implicitHeight: 30 } }
    Page { id: pg; x: 400; width: 200; height: 200; spacing: 5
        header: ToolBar { id: tb } footer: Rectangle { id: foot; implicitHeight: 20 }
        Rectangle { id: pc; implicitWidth: 40; implicitHeight: 40 } }
    ToolSeparator { id: ts; y: 300 }
    ToolSeparator { id: tsh; x: 50; y: 300; orientation: Qt.Horizontal }
    MenuSeparator { id: ms; y: 350 }
    TabBar { id: bar; x: 400; y: 300; width: 300; TabButton { id: t1 } TabButton { id: t2; width: 50 } TabButton { id: t3 } }
    SwipeView { id: sw; x: 400; y: 400; width: 200; height: 100; Rectangle { color: "red" } Rectangle { color: "green" } }
    Component { id: tabComponent; TabButton {} }
    function g(c) { return [c.x, c.y, c.width, c.height, c.implicitWidth, c.implicitHeight].join(",") }
    TestCase { name: "Panes"; when: windowShown
        function test_panes() {
            console.log("pane1", g(p1), p1.contentWidth, p1.contentHeight, p1.implicitContentWidth, p1.contentItem.objectName, g(p1.contentItem), r1.parent === p1.contentItem, p1.contentChildren.length, p1.focusPolicy, p1.background.color)
            console.log("pane2", g(p2), p2.contentWidth, p2.contentHeight)
            console.log("pane3", g(p3), p3.contentWidth, p3.contentHeight)
            console.log("frame", g(fr), fr.background.color, fr.background.border.color, fr.background.border.width)
            console.log("groupbox", g(gb), gb.topPadding, !!gb.label, gb.implicitLabelWidth, gb.label ? [gb.label.x, gb.label.y, gb.label.width].join(",") : "-", g(gb.background))
            gb.title = "Title"; console.log("groupbox title", gb.topPadding > 12, gb.label.text, gb.label.elide, gb.label.x, gb.label.width, gb.label.color)
            console.log("page", g(pg), g(tb), g(foot), g(pg.contentItem), pg.implicitHeaderHeight, pg.implicitFooterHeight, tb.position, pg.background.color, tb.background.color)
            console.log("toolsep", g(ts), ts.vertical, ts.padding, ts.verticalPadding, g(ts.contentItem), "h", g(tsh), g(tsh.contentItem))
            console.log("menusep", g(ms), ms.padding, ms.verticalPadding, g(ms.contentItem))
            console.log("tabbar", g(bar), bar.count, bar.currentIndex, g(t1), g(t2), g(t3), t1.checked, !!bar.contentItem, bar.contentWidth, bar.contentHeight, bar.position)
            mouseClick(t3, 10, 10); console.log("tab click", bar.currentIndex, t3.checked, t1.checked)
            bar.currentIndex = 0; console.log("set index", t1.checked, t3.checked)
            bar.addItem(tabComponent.createObject(bar)); bar.contentItem.forceLayout(); console.log("added", bar.count, g(bar.itemAt(0)), g(bar.itemAt(3)))
            console.log("swipe", g(sw), sw.count, sw.currentIndex, g(sw.itemAt(0)), g(sw.itemAt(1)), !!sw.contentItem, sw.interactive, sw.orientation)
            sw.currentIndex = 1; wait(400); console.log("swipe 1", sw.contentItem.contentX, sw.itemAt(1).x, sw.currentItem === sw.itemAt(1))
            mouseClick(p1, 5, 5); console.log("pane eats", p1.activeFocus)
        }
    }
}
