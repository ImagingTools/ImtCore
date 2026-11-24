import QtQuick
import QtQuick.Controls

// The Basic style of the range controls: the handle and groove of Slider and RangeSlider (pressed, focused, disabled,
// vertical), the bar of ProgressBar, the dots of PageIndicator, the indicators and content of SpinBox, the dial of Dial
Item {
    id: root
    width: 800
    height: 600
    Slider { id: slider; width: 200; value: 0.3 }
    Slider { id: vertical; x: 250; height: 200; orientation: Qt.Vertical; value: 0.25 }
    RangeSlider { id: range; y: 60; width: 200; first.value: 0.2; second.value: 0.7 }
    ProgressBar { id: progress; y: 120; value: 0.4 }
    PageIndicator { id: pages; y: 150; count: 4; currentIndex: 1 }
    SpinBox { id: spin; y: 200; value: 5 }
    Dial { id: dial; x: 400; value: 0.3 }
    function r(i) { return i ? [i.visible, i.x, i.y, i.width, i.height, i.color, i.border.width, i.border.color, i.radius, i.opacity].join(",") : "none" }
    function kids(i) { let out = []; for (let k = 0; k < i.children.length; k++) out.push(r(i.children[k])); return out.join(" ; ") }
    Component.onCompleted: {
        console.log("slider handle", r(slider.handle), "| groove", r(slider.background), "| fill", kids(slider.background))
        slider.pressed = true; console.log("slider pressed", r(slider.handle)); slider.pressed = false
        slider.forceActiveFocus(Qt.TabFocusReason); console.log("slider tab focus", r(slider.handle)); root.forceActiveFocus()
        slider.enabled = false; console.log("slider disabled", r(slider.handle), "| fill", kids(slider.background)); slider.enabled = true
        console.log("vertical handle", r(vertical.handle), "| groove", r(vertical.background), "| fill", kids(vertical.background))
        console.log("range", r(range.first.handle), "|", r(range.second.handle), "| groove", r(range.background), "| fill", kids(range.background))
        range.first.pressed = true; console.log("range first pressed", r(range.first.handle)); range.first.pressed = false
        console.log("progress", r(progress.background), "| content", progress.contentItem.x, progress.contentItem.width, progress.contentItem.height, progress.contentItem.color, progress.contentItem.progress)
        let dots = []
        let row = pages.contentItem.children
        for (let i = 0; i < row.length; i++) if (row[i].width === 8) dots.push(r(row[i]))
        console.log("pages", dots.join(" ; "))
        console.log("spin up", r(spin.up.indicator), kids(spin.up.indicator), "| down", r(spin.down.indicator), kids(spin.down.indicator), "| bg", r(spin.background), "| text", spin.contentItem.color, spin.contentItem.horizontalAlignment, spin.contentItem.readOnly)
        spin.up.pressed = true; console.log("spin up pressed", r(spin.up.indicator)); spin.up.pressed = false
        spin.enabled = false; console.log("spin disabled", r(spin.up.indicator), kids(spin.up.indicator), "| bg", r(spin.background)); spin.enabled = true
        console.log("dial", dial.background.color, dial.background.progress, dial.background.opacity, dial.background.startAngle, dial.background.endAngle, dial.handle.x, dial.handle.y, dial.handle.width, dial.handle.height, dial.handle.color)
        Qt.quit()
    }
}
