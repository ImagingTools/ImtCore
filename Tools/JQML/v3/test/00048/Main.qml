import QtQuick

Item {
   width: 500
   height: 500

   Text {
        anchors.left: parent.left
        anchors.right: parent.right
        text: "Hello"
        onWidthChanged: console.log('Text.onWidthChanged', width)
   }
}