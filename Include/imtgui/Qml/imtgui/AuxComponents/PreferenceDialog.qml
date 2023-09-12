import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Dialog {
    id: messageDialog;

    width: 500;

    title: qsTr("Preferences");

    notClosingButtons: "Apply";

    topPanelComp: Style.topPanelDialogDecorator !==undefined ? Style.topPanelDialogDecorator: topPanelDefault;

    property TreeItemModel settingsModel: TreeItemModel {};
    property SettingsProvider settingsProvider: null;

    onSettingsProviderChanged: {
        if (messageDialog.settingsProvider != null){
            messageDialog.settingsProvider.serverModelChanged.connect(messageDialog.onServerModelChanged);
        }
    }

    onSettingsModelChanged: {
        messageDialog.contentItem.settingsModel = messageDialog.settingsModel;
    }

    onLocalizationChanged: {
        messageDialog.buttonsModel.setProperty(0, "Name", qsTr("Apply"));

        let name = messageDialog.buttonsModel.get(1).Name;
        messageDialog.buttonsModel.setProperty(1, "Name", qsTr(name));

//        updateRepresentation();
    }

    function updateRepresentation(){
        messageDialog.contentItem.clearModels();

        let representatioModel = messageDialog.settingsProvider.getRepresentationModel();
        if (messageDialog.settingsProvider != null){
            messageDialog.contentItem.settingsModel = null;
            messageDialog.contentItem.settingsModel = representatioModel
        }
    }

    function onServerModelChanged(){
        console.log("Preference onServerModelChanged");
//        messageDialog.contentItem.clearModels();

//        let representatioModel = messageDialog.settingsProvider.getRepresentationModel();
//        if (messageDialog.settingsProvider != null){
//            messageDialog.contentItem.settingsModel = null;
//            messageDialog.contentItem.settingsModel = representatioModel
//        }
    }

    onFinished: {
        if (buttonId === "Apply"){
            messageDialog.buttons.setButtonState("Apply", false);

            messageDialog.buttonsModel.setProperty(1, "Name", qsTr("Close"));
        }
    }

    contentComp: Component {
        Preference {
            onModelChanged: {
                messageDialog.buttons.setButtonState("Apply", true);

                messageDialog.buttonsModel.setProperty(1, "Name", qsTr("Cancel"));
            }
        }
    }

    Component.onCompleted: {
        messageDialog.buttons.addButton({"Id":"Apply", "Name": qsTr("Apply"), "Enabled": false});
        messageDialog.buttons.addButton({"Id":"Close", "Name": qsTr("Close"), "Enabled": true});
    }

    Component{
        id: emptyDecorator;
        Item{
            property Item rootItem: null;
        }
    }
}
