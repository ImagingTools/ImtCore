import QtQuick 2.0
import Acf 1.0
import imtcontrols 1.0

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

            text: model.Value;

            onTextChanged: {
                console.log("onTextChanged", hostInput.text);
                if (model.Value !== hostInput.text){
                    model.Value = hostInput.text;
                }

                console.log("model.Host", model.Value);
            }
        }
    }
}
