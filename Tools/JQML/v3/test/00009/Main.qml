import QtQuick

Item {
    Item {
        id: item
        signal sendData(int num, string str)
        Component.onCompleted: {
            sendData(42, "Ответ")
        }
    }
    
    Connections {
        target: item
        function onSendData(num, str){
            console.log("num:", num, "str:", str)
        }
    }
}