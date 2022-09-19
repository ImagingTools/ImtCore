import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

FocusScope {
    id: container;

    height: bodyColumn.height + 40;

    property string titleId;
    property string titleName;

    property alias inputName: inputName.text;
    property alias inputId: inputId.text;

    property alias inputNameField: inputName;
    property alias inputIdField: inputId;

    property bool autoGenerate: false;

    onFocusChanged: {
        console.log("InputBody onFocusChanged", focus);

        if (focus){
            inputName.focus = focus;
        }
    }

    Column {
        id: bodyColumn;

        anchors.verticalCenter: parent.verticalCenter;
        anchors.right: parent.right;
        anchors.left: parent.left;
        anchors.rightMargin: 10;
        anchors.leftMargin: 10;

        width: container.width;

        spacing: 5;

        Text {
            id: titleName;

            text: container.titleName;
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        CustomTextField {
            id: inputName;

            width: bodyColumn.width;
            height: 30;

            KeyNavigation.tab: inputId;

            onAccepted: {
                if (checkValidId(inputId.text)){
                    buttonsDialog.buttonClicked("Ok");
                }
            }
        }

        Text {
            anchors.topMargin: 10;

            text: container.titleId;

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        CustomTextField {
            id: inputId;

            width: bodyColumn.width;
            height: 30;

            onTextInputFocusChanged: {
                if (textInputFocus && autoGenerate && inputId.text == ""){
                    generateKey();
                }
            }

            onTextChanged: {
                let state = checkValidId(inputId.text);
                buttonsDialog.setButtonState("Ok", state);
            }

            onAccepted: {
                if (checkValidId(inputId.text)){
                    buttonsDialog.buttonClicked("Ok");
                }
            }

            KeyNavigation.tab: inputName;
        }

        TreeItemModel {
            id: featureTableHeaders;

            Component.onCompleted: {
                let index = featureTableHeaders.InsertNewItem();
                featureTableHeaders.SetData("Name", "Name", index);
                featureTableHeaders.SetData("Id", "Name", index);
            }
        }

        AuxTable {
            id: featureTable;

            width: bodyColumn.width;
            height: 200;

            headers: featureTableHeaders;
            elements: model;

            itemHeight: 25;

            delegate: Item {

                height: 25;
                width: bodyColumn.width;

                CheckBox {
                    id: checkBox;

                    anchors.left: parent.left;
                    anchors.leftMargin: 10;
                    anchors.verticalCenter: parent.verticalCenter;
                }

                Text {
                    id: title;

                    anchors.left: checkBox.right;
                    anchors.leftMargin: 10;
                    anchors.verticalCenter: parent.verticalCenter;

                    text: model.Name;
                    color: Style.textColor;

                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                }
            }
        }
    }

    function checkValidId(inputId){

        if (inputId == ""){
            return false
        }

        for (let i = 0; i < model.GetItemsCount(); i++){
            let id = model.GetData("Id", i);
            if (id === inputId){
                return false;
            }
        }

        return true;
    }

    function generateKey(){
        console.log("EditDialog generateKey...");
        if (inputName.text !== "") {
            var key = inputName.text;
            key = key.replace(/\s+/g, '');
            inputId.text = key;
        }
    }
}
