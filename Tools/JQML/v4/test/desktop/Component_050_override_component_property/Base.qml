import QtQuick

Item {
    property string title
    property Component contentComp: null;
    property Component lazyComp: Component { Item { width: 10 } }
    property Component topPanelComp: Component { Item { width: 20 } }
    property string backgroundColor: "black"
}


