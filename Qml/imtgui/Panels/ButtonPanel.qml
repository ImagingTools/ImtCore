import QtQuick 2.15
import Qt5Compat.GraphicalEffects 6.0
import QtGraphicalEffects 1.12

import Acf 1.0
import imtcontrols 1.0

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

    property string shadowColor: Style.shadowColor;
    property string baseColor: Style.baseColor;

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

    property real horizontalWidth : horizontalListView.contentWidth;

    Component {
        id: defaultDelegate;

        Button {
            id: button;

            width: buttonPanel.delegateWidth;
            height: buttonPanel.delegateHeight;

            text: model.Name;

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
                fontPixelSize: Style.fontSizeNormal;

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
                visible: model.Name !== "" ? false : model.index == 0 ? false : model.index == (buttonPanel.verticalModel.getItemsCount() - 1) ? false : true ;
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
            ModalDialogManager.closeByComp(vertMenuComp);
        }
    }

    Component.onCompleted: {
        buttonPanel.compl = true;
    }


    onButtonModelChanged: {
        if (!buttonPanel.buttonModel){
            return;
        }

        if(buttonPanel.buttonModel.getItemsCount()){
            for(let i = 0; i < buttonPanel.buttonModel.getItemsCount(); i++){
                buttonPanel.buttonModel.setData("IsHorizontal", true, i);
            }
            buttonPanel.widthArr = [];
            buttonPanel.widthArrVer = [];
            buttonSizeRep.model = 0;
            buttonSizeRepVert.model = 0;
            buttonPanel.ready = false;
            buttonPanel.horizCount = 0;
            buttonSizeRep.model = buttonPanel.buttonModel;
            buttonSizeRepVert.model = buttonPanel.buttonModel;

            setReady();
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
        buttonPanel.assignModel();
        setModelPause.restart();
    }

    function clearModel(){
        buttonPanel.widthArr = [];
        buttonPanel.widthArrVer = [];
        buttonPanel.ready = false;
        buttonPanel.horizCount = 0;
        horizontalListView.model = 0;
        buttonSizeRep.model = 0;
        buttonSizeRepVert.model = 0;
        buttonPanel.horizontalModel.clear();
        buttonPanel.verticalModel.clear();
        buttonPanel.setOpenButtonVisible();
    }

    function setReady(){
        var count = buttonPanel.buttonModel.getItemsCount();
        let ok1 = buttonPanel.compl;
        let ok2 = count;
        let ok3 = buttonPanel.widthArr.length === count;
        let ok4 =  buttonPanel.widthArrVer.length === count;
        buttonPanel.ready =  ok1 && ok2 && ok3 && ok4;
    }

    function setOpenButtonVisible(){
        let count = 0;
        if(buttonPanel.canChangeOrder){
            if (buttonPanel.buttonModel){
                count = buttonPanel.buttonModel.getItemsCount();
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
                buttonPanel.rightOrderModel.clear();
                for(var i = 0; i < buttonPanel.buttonModel.getItemsCount(); i++){
                    var active = buttonPanel.buttonModel.getData("Active",i);
                    if(active){
                        buttonPanel.activeId = buttonPanel.buttonModel.getData("Id",i);
                    }
                    buttonPanel.rightOrderModel.insertNewItem()
                    buttonPanel.rightOrderModel.copyItemDataFromModel(i,buttonPanel.buttonModel,i);
                }
            }

            horizontalListView.model = buttonPanel.horizontalModel;
        }
    }

    function setVertMenuWidth(){
        if(!buttonPanel.canChangeOrder){
            let visibleCount_ = buttonPanel.buttonModel.getItemsCount() - buttonPanel.horizCount;
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

    function getMaxString(){
        var max = "";
        for(var i = 0; i < buttonPanel.buttonModel.getItemsCount(); i++){
            var str = buttonPanel.buttonModel.getData("Name",i);
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
            if (!buttonPanel.buttonModel){
                return;
            }

            if(buttonPanel.buttonModel.getItemsCount() !== undefined && buttonPanel.buttonModel.getItemsCount()){
                buttonPanel.setModels();
            }
            if(buttonPanel.hasActiveState && buttonPanel.buttonModel.getItemsCount() !== undefined && buttonPanel.buttonModel.getItemsCount()){
                var index = buttonPanel.checkActiveInVertical();
                if(index > -1){
                    buttonPanel.setModelsWithActive(index);
                }

                if(buttonPanel.buttonModel.getItemsCount() <= buttonPanel.horizCount){
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
            if(buttonPanel.buttonModel.getItemsCount() !== undefined && buttonPanel.buttonModel.getItemsCount()){
                buttonPanel.setModels();
            }

            if(buttonPanel.hasActiveState && buttonPanel.buttonModel.getItemsCount() !== undefined && buttonPanel.buttonModel.getItemsCount()){
                var index = buttonPanel.checkActiveInVertical();
                if(index > -1){
                    buttonPanel.setModelsWithActive(index);
                }

                if(buttonPanel.buttonModel.getItemsCount() <= buttonPanel.horizCount){
                    buttonPanel.setRightOrder();
                }
            }
        }
    }

    function setModels(){
        if(buttonPanel.canChangeOrder){
            buttonPanel.horizontalModel.clear();
            buttonPanel.verticalModel.clear();
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

        var count = buttonPanel.buttonModel.getItemsCount();
        if(count <= buttonPanel.horizCount){
            for(let i = 0; i < count; i++){
                if(buttonPanel.canChangeOrder){
                    buttonPanel.horizontalModel.insertNewItem()
                    buttonPanel.horizontalModel.copyItemDataFromModel(i,buttonPanel.buttonModel,i);
                }
                else {
                    buttonPanel.buttonModel.setData("IsHorizontal",true, i)
                }
            }
        }
        else{
            for(i = 0; i < buttonPanel.horizCount; i++){
                if(buttonPanel.canChangeOrder){
                    buttonPanel.horizontalModel.insertNewItem()
                    buttonPanel.horizontalModel.copyItemDataFromModel(i,buttonPanel.buttonModel,i);
                }
                else {
                    buttonPanel.buttonModel.setData("IsHorizontal",true, i)

                }
            }

            for(var k = buttonPanel.horizCount; k < count; k++){
                if(buttonPanel.canChangeOrder){
                    var kk = buttonPanel.verticalModel.insertNewItem();
                    buttonPanel.verticalModel.copyItemDataFromModel(kk,buttonPanel.buttonModel,k);
                }
                else {
                    buttonPanel.buttonModel.setData("IsHorizontal",false, k)
                }

            }
        }

        buttonPanel.setVertMenuWidth();

        if(buttonPanel.openST){
            buttonPanel.menuPositionCorrection();
        }

        if(buttonPanel.canChangeOrder){
            if(buttonPanel.openST && !buttonPanel.verticalModel.getItemsCount()){
                buttonPanel.openST = false;
                ModalDialogManager.closeByComp(vertMenuComp);
            }

        }
        else{
            if(buttonPanel.openST && buttonPanel.horizCount == buttonPanel.buttonModel.getItemsCount()){
                buttonPanel.openST = false;
                ModalDialogManager.closeByComp(vertMenuComp);
            }
        }

        buttonPanel.setOpenButtonVisible();
    }

    function setActive(buttonId){
        var retval = 0;
        for(var i = 0; i < buttonPanel.buttonModel.getItemsCount(); i++){
            var id = buttonPanel.buttonModel.getData("Id",i);
            if(id == buttonId){
                buttonPanel.buttonModel.setData("Active",true, i);
                retval = i;

            }
            else{
                buttonPanel.buttonModel.setData("Active",false, i);
            }
        }

        return retval;
    }

    function setModelsWithActive(index){
        if(buttonPanel.horizCount == 0){
            for(var i = 0; i < buttonPanel.verticalModel.getItemsCount(); i++){
                buttonPanel.verticalModel.setData("Active",(index == i), i);
            }
        }

        else{
            if (index < buttonPanel.horizCount){
                for(let i = 0; i < buttonPanel.horizontalModel.getItemsCount(); i++){
                    buttonPanel.horizontalModel.setData("Active",(index == i), i);
                }
            }

            else{//перезаполнение моделей
                buttonPanel.startActiveIndex = index;

                buttonPanel.proxiModel.clear();

                var count = buttonPanel.buttonModel.getItemsCount();

                var countVert = 0;
                for(i = index; i < count; i++){
                    buttonPanel.proxiModel.insertNewItem()
                    buttonPanel.proxiModel.copyItemDataFromModel(countVert,buttonPanel.buttonModel,i);
                    countVert++;
                }

                var countHoriz = 0;
                for(i = 0; i < count - countVert ; i++){
                    buttonPanel.proxiModel.insertNewItem()
                    buttonPanel.proxiModel.copyItemDataFromModel(countVert + countHoriz,buttonPanel.buttonModel,i);
                    countHoriz++;
                }

                buttonPanel.buttonModel.clear();
                for(i = 0; i < count  ; i++){
                    buttonPanel.buttonModel.insertNewItem()
                    buttonPanel.buttonModel.copyItemDataFromModel(i,buttonPanel.proxiModel,i);
                }

                buttonPanel.setModels();
            }//перезаполнение моделей
        }
    }//setModelsWithActive

    function checkActiveInVertical(){
        var index = false;
        for(var i = 0; i < buttonPanel.buttonModel.getItemsCount(); i++){
            var active = buttonPanel.buttonModel.getData("Active",i);
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
        buttonPanel.buttonModel.clear();
        for(var i = 0; i < buttonPanel.rightOrderModel.getItemsCount(); i++){
            buttonPanel.buttonModel.insertNewItem()
            buttonPanel.buttonModel.copyItemDataFromModel(i,buttonPanel.rightOrderModel,i);
        }

        for(let i = 0; i < buttonPanel.buttonModel.getItemsCount(); i++){
            var id = buttonPanel.buttonModel.getData("Id",i);
            buttonPanel.buttonModel.setData("Active",(id == buttonPanel.activeId), i);
        }

        buttonPanel.setModels();
    }

    Repeater {
        id: buttonSizeRep;

        anchors.right: parent ? parent.right : undefined;

        width: 0; height: 0;

        visible: false;
        clip: true;

        delegate: buttonPanel.buttonDelegate;

        onItemAdded: {
            let okLength = buttonPanel.widthArr.length == 0 ? true :
                                                              buttonPanel.widthArr.length < buttonPanel.buttonModel.getItemsCount();

            let width__ = item.width;

            let ok = item && width__ > 0 && okLength;

            if(ok){
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

        anchors.right: parent ? parent.right : undefined;

        width: 0; height: 0;

        visible: false;
        clip: true;

        delegate: buttonPanel.buttonDelegateVert;

        onItemAdded: {
            let okLength = buttonPanel.widthArrVer.length == 0 ? true :
                                                              buttonPanel.widthArrVer.length < buttonPanel.buttonModel.getItemsCount();

            let width__ = item.width;

            let ok = item && width__ > 0 && okLength;

            if(ok){
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
        color: "transparent";

        ListView{
            id: horizontalListView;

            anchors.verticalCenter: parent ? parent.verticalCenter : undefined;
            anchors.left: parent ? parent.left : undefined;
            anchors.leftMargin: !parent ? 0 : !buttonPanel.centered ? 0 : parent.width/2 - width/2;

            width: Math.min(contentWidth, parent.width);
            height: parent.height;

            boundsBehavior: Flickable.StopAtBounds;
            orientation: ListView.Horizontal;
            interactive: false;

            spacing: buttonPanel.horizontalSpacing;

            delegate: buttonPanel.buttonDelegate;
        }//horizontalListView
    }//horizontalListViewContainer

    Button{
        id: openButton;

        anchors.verticalCenter: parent ? parent.verticalCenter : undefined;
        anchors.left: horizontalListViewContainer.right;
        anchors.leftMargin: buttonPanel.mainMargin;

        width: buttonPanel.openButtonWidth;
        height: buttonPanel.openButtonHeight;
        text: buttonPanel.openButtonText;
        iconSource: buttonPanel.openButtonImageSource;
        visible: false;//buttonPanel.buttonModel.getItemsCount() && buttonPanel.buttonModel.getItemsCount() > buttonPanel.horizCount;

        onClicked: {
            buttonPanel.setVertMenuWidth();
            var point = buttonPanel.getMenuPoint();

            ModalDialogManager.openDialog(vertMenuComp, {
                                              "x":     point.x,
                                              "y":     point.y,
                                          });

            buttonPanel.vertMenuItem = ModalDialogManager.topItem;
            buttonPanel.openST = true;
        }
    }

    Component{
        id: vertMenuComp;

        Rectangle{
            id: verticalListViewContainer;

//            width: verticalListView.width + 2*buttonPanel.mainMargin;
//            height: verticalListView.height + 2*buttonPanel.mainMargin;
            width: verticalListView.width;
            height: verticalListView.height;
            radius: 4;
            border.width: 1;
            border.color: buttonPanel.shadowColor;
            color: buttonPanel.baseColor;

            clip: true;

            property Item root: null;
            property bool doNotCorrectPosition: true;
            property bool hiddenBackground: true;

            function onBackgroundClicked(){
                console.log("ButtonPanel onBackgroundClicked");

                ModalDialogManager.closeByComp(vertMenuComp);
                buttonPanel.openST = false;
            }

            ListView{
                id: verticalListView;

                anchors.horizontalCenter: parent.horizontalCenter;
                anchors.top: parent.top;
//                anchors.topMargin: buttonPanel.mainMargin;

                width: buttonPanel.verticalMenuWidth;
                height: contentHeight;

                clip: true;
                boundsBehavior: Flickable.StopAtBounds;
                interactive: false;

                spacing: buttonPanel.verticalSpacing;

                model: buttonPanel.canChangeOrder ? buttonPanel.verticalModel :
                                                    buttonPanel.buttonModel;

                delegate: buttonPanel.buttonDelegateVert;
            }//verticalListView
        }//verticalListViewContainer
    }
}
