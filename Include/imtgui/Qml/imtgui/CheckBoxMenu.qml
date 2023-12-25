import QtQuick 2.12
import Qt5Compat.GraphicalEffects 6.0
import QtGraphicalEffects 1.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

FocusScope{
    id: checkBoxMenu;

    width: 300;
    height: 34;

    property var model;
    property string nameId: "Name";

    property string currentText;

    property int radius: 2;
    property int currentIndex: -1;
    property string placeHolderText: "";
    property string multipleChoiceText: "Мультивыбор";//"Multiple choice";


    property color borderColor: checkBoxMenu.focus ? Style.iconColorOnSelected : Style.borderColor;
    property color backgroundColor: Style.baseColor;
    property bool isColor: false;
    property int dialogsCountPrev: 1000;
    property int dialogsCount: modalDialogManager.count;

    property alias image: cbArrowIcon;
    property alias imageSource: cbArrowIcon.source;
    property alias imageWidth:  cbArrowIcon.width;
    property alias imageHeight: cbArrowIcon.height;
    property alias imageRotation: cbArrowIcon.rotation;
    property alias imageVisible: cbArrowIcon.visible;

    property string checkImageSource: "../../../" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal);

    property int textSize: Style.fontSize_common;
    property string fontColor: Style.textColor;
    property string fontColorTitle: fontColor;

    property alias tooltipText: tooltip.text;
    property alias tooltipItem: tooltip;

    property bool isOpen: false;

    property TreeItemModel dataModel: TreeItemModel{};
    property TreeItemModel retModel: TreeItemModel{};

    property var menuItem : null;
    property bool canOpenMenu : true;
    property bool hasSearch : true;

    property int menuHeight: 400;
    property int delegateHeight: 60;

    signal finished();
    signal menuCreated();
    signal changedSignal()

    Component.onCompleted: {

    }

    onFinished: {

    }

    function openPopupMenu(){
        checkBoxMenu.isOpen = true;
        checkBoxMenu.dialogsCountPrev = modalDialogManager.count;
        var point = checkBoxMenu.mapToItem(null, 0, checkBoxMenu.height);
        modalDialogManager.openDialog(popupMenu, { "x":     point.x,
                                          "y":     point.y ,
                                          "width": checkBoxMenu.width,
                                          "dataModel": checkBoxMenu.dataModel,
                                          "rootItem": checkBoxMenu });

    }

    function closePopupMenu(){
        checkBoxMenu.finished();
        checkBoxMenu.isOpen = false;
        checkBoxMenu.focus = false;
        modalDialogManager.closeDialog();
    }

    function createMenuItem(){
        if(!menuItem){
            menuItem = popupMenu.createObject(checkBoxMenu);
            menuItem.width = checkBoxMenu.width;
            menuItem.dataModel = checkBoxMenu.dataModel;
            menuItem.rootItem = checkBoxMenu;
            menuCreated();
        }
    }

    Component {
        id: popupMenu;

        Item{
            id: popupMenuContainer;

            width: 400;
            height: checkBoxMenu.menuHeight;

            property Item root: null;
            property Item rootItem: null;
            property TreeItemModel dataModel: TreeItemModel{};
            signal setCheckedSignal();

            onDataModelChanged: {
                if(dataModel.GetItemsCount()){
                    popupMenuContainer.setAllChecked();
                    popupMenuContainer.setCheckedSignal();
                }
            }

            Component.onDestruction: {
                Events.unSubscribeEvent("DialogBackgroundClicked", popupMenuContainer.onBackgroundClicked)
            }

            onRootChanged: {
                //console.log("PopupMenu onRootChanged", root)
                /**
                    Opacity of the background = 0
                */
                root.backgroundItem.opacity = 0;

                /**
                    Close the dialog by clicking on the background
                */
                Events.subscribeEvent("DialogBackgroundClicked", popupMenuContainer.onBackgroundClicked)
            }

            function onBackgroundClicked(){
                console.log("ChechBoxMenu onBackgroundClicked");
                rootItem.closePopupMenu();
            }

            function setAllCheckedInModel(state_){

                let itemsCount = popupMenuContainer.dataModel.GetItemsCount();
                 for(var i = 0; i < itemsCount ; i++){
                     popupMenuContainer.dataModel.SetData("checkState", state_,i)
                     popupMenuContainer.rootItem.dataModel.SetData("checkState", state_,i)
                 }
                 popupMenuContainer.setCheckedSignal();
                 setCurrentText();
            }

            function setAllChecked(){
                let count = 0;
                let itemsCount = popupMenuContainer.dataModel.GetItemsCount();
                if(!itemsCount){
                    return;
                }
                for(var i = 0; i < itemsCount ; i++){
                    let checkSt = popupMenuContainer.dataModel.IsValidData("checkState",i) ?
                                popupMenuContainer.dataModel.GetData("checkState", i) : Qt.Unchecked

                    if(checkSt > 0){
                        count++;
                    }
                }
                let ok = count == itemsCount;
                if(ok){
                    checkBoxAll.checkState = Qt.Checked;
                }
                else {
                    checkBoxAll.checkState = Qt.Unchecked;
                }
                let retVal = ok ? Qt.Checked : Qt.Unchecked;
                setCurrentText();
                return retVal;
            }

            function setCurrentText(){
                let currText = "";
                let count = 0;
                let itemsCount = popupMenuContainer.dataModel.GetItemsCount();
                if(!itemsCount){
                    return;
                }
                for(var i = 0; i < itemsCount ; i++){
                    let checkSt = popupMenuContainer.dataModel.IsValidData("checkState",i) ?
                                popupMenuContainer.dataModel.GetData("checkState", i) : Qt.Unchecked

                    let name_ = popupMenuContainer.dataModel.GetData(checkBoxMenu.nameId,i);
                    if(checkSt){
                        if(currText !== ""){
                            currText = currText + ", ";
                        }
                        currText = currText + name_;
                        count++;
                    }
                }
//                if(count > 1){
//                    checkBoxMenu.currentText = checkBoxMenu.multipleChoiceText;
//                }
//                else {
//                    checkBoxMenu.currentText = currText;
//                }
                //checkBoxMenu.currentText = currText;
                if(popupMenuContainer.rootItem){
                    popupMenuContainer.rootItem.currentText = currText;
                }
            }

            DropShadow {
                id: dropShadow;

                anchors.fill: body;

                z: body.z-1

                horizontalOffset: 2;
                verticalOffset: 2;

                radius: 4;
                color: Style.shadowColor;

                source: body;
            }

            Rectangle{
                id: body;

                width: popupMenuContainer.width;
                height: popupMenuContainer.height;
                radius: popupMenuContainer.rootItem ? popupMenuContainer.rootItem.radius : 0;

                color: Style.baseColor;

                border.width: 1;
                border.color: Style.alternateBaseColor;

                MouseArea{
                    anchors.fill: parent;
                    onClicked: {
                        mouse.accepted = true;
                    }
                }

                CustomScrollbar {
                    id: scrollbar;

                    z: 100;

                    anchors.left: listView.right;
                    anchors.bottom: listView.bottom;

                    backgroundColor: Style.baseColor;

                    secondSize: 10;
                    targetItem: listView;

                }

                CheckBox{
                    id: checkBoxAll;

                    anchors.left: parent.left;
                    anchors.leftMargin: Style.size_mainMargin;
                    anchors.verticalCenter: searchBlock.verticalCenter;

                    width: 28;
                    height: 28;
                    radius: 5
                    mainMargin: 16;
                    borderColor:Style.color_gray;
                    borderWidth: 2;

                    text: checkBoxMenu.hasSearch ? "" : "Все";

                    imageSource: popupMenuContainer.rootItem && checkState == Qt.Checked ? popupMenuContainer.rootItem.checkImageSource: "";

                    onClicked: {
                        if(checkBoxAll.checkState == Qt.Unchecked){
                            checkBoxAll.checkState = Qt.Checked;
                            popupMenuContainer.setAllCheckedInModel(Qt.Checked)
                        }
                        else if(checkBoxAll.checkState == Qt.Checked){
                            checkBoxAll.checkState = Qt.Unchecked;
                            popupMenuContainer.setAllCheckedInModel(Qt.Unchecked);
                        }
                        popupMenuContainer.rootItem.changedSignal();
                    }


                }//CheckBox

                Rectangle {
                    id: searchBlock;

                    anchors.top: parent.top;
                    anchors.left: checkBoxAll.right;
                    anchors.right: parent.right;
                    anchors.leftMargin: Style.size_mainMargin;
                    anchors.rightMargin: Style.size_mainMargin;
                    anchors.topMargin: 10;

                    width: parent.width;
                    height: Style.size_TextFieldHeight;
                    border.width: 1;
                    border.color: Style.color_gray;
                    radius: Style.size_TextFieldRadius;

                    visible: checkBoxMenu.hasSearch;

                    CustomTextField {
                        id: searchTfc;

                        anchors.horizontalCenter: parent.horizontalCenter;
                        anchors.verticalCenter: parent.verticalCenter;

                        width: parent.width - 10;
                        height: parent.height - 2*parent.border.width;
                        radius: parent.radius;
                        color: "#ffffff";
                        fontColor: "#000000";
                        textSize: Style.fontSize_common;
                        borderColorConst: "transparent";
                        placeHolderText: "Поиск";
                        readOnly: !searchBlock.visible;

                    }
                }

                ListView{
                    id: listView;

                    anchors.top: searchBlock.bottom;
                    anchors.bottom: parent.bottom;
                    anchors.left: parent.left;
                    anchors.right: parent.right;
                    anchors.topMargin: Style.size_smallMargin;
                    anchors.bottomMargin: Style.size_smallMargin;
                    anchors.leftMargin: Style.size_mainMargin;
                    anchors.rightMargin: Style.size_mainMargin;

                    boundsBehavior: Flickable.StopAtBounds;
                    clip: true;
                    spacing: 0;
                    model: popupMenuContainer.dataModel;
                    delegate: Item{

                        width: listView.width;
                        height: visible ? checkBoxMenu.delegateHeight: 0;
                        visible: !searchTfc.text  ? true : checkBox.text.toLowerCase().search(searchTfc.text.toLowerCase()) >= 0;

                        CheckBox{
                            id: checkBox;

                            anchors.verticalCenter: parent.verticalCenter;

                            width: 28;
                            height: 28;
                            radius: 5
                            mainMargin: 16;
                            borderColor:Style.color_gray;

                            imageSource: popupMenuContainer.rootItem && checkState == Qt.Checked ? popupMenuContainer.rootItem.checkImageSource: "";
                            text: model[checkBoxMenu.nameId];
                            Component.onCompleted: {
                                popupMenuContainer.setCheckedSignal.connect(checkBox.setCheckState);
                                checkBox.setCheckState();
                            }

                            function setCheckState(){
                                if(popupMenuContainer.dataModel.IsValidData("checkState", model.index)){
                                    checkState = popupMenuContainer.dataModel.GetData("checkState", model.index);
                                }
                            }

                            onClicked: {
                                if(checkState == Qt.Unchecked){
                                    popupMenuContainer.dataModel.SetData("checkState", Qt.Checked, model.index);
                                    popupMenuContainer.rootItem.dataModel.SetData("checkState", Qt.Checked, model.index);
                                }
                                else if(checkState == Qt.Checked){
                                    popupMenuContainer.dataModel.SetData("checkState", Qt.Unchecked, model.index);
                                    popupMenuContainer.rootItem.dataModel.SetData("checkState", Qt.Unchecked, model.index);
                                }
                                checkBox.setCheckState();
                                popupMenuContainer.setAllChecked();
                                popupMenuContainer.rootItem.changedSignal();
                            }
                        }//CheckBox

                    }
                }
            }
        }

    }//Component

    Rectangle {
        id: cbMainRect;

        anchors.fill: parent;

        border.color: checkBoxMenu.borderColor;
        border.width: 1;

        radius: checkBoxMenu.radius;
        color: checkBoxMenu.backgroundColor;

        gradient: Gradient {

            GradientStop { position: 0.0; color: checkBoxMenu.isColor ? cbMainRect.color : Style.imagingToolsGradient1; }
            GradientStop { position: 0.97; color: checkBoxMenu.isColor ? cbMainRect.color : Style.imagingToolsGradient2; }
            GradientStop { position: 0.98; color: checkBoxMenu.isColor ? cbMainRect.color : Style.imagingToolsGradient3; }
            GradientStop { position: 1.0; color: checkBoxMenu.isColor ? cbMainRect.color : Style.imagingToolsGradient4; }

        }

        Text {
            id: cbTitleTxt;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: parent.left;
            anchors.leftMargin: 10;
            anchors.right: cbArrowIcon.left;
            anchors.rightMargin: 10;

            color: checkBoxMenu.fontColorTitle;
            font.family: Style.fontFamily;
            font.pixelSize: checkBoxMenu.textSize;

            text: checkBoxMenu.currentText !== "" ? checkBoxMenu.currentText : checkBoxMenu.placeHolderText;

            elide: Text.ElideRight;
        }

        Image {
            id: cbArrowIcon;

            anchors.right: cbMainRect.right;
            anchors.verticalCenter: cbMainRect.verticalCenter;
            anchors.rightMargin: 5;

            width: 12;
            height: 10;
            rotation: checkBoxMenu.isOpen ? 180 : 0
            source: "../../../" + Style.getIconPath("Icons/Down", "On", "Normal");

            sourceSize.width: width;
            sourceSize.height: height;
        }

        MouseArea {
            id: cbMouseArea;

            anchors.fill: parent;

            hoverEnabled: true;

            cursorShape: checkBoxMenu.changeable ? Qt.PointingHandCursor : Qt.ArrowCursor;

            onClicked: {
                checkBoxMenu.focus = true

                if (checkBoxMenu.canOpenMenu && heckBoxMenu.dataModel !==undefined && checkBoxMenu.dataModel !== null && checkBoxMenu.dataModel.GetItemsCount() > 0){
                    checkBoxMenu.openPopupMenu();
                }

            }

            onPressed: {
                if(tooltip.text !== ""){
                    tooltip.closeTooltip();
                }
            }

            onPositionChanged: {
                if(tooltip.text !== ""){
                    tooltip.show(mouseX, mouseY);
                }
            }

            onExited: {
                if(tooltip.text !== ""){
                    tooltip.hide();
                }
            }
        }

        CustomTooltip{
            id: tooltip;

            fitToTextWidth: true;
        }


    }
}

