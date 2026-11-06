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

    ListModel {
        id: places
        ListElement { lat: 59.91; lon: 10.75; label: "A" }
        ListElement { lat: 59.95; lon: 10.85; label: "B" }
    }

    Map {
        id: map
        anchors.fill: parent
        plugin: osm
        center: QtPositioning.coordinate(59.91, 10.75)
        zoomLevel: 11

        MapItemView {
            model: places
            delegate: MapQuickItem {
                coordinate: QtPositioning.coordinate(lat, lon)
                anchorPoint.x: 4
                anchorPoint.y: 8
                sourceItem: Rectangle {
                    width: 8
                    height: 8
                    color: "red"
                }
            }
        }

        MapRectangle {
            topLeft: QtPositioning.coordinate(59.96, 10.70)
            bottomRight: QtPositioning.coordinate(59.90, 10.85)
            color: "#400000ff"
        }

        MapPolygon {
            path: [
                QtPositioning.coordinate(59.90, 10.70),
                QtPositioning.coordinate(59.92, 10.74),
                QtPositioning.coordinate(59.91, 10.80),
                QtPositioning.coordinate(59.89, 10.78)
            ]
            color: "green"
            border.width: 2
        }
    }

    Timer {
        interval: 50
        running: true
        onTriggered: {
            var count = 0
            for (var i = 0; i < map.mapItems.length; ++i) {
                if (map.mapItems[i].coordinate !== undefined) {
                    var c = map.mapItems[i]
                    console.log("item", c.coordinate.latitude, Math.round(c.x * 1000) / 1000, Math.round(c.y * 1000) / 1000, c.width, c.height)
                    count++
                }
            }
            console.log("items", count)
            map.fitViewportToMapItems()
            console.log("fit", Math.round(map.zoomLevel * 1000) / 1000, Math.round(map.center.latitude * 10000) / 10000, Math.round(map.center.longitude * 10000) / 10000)
            Qt.quit()
        }
    }
}
