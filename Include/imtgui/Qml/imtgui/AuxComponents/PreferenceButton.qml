import QtQuick 2.12
import Acf 1.0

Item {
    property alias iconSource: preferenceButton.iconSource;
    property alias color: preferenceButton.color;
    property alias highlighted: preferenceButton.highlighted;

    AuxButton {
        id: preferenceButton;
        anchors.verticalCenter: parent.verticalCenter;

        width: Style.buttonWidthMedium;
        height: width;

        iconSource: "../../../" + Style.getIconPath("Icons/Settings", "On", "Normal");

        onClicked: {
            console.log("Preference button clicked !");
            Events.sendEvent("ShowPreferencePage");
        }
    }
}

