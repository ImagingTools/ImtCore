import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

FocusScope {
    id: container;

    height: bodyColumn.height + 40;

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

            text: editDialogContainer.titleName;

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        CustomTextField {
            id: inputName;

            width: bodyColumn.width;
            height: 30;

            text: editDialogContainer.valueName;

            KeyNavigation.tab: inputId;

            onTextChanged: {
                editDialogContainer.valueName = inputName.text;
            }

            onAccepted: {
                if (checkValidId(inputId.text)){
                    editDialogContainer.buttons.buttonClicked('Ok')
                }
            }
        }

        Text {
            anchors.topMargin: 10;

            text: editDialogContainer.titleId;

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        CustomTextField {
            id: inputId;

            width: bodyColumn.width;
            height: 30;

            text: editDialogContainer.valueId;

            onTextInputFocusChanged: {
                if (textInputFocus && inputId.text == ""){
                    generateKey();
                }
            }

            onTextChanged: {
                let flag = checkValidId(inputId.text);

                console.log("flag", flag);

                editDialogContainer.buttons.setButtonState("Ok", flag);

                if (flag){
                    editDialogContainer.valueId = inputId.text;
                }
            }

            onAccepted: {
                editDialogContainer.buttons.buttonClicked('Ok')
            }

            KeyNavigation.tab: inputName;
        }
    }

    function checkValidId(inputId){
        if (inputId == "" || !editDialogContainer.licensesModel){
            return false
        }

        for (let i = 0; i < editDialogContainer.licensesModel.GetItemsCount(); i++){
            let id = editDialogContainer.licensesModel.GetData("Id", i);
            if (id === inputId){
                return false;
            }
        }

        return true;
    }

    function generateKey(){
        inputId.text = "00.0001"
    }
}
