import QtQuick 2.12
import Qt5Compat.GraphicalEffects 1.0
import QtGraphicalEffects 1.12

import Acf 1.0

Rectangle {
    id: buttonPanel;

    width: 500;
    height: delegateHeight;

    color: "transparent";

    property int delegateWidth: Style.size_ButtonWidth;
    property int delegateHeight: Style.size_ButtonHeight;
    property int verticalMenuWidth: delegateWidth;
    property int visibleCount: 5;
    property int mainMargin: 10;

    property int horizCount: 0;

    property int horizontalSpacing: 50;
    property int verticalSpacing: 10;

    property bool hasShadow: true;

    property bool openST: false;

    property bool hasActiveState: false;
    property string activeId: "";

    property string openButtonText: "";
    property string openButtonImageSource: "";
    property int openButtonWidth: 32;
    property int openButtonHeight: 22;
    property int openDuration: 0;

    property string shadowColor: "lightgrey";
    property string baseColor: "#ffffff";
    property string buttonColor: Style.color_button;

    property TreeItemModel buttonModel: TreeItemModel{};
    property TreeItemModel horizontalModel: TreeItemModel{};
    property TreeItemModel verticalModel: TreeItemModel{};

    property TreeItemModel proxiModel: TreeItemModel{};
    property TreeItemModel rightOrderModel: TreeItemModel{};

    property Item vertMenuItem: null;

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
            buttonPanel.activeId = buttonId;
            var ind =  buttonPanel.setActive(buttonId);
            //console.log("_____SELECTED_INDEX___ ",ind)

            buttonPanel.setModelsWithActive(ind);

        }

        if(buttonPanel.openST){
            buttonPanel.openST = false;
            modalDialogManager.closeDialog();
        }

    }

    Component.onCompleted: {
        if(buttonPanel.buttonModel.GetItemsCount() !== undefined && buttonPanel.buttonModel.GetItemsCount()){
            buttonPanel.setModels();
        }
    }

    onWidthChanged: {
        if(widthPause){
            widthPause.stop();
            widthPause.start();
        }


        if(buttonPanel.vertMenuItem){
            var point = buttonPanel.mapToItem(null, buttonPanel.width - buttonPanel.verticalMenuWidth, buttonPanel.height + buttonPanel.mainMargin);

            buttonPanel.vertMenuItem.x = point.x;
            buttonPanel.vertMenuItem.y = point.y;


        }

    }

    onButtonModelChanged: {
        for(var i = 0; i < buttonPanel.buttonModel.GetItemsCount(); i++){
            var active = buttonPanel.buttonModel.GetData("Active",i);
            if(active){
                buttonPanel.activeId = buttonPanel.buttonModel.GetData("Id",i);
            }
            buttonPanel.rightOrderModel.InsertNewItem()
            buttonPanel.rightOrderModel.CopyItemDataFromModel(i,buttonPanel.buttonModel,i);
        }
        buttonPanel.setModels();
    }


    function getMaxString(){
        var max = "";
        for(var i = 0; i < buttonPanel.buttonModel.GetItemsCount(); i++){
            var str = buttonPanel.buttonModel.GetData("Name",i);
            if(str.length > max.length){
                max = str;
            }

        }
        return max;
    }

    PauseAnimation {
        id: widthPause;

        duration: 200;


        onFinished: {
            if(buttonPanel.buttonModel.GetItemsCount() !== undefined && buttonPanel.buttonModel.GetItemsCount()){
                buttonPanel.setModels();
            }

            if(buttonPanel.hasActiveState && buttonPanel.buttonModel.GetItemsCount() !== undefined && buttonPanel.buttonModel.GetItemsCount()){
                var index = buttonPanel.checkActiveInVertical();
                if(index > -1){
                    buttonPanel.setModelsWithActive(index);
                }

                if(buttonPanel.buttonModel.GetItemsCount() <= buttonPanel.horizCount){
                    buttonPanel.setRightOrder();
                }
            }


        }
    }

    function setModels(){
        buttonPanel.horizontalModel.Clear();
        buttonPanel.verticalModel.Clear();

        buttonPanel.horizCount  = Math.floor(Math.max(horizontalListViewContainer.width + buttonPanel.horizontalSpacing,0)  / (buttonPanel.delegateWidth + buttonPanel.horizontalSpacing));
        console.log("horizCount ",buttonPanel.horizCount);

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
        //console.log("_____HORIZ_COUNT___ ",buttonPanel.horizCount)

        if(buttonPanel.horizCount == 0){
            for(var i = 0; i < buttonPanel.verticalModel.GetItemsCount(); i++){
                buttonPanel.verticalModel.SetData("Active",(index == i), i);
            }

        }

        else{

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
    }//setModelsWithActive

    function checkActiveInVertical(){
        var index = false;
        for(var i = 0; i < buttonPanel.buttonModel.GetItemsCount(); i++){
            var active = buttonPanel.buttonModel.GetData("Active",i);
            if(active){
                index = i;
                break;
            }
        }

        if(index < buttonPanel.horizCount){
            index = -1;
        }

        return index;

    }

    function setRightOrder(){
        buttonPanel.buttonModel.Clear();
        for(var i = 0; i < buttonPanel.rightOrderModel.GetItemsCount(); i++){
            buttonPanel.buttonModel.InsertNewItem()
            buttonPanel.buttonModel.CopyItemDataFromModel(i,buttonPanel.rightOrderModel,i);
        }

        for(var i = 0; i < buttonPanel.buttonModel.GetItemsCount(); i++){
            var id = buttonPanel.buttonModel.GetData("Id",i);
            buttonPanel.buttonModel.SetData("Active",(id == buttonPanel.activeId), i);
        }

        buttonPanel.setModels();

    }

    Rectangle{
        id: horizontalListViewContainer;

        width: parent.width - openButton.width - openButton.anchors.rightMargin - buttonPanel.mainMargin -10;
        height: parent.height;
        color: buttonPanel.baseColor;
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

        width: buttonPanel.openButtonWidth;
        height: buttonPanel.openButtonHeight;
        radius: 4;
        hasText: buttonPanel.openButtonText !== "";
        hasIcon: buttonPanel.openButtonImageSource !== 0;
        textButton: buttonPanel.openButtonText;
        iconSource: buttonPanel.openButtonImageSource;
        fontPixelSize: 30;
        fontColor: containsMouse ? "black" : "gray";
        //color: containsMouse || verticalListViewContainer.openST ? buttonPanel.buttonColor : "transparent";
        borderColor: "transparent";
        highlighted: false;

        visible: buttonPanel.buttonModel.GetItemsCount() > buttonPanel.horizCount;

        onClicked: {
            //verticalListViewContainer.openST = !verticalListViewContainer.openST;

            var point = buttonPanel.mapToItem(null, buttonPanel.width - buttonPanel.verticalMenuWidth, buttonPanel.height + buttonPanel.mainMargin);
            modalDialogManager.openDialog(vertMenuComp, {
                                                       "x":     point.x,
                                                       "y":     point.y,
                                                        });

            buttonPanel.vertMenuItem = modalDialogManager.topItem;
            buttonPanel.openST = true;
        }


    }


    Component{
      id: vertMenuComp;

      Rectangle{
          id: verticalListViewContainer;

          width: buttonPanel.verticalMenuWidth + 2*buttonPanel.mainMargin;
          height: verticalListView.height + 1.5*buttonPanel.mainMargin;
          radius: 4;
          border.width: 1;
          border.color: buttonPanel.shadowColor;
          color: buttonPanel.baseColor;

          clip: true;

          property Item root: null;
          property bool doNotCorrectPosition: true;


          onRootChanged: {
              if(root){
                  root.backgroundItem.opacity = 0;
                  Events.subscribeEvent("DialogBackgroundClicked", verticalListViewContainer.onBackgroundClicked)
              }
          }

          function onBackgroundClicked(){
              if (root){
                  root.closeDialog();
              }
          }



          ListView{
              id: verticalListView;

              anchors.horizontalCenter: parent.horizontalCenter;
              anchors.top: parent.top;
              anchors.topMargin: buttonPanel.mainMargin;

              width: buttonPanel.verticalMenuWidth;
              height:  Math.min(contentHeight,(buttonPanel.delegateHeight * buttonPanel.visibleCount + spacing * (buttonPanel.visibleCount-1)));

              clip: true;
              boundsBehavior: Flickable.StopAtBounds;

              spacing: buttonPanel.verticalSpacing;

              model: buttonPanel.verticalModel;

              delegate: buttonPanel.buttonDelegate;



          }//verticalListView

      }//verticalListViewContainer
    }

}
