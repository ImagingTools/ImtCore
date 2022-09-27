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
                if (textInputFocus && autoGenerate && inputId.text == ""){
                    generateKey();
                }
            }

            onTextChanged: {
                let state = checkValidId(inputId.text);
//                editDialogContainer.buttons.setButtonState('Ok', state)
            }

            onAccepted: {
//                if (checkValidId(inputId.text)){
//                    editDialogContainer.buttons.buttonClicked('Ok')
//                }
                editDialogContainer.buttons.buttonClicked('Ok')
            }

            KeyNavigation.tab: inputName;
        }

        Text {
            anchors.topMargin: 10;

            text: qsTr("Subfeatures");

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        TableTreeViewEditor {
            id: treeView;

            width: bodyColumn.width;
            height: 200;

            modelItems: editDialogContainer.subFeaturesModel;

            clip: true;
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
