import QtQuick
import QtQuick.Controls

// the locale of a control: its own, else the one of the control or the window above, else the default
// (SpinBoxThemed of QmlAppTemplate: IntValidator { locale: control.locale.name })
ApplicationWindow {
    width: 200; height: 200
    Pane {
        id: pane
        locale: Qt.locale("de_DE")
        SpinBox { id: inner; validator: IntValidator { locale: inner.locale.name } }
    }
    SpinBox { id: outer }
    Popup { id: popup; locale: Qt.locale("it_IT"); SpinBox { id: inPopup } }
    Component.onCompleted: {
        console.log(outer.locale.name === Qt.locale().name, inner.locale.name, inner.validator.locale, pane.locale.name)
        pane.locale = Qt.locale("fr_FR")
        console.log(inner.locale.name, inner.validator.locale, inPopup.locale.name)
        popup.locale = Qt.locale("es_ES")
        console.log(inPopup.locale.name)
        Qt.quit()
    }
}
