import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Rectangle {
    id: container;

    color: Style.backgroundColor;

    property string text: qsTr("There is no connection to the server. Please check server url.");

    signal refresh();

    Component.onCompleted: {
        Events.subscribeEvent("SystemStatusChanged", container.onSystemStatusChanged)
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("SystemStatusChanged", container.onSystemStatusChanged)
    }

    function onSystemStatusChanged(status){
        if (status === "TRY_CONNECTING"){
            loading.visible = true;
        }
        else{
            loading.visible = false;
        }
    }

    Text {
        id: textNoConnection;

        anchors.centerIn: parent;

        width: parent.width - 100;

        text: container.text;

        horizontalAlignment: Text.AlignHCenter;

        color: Style.textColor;
        font.pixelSize: Style.fontSize_title;
        font.family: Style.fontFamily;

        wrapMode: Text.Wrap;
    }

    Loading {
        id: loading;

        anchors.fill: textNoConnection;

        visible: false;
    }

    BaseButton{
        id: buttonContainer;

        anchors.top: textNoConnection.bottom;
        anchors.topMargin: 20;
        anchors.horizontalCenter: parent.horizontalCenter;

        text: qsTr("Refresh");

        visible: false;

        decorator: Style.commonButtonDecorator !==undefined ? Style.commonButtonDecorator : defaultButtonDecorator;

        onClicked: {
            container.refresh();
        }
    }

    Component{
        id: defaultButtonDecorator;
        CommonButtonDecorator{
        }
    }
}
