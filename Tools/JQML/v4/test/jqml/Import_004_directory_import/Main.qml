import QtQuick
import "plain"
import "listed" as L

Item {
    Plain { id: plain }
    L.Listed { id: listed }

    Component.onCompleted: {
        console.log("plain", plain.name)
        console.log("listed", listed.name)
        console.log("palette", L.Palette.name, L.Palette.value)
        Qt.quit()
    }
}
