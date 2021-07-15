import QtQuick 2.12

Rectangle{
    id: container
    width: 1000
    height: 30// maxHeightElement.height
    color: "transparent"
    property string text: "Text"
    property real delegateWidth: count == 0 ? 0 : width/count
    property int count: array.length
    property bool selected: false
    property string  textColor: "#335777"
    property string fontName: ""
    property int fontSize: 12

    property int textTopMargin: 8

    //property var array: ["First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First First", "Second Second Second", "Third"]
    property var array: ["First", "Second", "Third"]

    property string maxSizeText: ""

    onArrayChanged: setContainerSize()

    function clearArray(){
        while(array.length > 0)
            array.pop()
    }

    function addToArray(str){
        array.push(str)
    }

    function setContainerSize(){
        for(var i = 0; i < array.length; i++){
            if(array[i].length > container.maxSizeText.length){
                container.maxSizeText = array[i]
            }
        }

    }

    signal clicked()

    Rectangle{
        id: selectionBackGround
        anchors.fill: parent
        color: "#4682B4"
        opacity: 0.2
        radius: 2
        visible: selected
    }

    Item{
        id:maxHeightElement
        height: maxSizeName.height + container.textTopMargin*2
        width: container.delegateWidth
        visible: false
        Text {
            id: maxSizeName
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 8
            font.pixelSize: container.fontSize
            font.family: container.fontName
            font.bold: false
            color: "red"//container.textColor
            width: container.delegateWidth - 16
            height: contentHeight
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            text: container.maxSizeText
        }


    }

    Row{
        id:row
        anchors.fill: parent
        Repeater{
            id: rep
            model: container.count
            Item{
                id:element
                height: container.height
                width: container.delegateWidth
                Text {
                    id: name
                    //anchors.verticalCenter: parent.verticalCenter
                    anchors.top: parent.top
                    anchors.topMargin: container.textTopMargin
                    anchors.left: parent.left
                    anchors.leftMargin: 8
                    font.pixelSize: container.fontSize
                    font.family: container.fontName
                    font.bold: false
                    color: container.textColor
                    width: container.delegateWidth - 16
                    height: contentHeight
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    text: container.array[index]
                }


            }
        }
    }



    MouseArea{
        id: ma
        anchors.fill: parent
        enabled: container.visible
        hoverEnabled: enabled
        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
        onClicked: container.clicked()
    }

}
