import QtQuick
import QtLocation
import QtPositioning

Item {
    width: 400
    height: 300

    Plugin {
        id: osm
        name: "osm"
    }

    Map {
        id: map
        anchors.fill: parent
        plugin: osm
        center: QtPositioning.coordinate(59.91, 10.75)
        zoomLevel: 10

        MapQuickItem {
            id: marker
            coordinate: QtPositioning.coordinate(59.91, 10.75)
            anchorPoint.x: 5
            anchorPoint.y: 7
            sourceItem: Rectangle {
                width: 10
                height: 14
                color: "red"
            }
        }

        MapCircle {
            id: circle
            center: QtPositioning.coordinate(59.91, 10.75)
            radius: 1000
            color: "blue"
            border.width: 2
        }

        MapPolyline {
            id: line
            path: [
                QtPositioning.coordinate(59.90, 10.70),
                QtPositioning.coordinate(59.92, 10.80)
            ]
            line.width: 3
            line.color: "green"
        }
    }

    function r(v) {
        return Math.round(v * 1000) / 1000
    }

    Component.onCompleted: {
        console.log("center", map.center.latitude, map.center.longitude)
        console.log("zoom", map.zoomLevel)
        var p = map.fromCoordinate(map.center, false)
        console.log("point", r(p.x), r(p.y))
        var q = map.fromCoordinate(QtPositioning.coordinate(59.92, 10.80), false)
        console.log("offset", r(q.x), r(q.y))
        var c = map.toCoordinate(Qt.point(300, 100), false)
        console.log("coordinate", c.latitude.toFixed(5), c.longitude.toFixed(5))
        var out = map.fromCoordinate(QtPositioning.coordinate(0, 0), true)
        console.log("clipped", isNaN(out.x), isNaN(out.y))
        console.log("marker", marker.x, marker.y)
        map.zoomLevel = 12
        console.log("zoom", map.zoomLevel)
        q = map.fromCoordinate(QtPositioning.coordinate(59.91, 10.76), false)
        console.log("zoom12", r(q.x), r(q.y))
        map.zoomLevel = -5
        console.log("negative zoom", map.zoomLevel)
        map.zoomLevel = 0.5
        console.log("min zoom", map.zoomLevel, map.minimumZoomLevel)
        map.zoomLevel = 100
        console.log("max zoom", map.zoomLevel)
        map.center = QtPositioning.coordinate(0, 0)
        console.log("center", map.center.latitude, map.center.longitude)
        var a = QtPositioning.coordinate(59.91, 10.75)
        var b = QtPositioning.coordinate(59.91, 10.76)
        console.log("distance", Math.round(a.distanceTo(b)))
        console.log("azimuth", Math.round(a.azimuthTo(b) * 100) / 100)
        var n = a.atDistanceAndAzimuth(1000, 0)
        console.log("north", n.latitude.toFixed(5), n.longitude.toFixed(5))
        console.log("valid", QtPositioning.coordinate(95, 0).isValid, QtPositioning.coordinate(1, 2).isValid)
        console.log("path", line.pathLength())
        line.addCoordinate(QtPositioning.coordinate(59.95, 10.9))
        console.log("path", line.pathLength())
        Qt.quit()
    }
}
