import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Dialog {
    id: messageDialog;

    width: 500;

    title: qsTr("Preferences");

    notClosingButtons: Enums.ButtonType.Apply;

//    topPanelComp: Style.topPanelDialogDecorator !==undefined ? Style.topPanelDialogDecorator: topPanelDefault;

    buttonsModel: ListModel{
        ListElement{Id: Enums.ButtonType.Apply; Name:qsTr("Apply"); Enabled: false}
        ListElement{Id: Enums.ButtonType.Cancel; Name:qsTr("Close"); Enabled: true}
    }

    property TreeItemModel settingsModel: TreeItemModel {};
    property SettingsProvider settingsProvider: null;

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
        messageDialog.buttonsModel.setProperty(0, "Name", qsTr("Apply"));

        let name = messageDialog.buttonsModel.get(1).Name;
        messageDialog.buttonsModel.setProperty(1, "Name", qsTr(name));
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

        messageDialog.buttons.setButtonState(Enums.ButtonType.Apply, false);
        messageDialog.buttonsModel.setProperty(1, "Name", qsTr("Close"));
    }

    onFinished: {
        if (buttonId == Enums.ButtonType.Apply){
            messageDialog.buttons.setButtonState(Enums.ButtonType.Apply, false);
            messageDialog.buttonsModel.setProperty(1, "Name", qsTr("Close"));
        }
    }

    contentComp: Component {
        Preference {
            width: messageDialog.width;

            onModelChanged: {
                console.log("onModelChanged");

                messageDialog.buttons.setButtonState(Enums.ButtonType.Apply, true);

                messageDialog.buttonsModel.setProperty(1, "Name", qsTr("Cancel"));
            }
        }
    }
}
