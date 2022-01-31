import QtQuick 2.12
import Acf 1.0

Rectangle{
    id: container;
    width: 1000;
    height: 35;
    color: "transparent";

    property string text: "Text";
    property real delegateWidth: count == 0 ? 0 : width/count;
    property int count: 0; // bodyArray.length;
    property bool selected: false;
    property int textTopMargin: 8;

    property var bodyArray:  [];

    property string maxSizeText: "";

    signal clicked;
    signal doubleClicked;

    onBodyArrayChanged: {
        container.setContainerSize();
    }

    function clearArray(){
        while(container.bodyArray.length > 0)
            container.bodyArray.pop();
        container.count = 0
    }

    function addToArray(str){
        container.bodyArray.push(str);
        container.count = container.bodyArray.length;
    }

    function setContainerSize(){
        for(var i = 0; i < container.bodyArray.length; i++){
            if(container.bodyArray[i].length > container.maxSizeText.length){
                container.maxSizeText = container.bodyArray[i];
            }
        }
    }

    Rectangle{
        id: selectionBackGround;
        anchors.fill: parent;
        color: "#4682B4";
        opacity: 0.2;
        radius: 2;
        visible: selected;
    }

    Item{
        id:maxHeightElement;
        height: maxSizeName.height + container.textTopMargin*2;
        width: container.delegateWidth;
        visible: false;
        Text {
            id: maxSizeName;
            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: parent.left;
            anchors.leftMargin: 8;
            font.bold: false;
            color: "red";
            width: container.delegateWidth - 16;
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
            text: container.maxSizeText;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_subtitle;
        }
    }

    ListView {
        id: dataList;
        anchors.fill: parent;
        clip: true;
        orientation: ListView.Horizontal;
        spacing: 0;
        model: container.count;
        delegate: Rectangle {
            id: deleg;
            width: container.delegateWidth;
            height: container.height;
            color: "transparent";
            Text {
                id: name;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: 8;
                width: container.delegateWidth - 16;

                color: Style.textColor;
                text: container.bodyArray[model.index];
                wrapMode: Text.WordWrap;

                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }
        }
    }


    MouseArea {
        id: ma;
        anchors.fill: parent;
        onClicked: {
            container.clicked();
        }

        onDoubleClicked: {
            container.doubleClicked();
        }
    }
}
