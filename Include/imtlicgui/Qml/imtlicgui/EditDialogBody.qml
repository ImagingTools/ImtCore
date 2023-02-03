import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

FocusScope {
    id: container;

    height: bodyColumn.height + 40;

    property alias treeViewEditor: treeView;

    property Item rootItem: null;

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

            text: container.rootItem ? container.rootItem.titleName : "";
        }

        CustomTextField {
            id: inputName;

            width: bodyColumn.width;
            height: 30;

            text: container.rootItem ? container.rootItem.valueName : "";

            KeyNavigation.tab: inputId;

            onTextChanged: {
//                editDialogContainer.valueName = inputName.text;

                container.rootItem.dialogModel.SetData("Name", inputName.text);
            }

            onAccepted: {
//                if (checkValidId(inputId.text)){
//                    editDialogContainer.buttons.buttonClicked('Ok')
//                }
                container.rootItem.buttons.buttonClicked('Ok');
            }
        }

        Text {
            anchors.topMargin: 10;

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

            text: container.rootItem ? container.rootItem.titleId : "";
        }

        CustomTextField {
            id: inputId;

            width: bodyColumn.width;
            height: 30;

            text: container.rootItem ? container.rootItem.valueId : "";

            onTextInputFocusChanged: {
                if (textInputFocus && inputId.text == ""){
                    container.generateKey();
                }
            }

            onTextChanged: {
                if (container.rootItem && inputId.text != container.rootItem.valueId){

                    let state = container.checkValidId(inputId.text);
                    if (state){
                        container.rootItem.dialogModel.SetData("Id", inputId.text);
                    }

                    container.rootItem.buttons.setButtonState('Ok', state)
                }
            }

            onAccepted: {
                container.rootItem.buttons.buttonClicked('Ok')
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

        for (let i = 0; i < container.rootItem.featuresModel.GetItemsCount(); i++){
            let id = container.rootItem.featuresModel.GetData("Id", i);
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
