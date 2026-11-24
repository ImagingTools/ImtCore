import QtQuick

// the corner radii of Rectangle (Qt 6.7): a corner not set is the radius and changes with it; undefined resets it
Rectangle {
    id: r
    width: 100; height: 60
    radius: 4
    topLeftRadius: 10
    property real tl: topLeftRadius
    property real br: bottomRightRadius
    onTopLeftRadiusChanged: console.log("topLeft changed", topLeftRadius)
    onBottomRightRadiusChanged: console.log("bottomRight changed", bottomRightRadius)
    Component.onCompleted: {
        console.log(topLeftRadius, topRightRadius, bottomLeftRadius, bottomRightRadius, tl, br)
        radius = 8
        console.log(topLeftRadius, topRightRadius, bottomRightRadius, tl, br)
        bottomRightRadius = 8
        bottomRightRadius = 8
        radius = 2
        console.log(bottomRightRadius, br)
        topLeftRadius = undefined
        console.log(topLeftRadius, tl)
        topLeftRadius = 0
        console.log(topLeftRadius, radius)
        Qt.quit()
    }
}
