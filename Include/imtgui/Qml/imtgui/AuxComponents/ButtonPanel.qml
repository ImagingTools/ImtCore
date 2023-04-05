import QtQuick 2.12
import QtGraphicalEffects 1.12

import Acf 1.0

Rectangle {
    id: buttonPanel;

    width: 500;
    height: delegateHeight;

    color: "transparent";

    property int delegateWidth: Style.size_ButtonWidth;
    property int delegateHeight: Style.size_ButtonHeight;
    property int visibleCount: 5;
    property int mainMargin: 10;

    property int horizontalSpacing: 50;
    property int verticalSpacing: 10;

    property bool hasShadow: true;

    property string shadowColor: "lightgray";

    property alias openST: verticalListViewContainer.openST;

    property TreeItemModel buttonModel: TreeItemModel{};
    property TreeItemModel horizontalModel: TreeItemModel{};
    property TreeItemModel verticalModel: TreeItemModel{};


    property Component buttonDelegate : defaultDelegate;

    Component {
        id: defaultDelegate;

        AuxButton {
            id: button;

            width: buttonPanel.delegateWidth;
            height: buttonPanel.delegateHeight;

            radius: Style.size_ButtonRadius;
            color: pressed ? Style.color_button_active : containsMouse ? Style.color_button_hovered : Style.color_button;
            fontColor: pressed ? "#ffffff" : Style.color_buttonText;
            fontPixelSize: Style.fontSize_common;
            highlighted: false;
            border.color:  "transparent";

            hasText: true;
            hasIcon: false;

            backgroundColor: Style.alternateBaseColor;

            textButton: model.Name;

            property string id: model.Id;
            property Item rootItem: buttonPanel;


            onClicked:{
                rootItem.clicked(id);
            }
        }

    }

    signal clicked(string buttonId);

    Component.onCompleted: {
        //setModels();
    }

    onButtonModelChanged: {
        setModels();

    }

    function setModels(){
        buttonPanel.horizontalModel.Clear();
        buttonPanel.verticalModel.Clear();

        var horizCount  = Math.floor((horizontalListViewContainer.width + buttonPanel.horizontalSpacing)  / (buttonPanel.delegateWidth + buttonPanel.horizontalSpacing));
        console.log("horizCount ",horizCount);

        var count = buttonPanel.buttonModel.GetItemsCount();
        if(count <= horizCount){
            buttonPanel.horizontalModel = buttonPanel.buttonModel;
        }
        else{
            for(var i = 0; i < horizCount; i++){
                buttonPanel.horizontalModel.InsertNewItem()
                buttonPanel.horizontalModel.CopyItemDataFromModel(i,buttonPanel.buttonModel,i);
            }

            for(var k = horizCount; k < count; k++){
                var kk = 0;
                buttonPanel.verticalModel.InsertNewItem();
                buttonPanel.verticalModel.CopyItemDataFromModel(k,buttonPanel.buttonModel,kk);
                kk++;
            }
        }
    }


    Rectangle{
        id: horizontalListViewContainer;

        width: parent.width - openButton.width - openButton.anchors.rightMargin - buttonPanel.mainMargin -10;
        height: parent.height;
        clip: true;

        ListView{
            id: horizontalListView;

            anchors.fill: parent;

            clip: true;
            boundsBehavior: Flickable.StopAtBounds;
            orientation: ListView.Horizontal;

            spacing: buttonPanel.horizontalSpacing;

            model: buttonPanel.horizontalModel;

            delegate: buttonPanel.buttonDelegate;

//            delegate: Rectangle{

//                width: buttonPanel.delegateWidth;
//                height: buttonPanel.delegateHeight;

//                color: "red";
//            }

        }//horizontalListView
    }//horizontalListViewContainer

    AuxButton{
        id: openButton;

//        anchors.verticalCenter: parent.verticalCenter;
        anchors.bottom: parent.bottom;
        anchors.right: parent.right;
        anchors.rightMargin: buttonPanel.mainMargin;

        width: 40;
        height: 30;
        radius: 4;
        hasText: true;
        textButton: ">>";
        fontPixelSize: 30;
        fontColor: containsMouse ? "black" : "gray";
        color: containsMouse || verticalListViewContainer.openST ? "lightgray" : "transparent";
        borderColor: "transparent";
        highlighted: false;

        onClicked: {
            verticalListViewContainer.openST = !verticalListViewContainer.openST;
        }


    }

    DropShadow {
        id: shadow;

        anchors.fill: verticalListViewContainer;

        horizontalOffset: 2;
        verticalOffset: 2;

        visible: buttonPanel.hasShadow && verticalListViewContainer.visible;

        radius: verticalListViewContainer.radius;
        color: buttonPanel.shadowColor;

        source: verticalListViewContainer;
    }

    Rectangle{
        id: verticalListViewContainer;

        anchors.right: parent.right;
        anchors.top: parent.bottom;
        anchors.topMargin: buttonPanel.mainMargin;

        width: buttonPanel.delegateWidth + 2*buttonPanel.mainMargin;
        height: 0;
        radius: 4;
        border.width: 1;
        border.color: "lightgray";

        clip: true;


        property bool openST: false;
        onOpenSTChanged: {
            if(openST){
                animHeightTo.start();
            }
            else{
                animHeightFrom.start();
            }
        }

        ListView{
            id: verticalListView;

            anchors.centerIn: parent;

            width: buttonPanel.delegateWidth;
            height:  buttonPanel.delegateHeight * buttonPanel.visibleCount + (spacing - 1) * buttonPanel.visibleCount;

            clip: true;
            boundsBehavior: Flickable.StopAtBounds;

            spacing: buttonPanel.verticalSpacing;

            model: buttonPanel.verticalModel;

            delegate: buttonPanel.buttonDelegate;

//            delegate: Rectangle{
//                anchors.horizontalCenter:  verticalListView.horizontalCenter;

//                width: buttonPanel.delegateWidth;
//                height: buttonPanel.delegateHeight;

//                color: "green";

//            }

        }//verticalListView

    }//verticalListViewContainer


    NumberAnimation {
        id: animHeightTo;

        target: verticalListViewContainer;
        property: "height";
        duration: 100;
        from:0;
        to:verticalListView.height + 2*buttonPanel.mainMargin;
    }

    NumberAnimation {
        id: animHeightFrom;

        target: verticalListViewContainer;
        property: "height";
        duration: 100;
        from:verticalListView.height + 2*buttonPanel.mainMargin;
        to:0;
    }

}
