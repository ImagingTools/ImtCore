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

    property int horizCount: 0;

    property int horizontalSpacing: 50;
    property int verticalSpacing: 10;

    property bool hasShadow: true;

    property string shadowColor: "lightgray";

    property alias openST: verticalListViewContainer.openST;

    property bool hasActiveState: false;

    property string openButtonText: "";
    property string openButtonImageSource: "";

    property TreeItemModel buttonModel: TreeItemModel{};
    property TreeItemModel horizontalModel: TreeItemModel{};
    property TreeItemModel verticalModel: TreeItemModel{};

    property TreeItemModel proxiModel: TreeItemModel{};



    property Component buttonDelegate : defaultDelegate;

    Component {
        id: defaultDelegate;

        AuxButton {
            id: button;

            width: buttonPanel.delegateWidth;
            height: buttonPanel.delegateHeight;

            radius: Style.size_ButtonRadius;
            color:  pressed ? Style.color_button_active : containsMouse ? Style.color_button_hovered : Style.color_button;
            fontColor: pressed ? "#ffffff" : Style.color_buttonText;
            fontPixelSize: Style.fontSize_common;
            highlighted: false;
            border.color:  "transparent";

            hasText: true;
            hasIcon: false;

            backgroundColor: Style.alternateBaseColor;

            textButton: model.Name;

            property bool active: model.Active !== undefined ? model.Active : false;
            property string id: model.Id;
            property Item rootItem: buttonPanel;


            onClicked:{
                rootItem.clicked(id);

            }
        }

    }

    signal clicked(string buttonId);

    onClicked: {
        if(buttonPanel.hasActiveState){
            var ind =  buttonPanel.setActive(buttonId);
            //console.log("_____SELECTED_INDEX___ ",ind)

            buttonPanel.setModelsWithActive(ind);

        }

        if(verticalListViewContainer.openST){
            verticalListViewContainer.openST = false;
        }

    }

    Component.onCompleted: {
        if(buttonPanel.buttonModel.GetItemsCount() !== undefined && buttonPanel.buttonModel.GetItemsCount()){
            buttonPanel.setModels();
        }
    }

    onWidthChanged: {
        if(buttonPanel.buttonModel.GetItemsCount() !== undefined && buttonPanel.buttonModel.GetItemsCount()){
            buttonPanel.setModels();
        }
    }

    onButtonModelChanged: {
        buttonPanel.setModels();

    }


    function setModels(){
        buttonPanel.horizontalModel.Clear();
        buttonPanel.verticalModel.Clear();

        buttonPanel.horizCount  = Math.floor((horizontalListViewContainer.width + buttonPanel.horizontalSpacing)  / (buttonPanel.delegateWidth + buttonPanel.horizontalSpacing));
        //console.log("horizCount ",buttonPanel.horizCount);

        var count = buttonPanel.buttonModel.GetItemsCount();
        if(count <= buttonPanel.horizCount){
            //buttonPanel.horizontalModel = buttonPanel.buttonModel;
            for(var i = 0; i < count; i++){
                buttonPanel.horizontalModel.InsertNewItem()
                buttonPanel.horizontalModel.CopyItemDataFromModel(i,buttonPanel.buttonModel,i);
            }
        }
        else{
            for(i = 0; i < buttonPanel.horizCount; i++){
                buttonPanel.horizontalModel.InsertNewItem()
                buttonPanel.horizontalModel.CopyItemDataFromModel(i,buttonPanel.buttonModel,i);
            }

            for(var k = buttonPanel.horizCount; k < count; k++){

                var kk = buttonPanel.verticalModel.InsertNewItem();
                buttonPanel.verticalModel.CopyItemDataFromModel(kk,buttonPanel.buttonModel,k);

            }
        }
    }


    function setActive(buttonId){
        var retval = 0;
        for(var i = 0; i < buttonPanel.buttonModel.GetItemsCount(); i++){
            var id = buttonPanel.buttonModel.GetData("Id",i);
            if(id == buttonId){
                buttonPanel.buttonModel.SetData("Active",true, i);
                retval = i;

            }
            else{
                buttonPanel.buttonModel.SetData("Active",false, i);
            }
        }

        return retval;

    }

    function setModelsWithActive(index){

        //console.log("_____SELECTED_INDEX___ ",index)

        if (index < buttonPanel.horizCount){
            for(var i = 0; i < buttonPanel.horizontalModel.GetItemsCount(); i++){
                buttonPanel.horizontalModel.SetData("Active",(index == i), i);
            }
        }

        else{//перезаполнение моделей

            buttonPanel.proxiModel.Clear();

            var count = buttonPanel.buttonModel.GetItemsCount();

            var countVert = 0;
            for(i = index; i < count; i++){
                buttonPanel.proxiModel.InsertNewItem()
                buttonPanel.proxiModel.CopyItemDataFromModel(countVert,buttonPanel.buttonModel,i);
                countVert++;
            }

            var countHoriz = 0;
            for(i = 0; i < count - countVert ; i++){
                buttonPanel.proxiModel.InsertNewItem()
                buttonPanel.proxiModel.CopyItemDataFromModel(countVert + countHoriz,buttonPanel.buttonModel,i);
                countHoriz++;
            }

            buttonPanel.buttonModel.Clear();
            for(i = 0; i < count  ; i++){
                buttonPanel.buttonModel.InsertNewItem()
                buttonPanel.buttonModel.CopyItemDataFromModel(i,buttonPanel.proxiModel,i);
            }

            buttonPanel.setModels();


        }//перезаполнение моделей

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


        }//horizontalListView
    }//horizontalListViewContainer

    AuxButton{
        id: openButton;

        anchors.bottom: parent.bottom;
        anchors.right: parent.right;
        anchors.rightMargin: buttonPanel.mainMargin;

        width: 36;
        height: 26;
        radius: 4;
        hasText: buttonPanel.openButtonText !== "";
        hasIcon: buttonPanel.openButtonImageSource !== 0;
        textButton: buttonPanel.openButtonText;
        iconSource: buttonPanel.openButtonImageSource;
        fontPixelSize: 30;
        fontColor: containsMouse ? "black" : "gray";
        color: containsMouse || verticalListViewContainer.openST ? Style.color_button : "transparent";
        borderColor: "transparent";
        highlighted: false;

        visible: buttonPanel.buttonModel.GetItemsCount() > buttonPanel.horizCount;

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
            height:  Math.min(contentHeight,(buttonPanel.delegateHeight * buttonPanel.visibleCount + (spacing - 1) * buttonPanel.visibleCount));

            clip: true;
            boundsBehavior: Flickable.StopAtBounds;

            spacing: buttonPanel.verticalSpacing;

            model: buttonPanel.verticalModel;

            delegate: buttonPanel.buttonDelegate;


        }//verticalListView

    }//verticalListViewContainer


    NumberAnimation {
        id: animHeightTo;

        target: verticalListViewContainer;
        property: "height";
        duration: 100;
        from: 0;
        to: verticalListView.height + 2*buttonPanel.mainMargin;
    }

    NumberAnimation {
        id: animHeightFrom;

        target: verticalListViewContainer;
        property: "height";
        duration: 100;
        from: verticalListView.height + 2*buttonPanel.mainMargin;
        to: 0;
    }

}
