import QtQuick
import QtQuick.Window

// the attached Screen of an item and of a QtObject; the sizes are of the platform, so only how they relate is compared
Item {
    QtObject { id: o; property int w: Screen.width; property int h: Screen.height }
    Component.onCompleted: {
        console.log(o.w === Screen.width, o.h === Screen.height, Screen.width > 0 || Screen.width === 0,
            Screen.desktopAvailableWidth <= Screen.width, Screen.desktopAvailableHeight <= Screen.height,
            Screen.devicePixelRatio >= 1, Screen.logicalPixelDensity > 0, Screen.pixelDensity > 0,
            Screen.primaryOrientation === (Screen.width >= Screen.height ? Qt.LandscapeOrientation : Qt.PortraitOrientation),
            Screen.orientationUpdateMask, typeof Screen.name, typeof Screen.manufacturer, typeof Screen.model, typeof Screen.serialNumber,
            typeof Screen.virtualX, typeof Screen.virtualY)
        console.log(Screen.angleBetween(Qt.PortraitOrientation, Qt.LandscapeOrientation), Screen.angleBetween(Qt.LandscapeOrientation, Qt.InvertedPortraitOrientation),
            Screen.angleBetween(Qt.PrimaryOrientation, Screen.primaryOrientation), Screen.angleBetween(Qt.InvertedLandscapeOrientation, Qt.PortraitOrientation),
            Screen.angleBetween(Qt.PortraitOrientation, Qt.InvertedPortraitOrientation), Screen.angleBetween(Qt.LandscapeOrientation, Qt.LandscapeOrientation))
        console.log(Qt.PrimaryOrientation, Qt.PortraitOrientation, Qt.LandscapeOrientation, Qt.InvertedPortraitOrientation, Qt.InvertedLandscapeOrientation)
        Qt.quit()
    }
}
