import QtQuick
import QtQuick.Controls.impl

// the Color singleton of QtQuick.Controls.impl (DrawerThemed of QmlAppTemplate: Color.transparent(palette.shadow, 0.5))
Item {
    Rectangle { id: r; color: Color.transparent("#000000", 0.5) }
    Component.onCompleted: {
        console.log(r.color, Color.transparent("red", 0.12), Color.blend("#000000", "#ffffff", 0.25), Color.blend("red", "blue", 0.5))
        Qt.quit()
    }
}
