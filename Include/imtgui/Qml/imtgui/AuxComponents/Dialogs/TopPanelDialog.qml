import QtQuick 2.12
import Acf 1.0
import imtgui 1.0


Rectangle {
    id: topPanelDialogContainer;

    width: 300;
    height: 40;

    color: Style.baseColor;

    property string title;
    property bool hasIcon: true;

    signal closeButtonClicked(string buttonId);

    onWidthChanged: {
        console.log("topPanelDialogContainer onWidthChanged", width);
    }

    onHeightChanged: {
        console.log("topPanelDialogContainer onHeightChanged", height);
    }

    Image {
        id: iconDialog;

        anchors.left: topPanelDialogContainer.left;
        anchors.leftMargin: 10;
        anchors.verticalCenter: topPanelDialogContainer.verticalCenter;

        width: 20;
        height: 20;

        sourceSize.height: height;
        sourceSize.width: width;

        source: "../../../../" + "Icons/" + Style.theme + "/LisaIcon.svg";

        visible: topPanelDialogContainer.hasIcon;
    }

    Text {
        id: titleDialog;

        anchors.left: topPanelDialogContainer.hasIcon ? iconDialog.right : parent.left;
        anchors.leftMargin: topPanelDialogContainer.hasIcon ? 10 : 15;
        anchors.verticalCenter: topPanelDialogContainer.verticalCenter;

        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_subtitle;

        text: topPanelDialogContainer.title;
    }

    AuxButton {
        id: closeButton;

        anchors.right: topPanelDialogContainer.right;
        anchors.verticalCenter: topPanelDialogContainer.verticalCenter;
        anchors.rightMargin: 15;

        width: 20;
        height: 20;

        iconSource: "../../../../" + "Icons/" + Style.theme + "/" + "Close" + "_" + "On" + "_" + "Normal" + ".svg";

        onClicked: {
            topPanelDialogContainer.closeButtonClicked("Exit");
        }
    }
}
