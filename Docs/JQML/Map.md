# Map in JQML v4 (QtLocation / QtPositioning)

The v4 engine (`Tools/JQML/v4`) draws a `Map` like Qt Location does: Web Mercator tiles and map items placed by
coordinates.

```qml
import QtQuick
import QtLocation
import QtPositioning

Map {
    id: map
    anchors.fill: parent
    plugin: Plugin { name: "osm" }
    center: QtPositioning.coordinate(59.91, 10.75)
    zoomLevel: 11

    WheelHandler {
        property: "zoomLevel"
        rotationScale: 1 / 120
    }

    DragHandler {
        target: null
        onTranslationChanged: map.pan(-delta.x, -delta.y)
    }

    MapQuickItem {
        coordinate: QtPositioning.coordinate(59.91, 10.75)
        anchorPoint.x: 5
        anchorPoint.y: 5
        sourceItem: Rectangle { width: 10; height: 10; radius: 5; color: "red" }
    }
}
```

## Interaction

A `Map` is **not interactive by itself**, the same as in Qt 6 (there is no `Map.gesture` any more). Add the pointer
handlers shown above to pan and zoom it. A map without handlers lets the mouse through to the items below it.

| Handler | Supported |
| --- | --- |
| `WheelHandler` | `property`, `rotationScale`, `rotation`, `orientation`, `enabled`, `target`, `acceptedDevices`, signal `wheel(event)`. The property of the target grows by `angle * rotationScale`, one notch is 120. |
| `DragHandler` | `target` (the parent; `null` moves nothing), `translation`, `activeTranslation`, `persistentTranslation`, `active`, `centroid`, `dragThreshold`, `xAxis` and `yAxis` (`enabled`, `minimum`, `maximum`), `acceptedButtons`, `enabled`, signals `translationChanged(delta)` and `canceled()`. |

A `DragHandler` starts when the pointer has moved by `dragThreshold` (10 px) and then takes the press from a `MouseArea`
below it, unless the `MouseArea` has `preventStealing: true`: a drag that starts on a marker pans the map, a click
without a drag still reaches the marker. The first translation after the start includes the distance of the
threshold, like in Qt.

`PointerDevice` and `Qt.platform.pluginName` exist. Every wheel of the browser counts as a mouse (and a touchpad).
`PinchHandler`, `TapHandler`, `HoverHandler` and `MapView` are not supported.

## Supported

| Type | Properties and methods |
| --- | --- |
| `Map` | `plugin`, `center`, `zoomLevel`, `minimumZoomLevel`, `maximumZoomLevel`, `mapReady`, `color`, `copyrightsVisible`, `mapItems`, `toCoordinate()`, `fromCoordinate()`, `alignCoordinateToPoint()`, `pan()`, `fitViewportToMapItems()`, `fitViewportToVisibleMapItems()`, `addMapItem()`, `removeMapItem()`, `clearMapItems()`, `clearData()` |
| `Plugin`, `PluginParameter` | `name`; only the `osm` plugin is drawn |
| `MapQuickItem` | `coordinate`, `sourceItem`, `anchorPoint`, `zoomLevel`; the mouse reaches the items of `sourceItem` |
| `MapCircle` | `center`, `radius` (meters), `color`, `border.color`, `border.width` |
| `MapRectangle` | `topLeft`, `bottomRight`, `color`, `border.*` |
| `MapPolyline` | `path`, `line.color`, `line.width`, `addCoordinate()`, `insertCoordinate()`, `setCoordinate()`, `removeCoordinate()`, `coordinateAt()`, `pathLength()` |
| `MapPolygon` | `path`, `color`, `border.*` and the same path methods |
| `MapItemView` | `model`, `delegate` (the delegates are map items) |
| `QtPositioning.coordinate()` | `latitude`, `longitude`, `altitude`, `isValid`, `distanceTo()`, `azimuthTo()`, `atDistanceAndAzimuth()` |

`coordinate { latitude: 1; longitude: 2 }`, `center.latitude = 1` and every other group of properties written as a block
work. A coordinate that is assigned is copied. `mapReady` becomes true when the map has a plugin and a size. The zoom
level is limited by `minimumZoomLevel` (at least the zoom at which the world fills the map), `maximumZoomLevel` (30)
and ignores negative values.

## Tests

The projection, the zoom limits, `fitViewportToMapItems()`, the properties, the group syntax and the defaults of the
handlers are compared with desktop Qt (`node test/desktop/run.mjs Map_001_basic`, `Map_002_view`, `Map_003_properties`,
`Handlers_001_defaults`, `Group_001_block_syntax`). Qt cannot receive mouse events without a window, so the behavior
of the handlers (drag, wheel, taking the press from a `MouseArea`) is tested through the scene by `node test/map.mjs`.

## Tiles

By default the tiles come from `https://tile.openstreetmap.org/{z}/{x}/{y}.png` and the map shows
"© OpenStreetMap contributors". Parameters of the `osm` plugin:

| Parameter | Meaning |
| --- | --- |
| `osm.mapping.custom.host` (or `osm.mapping.host`) | Tile server. `/{z}/{x}/{y}.png` is appended, unless the value already contains `{z}` or `%z` (then it is the whole template). |
| `osm.mapping.copyright` | Text of the attribution. Empty for a custom host unless this is set. |

The tile server has to allow cross-origin image requests (the OpenStreetMap server does). Follow the usage policy of
the tile server you use: https://operations.osmfoundation.org/policies/tiles/

## Not supported

Other plugins (`mapbox`, `esri`, `here` ...), map types, tilt and bearing, pinch and rotation, routes, geocoding and
`PositionSource`. `MapItemView.autoFitViewport` is stored but does nothing. `Map.children` also lists the map items (in
Qt they are only in `mapItems`).
