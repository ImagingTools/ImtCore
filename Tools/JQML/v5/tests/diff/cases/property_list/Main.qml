import QtQuick

// property list<T> (QQmlListProperty of a QML document): an empty list by default, objects declared in it owned by
// the object (no visual parent), assignment of arrays and of one object, push, list<int>, the changed signal, a
// binding on the length, a list declared in a type and set in a use of it
Item {
    id: root
    width: 100; height: 100
    property list<QtObject> empty
    property list<QtObject> objs: [
        QtObject { objectName: "a" },
        QtObject { objectName: "b" }
    ]
    property list<Item> items: [
        Item { objectName: "i1"; width: 10 },
        Rectangle { objectName: "i2"; width: 20 }
    ]
    property list<int> nums: [1, 2, 3]
    property list<string> words: ["x", "y"]
    property int objsLength: objs.length
    property int changes: 0
    onObjsChanged: changes++
    Holder { id: holder; items: [ QtObject { objectName: "h1" } ] }
    Holder { id: plain }
    QtObject { id: extra; objectName: "extra" }
    function names(list) {
        let out = []
        for (let i = 0; i < list.length; i++) out.push(list[i] ? list[i].objectName : "null")
        return out.join(",")
    }
    Component.onCompleted: {
        console.log("empty", empty.length, typeof empty)
        console.log("objs", objs.length, names(objs), objsLength)
        console.log("items", items.length, names(items), items[0].parent === null, items[1].width, root.children.length)
        console.log("nums", nums.length, nums[0] + nums[2], JSON.stringify(nums))
        console.log("words", words.length, words.join("-"))
        console.log("holder", holder.items.length, holder.count, names(holder.items), plain.items.length, plain.count)
        objs = [extra]
        console.log("assign array", objs.length, names(objs), objsLength, changes)
        objs = extra
        console.log("assign one", objs.length, names(objs), changes)
        objs.push(holder)
        console.log("push", objs.length, objsLength)
        plain.items = [extra, extra]
        console.log("plain set", plain.items.length, plain.count)
        nums = [5]
        console.log("nums set", nums.length, nums[0])
        objs = []
        console.log("cleared", objs.length, objsLength, changes)
        Qt.quit()
    }
}
