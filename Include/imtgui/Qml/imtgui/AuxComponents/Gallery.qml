import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Rectangle {
    id: gallery;

    color: "#000000";

    property TreeItemModel model: TreeItemModel{};
    property string nameId: "Source";
    property bool isOpen: visible;
    property int animDuration: 100;


    function open(){
        if(!gallery.visible && model && model.GetItemsCount()){
            gallery.width = 0;
            gallery.visible = true;
            widthAnimTo.start();
        }
    }

    function close(){
        if(gallery.visible){
            widthAnimFrom.start();
        }
    }

    NumberAnimation {
        id: widthAnimTo;

        target: gallery
        property: "width"
        duration: gallery.animDuration;
        from: 0; to: parent.width;
    }

    NumberAnimation {
        id: widthAnimFrom;

        target: gallery
        property: "width"
        duration: gallery.animDuration;
        from: parent.width; to: 0;
        onFinished: {
            gallery.visible = false;
        }
    }

    MouseArea{
        anchors.fill: parent;

        visible: parent.visible;
        enabled: visible;
        hoverEnabled: visible;
    }

    Item{
        id: header;

        width: parent.width;
        height: 50;

        Text {
            id: titleText;

            anchors.centerIn: parent;

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_title;

            text: "Галерея";

        }

        AuxButton{
            id: closeButton;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.right: parent.right;
            anchors.rightMargin: Style.size_smallMargin;

            width: 20;
            height: width;
            radius: width;

            enabled: visible;

            color: "transparent";

            border.color: "transparent";
            border.width: 0;
            hasIcon: true;
            iconSource:  "../../../" +Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);

            iconWidth: width;
            iconHeight: width;

            onClicked: {
                gallery.close();
            }
        }
    }

    Rectangle{
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: list.top;
        anchors.bottomMargin: 5;
        height: 1;
        color: "gray";
        opacity: 0.2;
        visible: false;
    }

    ListView{
        id: list;

        anchors.top: header.bottom;
        anchors.topMargin: 10;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 150;
        anchors.left: parent.left;
        anchors.right: parent.right;

        clip: true;
        boundsBehavior: Flickable.StopAtBounds;
        snapMode: ListView.SnapOneItem;
        orientation: ListView.Horizontal;
        model: gallery.model;
        delegate: Rectangle{
            width: list.width;
            height: list.height;
            color: "transparent";
            Image{
                id: image;

                anchors.fill: parent;
                fillMode: Image.PreserveAspectFit;
                verticalAlignment: Image.AlignTop;
                horizontalAlignment: Image.AlignHCenter;

                source: model[gallery.nameId]
            }
        }
        property int selectedIndex: 0;
        property real movementStartedX: 0;
        onMovementStarted: {
            movementStartedX = contentX;
        }
        onFlickStarted: {
            if(movementStartedX > contentX){
                if(selectedIndex > 0){
                    selectedIndex--;
                }
            }
            else {
                if(selectedIndex < list.count - 1){
                    selectedIndex++;
                }
            }

        }


    }//list

    Rectangle{
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: list.bottom;
        anchors.topMargin: 5;
        height: 1;
        color: "gray";
        opacity: 0.2;
    }

    Item{
        id: listPreviewContainer;
        anchors.top: list.bottom;
        anchors.topMargin: 20;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.leftMargin: 10;
        anchors.rightMargin: 10;

        clip: true;

        ListView{
            id: listPreview;

            anchors.top: parent.top;
            anchors.bottom: parent.bottom;
            anchors.left: parent.left;
            anchors.leftMargin:  listWidth/2 - delegateWidth/2 - addToMargin;
            width: contentWidth;

            clip: true;
            boundsBehavior: Flickable.StopAtBounds;
            snapMode: ListView.SnapOneItem;
            orientation: ListView.Horizontal;

            model: gallery.model;
            delegate: Rectangle{
                width: listPreview.delegateWidth;
                height: listPreview.height;
                radius: 2;
                color: "transparent";
                border.color: listPreview.selectedIndex == model.index ? Style.textColor : "transparent";
                Item{
                    id: imageContainer;

                    anchors.centerIn: parent;

                    width: parent.width - 8;
                    height:  parent.height - 8;

                    Image{
                        id: imagePreview;

                        anchors.fill: parent;
                        fillMode: Image.PreserveAspectFit;
                        verticalAlignment: Image.AlignTop;
                        horizontalAlignment: Image.AlignHCenter;

                        source: model[gallery.nameId]
                    }
                }
                MouseArea{
                    anchors.fill: parent;

                    property real startX: 0;
                    property point startPoint;
                    property real lastDeltaX;
                    onPressed: {
                        startPoint = mapToItem(gallery, mouse.x, 0);
                        startX = mouse.x;
                    }
                    onReleased: {
                        let position = mapToItem(gallery, mouse.x, 0)
                        if(position.x.toFixed(1) == startPoint.x.toFixed(1)){
                            list.selectedIndex = model.index;
                            list.positionViewAtIndex(model.index, ListView.Center);
                        }

                        else if(lastDeltaX > 0){//after moving
                            if(listPreview.addToMargin > listPreview.selectedIndex * listPreview.delegateWidth){
                                list.selectedIndex++;
                                list.positionViewAtIndex(list.selectedIndex, ListView.Center);
                            }
                        }
                        else {
                            if(listPreview.addToMargin < listPreview.selectedIndex * listPreview.delegateWidth){
                                list.selectedIndex--;
                                list.positionViewAtIndex(list.selectedIndex, ListView.Center);
                            }
                        }
                    }
                    onPositionChanged: {
                        let delta = startX - mouse.x;
                        lastDeltaX = delta;
                        //console.log(delta); //влево +, вправо -
                        if(delta > 0){
                            let ok  = (listPreview.addToMargin + delta) < (listPreview.selectedIndex + 1) * listPreview.delegateWidth && (listPreview.selectedIndex + 1) < listPreview.count;
                            if(ok){
                                listPreview.addToMargin += delta;
                            }
                        }
                        else {
                            let ok  = (listPreview.addToMargin + delta) > (listPreview.selectedIndex - 1) * listPreview.delegateWidth && (listPreview.selectedIndex - 1) >= 0;
                            if(ok){
                                listPreview.addToMargin += delta;
                            }

                        }
                    }
                }
            }
            property real listWidth: parent.width;
            property real delegateWidth: 80;
            property real addToMargin: 0;//  selectedIndex * delegateWidth;

            property int selectedIndex: list.selectedIndex;
            onSelectedIndexChanged: {
                addToMagrinAnim.from = addToMargin;
                addToMagrinAnim.to =   selectedIndex * delegateWidth;
                addToMagrinAnim.start();
            }

        }//listPreview
    }//listPreviewContainer;


    NumberAnimation {
        id: addToMagrinAnim;

        target: listPreview;
        property: "addToMargin";
        duration: 200;
    }

//    Rectangle{//for testing
//        id: centerLine;

//        anchors.top: parent.top;
//        anchors.bottom: parent.bottom;
//        anchors.horizontalCenter: parent.horizontalCenter;
//        width: 1;
//        color: "gray";
//    }

}
