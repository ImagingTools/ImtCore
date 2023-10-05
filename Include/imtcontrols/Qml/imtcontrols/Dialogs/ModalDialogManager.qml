import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: container;

    property var backgroundItem: null;
    property alias count: modalDialogModels.count;
    property Item topItem: null;
    visible: count > 0

    Component.onDestruction: {
        modalDialogModels.clear();
    }

    ListModel {
        id: modalDialogModels;
    }

    function openDialog(comp, parameters){
        console.log("DialogsManager addDialog", comp);
        modalDialogModels.append({"Component": comp, "Parameters": parameters});
    }

    function closeDialog(){
        console.log("DialogsManager closeDialog", modalDialogModels.count);

        if (modalDialogModels.count > 0){
            modalDialogModels.remove(modalDialogModels.count - 1);
        }
    }

    Repeater {
        id: modalDialogs;

        anchors.fill: parent;

        visible: modalDialogModels.count > 0;

        model: modalDialogModels;

        delegate: Item {
            id: dialogDelegate;

            anchors.fill: modalDialogs;

            Rectangle {
                id: background;

                anchors.fill: parent;

                color: "gray";
                visible: modalDialogs.visible;

                property alias backgroundAreaItem: backgroundArea;
                MouseArea {
                    id: backgroundArea;
                    anchors.fill: parent;
                    hoverEnabled: true;
                    preventStealing: true;
                    acceptedButtons: Qt.AllButtons

                    onWheel: {
                        wheel.accepted = true;
                    }
                    onClicked: {
                        Events.sendEvent("DialogBackgroundClicked");
                        mouse.accepted = true;
                    }
                    onDoubleClicked: {}
                    onReleased: {}
                    onPressAndHold: {}
                    onPressed: {}
                    onPositionChanged: {}
                }
            }

            Loader {
                id: dialogLoader;

                sourceComponent: model.Component;

                property real  mainWindowWidth: container.width;
                property real  mainWindowHeight: container.height;

                property real  mainWindowWidth_prev: 0;
                property real  mainWindowHeight_prev: 0;

                onMainWindowWidthChanged: {
                    if(dialogLoader.item && dialogLoader.item.centered){
                        dialogLoader.x = container.width/2 - dialogLoader.width/2;
                        dialogLoader.y = container.height/2 - dialogLoader.height/2;
                    }
                    else if(dialogLoader.item && !dialogLoader.item.centered){
                        var deltaWidth = mainWindowWidth - mainWindowWidth_prev;
                        mainWindowWidth_prev = mainWindowWidth;
                        dialogLoader.x += deltaWidth/2;
                    }


                }
                onMainWindowHeightChanged: {
                    if(dialogLoader.item && dialogLoader.item.centered){
                        dialogLoader.x = container.width/2 - dialogLoader.width/2;
                        dialogLoader.y = container.height/2 - dialogLoader.height/2;
                    }
                    else if(dialogLoader.item && !dialogLoader.item.centered){
                        var deltaHeight = mainWindowHeight - mainWindowHeight_prev;
                        mainWindowHeight_prev = mainWindowHeight;
                        dialogLoader.y += deltaHeight/2;
                    }
                }

                onLoaded: {
                    container.topItem = dialogLoader.item;

                    container.backgroundItem = background;

                    dialogLoader.item.root = container;
                    for (let key in model.Parameters) {
                        dialogLoader.item[key] = model.Parameters[key];
                    }
                    if (dialogLoader.item.centered){
                        //dialogLoader.anchors.centerIn = dialogDelegate;
                        dialogLoader.x = container.width/2 - dialogLoader.width/2;
                        dialogLoader.y = container.height/2 - dialogLoader.height/2;
                    }

                    if (dialogLoader.item.started){
                        dialogLoader.item.started();
                    }

                    console.log("dialogLoader.item.parent", dialogLoader.item.parent);

                    if (dialogLoader.item.forceFocus === undefined || dialogLoader.item.forceFocus){
                        dialogLoader.item.forceActiveFocus();
                    }

                    dialogLoader.mainWindowWidth_prev = container.width;
                    dialogLoader.mainWindowHeight_prev = container.height;
                }
            }
        }
    }
}
