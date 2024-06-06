import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
    id: window;

    anchors.fill: parent;

    color: Style.backgroundColor;

    property var localSettings;

    signal settingsUpdate();

    onLocalSettingsChanged: {
        preferenceDialog.settingsModel = localSettings;
    }

    Component.onCompleted: {
        Events.subscribeEvent("StartLoading", loading.start);
        Events.subscribeEvent("StopLoading", loading.stop);

        Style.setDecorators(decorators)
    }

    property Decorators decorators: decorators_

    Decorators {
        id: decorators_
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("StartLoading", loading.start);
        Events.unSubscribeEvent("StopLoading", loading.stop);
    }

    MouseArea{
        anchors.fill: parent;

        onClicked: {
            preferenceDialog.visible = true;
        }
    }

    Preference {
        id: preferenceDialog;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: buttons.top;

        onModelIsDirtyChanged: {
            buttons.setButtonState(Enums.apply, preferenceDialog.modelIsDirty);
        }
    }

    DialogButtons {
        id: buttons;

        anchors.bottom: parent.bottom;
        anchors.right: parent.right;
        anchors.rightMargin: 20;
        anchors.bottomMargin: 10;

        Component.onCompleted: {
            buttons.addButton({"Id":Enums.apply, "Name": qsTr("Apply"), "Enabled": false});
            buttons.addButton({"Id":Enums.ButtonType.Close, "Name": qsTr("Close"), "Enabled": true});
        }

        onButtonClicked: {
            if (buttonId == Enums.apply){
                window.settingsUpdate();
                preferenceDialog.modelIsDirty = false;
            }
            else if (buttonId == Enums.ButtonType.Close){
                if (preferenceDialog.modelIsDirty){
                    ModalDialogManager.openDialog(saveDialog, {"message": qsTr("Save all changes ?")});
                }
                else{
                    Qt.quit();
                }
            }
        }
    }

    Component {
        id: saveDialog;

        MessageDialog {
            Component.onCompleted: {
                buttonsModel.append({Id: Enums.cancel, Name:qsTr("Cancel"), Enabled: true})
            }

            onFinished: {
                console.log("saveDialog onFinished", buttonId);
                if (buttonId == Enums.yes){
                    window.settingsUpdate();

                    Qt.quit();
                }
                else if (buttonId == Enums.no){
                    Qt.quit();
                }
            }
        }
    }

    DialogManagerView {
        anchors.fill: parent;

        z: 30;
    }

    Loading {
        id: loading;

        anchors.fill: parent;

        visible: false;
    }
}


