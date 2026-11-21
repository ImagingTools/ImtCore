import QtQuick

// ?? and ** in bindings and functions, with ?. (optional chaining); (??= ||= &&= are errors in Qt 6.12)
Item {
    property var model: ({ idx: 0, txt: null })
    property int idx: model.idx ?? 5
    property string txt: model.txt ?? "none"
    property var deep: model?.nested?.value ?? "absent"
    property real power: 2 ** 10
    Component.onCompleted: {
        let a = null, n = 2
        n **= 3
        console.log(idx, txt, deep, power, a ?? 3, n, (undefined ?? null) ?? "x", 0 ?? 1)
        Qt.quit()
    }
}
