import QtQuick

// Item.transform: Scale (origin, xScale / yScale), Rotation (origin, angle), Translate, a list of them, with the item's
// own scale; the place of a child on the scene (mapToItem) through them; a Behavior on xScale; a change of a transform
Item {
    width: 400; height: 400
    Rectangle { id: a; x: 10; y: 20; width: 100; height: 50
        transform: Scale { id: s; origin.x: 50; origin.y: 25; xScale: 2; yScale: 0.5 } }
    Rectangle { id: b; x: 200; y: 20; width: 100; height: 50
        transform: [ Rotation { origin.x: 0; origin.y: 0; angle: 90 }, Translate { x: 30; y: 5 } ] }
    Rectangle { id: c; x: 50; y: 200; width: 60; height: 60; scale: 2
        transform: Translate { id: t; x: 10 } }
    Rectangle { id: d; x: 200; y: 200; width: 80; height: 40
        transform: Scale { id: z; xScale: 0; yScale: 0; Behavior on xScale { NumberAnimation { duration: 100 } } } }
    function at(item, x, y) { let p = item.mapToItem(null, x, y); return Math.round(p.x) + "," + Math.round(p.y) }
    Timer { running: true; interval: 50; onTriggered: {
        console.log("scale", at(a, 0, 0), at(a, 100, 50), s.origin.x, s.xScale, s.zScale, a.transform.length === undefined ? "one" : a.transform.length)
        console.log("list", at(b, 0, 0), at(b, 100, 0), at(b, 0, 50))
        console.log("own scale", at(c, 0, 0), at(c, 60, 60))
        t.x = 20; console.log("changed", at(c, 0, 0))
        z.xScale = 1; z.yScale = 1
        done.start()
    } }
    Timer { id: done; interval: 300; onTriggered: { console.log("behavior", z.xScale, at(d, 80, 40)); Qt.quit() } }
}
