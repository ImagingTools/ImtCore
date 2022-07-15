import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {

    AuxButton {
        id: preferenceButton;
        anchors.verticalCenter: parent.verticalCenter;

        width: 24;
        height: 24;

        iconSource: "../../../Icons/" + Style.theme + "/Settings_On_Normal.svg";

        onClicked: {
            console.log("Preference button clicked !", thubnailDecoratorContainer.localSettings);
//            var source = "AuxComponents/PreferenceDialog.qml";
//            var parameters = {};
//            parameters["localSettings"] = thubnailDecoratorContainer.localSettings;
//            thubnailDecoratorContainer.openDialog(source, parameters);
            thubnailDecoratorContainer.setPreferencesVisible(true);
        }
    }
}

