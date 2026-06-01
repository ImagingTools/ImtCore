import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/*!
    \qmltype ToolSeparator
    \inqmlmodule imtcontrols

    Separator line for tool bars and menus, API-compatible with Qt Quick
    Controls \c ToolSeparator.

    A vertical (default) or horizontal divider used to visually group
    \l ToolButton instances inside a \l ToolBar.

    \qml
    Row {
        ToolButton { text: "Cut" }
        ToolButton { text: "Copy" }
        ToolSeparator {}
        ToolButton { text: "Paste" }
    }
    \endqml

    \sa ToolBar, MenuSeparator
*/
Control {
    id: separator
    objectName: "ImtControlsToolSeparator"

    /*! Layout direction: \c Qt.Vertical (default) or \c Qt.Horizontal. */
    property int orientation: Qt.Vertical
    readonly property bool vertical: orientation === Qt.Vertical

    padding: Style.spacingXS

    implicitWidth:  vertical ? (Style.buttonBorderWidth + leftPadding + rightPadding)
                             : Style.controlHeightS
    implicitHeight: vertical ? Style.controlHeightS
                             : (Style.buttonBorderWidth + topPadding + bottomPadding)

    contentItem: Rectangle {
        color: Style.borderColor
        implicitWidth:  separator.vertical ? Style.buttonBorderWidth : 1
        implicitHeight: separator.vertical ? 1 : Style.buttonBorderWidth
    }
}
