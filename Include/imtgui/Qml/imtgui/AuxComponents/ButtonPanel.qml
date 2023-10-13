import QtQuick 2.15
import Qt5Compat.GraphicalEffects 1.0
import QtGraphicalEffects 1.12

import Acf 1.0

Rectangle {
    id: buttonPanel;

    width: 500;
    height: delegateHeight;

    color: "transparent";

    property int delegateWidth: 110;
    property int delegateHeight: 30;
    property int verticalMenuWidth: 1;
    property int visibleCount: 5;
    property int mainMargin: 10;

    property int horizCount: 0;

    property int horizontalSpacing: 50;
    property int verticalSpacing: 10;

    property bool hasShadow: true;

    property bool openST: false;

    property bool hasActiveState: false;
    property string activeId: "";
    property int startActiveIndex: 0;

    property bool centered: false;

    property string openButtonText: "";
    property string openButtonImageSource: "";
    property int openButtonWidth: 32;
    property int openButtonHeight: 22;
    property int openDuration: 0;

    property var widthArr: [];
    property var widthArrVer: [];

    property string shadowColor: "lightgrey";
    property string baseColor: "#ffffff";

    property TreeItemModel buttonModel: TreeItemModel{};
    property TreeItemModel horizontalModel: TreeItemModel{};
    property TreeItemModel verticalModel: TreeItemModel{};

    property TreeItemModel proxiModel: TreeItemModel{};
    property TreeItemModel rightOrderModel: TreeItemModel{};

    property Item vertMenuItem: null;

    property Component buttonDelegate : defaultDelegate;
    property Component buttonDelegateVert : defaultDelegateVert;

    property bool menuAlignRight: false;
    property bool canChangeOrder: false;

    property bool compl: false;
    property bool ready: false;

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

    Component{
        id: defaultDelegateVert;

        Item{
            width: model.Name == "" ? splitter.width : textButtonDelegate.width;
            height: model.Name == "" ? splitter.height : textButtonDelegate.height;

            TextButton{
                id: textButtonDelegate;

                legendColor: Style.textColor;
                indicatorColor: "transparent";

                legend: model.Name !== undefined ? model.Name : "";
                active: model.Active !== undefined ? model.Active : false;
                fontFamily: Style.fontFamily;
                fontBold: true;
                fontPixelSize: Style.fontSize_common;
                property Item rootItem: buttonPanel;
                property string id: model.Id !== undefined ? model.Id : "";

                visible: model.Name !== "";

                onClicked: {
                    rootItem.clicked(id);
                }
            }

            Rectangle{
                id: splitter;

                anchors.top: parent.top;

                width: buttonPanel.verticalMenuWidth;
                height: model.Name == "" && model.index == 0 ? -buttonPanel.verticalSpacing : 2;
                color: Style.textColor;
                visible: model.Name !== "" ? false : model.index == 0 ? false : model.index == (buttonPanel.verticalModel.GetItemsCount() - 1) ? false : true ;
            }

        }
    }

    signal clicked(string buttonId);

    onClicked: {
        if(buttonPanel.hasActiveState){
            buttonPanel.activeId = buttonId;
            var ind =  buttonPanel.setActive(buttonId);

            buttonPanel.setModelsWithActive(ind);

        }

        if(buttonPanel.openST){
            buttonPanel.openST = false;
            modalDialogManager.closeDialog();
        }

    }


    Component.onCompleted: {
        buttonPanel.compl = true;
    }
    onOpenSTChanged: {
        if(buttonPanel.openST){
            Events.subscribeEvent("DialogBackgroundClicked", buttonPanel.onBackgroundClicked)
        }
        else {
            Events.unSubscribeEvent("DialogBackgroundClicked", buttonPanel.onBackgroundClicked);
        }
    }

    onButtonModelChanged: {
        //console.log("BUTTON_PANEL_CHANGED ", buttonPanel.buttonModel.toJSON());
        if (!buttonPanel.buttonModel){
            return;
        }

        if(buttonPanel.buttonModel.GetItemsCount()){
            for(let i = 0; i < buttonPanel.buttonModel.GetItemsCount(); i++){
                buttonPanel.buttonModel.SetData("IsHorizontal", true, i);
            }
            buttonPanel.widthArr = [];
            buttonPanel.widthArrVer = [];
            buttonSizeRep.model = 0;
            buttonSizeRepVert.model = 0;
            buttonPanel.ready = false;
            buttonPanel.horizCount = 0;
            buttonSizeRep.model = buttonPanel.buttonModel;
            buttonSizeRepVert.model = buttonPanel.buttonModel;
        }
    }


    onWidthChanged: {
        if(buttonPanel.ready){
            if(widthPause){
                widthPause.restart();
            }
            if(buttonPanel.openST){
                buttonPanel.setVertMenuWidth();
                buttonPanel.menuPositionCorrection()
            }
        }
    }



    onReadyChanged: {
        if(buttonPanel.ready){
            buttonPanel.assignModel();
            setModelPause.restart();
        }
    }

    function updateModel(){

    }

    function clearModel(){
        buttonPanel.widthArr = [];
        buttonPanel.widthArrVer = [];
        buttonPanel.ready = false;
        buttonPanel.horizCount = 0;
        horizontalListView.model = 0;
        buttonSizeRep.model = 0;
        buttonSizeRepVert.model = 0;

//        if (buttonPanel.buttonModel){
//            buttonPanel.buttonModel.Clear();
//        }

        buttonPanel.horizontalModel.Clear();
        buttonPanel.verticalModel.Clear();
        buttonPanel.setOpenButtonVisible();

    }

    function setReady(){
        var count = buttonPanel.buttonModel.GetItemsCount();
        let ok1 = buttonPanel.compl;
        let ok2 = count;
        let ok3 = buttonPanel.widthArr.length === count;
        let ok4 =  buttonPanel.widthArrVer.length === count;
        buttonPanel.ready =  ok1 && ok2 && ok3 && ok4;

        //console.log("BUTTON_PANEL_CHANGED_READY_SET", buttonPanel.ready, ok1, ok2, ok3, ok4);
    }

    function setOpenButtonVisible(){
        let count = 0;
        if(buttonPanel.canChangeOrder){
            if (buttonPanel.buttonModel){
                count = buttonPanel.buttonModel.GetItemsCount();
            }
        }
        else {
            count = horizontalListView.count;
        }

        openButton.visible = count && count > buttonPanel.horizCount;
    }

    function menuPositionCorrection(){

        if(buttonPanel.vertMenuItem && buttonPanel.openST){
            var point = buttonPanel.getMenuPoint();

            buttonPanel.vertMenuItem.x = point.x;
            buttonPanel.vertMenuItem.y = point.y;

        }
    }

    function assignModel(){

        if(!buttonPanel.canChangeOrder){
            horizontalListView.model = buttonPanel.buttonModel;
        }
        else {
            if(buttonPanel.hasActiveState && buttonPanel.canChangeOrder){
                buttonPanel.rightOrderModel.Clear();
                for(var i = 0; i < buttonPanel.buttonModel.GetItemsCount(); i++){
                    var active = buttonPanel.buttonModel.GetData("Active",i);
                    if(active){
                        buttonPanel.activeId = buttonPanel.buttonModel.GetData("Id",i);
                    }
                    buttonPanel.rightOrderModel.InsertNewItem()
                    buttonPanel.rightOrderModel.CopyItemDataFromModel(i,buttonPanel.buttonModel,i);
                }
            }

            horizontalListView.model = buttonPanel.horizontalModel;
        }

    }

    function setVertMenuWidth(){
        if(!buttonPanel.canChangeOrder){
            let visibleCount_ = buttonPanel.buttonModel.GetItemsCount() - buttonPanel.horizCount;
            let maxVal = 0;
            for(let i = buttonPanel.widthArrVer.length - visibleCount_; i < buttonPanel.widthArrVer.length; i++){
                let width_ = buttonPanel.widthArrVer[i];
                if(width_ > maxVal){
                    maxVal = width_;
                }
            }
            buttonPanel.verticalMenuWidth = maxVal;

        }
    }

    function onBackgroundClicked(){
        console.log("ButtonPanel onBackgroundClicked");

        modalDialogManager.closeDialog();
        buttonPanel.openST = false;
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

    function getMenuPoint(){
        let x = openButton.x;
        if(buttonPanel.menuAlignRight){
            x = x - buttonPanel.verticalMenuWidth  - 2*buttonPanel.mainMargin + openButton.width;
        }
        let y = openButton.y + openButton.height + 4;
        return buttonPanel.mapToItem(null, x, y);
    }

    PauseAnimation {
        id: setModelPause;

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

    PauseAnimation {
        id: widthPause;

        duration: 100;


        onFinished: {
            if(buttonPanel.vertMenuItem){
                var point = buttonPanel.getMenuPoint();

                buttonPanel.vertMenuItem.x = point.x;
                buttonPanel.vertMenuItem.y = point.y;

            }
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
        if(buttonPanel.canChangeOrder){
            buttonPanel.horizontalModel.Clear();
            buttonPanel.verticalModel.Clear();
        }

        let width_max = 0;
        let counted = false;

        var countAdded = 0;


        var maxHorWidth = Math.max(horizontalListViewContainer.width ,0)

        for(var i = buttonPanel.startActiveIndex; i < buttonPanel.widthArr.length; i++){

            var width_ = buttonPanel.widthArr[i] + buttonPanel.horizontalSpacing;
            width_max += width_;

            if(width_max > maxHorWidth){
                counted = true;
                countAdded = i - buttonPanel.startActiveIndex;
                break;
            }
            else{
                countAdded++;
            }
        }

        let countAdded_2 = countAdded;

        if(!counted){
            for(let i = 0; i < buttonPanel.startActiveIndex; i++){

                let width_ = buttonPanel.widthArr[i] + buttonPanel.horizontalSpacing;

                width_max += width_;

                if(width_max > maxHorWidth){
                    countAdded_2 = countAdded + i;
                    break;
                }
                else{
                    countAdded_2++;
                }
            }
        }

        if(countAdded_2 < 0){
            countAdded_2 = 0;
        }

        buttonPanel.horizCount = countAdded_2;


        //
        var count = buttonPanel.buttonModel.GetItemsCount();
        if(count <= buttonPanel.horizCount){
            for(let i = 0; i < count; i++){
                if(buttonPanel.canChangeOrder){
                    buttonPanel.horizontalModel.InsertNewItem()
                    buttonPanel.horizontalModel.CopyItemDataFromModel(i,buttonPanel.buttonModel,i);
                }
                else {
                    buttonPanel.buttonModel.SetData("IsHorizontal",true, i)
                }
            }
        }
        else{
            for(i = 0; i < buttonPanel.horizCount; i++){
                if(buttonPanel.canChangeOrder){
                    buttonPanel.horizontalModel.InsertNewItem()
                    buttonPanel.horizontalModel.CopyItemDataFromModel(i,buttonPanel.buttonModel,i);
                }
                else {
                    buttonPanel.buttonModel.SetData("IsHorizontal",true, i)

                }
            }

            for(var k = buttonPanel.horizCount; k < count; k++){
                if(buttonPanel.canChangeOrder){
                    var kk = buttonPanel.verticalModel.InsertNewItem();
                    buttonPanel.verticalModel.CopyItemDataFromModel(kk,buttonPanel.buttonModel,k);
                }
                else {
                    buttonPanel.buttonModel.SetData("IsHorizontal",false, k)
                }

            }
        }

        buttonPanel.setVertMenuWidth();

        if(buttonPanel.openST){
            buttonPanel.menuPositionCorrection();
        }


        if(buttonPanel.canChangeOrder){
            if(buttonPanel.openST && !buttonPanel.verticalModel.GetItemsCount()){
                buttonPanel.openST = false;
                modalDialogManager.closeDialog();
            }

        }
        else{
            if(buttonPanel.openST && buttonPanel.horizCount == buttonPanel.buttonModel.GetItemsCount()){
                buttonPanel.openST = false;
                modalDialogManager.closeDialog();
            }

        }
        buttonPanel.setOpenButtonVisible();

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

        if(buttonPanel.horizCount == 0){
            for(var i = 0; i < buttonPanel.verticalModel.GetItemsCount(); i++){
                buttonPanel.verticalModel.SetData("Active",(index == i), i);
            }

        }

        else{

            if (index < buttonPanel.horizCount){
                for(let i = 0; i < buttonPanel.horizontalModel.GetItemsCount(); i++){
                    buttonPanel.horizontalModel.SetData("Active",(index == i), i);
                }
            }

            else{//перезаполнение моделей

                buttonPanel.startActiveIndex = index;

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

        for(let i = 0; i < buttonPanel.buttonModel.GetItemsCount(); i++){
            var id = buttonPanel.buttonModel.GetData("Id",i);
            buttonPanel.buttonModel.SetData("Active",(id == buttonPanel.activeId), i);
        }

        buttonPanel.setModels();

    }

    Repeater {
        id: buttonSizeRep;

        anchors.right: parent.right;

        width: 0; height: 0;

        visible: false;
        clip: true;

        delegate: buttonPanel.buttonDelegate;

        onItemAdded: {
            let okLength = buttonPanel.widthArr.length == 0 ? true :
                                                              buttonPanel.widthArr.length < buttonPanel.buttonModel.GetItemsCount();

            let width__ = item.width;

            let ok = item && width__ > 0 && okLength;

            if(ok){
                //console.log("width__",width__)
                buttonPanel.widthArr.push(width__);
                buttonPanel.setReady();

            }
            item.width = 0;
            item.height = 0;
            item.visible = false;
        }
    }

    Repeater {
        id: buttonSizeRepVert;

        anchors.right: parent.right;

        width: 0; height: 0;

        visible: false;
        clip: true;

        delegate: buttonPanel.buttonDelegateVert;

        onItemAdded: {
            let okLength = buttonPanel.widthArrVer.length == 0 ? true :
                                                              buttonPanel.widthArrVer.length < buttonPanel.buttonModel.GetItemsCount();

            let width__ = item.width;

            let ok = item && width__ > 0 && okLength;

            if(ok){
                //console.log("width__ver",width__)
                buttonPanel.widthArrVer.push(width__);
                buttonPanel.setReady();
                if(width__ > buttonPanel.verticalMenuWidth){
                    buttonPanel.verticalMenuWidth = width__;
                }
            }
            item.width = 0;
            item.height = 0;
            item.visible = false;
        }
    }

    Rectangle{
        id: horizontalListViewContainer;

        width: parent.width - openButton.width - buttonPanel.mainMargin ;
        height: parent.height;
        clip: true;
        color: "transparent";

        ListView{
            id: horizontalListView;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: parent.left;
            anchors.leftMargin: !buttonPanel.centered ? 0 : parent.width/2 - width/2;

            width: Math.min(contentWidth, parent.width);
            height: parent.height;

            clip: true;
            boundsBehavior: Flickable.StopAtBounds;
            orientation: ListView.Horizontal;

            spacing: buttonPanel.horizontalSpacing;

            delegate: buttonPanel.buttonDelegate;


        }//horizontalListView


    }//horizontalListViewContainer

    AuxButton{
        id: openButton;

        anchors.verticalCenter: parent.verticalCenter;
        anchors.left: horizontalListViewContainer.right;
        anchors.leftMargin: buttonPanel.mainMargin;

        width: buttonPanel.openButtonWidth;
        height: buttonPanel.openButtonHeight;
        radius: 4;
        hasText: buttonPanel.openButtonText !== "";
        hasIcon: buttonPanel.openButtonImageSource !== 0;
        textButton: buttonPanel.openButtonText;
        iconSource: buttonPanel.openButtonImageSource;
        fontPixelSize: 30;
        fontColor: containsMouse ? "black" : "gray";
        pressed: buttonPanel.openST;
        color: buttonPanel.openST ? Style.hover : defaultColor;

        visible: false;//buttonPanel.buttonModel.GetItemsCount() && buttonPanel.buttonModel.GetItemsCount() > buttonPanel.horizCount;

        onClicked: {
            buttonPanel.setVertMenuWidth();
            var point = buttonPanel.getMenuPoint();

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

            width: verticalListView.width + 2*buttonPanel.mainMargin;
            height: verticalListView.height + 2*buttonPanel.mainMargin;
            radius: 4;
            border.width: 1;
            border.color: buttonPanel.shadowColor;
            color: buttonPanel.baseColor;


            clip: true;

            property Item root: null;
            property bool doNotCorrectPosition: true;


            onRootChanged: {
                if(verticalListViewContainer.root){
                    verticalListViewContainer.root.backgroundItem.opacity = 0;
                }
            }

            ListView{
                id: verticalListView;

                anchors.horizontalCenter: parent.horizontalCenter;
                anchors.top: parent.top;
                anchors.topMargin: buttonPanel.mainMargin;

                width: buttonPanel.verticalMenuWidth;
                height: contentHeight;

                clip: true;
                boundsBehavior: Flickable.StopAtBounds;

                spacing: buttonPanel.verticalSpacing;

                model: buttonPanel.canChangeOrder ? buttonPanel.verticalModel :
                                                    buttonPanel.buttonModel;


                delegate: buttonPanel.buttonDelegateVert;



            }//verticalListView

        }//verticalListViewContainer
    }

}
