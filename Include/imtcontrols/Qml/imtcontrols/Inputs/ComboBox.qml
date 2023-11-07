import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

ControlBase {
    id: comboBoxContainer;

    decoratorComponent: Style.isQtStyle ? DecoratorsQt.comboBoxDecorator: Decorators.comboBoxDecorator;

    property var model;

    property color borderColor: comboBoxContainer.focus ? Style.iconColorOnSelected : Style.borderColor;

    property color backgroundColor: Style.baseColor;

    property string currentText;

    property bool changeable: true;

    property bool isColor: false;

    property bool textCentered: false;
    property bool menuVisible: false;
    property bool hiddenBackground: true;
    property bool openST: false;
    property bool compTextCentered:  false;
    property bool visibleScrollBar: true;
    property string compMainColor: "transparent";
    property string compSelectedColor: Style.selectedColor;
    property bool moveToEnd: false;
    property int moveToIndex: -1;

    property int dialogsCountPrev: 1000;
    property int dialogsCount: modalDialogManager.count;
    property int shownItemsCount: 5;

    property int radius: 5;
    property int currentIndex: -1;

    property string placeHolderText: "";

    property real contentY;

    // ID for display in combo box delegates
    property string nameId: "Name";

    property int selectedIndex: -1;
    property bool hoverBlocked: true;

    property Component delegate: PopupMenuDelegate{
        width: comboBoxContainer.width;
        height: comboBoxContainer.itemHeight;
        textSize: comboBoxContainer.textSize;
        fontColor: comboBoxContainer.fontColor;
        textCentered: comboBoxContainer.compTextCentered;
        mainColor: comboBoxContainer.compMainColor;
        selectedColor: comboBoxContainer.compSelectedColor;
        rootItem: comboBoxContainer;
    };

    property alias containsMouse: cbMouseArea.containsMouse;

    property int textSize: Style.fontSize_common;
    property int itemHeight: 26;
    property string fontColor: Style.textColor;
    property string fontColorTitle: fontColor;

    property alias tooltipText: tooltip.text;
    property alias tooltipItem: tooltip;
    property bool isOpen: false;

    signal clicked();
    signal finished(string commandId, int index);

    onFinished: {
        console.log("ComboFinished")
        comboBoxContainer.currentIndex = index;
        comboBoxContainer.isOpen = false;
    }

    onOpenSTChanged: {
        selectedIndex = -1;
    }

    onModelChanged: {
        if (!comboBoxContainer.model){
            return;
        }

        if (comboBoxContainer.currentIndex > -1){
            comboBoxContainer.currentText = comboBoxContainer.model.GetData(comboBoxContainer.nameId);
        }
    }

    onCurrentIndexChanged: {
        console.log("ComboBox onCurrentIndexChanged", comboBoxContainer.currentIndex);
        if (!comboBoxContainer.model){
            return;
        }

        if (comboBoxContainer.currentIndex > -1){
            if (comboBoxContainer.model.ContainsKey(comboBoxContainer.nameId, comboBoxContainer.currentIndex)){
                let name = comboBoxContainer.model.GetData(comboBoxContainer.nameId, comboBoxContainer.currentIndex);
                comboBoxContainer.currentText = name;
            }
        }
        else{
            comboBoxContainer.currentText = "";
        }
    }

    Component {
        id: popupMenu;
        PopupMenuDialog {
            id: popup;

            delegate: comboBoxContainer.delegate;
            width: comboBoxContainer.width;
            itemHeight: comboBoxContainer.itemHeight;
            hiddenBackground: comboBoxContainer.hiddenBackground;
            textSize: comboBoxContainer.textSize;
            fontColor: comboBoxContainer.fontColor;
            shownItemsCount: comboBoxContainer.shownItemsCount;
            moveToEnd: comboBoxContainer.moveToEnd;
            moveToIndex: comboBoxContainer.moveToIndex;
            rootItem: comboBoxContainer;
            visibleScrollBar: comboBoxContainer.visibleScrollBar;
            onFinished: {
                console.log("MenuFinished")

                popup.root.closeDialog();
            }
            Component.onCompleted: {
                comboBoxContainer.finished.connect(popup.finished);
            }
        }
    }

    Component.onCompleted: {

    }

    onDecoratorComponentChanged: {
        bindCurrentIndex.target = decorator
    }

    onDialogsCountChanged: {
        comboBoxContainer.openST = comboBoxContainer.dialogsCount > comboBoxContainer.dialogsCountPrev;
        if(!comboBoxContainer.openST && comboBoxContainer.dialogsCountPrev < 1000){
            comboBoxContainer.dialogsCountPrev = 1000;
        }
    }

    Binding {
        id: bindCurrentIndex;
        property: "currentIndex"
        value: comboBoxContainer.currentIndex;
    }

    function openPopupMenu(){
        comboBoxContainer.isOpen = true;
        comboBoxContainer.dialogsCountPrev = modalDialogManager.count;
        //var point = comboBoxContainer.mapToItem(thumbnailDecoratorContainer, 0, comboBoxContainer.height);
        var point = comboBoxContainer.mapToItem(null, 0, comboBoxContainer.height);
        modalDialogManager.openDialog(popupMenu, { "x":     point.x,
                                          "y":     point.y,
                                          "model": comboBoxContainer.model,
                                          "width": comboBoxContainer.width,
                                          "countVisibleItem": 5 });

    }

    function closePopupMenu(){
        modalDialogManager.closeDialog();
        comboBoxContainer.isOpen = false;
    }


    MouseArea {
        id: cbMouseArea;

        anchors.fill: parent;
        hoverEnabled: true;

        cursorShape: comboBoxContainer.changeable ? Qt.PointingHandCursor : Qt.ArrowCursor;

        onClicked: {
            comboBoxContainer.focus = true;
            comboBoxContainer.forceActiveFocus();

            if (comboBoxContainer.changeable && comboBoxContainer.model !==undefined && comboBoxContainer.model.GetItemsCount() > 0){
                comboBoxContainer.openPopupMenu();
            }

            comboBoxContainer.clicked();
            console.log("comboBoxContainer.clicked()")
        }

        onPressed: {
            if(tooltip.text !== ""){
                tooltip.closeTooltip();
            }
        }

        onEntered: {
            if(tooltip.text !== ""){
                pauseTooltip.stop();
                pauseTooltip.start();

            }

        }

        onExited: {
            if(tooltip.text !== ""){
                pauseTooltip.stop();
                tooltip.closeTooltip();
            }
        }
    }

    CustomTooltip{
        id: tooltip;
    }

    PauseAnimation {
        id: pauseTooltip;

        duration: tooltip.waitingDuration;
        onFinished: {
            tooltip.openTooltip(cbMouseArea.mouseX, cbMouseArea.mouseY);
        }
    }
}
