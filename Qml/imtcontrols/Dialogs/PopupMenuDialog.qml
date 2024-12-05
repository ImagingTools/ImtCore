import QtQuick 2.12
import Qt5Compat.GraphicalEffects 6.0
import QtGraphicalEffects 1.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

ControlBase {
    id: popupMenuContainer;

    width: itemWidth;
    heightFromDecorator: true;

    decorator: Style.popupDecorator;

    property var model;

    // Main dialog manager reference
    property Item root: null;

    property int itemWidth: 200;
    property int itemHeight: 30;
    property int shownItemsCount: 5;

    property int textSize: Style.fontSize_common;
    property string fontColor: Style.textColor;

    property bool hiddenBackground: true;
    property bool visibleScrollBar: true;
    property bool moveToEnd: false;
    property int moveToIndex: -1;

    property var contentY: decorator_ ? decorator_.contentY : null;

    property bool forceFocus: false;

    // The current index of the delegate that the focus is on
    property int selectedIndex: -1;
    property bool hoverBlocked: true;

    // ID for display in combo box delegates
    property string nameId: "Name";

    property Component delegate: PopupMenuDelegate {
        width: popupMenuContainer.width;
        height: popupMenuContainer.itemHeight;

        text: model[popupMenuContainer.nameId];
        iconSource: model.Icon === "" ? "" :  enabled ? "../../../../" + Style.getIconPath(model.Icon, Icon.State.On, Icon.Mode.Normal) :
                                                        "../../../../" + Style.getIconPath(model.Icon, Icon.State.Off, Icon.Mode.Disabled);
        enabled: model.IsEnabled != null && model.IsEnabled != undefined ? model.IsEnabled : true;

        selected: popupMenuContainer.selectedIndex == model.index;

        onClicked: {
            popupMenuContainer.finished(model.Id, model.index);
        }

        onEntered: {
            popupMenuContainer.selectedIndex = model.index;
        }
    }

    signal finished(string commandId, int index);
    signal started();

    //forShortcutEnter
    Component.onCompleted: {
        Events.subscribeEvent("AppSizeChanged", onAppSizeChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("AppSizeChanged", onAppSizeChanged);
    }

    onFinished: {
        close();
    }

    function onAppSizeChanged(parameters){
        onBackgroundClicked();
    }

	function onBackgroundClicked(events){
		popupMenuContainer.finished('', -1);
    }

    function close(){
        if (root){
            root.closeDialog();
        }
    }

    function onEscape(){
        popupMenuContainer.finished('', -1);
    }

    function onReturn(){
        if(popupMenuContainer.selectedIndex >= 0){
            var id = popupMenuContainer.model.getData("Id", popupMenuContainer.selectedIndex);
            popupMenuContainer.finished(id, popupMenuContainer.selectedIndex);
        }
    }

    function onKeyboardUp(){
        if(popupMenuContainer.selectedIndex > 0){
            popupMenuContainer.selectedIndex--;

            popupMenuContainer.decorator_.contentYCorrection(false);
        }
    }

    function onKeyboardDown(){
        if(popupMenuContainer.selectedIndex < popupMenuContainer.model.getItemsCount() - 1){
            popupMenuContainer.selectedIndex++;

            popupMenuContainer.decorator_.contentYCorrection(true);
        }
    }

    Shortcut {
        sequence: "Escape";
        enabled: popupMenuContainer.visible;
        onActivated: {
           popupMenuContainer.onEscape();
        }
    }

    Shortcut {
        sequence: "Return";
        enabled: popupMenuContainer.visible;
        onActivated: {
            popupMenuContainer.onReturn();
        }
    }

    Shortcut {
        sequence: "Up";
        enabled: popupMenuContainer.visible;
        onActivated: {
            popupMenuContainer.onKeyboardUp();
        }
    }

    Shortcut {
        sequence: "Down";
        enabled: popupMenuContainer.visible;
        onActivated: {
            popupMenuContainer.onKeyboardDown();
        }
    }
}


