import QtQuick 2.0
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: urlEditor;

    width: 270;
    height: content.height;

    Column {
        id: content;

        width: parent.width;

        spacing: 10;

//        BaseText {
//            width: parent.width;
//            text: qsTr("Host");
//        }

        CustomTextField {
            id: hostInput;

            width: parent.width;
            height: 30;

            text: model.Value;

            onTextChanged: {
                console.log("onTextChanged", hostInput.text);
                if (model.Value !== hostInput.text){
                    model.Value = hostInput.text;
                }

                console.log("model.Host", model.Value);
            }
        }

//        BaseText {
//            width: parent.width;
//            text: qsTr("Port");
//        }

//        CustomTextField {
//            id: portInput;

//            width: parent.width;
//            height: 30;

//            text: model.Port;

//            textInputValidator: intValid;

//            onTextChanged: {
//                if (model.Port !== portInput.text){
//                    model.Port = portInput.text;
//                }
//            }
//        }

//        RegExpValidator {
//            id: intValid;

//            regExp: /^[1-9]\d*$/;
//        }
    }
}
