import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {

    AuxButton {
        id: loginButton;
        anchors.verticalCenter: parent.verticalCenter;

        width: 24;
        height: 24;

        iconSource: "../../../Icons/" + Style.theme + "/Account.svg";

        onClicked: {
//            console.log("Login button clicked !", thumbnailDecoratorContainer.localSettings);
//            console.log(loginButton.iconSource)
            var source = "AuxComponents/InputDialog.qml";
            var parameters = {};
//            parameters["localSettings"] = thumbnailDecoratorContainer.localSettings;
            thumbnailDecoratorContainer.openDialog(source, parameters);
        }
    }
}

