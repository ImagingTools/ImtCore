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
    property int itemHeight: 26;
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
        iconSource: model.Icon === "" ? "" : "../../../../" + Style.getIconPath(model.Icon, Icon.State.On, Icon.Mode.Normal);

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
        console.log("Popup onCompleted", selectedIndex);
        Events.subscribeEvent("AppSizeChanged", onAppSizeChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("DialogBackgroundClicked", popupMenuContainer.onBackgroundClicked)
        Events.unSubscribeEvent("AppSizeChanged", onAppSizeChanged);
    }

    onFinished: {
        close();
    }

    onSelectedIndexChanged: {
        console.log("Popup onSelectedIndexChanged", selectedIndex);
    }

    onRootChanged: {
        /**
            Opacity of the background = 0
        */
        if(popupMenuContainer.hiddenBackground){
            popupMenuContainer.root.backgroundItem.opacity = 0;
        }

        /**
            Close the dialog by clicking on the background
        */
        Events.subscribeEvent("DialogBackgroundClicked", popupMenuContainer.onBackgroundClicked)
    }

    function onAppSizeChanged(parameters){
        onBackgroundClicked();
    }

    function onBackgroundClicked(){
        popupMenuContainer.finished('', -1);
    }

    function close(){
        if (root){
            root.closeDialog();
        }
    }

    Shortcut {
        sequence: "Escape";
        enabled: popupMenuContainer.visible;
        onActivated: {
            popupMenuContainer.finished('', -1);
        }
    }

    Shortcut {
        sequence: "Return";
        enabled: popupMenuContainer.visible;
        onActivated: {
            if(popupMenuContainer.selectedIndex >= 0){
                var id = popupMenuContainer.model.GetData("Id", popupMenuContainer.selectedIndex);
                popupMenuContainer.finished(id, popupMenuContainer.selectedIndex);
            }
        }
    }

    Shortcut {
        sequence: "Up";
        enabled: popupMenuContainer.visible;
        onActivated: {
            if(popupMenuContainer.selectedIndex > 0){
                popupMenuContainer.selectedIndex--;

                popupMenuContainer.decorator_.contentYCorrection(false);
            }
        }
    }

    Shortcut {
        sequence: "Down";
        enabled: popupMenuContainer.visible;
        onActivated: {
            if(popupMenuContainer.selectedIndex < popupMenuContainer.model.GetItemsCount() - 1){
                popupMenuContainer.selectedIndex++;

                popupMenuContainer.decorator_.contentYCorrection(true);
            }
        }
    }
}


