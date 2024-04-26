import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

ControlBase {
    id: comboBoxContainer;

    decorator: Style.comboBoxDecorator

    property var model:0;

    property color borderColor: comboBoxContainer.activeFocus ? Style.iconColorOnSelected : Style.borderColor;

    property color backgroundColor: changeable ? Style.baseColor : Style.alternateBaseColor;

    property string currentText;

    property bool changeable: true;

    property bool isColor: false;

    property bool textCentered: false;
    property bool hiddenBackground: true;
    property bool openST: false;

    property bool visibleScrollBar: true;
    property string compMainColor: "transparent";
    property string compSelectedColor: Style.selectedColor;
    property bool moveToEnd: false;
    property int moveToIndex: currentIndex;

    property int shownItemsCount: 5;

    property int radius: Style.comboBoxRadius;
    property int currentIndex: -1;

    property string placeHolderText: "";

    property real contentY;

    // ID for display in combo box delegates
    property string nameId: "Name";

    property bool hoverBlocked: true;

    property var popup: null;

    property Component delegate: Component {PopupMenuDelegate{
        width: comboBoxContainer.width;
        height: comboBoxContainer.itemHeight;

        highlighted: comboBoxContainer.currentIndex == model.index
        text: model[comboBoxContainer.nameId];

        selected: comboBoxContainer.popup ? comboBoxContainer.popup.selectedIndex == model.index : false;

        onClicked: {
            if (comboBoxContainer.popup){
                comboBoxContainer.popup.finished(model.Id, model.index)
            }
        }

        onEntered: {
            if (comboBoxContainer.popup){
                comboBoxContainer.popup.selectedIndex = model.index;
            }
        }
    }
    }

    property alias containsMouse: cbMouseArea.containsMouse;

    property int textSize: Style.fontSize_common;
    property int itemHeight: 26;
    property string fontColor: Style.textColor;
    property string fontColorTitle: fontColor;

    property alias tooltipText: tooltip.text;
    property alias tooltipItem: tooltip;
    property bool isOpen: false;

    signal accepted();
    signal activated();
    signal highlighted(int index);

    signal clicked();
    signal finished(string commandId, int index);

    onFinished: {
        if (index >= 0){
            comboBoxContainer.currentIndex = index;
        }

        comboBoxContainer.isOpen = false;
    }

    onModelChanged: {
        if (!comboBoxContainer.model){
            return;
        }

        if (comboBoxContainer.currentIndex > -1){
            comboBoxContainer.currentText = comboBoxContainer.model.GetData(comboBoxContainer.nameId, comboBoxContainer.currentIndex);
        }
    }

    onCurrentIndexChanged: {
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

    property Component popupMenuComp: Component {
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
            visibleScrollBar: comboBoxContainer.visibleScrollBar;
            selectedIndex: comboBoxContainer.currentIndex;

            onFinished: {
                comboBoxContainer.finished(commandId, index)
            }

            onStarted: {
                comboBoxContainer.popup = popup;
            }
        }
    }

    onDecoratorChanged: {
        bindCurrentIndex.target = decorator_
    }

    Binding {
        id: bindCurrentIndex;
        property: "currentIndex"
        value: comboBoxContainer.currentIndex;
    }

    function closePopupMenu(){
        if (comboBoxContainer.popup){
            comboBoxContainer.popup.close();
        }

        comboBoxContainer.isOpen = false;
    }

    function openPopupMenu(){
        var point = comboBoxContainer.mapToItem(null, 0, comboBoxContainer.height);
        modalDialogManager.openDialog(popupMenuComp, {
                                          "x":     point.x,
                                          "y":     point.y,
                                          "model": comboBoxContainer.model,
                                          "width": comboBoxContainer.width});

        comboBoxContainer.isOpen = true;
    }

    MouseArea {
        id: cbMouseArea;

        anchors.fill: parent;
        hoverEnabled: true;

        cursorShape: comboBoxContainer.changeable ? Qt.PointingHandCursor : Qt.ArrowCursor;

        onClicked: {
            if (!comboBoxContainer.model || !comboBoxContainer.changeable){
                return;
            }

            comboBoxContainer.focus = true;
            comboBoxContainer.forceActiveFocus();

            if (comboBoxContainer.model !==undefined && comboBoxContainer.model.GetItemsCount() > 0){
                comboBoxContainer.openPopupMenu();
            }

            comboBoxContainer.clicked();
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

    Shortcut {
        sequence: "Space";
        enabled: !comboBoxContainer.isOpen && (comboBoxContainer.activeFocus) && comboBoxContainer.visible;
        onActivated: {
            comboBoxContainer.openPopupMenu();
        }
    }

    Shortcut {
        sequence: "Ctrl+Up";
        enabled: !comboBoxContainer.isOpen && comboBoxContainer.activeFocus && comboBoxContainer.visible;
        onActivated: {
            if(comboBoxContainer.model !==undefined && comboBoxContainer.model.GetItemsCount() && comboBoxContainer.currentIndex > 0){
                comboBoxContainer.currentIndex--;
            }

        }
    }

    Shortcut {
        sequence: "Ctrl+Down";
        enabled: !comboBoxContainer.isOpen && comboBoxContainer.activeFocus && comboBoxContainer.visible;
        onActivated: {
            if(comboBoxContainer.model !==undefined && comboBoxContainer.model.GetItemsCount() && comboBoxContainer.currentIndex < (comboBoxContainer.model.GetItemsCount()-1)){
                comboBoxContainer.currentIndex++;
            }

        }
    }
}
