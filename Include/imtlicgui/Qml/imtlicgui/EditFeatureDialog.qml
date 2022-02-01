import QtQuick 2.0
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
//import '../../../imtgui/Qml/imtgui/AuxComponents'

Rectangle {
    id: container;
    width: 400;
    height: 280;
    radius: 2;
    color: Style.backgroundColor;
    clip: true;
    focus: true;

    property Item resultItem;
    property Item loaderDialog;
    property Item collectionViewFeatures;
    property string featureId;
    property string featureName;
    property real backgroundOpacity: 0.4;
    signal okClicked(string newId, string newName);
    signal cancelClicked();
    property bool backgroundExist: true;
    property bool centered: true;

    Keys.onPressed: {
        console.log("EditFeatureDialog Keys.onPressed", event.key);
        if (event.key === Qt.Key_Tab) {
            console.log("event.key tab", event.key);
        }
    }

//    onFeatureIdChanged: {
//        var msg = container.validateId(container.featureId);

//        errorIdMessage.text = msg;
//    }

    Component.onCompleted: {
        console.log("EditFeatureDialog Component.onCompleted", container.featureId);
//        var msg = container.validateId(container.featureId);

//        errorIdMessage.text = msg;
    }

    function validateId(id) {
        if (id === "") {
            return "Id can't be empty!";
        }

        var dataModelLocal = collectionViewFeatures.model.GetData("data");

        if (id[0] !== "#") {
            id = "#" + id;
        }

        for (var i = 0; i < dataModelLocal.GetItemsCount(); i++) {

            if (dataModelLocal.GetData("Id", i ) === id &&
                    collectionViewFeatures.selectedIndex !== i) {
                return "Id already exist!";
            }
        }

        return "";
    }

    function validateName(name) {
        if (name === "") {
            return "Name can't be empty!";
        }
        return "";
    }

    function generateKey() {
        console.log("EditFeatureDialog generateKey...");
        if (tfcFeatureIdText.text === "" && tfcFeatureNameText.text !== "") {
            var key = "#" + tfcFeatureNameText.text;
            key = key.replace(/\s+/g, '');
            tfcFeatureIdText.text = key;
            console.log("EditFeatureDialog key has been generated!");
        }
    }

    function exit(status) {
        var parameters  = {};
        if (status === "ok") {

            parameters["newFeatureId"] = tfcFeatureIdText.text;
            parameters["newFeatureName"] = tfcFeatureNameText.text;

            parameters["dialog"] = "EditFeature";
            parameters["loaderDialog"] = container.loaderDialog;
        }
        parameters["status"] = status;
        container.resultItem.dialogResult(parameters);
    }

    MouseArea {
        anchors.fill: parent;
    }

    Rectangle {
        id: editFeatureDialogTopPanel;
        width: container.width;
        height: 40;
        border.color: container.color;
        color: Style.baseColor;
        Image {
            id: iconEditFeatureDialog;
            width: 15;
            height: 15;
            anchors.left: editFeatureDialogTopPanel.left;
            anchors.verticalCenter: editFeatureDialogTopPanel.verticalCenter;
            source: "../../../" + "Icons/" + Style.theme + "/" + "Icon" + "_" + "On" + "_" + "Normal" + ".svg";
        }

        Text {
            id: titleEditFeatureDialog;
            anchors.left: iconEditFeatureDialog.right;
            anchors.leftMargin: 10;
            anchors.verticalCenter: editFeatureDialogTopPanel.verticalCenter;
            anchors.horizontalCenter: editFeatureDialogTopPanel.horizontalCenter;
            text: "Edit feature";
            //color: Style.theme == "Dark"? "black": Style.textColor;
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        AuxButton {
            id: editFeatureDialogCloseButton;
            anchors.right: editFeatureDialogTopPanel.right;
            anchors.verticalCenter: editFeatureDialogTopPanel.verticalCenter;
            anchors.rightMargin: 15;
            width: 15;
            height: 15;
            iconSource: "../../../" + "Icons/" + Style.theme + "/Close_On_Normal.svg";

            onClicked: {
                container.exit("close");
                loaderDialog.closeItem();
            }
        }
    }

    Rectangle {
        id: editFeatureDialogBody;
        anchors.topMargin: 20;
        color: container.color;
        anchors.top: editFeatureDialogTopPanel.bottom;
        anchors.horizontalCenter: container.horizontalCenter;
        anchors.verticalCenter: container.verticalCenter;
        width: container.width - 50;
        height: container.height - 50;

        Text {
            id: titleFeatureName;
            text: qsTr("Feature Name");
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        Rectangle {
            id: tfcFeatureName;
            width: editFeatureDialogBody.width;
            height: 45;
            anchors.top: titleFeatureName.bottom;
            color: Style.imagingToolsGradient1;
            border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;

            TextFieldCustom {
                id: tfcFeatureNameText;
                width: tfcFeatureName.width - 22;
                height: 23;
                text: container.featureName;
                focus: true;

                borderColor: errorNameMessage.text !== "" ? Style.errorTextColor : "#00BFFF";
                anchors.horizontalCenter: tfcFeatureName.horizontalCenter;
                anchors.verticalCenter: tfcFeatureName.verticalCenter;

                onTextChanged: {
                    errorNameMessage.text = "";

                    var nameMessage = container.validateName(tfcFeatureNameText.text);
                    if (nameMessage !== "") {
                       errorNameMessage.text = nameMessage;
                    }
                }

            }
        }

        Text {
            id: titleFeatureId;
            anchors.top: tfcFeatureName.bottom;
            anchors.topMargin: 20;
            text: qsTr("Feature - ID");

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        Rectangle {
            id: tfcFeatureId;
            width: editFeatureDialogBody.width;
            height: 45;
            anchors.top: titleFeatureId.bottom;
            color: Style.imagingToolsGradient1;
            border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;

            TextFieldCustom {
                id: tfcFeatureIdText;
                width: tfcFeatureId.width - 22;
                height: 23;
                text: container.featureId;
                anchors.horizontalCenter: tfcFeatureId.horizontalCenter;
                anchors.verticalCenter: tfcFeatureId.verticalCenter;
                borderColor: errorIdMessage.text !== "" ? Style.errorTextColor : "#00BFFF";
//                onInputTextChanged: {
//                    errorIdMessage.text = "";

//                    var idMessage = container.validateId(tfcFeatureIdText.text);
//                    if (idMessage !== "") {
//                       errorIdMessage.text =idMessage;
//                    }
//                }

                onTextChanged: {
                    errorIdMessage.text = "";

                    var idMessage = container.validateId(tfcFeatureIdText.text);
                    if (idMessage !== "") {
                       errorIdMessage.text =idMessage;
                    }
                }

                onFocusChanged: {
                    container.generateKey();
                }

            }
        }

        Rectangle {
            id: okButton;
            anchors.top: tfcFeatureId.bottom;
            anchors.right: cancelButton.left;
            anchors.rightMargin: 15;
            anchors.topMargin: 30;
            width: 70;
            height: 25;
            color: Style.backgroundColor;
            border.color: okButtonMa.containsMouse ? Style.iconColorOnSelected : Style.theme === "Light" ? "#d0d0d2" : "#3a3b3b" ;

            Text {
                anchors.horizontalCenter: parent.horizontalCenter;
                anchors.verticalCenter: parent.verticalCenter;
                color: Style.textColor;
                text: qsTr("ОК");
                font.family: Style.fontFamily;
            }

            MouseArea {
                id: okButtonMa;
                anchors.fill: parent;
                //hoverEnabled: errorIdMessage.text !== "" ? true : false;
                hoverEnabled: true;
                cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;
                visible:tfcFeatureNameText.text !== "" && tfcFeatureIdText.text !== "" && errorIdMessage.text === "" && errorNameMessage.text === "";
                onClicked: {
                    if (tfcFeatureIdText.text[0] !== "#") {
                        tfcFeatureIdText.text = "#" + tfcFeatureIdText.text;
                    }

                    container.okClicked(tfcFeatureIdText.text, tfcFeatureNameText.text);
                    container.exit("ok");
                    loaderDialog.closeItem();
                }
            }
        }

        Rectangle {
            id: cancelButton;
            width: 70;
            height: 25;
            color: Style.backgroundColor;
            border.color: cancelButtonMa.containsMouse ? Style.iconColorOnSelected : Style.theme === "Light" ? "#d0d0d2" : "#3a3b3b" ;
            anchors.top: tfcFeatureId.bottom;
            anchors.topMargin: 30;
            anchors.right: tfcFeatureId.right;


            Text {
                anchors.horizontalCenter: parent.horizontalCenter;
                anchors.verticalCenter: parent.verticalCenter;
                color: Style.textColor;
                font.family: Style.fontFamily;
                text: qsTr("Cancel");
            }

            MouseArea {
                id: cancelButtonMa;
                anchors.fill: parent;
                hoverEnabled: enabled;
                cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;

                onClicked: {
                    container.exit("cancel");
                    loaderDialog.closeItem();
                }
            }
        }

        Text {
            id: errorIdMessage;

            anchors.top: tfcFeatureId.bottom;
            anchors.left: editFeatureDialogBody.left;

            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

            color: Style.errorTextColor;

            text: "";
        }

        Text {
            id: errorNameMessage;

            anchors.top: tfcFeatureName.bottom;
            anchors.left: editFeatureDialogBody.left;

            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

            color: Style.errorTextColor;

            text: "";
        }
    }
}
