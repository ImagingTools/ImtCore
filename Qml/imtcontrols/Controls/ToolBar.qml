import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/*!
    \qmltype ToolBar
    \inqmlmodule imtcontrols

    Container for action-oriented controls, API-compatible with Qt Quick
    Controls \c ToolBar.

    ToolBar is a \l Pane-like surface intended to host \l ToolButton,
    \l ToolSeparator and similar controls, usually laid out in a \c Row.
    It can be placed at the top or bottom of a page.

    \qml
    ToolBar {
        Row {
            anchors.fill: parent
            spacing: 4
            ToolButton { text: "Open" }
            ToolButton { text: "Save" }
            ToolSeparator {}
            ToolButton { text: "Cut" }
        }
    }
    \endqml

    \sa ToolButton, ToolSeparator
*/
Control {
    id: toolBar
    objectName: "ImtControlsToolBar"

    /*! Where the bar is docked: \c ToolBar.Header (top) or
        \c ToolBar.Footer (bottom). Affects only the separating border edge. */
    property int position: ToolBar.Header
    enum Position { Header, Footer }

    leftPadding:   Style.spacingS
    rightPadding:  Style.spacingS
    topPadding:    Style.spacingXS
    bottomPadding: Style.spacingXS

    implicitHeight: Math.max(Style.controlHeightL,
                             implicitContentHeight + topPadding + bottomPadding)

    background: Rectangle {
        color: Style.backgroundColor
        // Single divider line on the side facing the page content.
        Rectangle {
            width: parent.width
            height: Style.buttonBorderWidth
            color: Style.borderColor
            anchors.bottom: toolBar.position === ToolBar.Header ? parent.bottom : undefined
            anchors.top:    toolBar.position === ToolBar.Footer ? parent.top : undefined
        }
    }
}
