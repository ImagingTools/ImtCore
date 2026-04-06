import QtQuick

Item {
    property string message: "Динамический элемент"
    
    Component.onCompleted: {
        console.log("DynamicItem.onCompleted")
    }
}