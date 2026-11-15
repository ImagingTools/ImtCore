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
        contentWidth: 600
        contentHeight: 1000
        boundsBehavior: Flickable.StopAtBounds
        clip: true

        Flickable {
            id: inner
            width: 600
            height: 1000
            contentWidth: 700
            contentHeight: 1000
            boundsBehavior: Flickable.StopAtBounds
        }
    }

    TestCase {
        id: tc
        name: "wheel"
        when: windowShown

        function test_wheel() {
            var plan = [[0, -120], [-120, 0], [-120, 0], [-120, -120], [-120, 0], [0, -120], [120, 120], [120, 0], [120, 0], [0, 120]]
            var out = []
            for (var i = 0; i < plan.length; i++) {
                mouseWheel(root, 50, 50, plan[i][0], plan[i][1])
                wait(350)
                out.push([Math.round(outer.contentX), Math.round(outer.contentY), Math.round(inner.contentX), Math.round(inner.contentY)])
            }
            console.log("outer x y, inner x y", JSON.stringify(out))
        }
    }
}
