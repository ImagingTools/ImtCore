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

        ListView {
            id: inner
            width: 200
            height: 1000
            orientation: ListView.Horizontal
            model: 6
            boundsBehavior: Flickable.StopAtBounds
            delegate: Rectangle { width: 100; height: 100 }
        }
    }

    TestCase {
        id: tc
        name: "wheel"
        when: windowShown

        function test_wheel() {
            var plan = [[-120, 0], [-120, 0], [-120, 0], [-120, 0], [-120, 0], [120, 0], [120, 0], [120, 0], [120, 0], [120, 0], [0, -120], [0, 120]]
            var out = []
            for (var i = 0; i < plan.length; i++) {
                mouseWheel(root, 50, 50, plan[i][0], plan[i][1])
                wait(350)
                out.push([Math.round(outer.contentY), Math.round(inner.contentX)])
            }
            console.log("outer y, inner x", JSON.stringify(out))
        }
    }
}
