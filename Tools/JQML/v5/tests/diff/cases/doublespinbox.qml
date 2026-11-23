import QtQuick
import QtQuick.Controls
import QtTest

// DoubleSpinBox: the defaults, rounding to decimals, from / to / clamping, stepSize, wrap, decimals changed, the display
// text in a locale, textFromValue / valueFromText with decimals, the indicators and keys, increase / decrease
Item {
    width: 400; height: 300
    property var log: []
    DoubleSpinBox { id: d; locale: Qt.locale("en_US"); onValueModified: log.push("modified " + value) }
    DoubleSpinBox { id: e; y: 60; locale: Qt.locale("de_DE"); from: -10; to: 2000; value: 1234.5678; stepSize: 0.25; decimals: 3 }
    DoubleSpinBox { id: f; y: 120; locale: Qt.locale("en_US"); value: 3.14159
        textFromValue: function(value, decimals, locale) { return "~" + value.toFixed(decimals) }
        valueFromText: function(text, locale) { return Number(text.replace("~", "")) } }
    TestCase { name: "Double"; when: windowShown
        function test_a() {
            console.log("d", d.from, d.to, d.value, d.stepSize, d.decimals, d.displayText, d.wrap, d.editable, d.inputMethodHints, d.validator !== null, d.validator.decimals, d.validator.top)
            console.log("e", e.value, e.displayText, e.from, e.to, e.decimals)
            console.log("f", f.value, f.displayText)
            d.value = 1.23456; console.log("round", d.value, d.displayText)
            d.value = 150; console.log("clamp", d.value); d.value = -3; console.log("clamp", d.value)
            d.decimals = 1; d.value = 2.26; console.log("decimals 1", d.value, d.displayText, d.to)
            d.decimals = 2; d.stepSize = 0.5; d.increase(); console.log("increase", d.value); d.decrease(); d.decrease(); console.log("decrease", d.value)
            d.value = 99.9; d.to = 99.99; d.wrap = true; d.increase(); console.log("wrap", d.value)
            d.forceActiveFocus(); keyClick(Qt.Key_Down); console.log("key down", d.value, log.join(",")); log = []
            mouseClick(d, d.width - 10, d.height / 2); console.log("up", d.value, log.join(",")); log = []
            mouseClick(d, 10, d.height / 2); console.log("down", d.value, log.join(",")); log = []
            e.increase(); console.log("e", e.value, e.displayText)
            f.value = 2; console.log("f", f.displayText)
        }
    }
}
