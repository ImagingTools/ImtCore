import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

FocusScope {
    id: container;

    height: bodyColumn.height + 40;

    property string titleId;
    property string titleName;

//    property string valueId;
//    property string valueName;

    property alias inputName: inputName.text;
    property alias inputId: inputId.text;

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

//            text: container.valueName;
            focus: true;

            onTextChanged: {
                container.valueName = inputName.text;
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

//            text: container.valueId;

            onTextChanged: {
                if(checkInputId(inputId.text)){
//                    container.valueId = inputId.text;
                }
                else{

                }
            }
        }
    }

    function checkInputId(inputId){
        for (let i = 0; i < model.GetItemsCount(); i++){
            let id = model.GetData("Id", i);
            if (id === inputId){
                return false;
            }
        }

        return true;
    }
}
