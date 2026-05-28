import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/*!
    \qmltype MenuItemDelegate
    \inqmlmodule imtcontrols
    \internal

    Default visual delegate for items rendered by \l Menu's internal
    ListView. Reads the row's MenuItem from \c model.menuItem and renders
    icon, text, optional check mark and submenu arrow.
*/
Rectangle {
    id: row

    property var menuItem: null
    property var menu: null
    property int rowIndex: -1
    property bool current: menu && menu.currentIndex === rowIndex
    property bool hovered: mouseArea.containsMouse
    property bool isSeparator: menuItem && menuItem.isMenuSeparator === true

    width: parent ? parent.width : implicitWidth
    implicitWidth: Style.menuMinWidth
    height: isSeparator
            ? (menuItem ? menuItem.height : Style.marginS)
            : (menuItem ? menuItem.itemHeight : Style.controlHeightM)

    color: !menuItem || !menuItem.enabled
            ? "transparent"
            : (current || hovered || (menuItem.highlighted === true)
               ? Style.buttonHoverColor
               : "transparent")

    // Separator just renders a thin horizontal line; the actual MenuSeparator
    // object is kept in Menu's contentData and is not reparented.
    Rectangle {
        anchors.centerIn: parent
        width: row.width - Style.marginS
        height: Style.buttonBorderWidth
        color: Style.borderColor
        visible: row.isSeparator
    }

    Item {
        anchors.fill: parent
        anchors.leftMargin:  Style.marginM
        anchors.rightMargin: Style.marginM
        visible: !row.isSeparator

        Image {
            id: icon
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            source: row.menuItem ? row.menuItem.iconSource : ""
            width:  source != "" ? Style.iconSizeS : 0
            height: width
            fillMode: Image.PreserveAspectFit
            visible: source != ""
            sourceSize.width: width
            sourceSize.height: height
        }

        // Check mark for checkable items.
        Image {
            id: check
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: icon.right
            anchors.leftMargin: visible ? Style.marginXS : 0
            width: visible ? Style.iconSizeS : 0
            height: width
            source: row.menuItem && row.menuItem.checked
                    ? "qrc:/" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal)
                    : ""
            visible: row.menuItem && row.menuItem.checkable && row.menuItem.checked
            sourceSize.width: width
            sourceSize.height: height
        }

        Text {
            id: label
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: check.visible ? check.right : (icon.visible ? icon.right : parent.left)
            anchors.leftMargin: (check.visible || icon.visible) ? Style.marginS : 0
            anchors.right: rightCluster.left
            anchors.rightMargin: Style.marginS
            text: row.menuItem ? _strip(row.menuItem.text) : ""
            elide: Text.ElideRight
            color: row.menuItem && row.menuItem.enabled
                   ? Style.textColor
                   : Style.inactiveTextColor
            font.family: Style.fontFamily
            font.pixelSize: Style.fontSizeM

            // Strip mnemonic markers ('&X' -> 'X'); a future iteration may
            // render the next char with an underline when Alt is held.
            function _strip(s) {
                if (!s) return "";
                return String(s).replace(/&&/g, "\u0001")
                                .replace(/&(.)/g, "$1")
                                .replace(/\u0001/g, "&");
            }
        }

        // Right-aligned cluster: shortcut text + submenu arrow.
        Row {
            id: rightCluster
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            spacing: Style.marginXS
            Text {
                visible: row.menuItem && row.menuItem.shortcut !== ""
                text: row.menuItem ? row.menuItem.shortcut : ""
                color: Style.inactiveTextColor
                font.pixelSize: Style.fontSizeS
            }
            Text {
                visible: row.menuItem && row.menuItem.hasSubmenu === true
                text: "\u25B6"   // black right-pointing triangle
                color: Style.textColor
                font.pixelSize: Style.fontSizeXS
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        enabled: row.menuItem && row.menuItem.enabled && !row.isSeparator
        onContainsMouseChanged: {
            if (containsMouse && row.menu)
                row.menu._onItemHover(row.rowIndex)
        }
        onClicked: {
            if (row.menu) row.menu._onItemClick(row.rowIndex)
        }
    }
}
