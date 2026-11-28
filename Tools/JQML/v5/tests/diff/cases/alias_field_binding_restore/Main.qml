import QtQuick
Item {
    id: root
    property int xSource: 5
    property int ySource: 6
    property bool overrideValue: false
    Box {
        id: box
        fieldX: root.xSource
        fieldY: root.ySource

        states: State {
            name: "override"
            PropertyChanges { target: box; region: Qt.rect(31, 32, 33, 34) }
        }
    }
    Binding {
        target: box
        property: "region"
        value: Qt.rect(21, 22, 23, 24)
        when: root.overrideValue
    }
    function report(stage){ console.log(stage, box.fieldX, box.fieldY, box.region.width) }
    Component.onCompleted: {
        report("initial")
        root.overrideValue = true
        root.xSource = 7
        root.ySource = 8
        report("override")
        root.overrideValue = false
        report("restored")
        box.state = "override"
        root.xSource = 9
        root.ySource = 10
        report("state")
        box.state = ""
        report("after state")
        Qt.quit()
    }
}
