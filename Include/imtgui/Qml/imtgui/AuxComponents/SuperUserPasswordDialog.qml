import QtQuick 2.15
import Acf 1.0
import imtgui 1.0

Dialog {
    id: inputDialogContainer;

    topPanelComp: Style.topPanelDialogDecorator !==undefined ? Style.topPanelDialogDecorator: topPanelDefault;

    property string message;
    property string inputValue;

    Component.onCompleted: {
        console.log("InputDialog onCompleted", inputDialogContainer);

        inputDialogContainer.buttons.addButton({"Id":"Ok", "Name":"OK", "Enabled": false, "Active": true});
    }

    onFinished: {
        if (buttonId === "Ok"){
            inputDialogContainer.inputValue = contentItem.inputValue;
        }
    }

    contentComp: Item {
        id: inputDialogBodyContainer;

        height: columnBody.height + 30;

        property alias inputValue: inputField.text;

        onFocusChanged: {
            if (inputDialogBodyContainer.focus){
                inputField.focus = focus;
            }
        }

        Column {
            id: columnBody;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.right: parent.right;
            anchors.left: parent.left;
            anchors.rightMargin: 10;
            anchors.leftMargin: 10;

            spacing: 10;

//            Text {
//                width: columnBody.width;

//                text: qsTr("Database Settings");
//                color: Style.textColor;
//                font.family: Style.fontFamily;
//                font.pixelSize: Style.fontSize_common;
//                elide: Text.ElideRight;

//                wrapMode: Text.WordWrap;
//            }

//            Rectangle {
//                id: databaseBlock;

//                width: parent.width;
//                height: databaseColumn.height + 25;

//                color: "transparent";

//                border.width: 1;
//                border.color: Style.borderColor;

//                Column {
//                    id: databaseColumn;

//                    anchors.horizontalCenter: databaseBlock.horizontalCenter;
//                    anchors.verticalCenter: databaseBlock.verticalCenter;

//                    width: parent.width - 20;

//                    spacing: 7;

//                    Text {
//                        id: databaseNameTitle;

//                        text: qsTr("Database Name");
//                        color: Style.textColor;

//                        font.family: Style.fontFamily;
//                        font.pixelSize: Style.fontSize_common;
//                    }

//                    CustomTextField {
//                        id: countryInput;

//                        height: 30;
//                        width: databaseColumn.width;

//                        onEditingFinished: {

//                        }
//                    }

//                    Text {
//                        id: usernameTitle;

//                        text: qsTr("Username");
//                        color: Style.textColor;

//                        font.family: Style.fontFamily;
//                        font.pixelSize: Style.fontSize_common;
//                    }

//                    CustomTextField {
//                        id: usernameInput;

//                        height: 30;
//                        width: databaseColumn.width;

//                        onEditingFinished: {

//                        }
//                    }

//                    Text {
//                        id: passwordTitle;

//                        text: qsTr("Password");
//                        color: Style.textColor;

//                        font.family: Style.fontFamily;
//                        font.pixelSize: Style.fontSize_common;
//                    }

//                    CustomTextField {
//                        id: passwordInput;

//                        height: 30;
//                        width: databaseColumn.width;

//                        echoMode: TextInput.Password;

//                        onEditingFinished: {

//                        }
//                    }
//                }
//            }

            Text {
                id: message;

                width: columnBody.width;

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;

                elide: Text.ElideRight;
                wrapMode: Text.WordWrap;

                text: inputDialogContainer.message;
            }

            CustomTextField {
                id: inputField;

                anchors.horizontalCenter: parent.horizontalCenter;

                width: columnBody.width;
                height: 30;

                text: inputDialogContainer.inputValue;

                echoMode: TextInput.Password;
                property bool ok: inputField.text !== "";

                onOkChanged: {
                    inputDialogContainer.buttons.setButtonState("Ok", inputField.ok);
                }

                onAccepted: {
                    if(inputField.ok){
                        inputDialogContainer.buttons.buttonClicked("Ok");
                    }
                }

                Loader{
                    id: inputDecoratorLoader;

                    sourceComponent: Style.textFieldDecorator !==undefined ? Style.textFieldDecorator: emptyDecorator;
                    onLoaded: {
                        if(inputDecoratorLoader.item){
                            inputDecoratorLoader.item.rootItem = inputField;
                        }
                    }
                }

                AuxButton {
                    id: eyeButton;

                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.right: parent.right;
                    anchors.rightMargin: 4;

                    height: Math.min(24, parent.height - 10);
                    width: height;

                    highlighted: Style.highlightedButtons !==undefined ? Style.highlightedButtons : containsMouse;

                    iconSource: inputField.echoMode == TextInput.Password ? "../../../Icons/" + Style.theme + "/HiddenPassword.svg" :
                                          inputField.echoMode == TextInput.Normal ? "../../../Icons/" + Style.theme + "/ShownPassword.svg" : "";

                    onClicked: {
                        if(inputField.echoMode == TextInput.Password){
                            inputField.echoMode = TextInput.Normal;
                        }
                        else if(inputField.echoMode == TextInput.Normal){
                            inputField.echoMode = TextInput.Password;
                        }
                    }


                }
            }
        }
    }

    Component{
        id: emptyDecorator;
        Item{
            property Item rootItem;
        }
    }

    Loader{
        id: messageDecoratorLoader;

        sourceComponent: Style.messageDecorator !==undefined ? Style.messageDecorator: emptyDecorator;
        onLoaded: {
            if(messageDecoratorLoader.item){
                messageDecoratorLoader.item.rootItem = inputDialogContainer;
            }

            inputDialogContainer.width = 300;
        }
    }
}
