import QtQuick

Mid {
    decorator: Component { Item { property var baseElement: null } }

    property Component contentComp: null
    property Component topPanelComp: Component { Item { width: 20 } }
    property int radius: 4
}
