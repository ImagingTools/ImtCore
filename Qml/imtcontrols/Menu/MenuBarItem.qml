import QtQuick 2.12
import imtcontrols 1.0

/*!
    \qmltype MenuBarItem
    \inqmlmodule imtcontrols

    One labelled cell in a \l MenuBar. Hovering switches the currently open
    menu when another bar item is already showing its menu, matching
    standard desktop UX.
*/
Item {
    id: item

    /*! Display title. */
    property string text: ""

    /*! Associated Menu (set by MenuBar when adding entries). */
    property var menu: null

    /*! Index in the parent MenuBar's items list. */
    property int barIndex: -1

    /*! Reference back to the MenuBar. */
    property var menuBar: null

    /*! Highlighted state (hover or open). */
    property bool highlighted: (menu && menu.opened) || hover.containsMouse

    implicitHeight: (typeof Style !== "undefined" && Style.controlHeightM) ? Style.controlHeightM : 28
    implicitWidth: label.implicitWidth + 24
    width: implicitWidth
    height: implicitHeight

    Rectangle {
        anchors.fill: parent
        color: item.highlighted
                ? ((typeof Style !== "undefined" && Style.buttonHoverColor) ? Style.buttonHoverColor : "#e5f0ff")
                : "transparent"
        radius: (typeof Style !== "undefined" && Style.buttonRadius) ? Style.buttonRadius : 3
    }

    Text {
        id: label
        anchors.centerIn: parent
        text: item.text
        color: (typeof Style !== "undefined" && Style.textColor) ? Style.textColor : "#222"
        font.family: (typeof Style !== "undefined" && Style.fontFamily) ? Style.fontFamily : ""
        font.pixelSize: (typeof Style !== "undefined" && Style.fontSizeM) ? Style.fontSizeM : 13
    }

    MouseArea {
        id: hover
        anchors.fill: parent
        hoverEnabled: true
        onClicked: if (menuBar) menuBar._toggleAt(barIndex)
        onEntered:  if (menuBar) menuBar._onHover(barIndex)
    }
}
