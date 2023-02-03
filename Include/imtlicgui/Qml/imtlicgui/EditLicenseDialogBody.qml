import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

FocusScope {
    id: container;

    height: bodyColumn.height + 40;

    property Item rootItem: null;

    onFocusChanged: {
        console.log("InputBody onFocusChanged", container.focus);

        if (container.focus){
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

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

            text: container.rootItem ? container.rootItem.titleName : "";
        }

        CustomTextField {
            id: inputName;

            width: bodyColumn.width;
            height: 30;

            text: container.rootItem ? container.rootItem.valueName: "";

            KeyNavigation.tab: inputId;

            onTextChanged: {
                container.rootItem.valueName = inputName.text;
            }

            onAccepted: {
                if (container.checkValidId(inputId.text)){
                    container.rootItem.buttons.buttonClicked('Ok')
                }
            }
        }

        Text {
            anchors.topMargin: 10;

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

            text: container.rootItem ? container.rootItem.titleId: "";
        }

        CustomTextField {
            id: inputId;

            width: bodyColumn.width;
            height: 30;

            text: container.rootItem ? container.rootItem.valueId : "";

            onTextInputFocusChanged: {
                if (inputIdtextInputFocus && inputId.text == ""){
                    container.generateKey();
                }
            }

            onTextChanged: {
                let flag = container.checkValidId(inputId.text);

                console.log("flag", flag);

                container.rootItem.buttons.setButtonState("Ok", flag);

                if (flag){
                    container.rootItem.valueId = inputId.text;
                }
            }

            onAccepted: {
                container.rootItem.buttons.buttonClicked('Ok')
            }

            KeyNavigation.tab: inputName;
        }
    }

    function checkValidId(inputId){
        if (inputId == "" || !container.rootItem.licensesModel){
            return false
        }

        for (let i = 0; i < container.rootItem.licensesModel.GetItemsCount(); i++){
            let id = container.rootItem.licensesModel.GetData("Id", i);
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
