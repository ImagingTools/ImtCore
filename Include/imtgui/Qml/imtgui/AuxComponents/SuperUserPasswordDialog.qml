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

        height: columnBody.height + 60;

        property alias inputValue: inputField.text;

        onFocusChanged: {
            if (focus){
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

            Text {
                id: message;

                width: columnBody.width;

                text: inputDialogContainer.message;
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
                elide: Text.ElideRight;

                wrapMode: Text.WordWrap;
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
