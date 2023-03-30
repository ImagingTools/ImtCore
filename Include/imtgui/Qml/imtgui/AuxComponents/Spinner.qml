import QtQuick 2.12
import Acf 1.0

Rectangle {
    id: root;

    width: 50;
    height: 60;

    property int itemHeight: 20;

    property int currentValue: -1;

    property alias model: listView.model;

    function setValue(value){
        console.log("setValue", value)
        console.log("currentValue", root.currentValue)
        if (root.currentValue !== value){
            console.log("listView.count", listView.count)
            for (let i = 0; i < listView.count; i++){
                if (value === listView.model.get(i).value){

                    console.log("gotoIndex", listView.count)

                    root.gotoIndex(i);

                    break;
                }
            }
        }
    }

    MouseArea {
        anchors.fill: listView;

        onWheel: {
            if (wheel.angleDelta.y > 0){
                root.prev();
            }
            else{
                root.next();
            }
        }
    }

    ListModel {
        id: defaultModel;
        Component.onCompleted: {
            defaultModel.append({ value: -1, text: " " })
            for(var i = 0; i <= 200; i++){
                var norm = i.toString();
                if( i < 10 ) norm = "0" + i
                defaultModel.append({ value: i, text: norm })
            }
            defaultModel.append({ value: -1, text: " " })
        }
    }

    ListView {
        id: listView;

        width: root.width;
        height: parent.height;

        clip: true;

        model: defaultModel;

        onCurrentIndexChanged: {
            if (listView.currentIndex >= 0){
                root.currentValue = listView.model.get(listView.currentIndex).value;
            }
        }

        delegate: Rectangle {
            height: root.itemHeight;
            width: listView.width;

            color: Style.baseColor;

            Text {
                anchors.centerIn: parent;
                text: model.text;

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            MouseArea {
                z: 100;

                anchors.fill: parent

                onClicked: {
                    root.gotoIndex(model.index)
                }
            }
        }

        onMovementEnded: {
            var centralIndex = listView.indexAt(listView.contentX+1,listView.contentY+itemHeight+itemHeight/2)
            root.gotoIndex(centralIndex)
        }
    }


    Rectangle {
        id: selector;

        anchors.centerIn: root;

        width: listView.width;
        height: root.itemHeight;

        border.width: 2;
        border.color: Style.textSelected;

        color: "transparent";
    }

    function gotoIndex(inIndex){
        var begPos = listView.contentY;
        var destPos;
        listView.positionViewAtIndex(inIndex, ListView.Center);
        destPos = listView.contentY;
        listView.currentIndex = inIndex
    }

    function next(){
        if (listView.currentIndex < listView.count - 1){
            root.gotoIndex(listView.currentIndex + 1)
        }
    }

    function prev(){
        if (listView.currentIndex > 0){
            root.gotoIndex(listView.currentIndex - 1)
        }
    }
}
