import QtQuick
Item {
    id: root
    property int xSource: 5
    property int ySource: 6
    Box { id: box; fieldX: root.xSource; fieldY: root.ySource }
    function report(stage){ console.log(stage, box.fieldX, box.fieldY, box.region.width) }
    Component.onCompleted: {
        report("initial")
        root.xSource = 9
        root.ySource = 10
        report("changed")
        box.fieldX = 11
        root.xSource = 13
        root.ySource = 12
        report("after field write")
        box.fieldX = Qt.binding(function(){ return root.xSource })
        report("new binding")
        box.region = Qt.rect(15, 16, 17, 18)
        root.xSource = 20
        root.ySource = 21
        report("after whole write")
        Qt.quit()
    }
}
