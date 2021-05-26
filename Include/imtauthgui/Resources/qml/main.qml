import QtQuick 2.0
import QtWebSockets 1.15
//import ACF 1.0

Item {
    width: 300
    height: 500

    Timer {
        id: timer
        interval: 1000
        running: false
        repeat: true
        onTriggered: {
            //console.log("onTimer")
            if (webSocket.status === 3){
                var url = webSocket.url
                webSocket.reload = true
                webSocket.active = false
                webSocket.active = true
            }
        }
    }

    WebSocket {
        id: webSocket
        active: true
        url: "ws://127.0.0.1:8888/Lisa/ws"
        property bool reload: false
        onTextMessageReceived: {
            console.log("onTextMessageReceived", message)
            var objectArray = JSON.parse(message)
            var dataObj = objectArray["data"][0]
            console.log("dataObj", dataObj)
            contactEditor.updateShortData(dataObj)
        }
        onBinaryMessageReceived: {
            console.log("onBinaryMessageReceived", message)
        }

        onStatusChanged: {
            console.log("WebSocket onStatusChanged",webSocket.status)
            if (webSocket.status === 3){
                timer.running = true
            }

            if (webSocket.status == 1 && reload == true){
                reload = false
                timer.running = false
                var modelSource = contactInfoTreeItemModel.source
                contactInfoTreeItemModel.source = ""
                contactInfoTreeItemModel.source = modelSource
            }
        }
    }
    
    TreeItemModel {
        id:contactInfoTreeItemModel
        source: "../../Models/__ContactInfo__"
        target: "../../Models/__ContactInfo__"
    }

    ContactInfoEditor {
        id: contactEditor
        width: parent.width
        height: 300
        contactInfoModel: contactInfoTreeItemModel
    }

}


