import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/*!
    \qmltype Pane
    \inqmlmodule imtcontrols

    Plain surface container, API-compatible with Qt Quick Controls \c Pane.

    Pane provides a background matching the application surface colour and
    default padding around its content. Use it to group controls onto a
    single, visually consistent surface.

    \qml
    Pane {
        padding: 12
        Column {
            spacing: 8
            Label { text: "Settings" }
            Switch { text: "Enabled" }
        }
    }
    \endqml

    \sa Control, Frame, GroupBox
*/
Control {
    id: pane
    objectName: "ImtControlsPane"

    padding: Style.spacingM

    background: Rectangle {
        color: Style.backgroundColor2
        border.width: 0
    }
}
