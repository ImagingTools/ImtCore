import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import QtQuick.Window 2.2

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

    // Top level dialog finished
    signal finished(string result);

    onFinished: {
        console.log("DialogsManager onFinished", result);
    }

    function openDialog(comp, parameters, mode, callback){
        console.log("DialogsManager addDialog", comp, parameters, mode, callback);
        let dialogMode = mode !== undefined ? mode : Style.dialogMode !== undefined ? Style.dialogMode: "Internal";

        /*for Windows style dialogs*/
        if(dialogMode == "External"){
            container.openWindow(comp, parameters);
        }
        else {
            modalDialogModels.append({"Component": comp, "Parameters": parameters});

            if (callback){
                finished.connect(callback);
            }
        }

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

                clip: false;

                onMainWindowWidthChanged: {
                    var doNotCorrectPosition = !dialogLoader.item ? false : dialogLoader.item.doNotCorrectPosition == undefined ? false : dialogLoader.item.doNotCorrectPosition;
                    var centered = !dialogLoader.item ? false : dialogLoader.item.centered == undefined ? false : dialogLoader.item.centered;
                    if(dialogLoader.item && !doNotCorrectPosition){
                        if(dialogLoader.item && centered){
                            dialogLoader.x = container.width/2 - dialogLoader.width/2;
                            dialogLoader.y = container.height/2 - dialogLoader.height/2;
                        }
                        else if(dialogLoader.item && !centered){
                            var deltaWidth = mainWindowWidth - mainWindowWidth_prev;
                            mainWindowWidth_prev = mainWindowWidth;
                            dialogLoader.x += deltaWidth/2;
                        }
                    }

                }
                onMainWindowHeightChanged: {
                    var doNotCorrectPosition = !dialogLoader.item ? false : dialogLoader.item.doNotCorrectPosition == undefined ? false : dialogLoader.item.doNotCorrectPosition;
                    var centered = !dialogLoader.item ? false : dialogLoader.item.centered == undefined ? false : dialogLoader.item.centered;
                    if(dialogLoader.item && !doNotCorrectPosition){
                        if(dialogLoader.item && centered){
                            dialogLoader.x = container.width/2 - dialogLoader.width/2;
                            dialogLoader.y = container.height/2 - dialogLoader.height/2;
                        }
                        else if(dialogLoader.item && !centered){
                            var deltaHeight = mainWindowHeight - mainWindowHeight_prev;
                            mainWindowHeight_prev = mainWindowHeight;
                            dialogLoader.y += deltaHeight/2;
                        }
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

                    if (dialogLoader.item.finished){
                        dialogLoader.item.finished.connect(container.finished);
                    }

                    dialogLoader.mainWindowWidth_prev = container.width;
                    dialogLoader.mainWindowHeight_prev = container.height;
                }
            }
        }
    }

    /*for Windows style dialogs*/
    function openWindow(comp, parameters){
        console.log("DialogsManager addWindow", comp);
        let obj = windowComp.createObject(container);
        obj.parameters = parameters;
        obj.sourceComponent = comp;
        for (let key in parameters) {
            if(key == "modality"){
                obj[key] = parameters[key];
            }
        }
        obj.show();
    }

    Component {
        id: windowComp;

        Window {
            id: windowObj;

            title: " ";

            flags: Qt.Dialog;
            modality: Qt.NonModal;//Qt.WindowModal

            width: 100;
            height: 100;

            property var parameters;
            property alias sourceComponent: windowLoader.sourceComponent;
            property Item item: !windowLoader.item ? null : windowLoader.item;

            function closeFunc(buttonId){
                windowObj.close();
            }

            Component.onDestruction:  {
            }

            onClosing:{
                console.log("closing window")
                windowObj.destroy();
            }

            onWidthChanged: {
                if(windowLoader.item){
                    windowLoader.item.width = windowObj.width;
                }
            }

            onHeightChanged: {
                if(windowLoader.item){
                    windowLoader.item.height = windowObj.height;
                }
            }

            Loader{
                id: windowLoader;

                onLoaded: {
                    item.finished.connect(windowObj.closeFunc);
                    windowObj.width = item.width;
                    windowObj.height = item.height;

                    for (let key in windowObj.parameters) {
                        if(item[key] !== undefined){
                            item[key] = windowObj.parameters[key];
                        }
                    }

                }
            }

        }

    }
}
