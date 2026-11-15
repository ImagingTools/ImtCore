import QtQuick
import QtTest

Item {
    id: root
    width: 300
    height: 300

    Flickable {
        id: outer
        width: 200
        height: 200
        contentWidth: 200
        contentHeight: 1000
        boundsBehavior: Flickable.StopAtBounds
        clip: true

        GridView {
            id: inner
            width: 200
            height: 1000
            cellWidth: 50
            cellHeight: 20
            model: 24
            boundsBehavior: Flickable.StopAtBounds
            delegate: Rectangle { width: 50; height: 20 }
        }
    }

    TestCase {
        id: tc
        name: "wheel"
        when: windowShown


        function test_wheel() {
            var plan = [-120, -120, -120, -120, -120, -120, 120, 120, 120, 120, 120, 120, 120, 120]
            var out = []
            for (var i = 0; i < plan.length; i++) {
                mouseWheel(root, 50, 50, 0, plan[i])
                wait(350)
                out.push([Math.round(outer.contentY), Math.round(inner.contentY)])
            }
            console.log("outer, inner", JSON.stringify(out))
        }
    }
}
