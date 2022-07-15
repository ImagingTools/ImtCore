import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0


Rectangle {
    id: topPanelDialogContainer;

    width: 300;
    height: 40;

    color: Style.baseColor;

    property string title;

    signal closeButtonClicked();

    Image {
        id: iconDialog;

        anchors.left: topPanelDialogContainer.left;
        anchors.leftMargin: 10;
        anchors.verticalCenter: topPanelDialogContainer.verticalCenter;

        width: 20;
        height: 20;

        sourceSize.height: height;
        sourceSize.width: width;

        source: "../../../" + "Icons/" + Style.theme + "/LisaIcon.svg";
    }

    Text {
        id: titleDialog;

        anchors.left: iconDialog.right;
        anchors.leftMargin: 10;
        anchors.verticalCenter: topPanelDialogContainer.verticalCenter;

        text: topPanelDialogContainer.title;
        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_subtitle;
    }

    AuxButton {
        id: closeButton;

        anchors.right: topPanelDialogContainer.right;
        anchors.verticalCenter: topPanelDialogContainer.verticalCenter;
        anchors.rightMargin: 15;

        width: 20;
        height: 20;

        iconSource: "../../../" + "Icons/" + Style.theme + "/" + "Close" + "_" + "On" + "_" + "Normal" + ".svg";

        onClicked: {
            topPanelDialogContainer.closeButtonClicked();
        }
    }
}
