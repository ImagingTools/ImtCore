import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import QtQuick.Window 2.2

Item {
    id: container;

    property ListModel dialogsModel: ModalDialogManager.modalDialogModels;

    Component.onCompleted: {
        ModalDialogManager.activeView = container;
    }

    Repeater {
        id: modalDialogs;

        anchors.fill: parent;

        visible: container.dialogsModel.count > 0;

        model: container.dialogsModel;

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
                        mouse.accepted = true;
                    }
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

                property real itemWidth: item.width;
                property real itemHeight: item.height;

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

                onItemWidthChanged: {
                    if (dialogLoader.item.centered){
                        dialogLoader.x = container.width/2 - dialogLoader.width/2;
                        dialogLoader.y = container.height/2 - dialogLoader.height/2;
                    }
                }
                onItemHeightChanged: {
                    if (dialogLoader.item.centered){
                        dialogLoader.x = container.width/2 - dialogLoader.width/2;
                        dialogLoader.y = container.height/2 - dialogLoader.height/2;
                    }
                }

                onLoaded: {
                    ModalDialogManager.topItem = dialogLoader.item;
                    ModalDialogManager.backgroundItem = background;

                    dialogLoader.item.root = ModalDialogManager;

                    if (dialogLoader.item.onBackgroundClicked){
                        backgroundArea.clicked.connect(dialogLoader.item.onBackgroundClicked);
                    }

                    if (dialogLoader.item.hiddenBackground != undefined && dialogLoader.item.hiddenBackground){
                        background.opacity = 0;
                    }

					if(dialogLoader.item.noMouseArea != undefined && dialogLoader.item.noMouseArea){
						backgroundArea.visible = false;
					}

                    for (let key in model.Parameters) {
                        dialogLoader.item[key] = model.Parameters[key];
                    }

                    if (dialogLoader.item.centered){
                        dialogLoader.x = container.width/2 - dialogLoader.width/2;
                        dialogLoader.y = container.height/2 - dialogLoader.height/2;
                    }

                    if (dialogLoader.item.started){
                        dialogLoader.item.started();
                    }

                    if (dialogLoader.item.forceFocus === undefined || dialogLoader.item.forceFocus){
                        dialogLoader.item.forceActiveFocus();
                    }

                    if (dialogLoader.item.finished){
                        dialogLoader.item.finished.connect(ModalDialogManager.finished);
                    }

                    dialogLoader.mainWindowWidth_prev = container.width;
                    dialogLoader.mainWindowHeight_prev = container.height;
                }
            }
        }
    }
}
