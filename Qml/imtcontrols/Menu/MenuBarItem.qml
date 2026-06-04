import QtQuick 2.12
import Acf 1.0
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

    /*! Highlighted state (hover, open or keyboard focus on this index). */
    property bool highlighted: (menu && menu.opened)
                                || hover.containsMouse
                                || (menuBar && menuBar.activeFocus
                                    && menuBar.focusedIndex === barIndex)

    implicitHeight: Style.controlHeightM
    implicitWidth: label.implicitWidth + Style.marginXL
    width: implicitWidth
    height: implicitHeight

    Rectangle {
        anchors.fill: parent
        color: item.highlighted
                ? Style.buttonHoverColor
                : "transparent"
        radius: Style.buttonRadius
        border.width: (item.menuBar && item.menuBar.activeFocus
                       && item.menuBar.focusedIndex === item.barIndex
                       && !(item.menu && item.menu.opened))
                      ? Style.buttonBorderWidth : 0
        border.color: Style.buttonBorderFocusColor
    }

    Text {
        id: label
        anchors.centerIn: parent
        text: item.text
        color: Style.textColor
        font.family: Style.fontFamily
        font.pixelSize: Style.fontSizeM
    }

    MouseArea {
        id: hover
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            if (item.menuBar) item.menuBar._toggleAt(item.barIndex)
        }
        onEntered:  {
            if (item.menuBar) item.menuBar._onHover(item.barIndex)
        }
    }
}
