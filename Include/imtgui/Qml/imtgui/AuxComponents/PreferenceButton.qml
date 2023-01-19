import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    property alias iconSource: preferenceButton.iconSource;
    property alias color: preferenceButton.color;
    property alias highlighted: preferenceButton.highlighted;

    AuxButton {
        id: preferenceButton;
        anchors.verticalCenter: parent.verticalCenter;

        width: 24;
        height: 24;

        iconSource: "../../../Icons/" + Style.theme + "/Settings_On_Normal.svg";

        onClicked: {
            console.log("Preference button clicked !");
//            var source = "AuxComponents/PreferenceDialog.qml";
//            var parameters = {};
//            parameters["localSettings"] = thumbnailDecoratorContainer.localSettings;
//            thumbnailDecoratorContainer.openDialog(source, parameters);
            thumbnailDecoratorContainer.setPreferencesVisible(true);
        }
    }
}

