import QtQuick
import QtQuick.Controls
import QtTest

// Slider, RangeSlider, Dial, SpinBox, PageIndicator, ProgressBar: the geometry of their parts, a press on the track and
// on the handle, a drag, keys, snapping, live, clamping, the nearer handle of a RangeSlider, the angle of a Dial, the
// indicators, keys and wrap of a SpinBox, a click on a page
Item {
    width: 600; height: 500
    property var log: []
    Slider { id: s; width: 212; onMoved: log.push("moved " + value.toFixed(3)); onValueChanged: log.push("value " + value.toFixed(3)); onPressedChanged: log.push("pressed " + pressed) }
    Slider { id: st; y: 50; width: 212; from: 10; to: 20; stepSize: 2.5; snapMode: Slider.SnapAlways }
    Slider { id: sv; x: 300; height: 212; orientation: Qt.Vertical }
    RangeSlider { id: rs; y: 100; width: 212; first.value: 0.25; second.value: 0.75 }
    Dial { id: d; y: 150 }
    SpinBox { id: sb; x: 300; y: 300; onValueModified: log.push("modified " + value) }
    PageIndicator { id: pi; x: 400; count: 5; interactive: true }
    ProgressBar { id: pb; y: 360; value: 0.4 }
    function f(c) { return log.join(", ") }
    TestCase { name: "Range"; when: windowShown
        function flush(n) { console.log(n + ": " + log.join(", ")); log = [] }
        function test_slider() {
            console.log("slider", s.implicitWidth, s.implicitHeight, s.handle.x, s.handle.y, s.background.x, s.background.y, s.background.width, s.background.height, s.position, s.visualPosition, s.live, s.focusPolicy, s.touchDragThreshold)
            mousePress(s, 106, 20); flush("press middle")
            mouseMove(s, 150, 20); flush("move")
            mouseRelease(s, 150, 20); flush("release")
            mouseClick(s, s.handle.x + 14, 20); flush("click handle")
            s.forceActiveFocus(); keyClick(Qt.Key_Right); flush("right"); keyClick(Qt.Key_Left); keyClick(Qt.Key_Down); flush("left down"); keyClick(Qt.Key_Home); flush("home")
            s.increase(); flush("increase()"); console.log("valueAt", s.valueAt(0.5), st.valueAt(0.33))
            mousePress(st, 70, 20); mouseMove(st, 72, 20); console.log("snap", st.value, st.position); mouseRelease(st, 72, 20)
            st.value = 13; console.log("set 13", st.value); st.value = 30; console.log("set 30", st.value, st.position)
            mouseClick(sv, 20, 50); console.log("vertical", sv.value.toFixed(3), sv.handle.y, sv.visualPosition.toFixed(3))
            s.live = false; mousePress(s, 50, 20); mouseMove(s, 100, 20); flush("not live move"); mouseRelease(s, 100, 20); flush("not live release")
        }
        function test_range() {
            console.log("range", rs.implicitWidth, rs.first.handle.x, rs.second.handle.x, rs.first.position, rs.second.visualPosition)
            mousePress(rs, 160, 20); console.log("press near second", rs.first.pressed, rs.second.pressed); mouseMove(rs, 180, 20); mouseRelease(rs, 180, 20)
            console.log("after", rs.first.value.toFixed(3), rs.second.value.toFixed(3))
            rs.first.value = 0.9; console.log("first beyond", rs.first.value, rs.second.value)
            rs.setValues(0.1, 0.2); console.log("setValues", rs.first.value, rs.second.value)
        }
        function test_dial() {
            console.log("dial", d.implicitWidth, d.implicitHeight, d.angle, d.startAngle, d.endAngle, d.inputMode, d.wrap, d.handle.x, d.handle.y)
            mouseClick(d, 92, 10); console.log("top", d.value.toFixed(3), d.angle.toFixed(1))
            mouseClick(d, 180, 92); console.log("right", d.value.toFixed(3), d.angle.toFixed(1))
            d.value = 1; console.log("max", d.angle)
        }
        function test_spin() {
            console.log("spin", sb.implicitWidth, sb.implicitHeight, sb.from, sb.to, sb.value, sb.stepSize, sb.editable, sb.displayText, sb.leftPadding, sb.rightPadding, sb.up.indicator.x, sb.down.indicator.x, sb.contentItem.x, sb.contentItem.width, sb.focusPolicy, sb.wrap)
            mouseClick(sb, sb.width - 10, 20); flush("up click")
            mouseClick(sb, 10, 20); mouseClick(sb, 10, 20); flush("down click")
            sb.forceActiveFocus(); keyClick(Qt.Key_Up); keyClick(Qt.Key_Up); flush("keys up"); keyClick(Qt.Key_PageUp); flush("page up")
            mouseWheel(sb, 70, 20, 0, 120); flush("wheel")
            sb.value = 200; console.log("clamped", sb.value); sb.wrap = true; sb.value = 99; sb.increase(); console.log("wrap", sb.value)
            console.log("text", sb.textFromValue(42, Qt.locale()), sb.valueFromText("17", Qt.locale()))
        }
        function test_page() {
            console.log("page", pi.implicitWidth, pi.implicitHeight, pi.currentIndex, pi.contentItem.children.length, pi.focusPolicy)
            mouseClick(pi, 6 + 14 * 2 + 4, 10); console.log("click 2", pi.currentIndex)
            console.log("progress", pb.implicitWidth, pb.implicitHeight, pb.position, pb.visualPosition, pb.indeterminate, pb.background.y, pb.contentItem.width)
        }
    }
}
