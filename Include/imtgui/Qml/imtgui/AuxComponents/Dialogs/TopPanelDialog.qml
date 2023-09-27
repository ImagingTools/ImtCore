import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Rectangle {
    id: topPanelDialogContainer;

    width: 300;
    height: 40;

    color: Style.baseColor;

    property string title;
    property bool hasIcon: true;

    signal closeButtonClicked(string buttonId);

    Image {
        id: iconDialog;

        anchors.left: topPanelDialogContainer.left;
        anchors.leftMargin: 10;
        anchors.verticalCenter: topPanelDialogContainer.verticalCenter;

        width: 20;
        height: 20;

        sourceSize.height: height;
        sourceSize.width: width;

        source: "../../../../" + Style.getIconPath("Icons/AppIcon", Icon.State.On, Icon.Mode.Normal)

        visible: topPanelDialogContainer.hasIcon;
    }

    Text {
        id: titleDialog;

        anchors.left: topPanelDialogContainer.hasIcon ? iconDialog.right : parent.left;
        anchors.leftMargin: topPanelDialogContainer.hasIcon ? 10 : 15;
        anchors.verticalCenter: topPanelDialogContainer.verticalCenter;
        anchors.right: closeButton.left;
        anchors.rightMargin: 10;

        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_subtitle;

        text: topPanelDialogContainer.title;

        elide: Text.ElideRight;
    }

    AuxButton {
        id: closeButton;

        anchors.right: topPanelDialogContainer.right;
        anchors.verticalCenter: topPanelDialogContainer.verticalCenter;
        anchors.rightMargin: 15;

        width: 20;
        height: 20;

        iconSource: "../../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);

        onClicked: {
            topPanelDialogContainer.closeButtonClicked("Exit");
        }
    }
}
