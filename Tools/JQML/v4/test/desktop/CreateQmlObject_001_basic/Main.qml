import QtQuick

// Qt.createQmlObject(qml, parent[, url]): the object of a QML text as a child of the parent, whose ids its bindings see; null for an
// empty text; an Error for a text that does not parse, for no parent. The message of a parse error is the parser of each side, so
// only its place is compared. What it does with types and properties that do not exist is in test/create-qml-object.mjs.
Item {
    id: root
    width: 300
    height: 300
    property int base: 7

    function tryCreate(label, code, parent, path) {
        try {
            var o = path === undefined ? Qt.createQmlObject(code, parent) : Qt.createQmlObject(code, parent, path)
            console.log(label + ": ok", o === null ? "null" : typeof o, o !== null && label !== "qobject" && parent === root ? o.parent === parent : "-")
            return o
        } catch (e) {
            var m = String(e.message).replace(/\n    \S*?:(\d+):(\d+): .*/, " at $1:$2")
            console.log(label + ": error", e.name, JSON.stringify(m))
            return null
        }
    }

    Component.onCompleted: {
        var r = tryCreate("rect", 'import QtQuick\nRectangle { width: 20; height: 30; color: "red" }', root)
        console.log("rect", r.width, r.height, root.children.length, r.parent === root)
        var b = tryCreate("binding to parent id", 'import QtQuick\nRectangle { width: root.base * 2; height: parent.height }', root)
        console.log("binding", b.width, b.height)
        root.base = 10
        console.log("binding after", b.width)
        var h = tryCreate("handler", 'import QtQuick\nItem { property int n: 0; signal ping(); onPing: root.base = 99 + n; Component.onCompleted: n = 1 }', root)
        h.ping()
        console.log("handler", root.base, h.n)
        var ids = tryCreate("inner ids", 'import QtQuick\nItem { Rectangle { id: inner; width: 5 } Text { id: t; text: "w" + inner.width } }', root)
        console.log("inner", ids.children.length, ids.children[1].text)
        console.log("inner id outside", typeof inner)
        tryCreate("syntax error", 'import QtQuick\nRectangle { width: }', root)
        tryCreate("empty", '', root)
        tryCreate("null parent", 'import QtQuick\nItem { width: 3 }', null)
        tryCreate("undefined parent", 'import QtQuick\nItem { width: 3 }', undefined)
        tryCreate("qobject", 'import QtQuick\nQtObject { property int a: 4 }', root)
        tryCreate("non-visual parent", 'import QtQuick\nItem { }', Qt.createQmlObject('import QtQuick\nQtObject { }', root))
        tryCreate("runtime error in binding", 'import QtQuick\nItem { property int a: nothere.x }', root)
        tryCreate("with path", 'import QtQuick\nItem { }', root, "file:///C:/some/dir/dyn.qml")
        tryCreate("bad syntax with path", 'import QtQuick\nItem { width: }', root, "file:///C:/some/dir/dyn.qml")
        var again = tryCreate("same text again", 'import QtQuick\nItem { objectName: "z" }', root)
        var second = tryCreate("same text twice", 'import QtQuick\nItem { objectName: "z" }', root)
        console.log("twice", again !== second, again.objectName, second.objectName)
        Qt.quit()
    }
}
