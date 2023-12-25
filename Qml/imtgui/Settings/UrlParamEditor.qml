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

        CustomTextField {
            id: hostInput;

            width: parent.width;
            height: 30;

            text: model.Host;

            onTextChanged: {
                if (model.Host !== hostInput.text){
                    model.Host = hostInput.text;
                }
            }
        }

        CustomTextField {
            id: portInput;

            width: parent.width;
            height: 30;

            text: model.Port;

            onTextChanged: {
                if (model.Port !== portInput.text){
                    model.Port = portInput.text;
                }
            }
        }
    }
}
