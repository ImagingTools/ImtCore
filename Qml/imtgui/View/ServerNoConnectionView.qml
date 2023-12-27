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

    function onSystemStatusChanged(parameters){
        console.log("onSystemStatusChanged");

        let status = parameters["Status"];
        let message = parameters["Message"];

        console.log("status", status);

        loading.visible = status === "TRY_CONNECTING";
        textNoConnection.text = message;
    }

    Column {
        id: content;

        anchors.centerIn: parent;

        width: parent.width - 100;

        spacing: 10;

        Text {
            id: textNoConnection;

            anchors.horizontalCenter: parent.horizontalCenter;

            horizontalAlignment: Text.AlignHCenter;

            color: Style.textColor;
            font.pixelSize: Style.fontSize_title;
            font.family: Style.fontFamily;

            wrapMode: Text.Wrap;
        }

        Loading {
            id: loading;

            anchors.horizontalCenter: parent.horizontalCenter;

            width: 100;
            height: 100;

            visible: false;
        }
    }

//    BaseButton{
//        id: buttonContainer;

//        anchors.top: textNoConnection.bottom;
//        anchors.topMargin: 20;
//        anchors.horizontalCenter: parent.horizontalCenter;

//        text: qsTr("Refresh");

//        visible: false;

//        decorator: Style.commonButtonDecorator !==undefined ? Style.commonButtonDecorator : defaultButtonDecorator;

//        onClicked: {
//            container.refresh();
//        }
//    }

//    Component{
//        id: defaultButtonDecorator;
//        CommonButtonDecorator{
//        }
//    }
}
