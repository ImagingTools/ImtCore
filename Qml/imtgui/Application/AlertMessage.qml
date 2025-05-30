import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
    id: alertMessage;

	width: Style.sizeHintL;
	height: Style.controlHeightL;

    color: "#f9dedc";

	property int margin: Style.marginM;
    property string message: "";
    property string colorText: Style.textColor;
    property string iconSource: "../../../" + Style.getIconPath("Icons/Alert", Icon.State.On, Icon.Mode.Normal);

    Image {
        id: icon;

        anchors.left: parent.left;
        anchors.leftMargin: alertMessage.margin;
        anchors.verticalCenter: parent.verticalCenter;

		width: Style.iconSizeM;
		height: width;

        sourceSize.height: height;
        sourceSize.width: width;

        source: alertMessage.iconSource;
    }

    BaseText {
        id: message;

        anchors.left: icon.right;
        anchors.leftMargin: alertMessage.margin;
        anchors.right: parent.right;
        anchors.rightMargin: alertMessage.margin;
        anchors.verticalCenter: parent.verticalCenter;

        text: alertMessage.message;

        color: alertMessage.colorText;
    }
}
