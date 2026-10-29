import QtQuick

Item {
    Component.onCompleted: {
        var show = function(value) {
            if (value === undefined)
                return "undefined"
            if (value === null)
                return "null"
            return "" + value
        }

        var ticks = 0

        var tick = function() {
            ticks = ticks + 1
            return 7
        }

        var greet = function() {
            return "hello"
        }

        var fn = null
        console.log("skip " + show(fn?.(tick())) + " " + ticks)

        console.log("direct " + show(greet?.()))

        fn = greet
        console.log("arg " + show(fn?.(tick())) + " " + ticks)

        var obj = {}
        console.log("method-miss " + show(obj.method?.()))
        obj.method = function() {
            return "button"
        }
        console.log("method " + show(obj.method?.()))

        var none = null
        console.log("safe-method " + show(none?.method?.()))

        Qt.quit()
    }
}
