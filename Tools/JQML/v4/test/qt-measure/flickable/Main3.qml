import QtQuick
import QtTest

Item {
    id: root
    width: 400
    height: 400
    TestCase { id: tc; name: "measure"; when: windowShown }
    Flickable {
        id: flick
        x: 0; y: 0; width: 300; height: 300
        contentWidth: 20000
        contentHeight: 300
        boundsBehavior: Flickable.StopAtBounds
    }
    property var t0: 0
    // steps: list of [dx, delay]; release after idle ms
    function run(name, steps, idle) {
        flick.contentX = 5000
        tc.wait(20)
        var x = 280
        tc.mousePress(flick, x, 150)
        var times = []
        for (var i = 0; i < steps.length; i++) {
            x -= steps[i][0]
            tc.mouseMove(flick, x, 150, steps[i][1])
            times.push(Date.now())
        }
        var atRelease = flick.contentX
        var tr = Date.now()
        tc.mouseRelease(flick, x, 150, Qt.LeftButton, Qt.NoModifier, idle)
        var after = flick.contentX
        tc.wait(1800)
        var dist = flick.contentX - after
        var gaps = []
        for (var j = 1; j < times.length; j++) gaps.push(times[j] - times[j - 1])
        console.log(name, "idle", idle, "gaps", gaps.join(","), "contentX at release", atRelease.toFixed(1), "after", after.toFixed(1), "flick dist", dist.toFixed(1), "v", Math.sqrt(2 * 1500 * Math.max(0, dist)).toFixed(0), "flicking-start-v")
    }
    function rep(n, dx, d) { var l = []; for (var i = 0; i < n; i++) l.push([dx, d]); return l }
    Timer {
        interval: 200
        running: true
        onTriggered: {
            run("20px/10ms x8", rep(8, 20, 10), 0)
            run("20px/20ms x8", rep(8, 20, 20), 0)
            run("10px/10ms x12", rep(12, 10, 10), 0)
            run("40px/16ms x8", rep(8, 40, 16), 0)
            run("acc 5,10,20,40,80 /10", [[5,10],[5,10],[5,10],[10,10],[20,10],[40,10],[80,10]], 0)
            run("dec 80,40,20,10,5,5 /10", [[20,10],[20,10],[80,10],[40,10],[20,10],[10,10],[5,10]], 0)
            for (var idle = 0; idle <= 200; idle += 40) run("20px/10ms x8 then idle", rep(8, 20, 10), idle)
            run("20px/10ms x3", rep(3, 20, 10), 0)
            run("30px/10ms x2", rep(2, 30, 10), 0)
            run("50px/10ms x8", rep(8, 50, 10), 0)
            Qt.quit()
        }
    }
}
