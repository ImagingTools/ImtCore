import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

DocumentBase {
    id: userEditorContainer;

    commandsDelegatePath: "../../imtlicgui/UserEditorCommandsDelegate.qml"

    UndoRedoManager {
        id: undoRedoManager;

        commandsId: userEditorContainer.commandsId;
        editorItem: userEditorContainer;

        onModelParsed: {
            updateGui();
        }
    }

    Flickable {
        anchors.fill: parent;
        anchors.leftMargin: 20;

        contentWidth: bodyColumn.width;
        contentHeight: bodyColumn.height + 50;

        boundsBehavior: Flickable.StopAtBounds;

        Column {
            id: bodyColumn;

            width: 500;

            spacing: 7;

            Text {
                id: titleUsername;

                text: qsTr("Username");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            RegExpValidator {
                id: regexValid;

                Component.onCompleted: {
                    console.log("RegExpValidator onCompleted");
                    let regex = preferenceDialog.getInstanceMask();

                    let re = new RegExp(regex)
                    if (re){
                        regexValid.regExp = re;
                    }
                }
            }

            CustomTextField {
                id: usernameInput;

                width: parent.width;
                height: 30;

                placeHolderText: qsTr("Enter the Username");

                textInputValidator: regexValid;

                onTextChanged: {
                    documentModel.SetData("Username", usernameInput.text);
                }
            }
            Text {
                id: titleName;

                text: qsTr("Name");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            CustomTextField {
                id: nameInput;

                width: parent.width;
                height: 30;

                placeHolderText: qsTr("Enter the Name");

                textInputValidator: regexValid;

                onTextChanged: {
                    documentModel.SetData("Name", nameInput.text);
                }
            }

            Text {
                id: titleMail;

                text: qsTr("Email address");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            CustomTextField {
                id: mailInput;

                width: parent.width;
                height: 30;

                placeHolderText: qsTr("Enter the Email");

                textInputValidator: regexValid;

                onTextChanged: {
                    documentModel.SetData("Mail", nameInput.text);
                }
            }

        }//Column bodyColumn
    }//Flickable
}//Container
