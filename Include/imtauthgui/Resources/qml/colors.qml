import QtQuick 2.0
//import ACF 1.0
//import "../qml"


Rectangle {
    width: 300
    height: 500

    TreeItemModel {
        id: testModel
        source: "../../Models/__CREATE_COLORS_IN_CTREE__?size=70"
        onStateChanged: {
            if(state === "Ready"){
                mycanvas.paint()
            }
        }
    }
    
    Canvas{
         id: mycanvas
         width: 700
         height: 700
         onPaint: {
             var ctx = getContext("2d");
             var header = testModel.GetData("data")
//             for(var i = 0; i < 5; i++){
             for(var i = 0; i < header.length; i++){
//                 var row = header["data " + i]
                 var row = header[i]["data " + i]
                 for(var j = 0; j < row.length; j++){
//                   for(var j = 0; j < 5; j++){
                     ctx.beginPath()
                     ctx.arc(5 + 10*j, 5 + 10*i, 5, 0, 2*Math.PI, false)
                     ctx.fillStyle = row[j]["Color " + j]
//                     ctx.fillStyle = 'rgba(0, 0, 0, 0.5)'
                     ctx.fill()
                     ctx.lineWidth = 1
                     ctx.strokeStyle = 'rgba(0, 0, 0, 0.5)'
                     ctx.stroke()
                     ctx.closePath()
                 }
             }
         }
         MouseArea {
             anchors.fill: parent
             hoverEnabled: true
             onPositionChanged: {
                 hintRect.x = mouseX
                 hintRect.y = mouseY + 13
                 var i = parseInt(mouseY / 10, 10);
                 var j =  parseInt(mouseX / 10, 10);
                var header = testModel.GetData("data")
                var row = header[i]["data " + i]
                 hintRect.text = row[j]["Color " + j]
//                 hintRect.text = "i " + i + "j " + j
                 hintRect.visible = true
//                 hintTimer.restart()
             }
         }
         Rectangle {
             id: hintRect
             width: 60
             height: 20
             color: "yellow"
             visible: false
             property alias text: hintText.text
             Text {
                 anchors.centerIn: parent
                 id: hintText
             }
         }
         Timer {
             id: hintTimer
                 interval: 500; running: false; repeat: false
                 onTriggered: {
                     hintRect.visible = false
                 }
             }
     }


}


