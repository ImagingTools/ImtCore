import QtQuick 2.0
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0

Rectangle {
    id: containerFeatureEdit;

    width: 400;
    height: 280;

    radius: 2;
    color: Style.backgroundColor;
    clip: true;

    property Item resultItem;
    property Item loaderDialog;
    property Item collectionViewFeatures;
    property Item thumbnailItem;

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
    }

    onFocusChanged: {
        console.log("EditFeatureDialog onFocusChanged", containerFeatureEdit.focus);

        if (containerFeatureEdit.focus){
            tfcFeatureNameText.setFocus(true);
        }
    }

    Keys.onPressed: {
        console.log("EditFeatureDialog keys pressed")
        if (event.key === Qt.Key_Escape){
            console.log('Key tab was pressed');
            cancelButton.clicked();
        }
    }

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
        containerFeatureEdit.resultItem.forceActiveFocus();
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
            border.color: Style.borderColor;

            onFocusChanged: {
                console.log("Rectangle tfcFeatureName onFocusChanged", tfcFeatureName.focus);

                if (tfcFeatureName.focus){
                    tfcFeatureNameText.setFocus(true);
                }
            }

            TextFieldCustom {
                id: tfcFeatureNameText;

                anchors.horizontalCenter: tfcFeatureName.horizontalCenter;
                anchors.verticalCenter: tfcFeatureName.verticalCenter;

                width: tfcFeatureName.width - 22;
                height: 30;

                text: containerFeatureEdit.featureName;

                correctData: errorNameMessage.text !== "";
                borderColor: errorNameMessage.text !== "" ? Style.errorTextColor : Style.iconColorOnSelected;

                onTextChanged: {
                    errorNameMessage.text = "";

                    var nameMessage = containerFeatureEdit.validateName(tfcFeatureNameText.text);
                    if (nameMessage !== "") {
                       errorNameMessage.text = nameMessage;
                    }
                }

                KeyNavigation.tab: tfcFeatureId;
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
            border.color: Style.borderColor;

            onFocusChanged: {
                console.log("Rectangle tfcFeatureId onFocusChanged");
                tfcFeatureIdText.setFocus(true);
            }

//            KeyNavigation.tab: tfcFeatureName;

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

//                KeyNavigation.tab: tfcFeatureName;
                KeyNavigation.tab: okButton;
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

            KeyNavigation.tab: cancelButton;

            onClicked: {
                if (tfcFeatureIdText.text[0] !== "#") {
                    tfcFeatureIdText.text = "#" + tfcFeatureIdText.text;
                }

                containerFeatureEdit.okClicked(tfcFeatureIdText.text, tfcFeatureNameText.text);
                containerFeatureEdit.exit("ok");
                loaderDialog.closeItem();
            }

            onFocusChanged: {
                console.log("EditFeatureDialog okButton onFocusChanged");

                if (okButton.focus){
                    okButton.highlighted = true;
                }
                else{
                    okButton.highlighted = false;
                }
            }

            Keys.onPressed: {
                console.log("AuxButton okButton Key pressed!")
                if(event.key === Qt.Key_Return){
                    okButton.clicked();
//                    console.log("key_enter");
//                    if (tfcFeatureIdText.text[0] !== "#") {
//                        tfcFeatureIdText.text = "#" + tfcFeatureIdText.text;
//                    }

//                    containerFeatureEdit.okClicked(tfcFeatureIdText.text, tfcFeatureNameText.text);
//                    containerFeatureEdit.exit("ok");
//                    loaderDialog.closeItem();
                }
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

            KeyNavigation.tab: tfcFeatureName;

            onClicked: {
                containerFeatureEdit.exit("cancel");
                loaderDialog.closeItem();
                containerFeatureEdit.resultItem.forceActiveFocus();
            }

            onFocusChanged: {
                console.log("EditFeatureDialog cancelButton onFocusChanged");

                if (cancelButton.focus){
                    cancelButton.highlighted = true;
                }
                else{
                    cancelButton.highlighted = false;
                }
            }

            Keys.onPressed: {
                console.log("AuxButton cancelButton Key pressed!")
                if(event.key === Qt.Key_Return) {
//                    containerFeatureEdit.exit("cancel");
//                    loaderDialog.closeItem();
//                    containerFeatureEdit.resultItem.forceActiveFocus();
                    cancelButton.clicked();
                }
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
