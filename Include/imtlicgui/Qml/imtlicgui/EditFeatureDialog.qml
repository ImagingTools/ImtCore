import QtQuick 2.0
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
//import '../../../imtgui/Qml/imtgui/AuxComponents'

Rectangle {
    id: containerFeatureEdit;

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

    property bool backgroundExist: true;
    property bool clickBackgroundClose: false;
    property bool centered: true;

    signal okClicked(string newId, string newName);
    signal cancelClicked();

    Component.onCompleted: {
        console.log("EditFeatureDialog onCompleted");
//        tfcFeatureNameText.activeFocusOnPress = true;

//        containerFeatureEdit.forceActiveFocus();
//        tfcFeatureName.forceActiveFocus();

        tfcFeatureNameText.forceActiveFocus();

        if (tfcFeatureNameText.activeFocus){
            console.log("EditFeatureDialog active focus on textinput");
        }
    }

//    Keys.onPressed: {
//        if (event.key === Qt.Key_Tab) {
//            console.log('Key tab was pressed');
//            event.accepted = true;
//        }
//    }

    function validateId(id) {
        if (id === "") {
            return "Id can't be empty!";
        }

        if (!containerFeatureEdit.collectionViewFeatures){
            return "";
        }

        var dataModelLocal = containerFeatureEdit.collectionViewFeatures.collectionViewModel.GetData("data");

        if (id[0] !== "#") {
            id = "#" + id;
        }

        for (var i = 0; i < dataModelLocal.GetItemsCount(); i++) {

            if (dataModelLocal.GetData("Id", i ) === id &&
                    containerFeatureEdit.collectionViewFeatures.selectedIndex !== i) {
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
            parameters["loaderDialog"] = containerFeatureEdit.loaderDialog;
        }
        parameters["dialog"] = "EditFeature";
        parameters["status"] = status;
        containerFeatureEdit.resultItem.dialogResult(parameters);
    }

    MouseArea {
        anchors.fill: parent;
    }

    Rectangle {
        id: editFeatureDialogTopPanel;

        width: containerFeatureEdit.width;
        height: 40;

        border.color: containerFeatureEdit.color;
        color: Style.baseColor;

        Image {
            id: iconEditFeatureDialog;

            anchors.left: editFeatureDialogTopPanel.left;
            anchors.verticalCenter: editFeatureDialogTopPanel.verticalCenter;

            width: 20;
            height: 20;

            source: "../../../" + "Icons/" + Style.theme + "/" + "Icon" + "_" + "On" + "_" + "Normal" + ".svg";
        }

        Text {
            id: titleEditFeatureDialog;

            anchors.left: iconEditFeatureDialog.right;
            anchors.leftMargin: 10;
            anchors.verticalCenter: editFeatureDialogTopPanel.verticalCenter;
            anchors.horizontalCenter: editFeatureDialogTopPanel.horizontalCenter;

            text: "Edit feature";
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_subtitle;
        }

        AuxButton {
            id: editFeatureDialogCloseButton;

            anchors.right: editFeatureDialogTopPanel.right;
            anchors.verticalCenter: editFeatureDialogTopPanel.verticalCenter;
            anchors.rightMargin: 15;

            width: 20;
            height: 20;

            iconSource: "../../../" + "Icons/" + Style.theme + "/Close_On_Normal.svg";

            onClicked: {
                containerFeatureEdit.exit("close");
                loaderDialog.closeItem();
            }
        }
    }

    Rectangle {
        id: editFeatureDialogBody;

        anchors.topMargin: 20;
        anchors.top: editFeatureDialogTopPanel.bottom;
        anchors.horizontalCenter: containerFeatureEdit.horizontalCenter;
        anchors.verticalCenter: containerFeatureEdit.verticalCenter;

        width: containerFeatureEdit.width - 50;
        height: containerFeatureEdit.height - 50;

        color: containerFeatureEdit.color;

        Text {
            id: titleFeatureName;

            text: qsTr("Feature Name");
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        Rectangle {
            id: tfcFeatureName;

            anchors.top: titleFeatureName.bottom;

            width: editFeatureDialogBody.width;
            height: 45;

            color: Style.imagingToolsGradient1;

            border.width: 1;
            border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;

            TextFieldCustom {
                id: tfcFeatureNameText;

                anchors.horizontalCenter: tfcFeatureName.horizontalCenter;
                anchors.verticalCenter: tfcFeatureName.verticalCenter;

                width: tfcFeatureName.width - 22;
                height: 30;

                text: containerFeatureEdit.featureName;
                focus: true;
                correctData: errorNameMessage.text !== "";
                borderColor: errorNameMessage.text !== "" ? Style.errorTextColor : Style.iconColorOnSelected;

                onTextChanged: {
                    errorNameMessage.text = "";

                    var nameMessage = containerFeatureEdit.validateName(tfcFeatureNameText.text);
                    if (nameMessage !== "") {
                       errorNameMessage.text = nameMessage;
                    }
                }
            }
        }

        Text {
            id: titleFeatureId;

            anchors.top: tfcFeatureName.bottom;
            anchors.topMargin: 25;

            text: qsTr("Feature - ID");

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        Rectangle {
            id: tfcFeatureId;

            anchors.top: titleFeatureId.bottom;

            width: editFeatureDialogBody.width;
            height: 45;

            color: Style.imagingToolsGradient1;

            border.width: 1;
            border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;

            TextFieldCustom {
                id: tfcFeatureIdText;

                anchors.horizontalCenter: tfcFeatureId.horizontalCenter;
                anchors.verticalCenter: tfcFeatureId.verticalCenter;

                width: tfcFeatureId.width - 22;
                height: 30;

                text: containerFeatureEdit.featureId;
                borderColor: errorIdMessage.text !== "" ? Style.errorTextColor : Style.iconColorOnSelected;
                correctData: errorIdMessage.text !== "";

                onTextChanged: {

                    errorIdMessage.text = "";

                    var idMessage = containerFeatureEdit.validateId(tfcFeatureIdText.text);
                    if (idMessage !== "") {
                       errorIdMessage.text = idMessage;
                    }
                }

                onFocusChanged: {
                    containerFeatureEdit.generateKey();
                }
            }
        }

        AuxButton {
            id: okButton;

            anchors.top: tfcFeatureId.bottom;
            anchors.right: cancelButton.left;
            anchors.rightMargin: 15;
            anchors.topMargin: 30;

            width: 70;
            height: 25;

            hasText: true;
            hasIcon: false;

            textButton: "OK";
            borderColor: okButton.highlighted ? Style.iconColorOnSelected :
                                              okButton.enabled ? Style.buttonColor :
                                                                 Style.imagingToolsGradient2;
            backgroundColor: okButton.enabled ? Style.imagingToolsGradient1 : Style.backgroundColor;

            enabled: errorIdMessage.text === "" && errorNameMessage.text === "";

            onClicked: {
                if (tfcFeatureIdText.text[0] !== "#") {
                    tfcFeatureIdText.text = "#" + tfcFeatureIdText.text;
                }

                containerFeatureEdit.okClicked(tfcFeatureIdText.text, tfcFeatureNameText.text);
                containerFeatureEdit.exit("ok");
                loaderDialog.closeItem();
            }
        }

        AuxButton {
            id: cancelButton;

            anchors.top: tfcFeatureId.bottom;
            anchors.topMargin: 30;
            anchors.right: tfcFeatureId.right;

            width: 70;
            height: 25;

            hasText: true;
            hasIcon: false;

            textButton: "Cancel";
            borderColor: cancelButton.highlighted ? Style.iconColorOnSelected : Style.buttonColor;
            backgroundColor: Style.imagingToolsGradient1;

            onClicked: {
                containerFeatureEdit.exit("cancel");
                loaderDialog.closeItem();
            }
        }

        Text {
            id: errorIdMessage;

            anchors.top: tfcFeatureId.bottom;
            anchors.topMargin: 3;
            anchors.left: editFeatureDialogBody.left;
            anchors.leftMargin: 10;

            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

            color: Style.errorTextColor;

            text: "";
        }

        Text {
            id: errorNameMessage;

            anchors.top: tfcFeatureName.bottom;
            anchors.topMargin: 3;
            anchors.left: editFeatureDialogBody.left;
            anchors.leftMargin: 10;

            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

            color: Style.errorTextColor;

            text: "";
        }
    }
}
