import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/*!
    \qmltype Frame
    \inqmlmodule imtcontrols

    Bordered surface container, API-compatible with Qt Quick Controls
    \c Frame.

    Frame is a \l Pane with a visible 1px border and rounded corners. It is
    used to visually delimit a group of controls.

    \qml
    Frame {
        Column {
            spacing: 8
            CheckBox { text: "Option A" }
            CheckBox { text: "Option B" }
        }
    }
    \endqml

    \sa Pane, GroupBox
*/
Control {
    id: frame
    objectName: "ImtControlsFrame"

    padding: Style.spacingM

    background: Rectangle {
        color: "transparent"
        radius: Style.radiusM
        border.width: Style.buttonBorderWidth
        border.color: Style.borderColor
    }
}
