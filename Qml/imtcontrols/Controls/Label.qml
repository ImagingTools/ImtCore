import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/*!
    \qmltype Label
    \inqmlmodule imtcontrols

    Styled text label, API-compatible with Qt Quick Controls \c Label.

    Label extends \c Text with the application's default font family, pixel
    size and text colour, so labels look consistent without repeating style
    bindings. All standard \c Text properties (\c wrapMode, \c elide,
    \c horizontalAlignment, ...) remain available.

    \qml
    Label {
        text: qsTr("User name")
        font.bold: true
    }
    \endqml

    \sa Title
*/
Text {
    id: label
    objectName: "ImtControlsLabel"

    color: enabled ? Style.textColor : Style.inactiveTextColor
    font.family: Style.fontFamily
    font.pixelSize: Style.fontSizeM
    verticalAlignment: Text.AlignVCenter
    textFormat: Text.PlainText
}
