import QtQuick
import QtLocation
import QtPositioning

Item {
    id: root
    width: 400
    height: 300

    property int centerChanges: 0
    property int readyChanges: 0

    Plugin {
        id: osm
        name: "osm"
    }

    Map {
        id: map
        anchors.fill: parent
        plugin: osm
        center: QtPositioning.coordinate(50, 10)
        zoomLevel: 6

        onCenterChanged: root.centerChanges++
        onMapReadyChanged: root.readyChanges++

        MapQuickItem {
            id: pin
            coordinate {
                latitude: 51.5
                longitude: -0.12
            }
            sourceItem: Rectangle {
                width: 6
                height: 6
            }
        }

        MapCircle {
            id: circle
            center {
                latitude: 40
                longitude: 20
            }
            radius: 5000
        }
    }

    Timer {
        interval: 100
        running: true
        onTriggered: {
            console.log("no gesture", typeof map.gesture)
            console.log("ready", map.mapReady, root.readyChanges)
            console.log("pin", pin.coordinate.latitude, pin.coordinate.longitude)
            console.log("circle", circle.center.latitude, circle.center.longitude)

            var before = root.centerChanges
            map.center.latitude = 52
            console.log("latitude", map.center.latitude, map.center.longitude, root.centerChanges - before)
            map.center.longitude = 11
            console.log("longitude", map.center.latitude, map.center.longitude, root.centerChanges - before)

            pin.coordinate.latitude = 10
            pin.coordinate.longitude = 20
            console.log("pin moved", pin.coordinate.latitude, pin.coordinate.longitude)

            // a coordinate that was assigned is a copy
            var c = QtPositioning.coordinate(30, 15)
            map.center = c
            c.latitude = 1
            console.log("copy", map.center.latitude, c.latitude)

            // an invalid coordinate is ignored
            map.center = QtPositioning.coordinate(200, 0)
            console.log("invalid", map.center.latitude, map.center.longitude)

            map.zoomLevel = -3
            console.log("negative", map.zoomLevel)
            map.zoomLevel = 100
            console.log("too large", map.zoomLevel)
            Qt.quit()
        }
    }
}
