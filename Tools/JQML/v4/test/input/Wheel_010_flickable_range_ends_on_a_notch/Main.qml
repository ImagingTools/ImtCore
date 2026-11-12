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

        Flickable {
            id: inner
            width: 200
            height: 1000
            contentWidth: 200
            contentHeight: 1144
            boundsBehavior: Flickable.StopAtBounds
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
