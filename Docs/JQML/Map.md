# Map in JQML v4 (QtLocation / QtPositioning)

The v4 engine (`Tools/JQML/v4`) draws a `Map` like Qt Location does: Web Mercator tiles and map items placed by
coordinates.

```qml
import QtQuick
import QtLocation
import QtPositioning

Map {
    anchors.fill: parent
    plugin: Plugin { name: "osm" }
    center: QtPositioning.coordinate(59.91, 10.75)
    zoomLevel: 11

    MapQuickItem {
        coordinate: QtPositioning.coordinate(59.91, 10.75)
        anchorPoint.x: 5
        anchorPoint.y: 5
        sourceItem: Rectangle { width: 10; height: 10; radius: 5; color: "red" }
    }
}
```

## Supported

| Type | Properties and methods |
| --- | --- |
| `Map` | `plugin`, `center`, `zoomLevel`, `minimumZoomLevel`, `maximumZoomLevel`, `color`, `copyrightsVisible`, `gesture.enabled`, `mapItems`, `toCoordinate()`, `fromCoordinate()`, `alignCoordinateToPoint()`, `pan()`, `fitViewportToMapItems()`, `fitViewportToVisibleMapItems()`, `addMapItem()`, `removeMapItem()`, `clearMapItems()`, `clearData()` |
| `Plugin`, `PluginParameter` | `name`; only the `osm` plugin is drawn |
| `MapQuickItem` | `coordinate`, `sourceItem`, `anchorPoint`, `zoomLevel`; the mouse reaches the items of `sourceItem` |
| `MapCircle` | `center`, `radius` (meters), `color`, `border.color`, `border.width` |
| `MapRectangle` | `topLeft`, `bottomRight`, `color`, `border.*` |
| `MapPolyline` | `path`, `line.color`, `line.width`, `addCoordinate()`, `insertCoordinate()`, `setCoordinate()`, `removeCoordinate()`, `coordinateAt()`, `pathLength()` |
| `MapPolygon` | `path`, `color`, `border.*` and the same path methods |
| `MapItemView` | `model`, `delegate` (the delegates are map items) |
| `QtPositioning.coordinate()` | `latitude`, `longitude`, `altitude`, `isValid`, `distanceTo()`, `azimuthTo()`, `atDistanceAndAzimuth()` |

The projection, the zoom limits, `fitViewportToMapItems()` and the values above are compared with desktop Qt by
`test/jqml/Map_001_basic` and `test/jqml/Map_002_view` (`node test/desktop/run.mjs Map_001_basic`).

## Tiles

By default the tiles come from `https://tile.openstreetmap.org/{z}/{x}/{y}.png` and the map shows
"© OpenStreetMap contributors". Parameters of the `osm` plugin:

| Parameter | Meaning |
| --- | --- |
| `osm.mapping.custom.host` (or `osm.mapping.host`) | Tile server. `/{z}/{x}/{y}.png` is appended, unless the value already contains `{z}` or `%z` (then it is the whole template). |
| `osm.mapping.copyright` | Text of the attribution. Empty for a custom host unless this is set. |

The tile server has to allow cross-origin image requests (the OpenStreetMap server does). Follow the usage policy of
the tile server you use: https://operations.osmfoundation.org/policies/tiles/

## Interaction

Dragging pans, the wheel zooms by 0.5 per notch around the pointer, a double click zooms in by 1.
`gesture.enabled: false` turns this off. A `MouseArea` over the map takes the mouse as in Qt.

## Not supported

Other plugins (`mapbox`, `esri`, `here` ...), map types, tilt and bearing, pinch and rotation gestures, routes,
geocoding and `PositionSource`. `MapItemView.autoFitViewport` is stored but does nothing. `Map.children` also lists the
map items (in Qt they are only in `mapItems`).
