import QtQuick 2.12
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

    property var menuItem: model && model.menuItem ? model.menuItem : null
    property var menu: null
    property int rowIndex: model ? model.index : -1
    property bool current: menu && menu.currentIndex === rowIndex
    property bool isSeparator: menuItem && menuItem.isMenuSeparator === true

    width: ListView.view ? ListView.view.width : implicitWidth
    implicitWidth: 160
    height: isSeparator
            ? (menuItem ? menuItem.height : 8)
            : (menuItem ? menuItem.itemHeight : 28)

    color: !menuItem || !menuItem.enabled
            ? "transparent"
            : (current || (menuItem.highlighted === true)
               ? ((typeof Style !== "undefined" && Style.buttonHoverColor) ? Style.buttonHoverColor : "#e5f0ff")
               : "transparent")

    // Separator just renders the MenuSeparator item full-row.
    Loader {
        anchors.fill: parent
        active: row.isSeparator
        sourceComponent: row.isSeparator && row.menuItem ? null : null
        // Render directly: the MenuSeparator owns its own rectangle.
        // We re-render a simple line so we don't reparent menuItem itself
        // (it remains a child of Menu's contentData).
        Rectangle {
            anchors.centerIn: parent
            width: row.width - 8
            height: 1
            color: (typeof Style !== "undefined" && Style.borderColor) ? Style.borderColor : "#cccccc"
            visible: row.isSeparator
        }
    }

    Item {
        anchors.fill: parent
        anchors.leftMargin:  (typeof Style !== "undefined" && Style.marginM) ? Style.marginM : 8
        anchors.rightMargin: (typeof Style !== "undefined" && Style.marginM) ? Style.marginM : 8
        visible: !row.isSeparator

        Image {
            id: icon
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            source: row.menuItem ? row.menuItem.iconSource : ""
            width:  source != "" ? ((typeof Style !== "undefined" && Style.iconSizeS) ? Style.iconSizeS : 16) : 0
            height: width
            fillMode: Image.PreserveAspectFit
            visible: source != ""
        }

        // Check mark for checkable items.
        Rectangle {
            id: check
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: icon.right
            anchors.leftMargin: visible ? 6 : 0
            width: visible ? 12 : 0
            height: width
            radius: 2
            color: row.menuItem && row.menuItem.checked
                   ? ((typeof Style !== "undefined" && Style.buttonPressedColor) ? Style.buttonPressedColor : "#3a83f0")
                   : "transparent"
            border.width: 1
            border.color: (typeof Style !== "undefined" && Style.borderColor) ? Style.borderColor : "#888"
            visible: row.menuItem && row.menuItem.checkable
        }

        Text {
            id: label
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: check.visible ? check.right : (icon.visible ? icon.right : parent.left)
            anchors.leftMargin: (check.visible || icon.visible) ? 8 : 0
            anchors.right: rightCluster.left
            anchors.rightMargin: 8
            text: row.menuItem ? _strip(row.menuItem.text) : ""
            elide: Text.ElideRight
            color: row.menuItem && row.menuItem.enabled
                   ? ((typeof Style !== "undefined" && Style.textColor) ? Style.textColor : "#222")
                   : ((typeof Style !== "undefined" && Style.inactiveTextColor) ? Style.inactiveTextColor : "#999")
            font.family: (typeof Style !== "undefined" && Style.fontFamily) ? Style.fontFamily : ""
            font.pixelSize: (typeof Style !== "undefined" && Style.fontSizeM) ? Style.fontSizeM : 13

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
            spacing: 6
            Text {
                visible: row.menuItem && row.menuItem.shortcut !== ""
                text: row.menuItem ? row.menuItem.shortcut : ""
                color: (typeof Style !== "undefined" && Style.inactiveTextColor) ? Style.inactiveTextColor : "#888"
                font.pixelSize: (typeof Style !== "undefined" && Style.fontSizeSmall) ? Style.fontSizeSmall : 11
            }
            Text {
                visible: row.menuItem && row.menuItem.hasSubmenu === true
                text: "\u25B6"   // black right-pointing triangle
                color: (typeof Style !== "undefined" && Style.textColor) ? Style.textColor : "#222"
                font.pixelSize: 10
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        enabled: row.menuItem && row.menuItem.enabled && !row.isSeparator
        onEntered: if (row.menu) row.menu._onItemHover(row.rowIndex)
        onClicked: if (row.menu) row.menu._onItemClick(row.rowIndex)
    }
}
