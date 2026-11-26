import QtQuick

// Type annotations of functions (Qt 6): the parameters and the return type, a list type, a
// function of a signal handler
Item {
    id: root
    signal done(value: int)
    function twice(n: int): int { return n * 2 }
    function join(items: list<string>, sep: string): string { return items.join(sep) }
    function noType(a, b: real) { return a + b }
    property var arrow: x => x + 0.5
    property var cond: { "a": 1 }
    onDone: function(value: int) { console.log("done", value) }
    Component.onCompleted: {
        console.log(twice(4), join(["a", "b"], "-"), noType(1, 2), arrow(1), cond.a, "x: 1".length)
        done(7)
        Qt.quit()
    }
}
