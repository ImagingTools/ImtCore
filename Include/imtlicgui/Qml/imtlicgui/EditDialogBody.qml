import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

FocusScope {
    id: container;

    height: bodyColumn.height + 40;

    property alias treeViewEditor: treeView;

    onFocusChanged: {
        console.log("InputBody onFocusChanged", container.focus);

        if (container.focus){
            inputName.focus = container.focus;
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

            text: editDialogContainer.titleName;
        }

        CustomTextField {
            id: inputName;

            width: bodyColumn.width;
            height: 30;

            text: editDialogContainer.valueName;

            KeyNavigation.tab: inputId;

            onTextChanged: {
//                editDialogContainer.valueName = inputName.text;

                dialogModel.SetData("Name", inputName.text);
            }

            onAccepted: {
//                if (checkValidId(inputId.text)){
//                    editDialogContainer.buttons.buttonClicked('Ok')
//                }
                editDialogContainer.buttons.buttonClicked('Ok')
            }
        }

        Text {
            anchors.topMargin: 10;

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

            text: editDialogContainer.titleId;
        }

        CustomTextField {
            id: inputId;

            width: bodyColumn.width;
            height: 30;

            text: editDialogContainer.valueId;

            onTextInputFocusChanged: {
                if (textInputFocus && inputId.text == ""){
                    container.generateKey();
                }
            }

            onTextChanged: {
                if (inputId.text != valueId){

                    let state = container.checkValidId(inputId.text);
                    if (state){
                        dialogModel.SetData("Id", inputId.text);
                    }

                    editDialogContainer.buttons.setButtonState('Ok', state)
                }
            }

            onAccepted: {
                editDialogContainer.buttons.buttonClicked('Ok')
            }

            KeyNavigation.tab: inputName;
        }

        Text {
            anchors.topMargin: 10;

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

            text: qsTr("Subfeatures");
        }

        TableTreeViewEditor {
            id: treeView;

            width: bodyColumn.width;
            height: 300;

//            modelItems: editDialogContainer.subFeaturesModel;

            clip: true;
        }
    }

    function checkValidId(inputId){
        if (inputId == ""){
            return false
        }

        for (let i = 0; i < featuresModel.GetItemsCount(); i++){
            let id = featuresModel.GetData("Id", i);
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
