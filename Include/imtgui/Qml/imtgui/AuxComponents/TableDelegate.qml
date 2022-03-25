import QtQuick 2.12
import Acf 1.0

Rectangle {
    id: tableDelegateContainer;

    width: 1000;
    height: 35;

    color: "transparent";

    property string text: "Text";
    property string maxSizeText: "";

    property int textTopMargin: 8;
    property int count: 0; // bodyArray.length;

    property bool selected: false;

    property var bodyArray:  [];

    property real delegateWidth: count == 0 ? 0 : width/count;

    signal clicked();
    signal tableDelegateRrightButtonMouseClicked(int mX, int mY);
    signal doubleClicked;

    onBodyArrayChanged: {
        tableDelegateContainer.setContainerSize();
    }

    onXChanged: {
        console.log("TableDelegate onXChanged", tableDelegateContainer.x);
    }

    function clearArray(){
        while(tableDelegateContainer.bodyArray.length > 0)
            tableDelegateContainer.bodyArray.pop();
        tableDelegateContainer.count = 0
    }

    function addToArray(str){
        tableDelegateContainer.bodyArray.push(str);
        tableDelegateContainer.count = tableDelegateContainer.bodyArray.length;
    }

    function setContainerSize(){
        for(var i = 0; i < tableDelegateContainer.bodyArray.length; i++){
            if(tableDelegateContainer.bodyArray[i].length > tableDelegateContainer.maxSizeText.length){
                tableDelegateContainer.maxSizeText = tableDelegateContainer.bodyArray[i];
            }
        }
    }

    Rectangle {
        id: selectionBackGround;

        anchors.fill: parent;

        color: Style.selectedColor;

        radius: 2;
        visible: selected;
    }

    Item {
        id: maxHeightElement;

        height: maxSizeName.height + tableDelegateContainer.textTopMargin*2;
        width: tableDelegateContainer.delegateWidth;

        visible: false;

        Text {
            id: maxSizeName;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: parent.left;
            anchors.leftMargin: 8;

            width: tableDelegateContainer.delegateWidth - 16;

            color: "red";

            wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
            text: tableDelegateContainer.maxSizeText;

            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_subtitle;
            font.bold: false;
        }
    }

    ListView {
        id: dataList;

        anchors.fill: parent;

        clip: true;
        orientation: ListView.Horizontal;
        spacing: 0;
        model: tableDelegateContainer.count;

        delegate: Rectangle {
            id: deleg;

            width: tableDelegateContainer.delegateWidth;
            height: tableDelegateContainer.height;

            color: "transparent";

            Text {
                id: name;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: 8;

                width: tableDelegateContainer.delegateWidth - 16;

                color: Style.textColor;
                text: tableDelegateContainer.bodyArray[model.index];

                elide: Text.ElideRight;

                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }
        }
    }


    MouseArea {
        id: ma;

        anchors.fill: parent;

        acceptedButtons: Qt.LeftButton | Qt.RightButton;

        onClicked: {
            if (mouse.button === Qt.RightButton) {
                console.log("TableDelegate onRightButtonMouseClicked");
                thubnailDecoratorContainer.closeDialog();

                tableDelegateContainer.tableDelegateRrightButtonMouseClicked(this.mouseX, this.mouseY);
            }
            tableDelegateContainer.clicked();
        }

        onDoubleClicked: {
            tableDelegateContainer.doubleClicked();
        }
    }
}
