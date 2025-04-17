import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Item {
    property alias iconSource: preferenceButton.iconSource;

    ToolButton {
        id: preferenceButton;

        anchors.verticalCenter: parent ? parent.verticalCenter : undefined;

        width: Style.buttonWidthMedium;
        height: width;

        iconSource: "../../../" + Style.getIconPath("Icons/Settings", "On", "Normal");

        onClicked: {
            Events.sendEvent("ShowPreferencePage");
        }
    }
}

