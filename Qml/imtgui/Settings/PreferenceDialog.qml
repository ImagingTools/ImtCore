import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Dialog {
    id: messageDialog;

    width: 500;

    title: qsTr("Preferences");

    notClosingButtons: Enums.apply;

//    topPanelComp: Style.topPanelDialogDecorator !==undefined ? Style.topPanelDialogDecorator: topPanelDefault;

    property TreeItemModel settingsModel: TreeItemModel {};
    property SettingsProvider settingsProvider: null;

    Component.onCompleted: {
        buttonsModel.append({Id: Enums.apply, Name:qsTr("Apply"), Enabled: false})
        buttonsModel.append({Id: Enums.cancel, Name:qsTr("Close"), Enabled: true})
    }

    onSettingsProviderChanged: {
        if (messageDialog.settingsProvider != null){
            messageDialog.settingsProvider.serverModelChanged.connect(messageDialog.updateRepresentation);
            messageDialog.settingsProvider.localModelChanged.connect(messageDialog.updateRepresentation);
        }
    }

    onSettingsModelChanged: {
        messageDialog.contentItem.settingsModel = messageDialog.settingsModel;
    }

    onLocalizationChanged: {
        messageDialog.buttonsModel.clear();
        messageDialog.buttonsModel.append({Id: Enums.apply, Name:qsTr("Apply"), Enabled: false})
        messageDialog.buttonsModel.append({Id: Enums.cancel, Name:qsTr("Close"), Enabled: true})
    }

    function updateRepresentation(){
        if (!messageDialog || !messageDialog.contentItem){
            return;
        }

        messageDialog.contentItem.clearModels();

        let representatioModel = messageDialog.settingsProvider.getRepresentationModel();
        if (messageDialog.settingsProvider != null){
            messageDialog.contentItem.settingsModel = null;
            messageDialog.contentItem.settingsModel = representatioModel
        }

        messageDialog.buttons.setButtonState(Enums.apply, false);
        messageDialog.buttonsModel.setProperty(1, "Name", qsTr("Close"));
    }

    onFinished: {
        if (buttonId == Enums.apply){
            messageDialog.buttons.setButtonState(Enums.apply, false);
            messageDialog.buttonsModel.setProperty(1, "Name", qsTr("Close"));
        }
    }

    contentComp: Component {
        Preference {
            width: messageDialog.width;
            onModelChanged: {
                console.log("onModelChanged");

                messageDialog.buttons.setButtonState(Enums.apply, true);

                messageDialog.buttonsModel.setProperty(1, "Name", qsTr("Cancel"));
            }
        }
    }
}
