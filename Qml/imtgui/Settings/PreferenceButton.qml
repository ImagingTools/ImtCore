import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Item {
    property alias iconSource: preferenceButton.iconSource;

    Button {
        id: preferenceButton;

        anchors.verticalCenter: parent ? parent.verticalCenter : undefined;

        width: Style.buttonWidthMedium;
        height: width;

        iconSource: "../../../" + Style.getIconPath("Icons/Settings", "On", "Normal");

        onClicked: {
            console.log("Preference button clicked !");
            Events.sendEvent("ShowPreferencePage");
        }
    }
}

