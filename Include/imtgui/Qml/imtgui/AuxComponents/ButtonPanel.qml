import QtQuick 2.12
import QtGraphicalEffects 1.12

import Acf 1.0

Rectangle {
    id: buttonPanel;

    width: 500;
    height: delegateHeight;

    color: "transparent";

    property int delegateWidth: 120;
    property int delegateHeight: 40;
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


    onButtonModelChanged: {
        buttonPanel.horizontalModel.Clear();
        buttonPanel.verticalModel.Clear();

        var horizCount  = Math.floor((horizontalListView.width + buttonPanel.horizontalSpacing)  / (buttonPanel.delegateWidth + buttonPanel.horizontalSpacing));
        var count = buttonModel.GetItemsCount();
        if(count <= horizCount){
            buttonPanel.horizontalModel = buttonPanel.buttonModel;
        }
        else{
            for(var i = 0; i < horizCount; i++){

            }

            for(var k = horizCount; k < count; k++){

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

            model: 10;

            delegate: Rectangle{

                width: buttonPanel.delegateWidth;
                height: buttonPanel.delegateHeight;

                color: "red";
            }

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

            model: 10;

            delegate: Rectangle{
                anchors.horizontalCenter:  verticalListView.horizontalCenter;

                width: buttonPanel.delegateWidth;
                height: buttonPanel.delegateHeight;

                color: "green";

            }

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
