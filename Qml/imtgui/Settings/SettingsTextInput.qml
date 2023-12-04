import QtQuick 2.0
import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0

Item {
    id: settingsTextInputContainer;

    width: 270;
    height: 30;

    CustomTextField {
        id: tfcTextInput;

        anchors.fill: parent;

        text: model.Value;

        echoMode: model.Id === "Password" ? TextInput.Password: TextInput.Normal;

        onTextChanged: {
            if (model.Value != tfcTextInput.text){
                model.Value = tfcTextInput.text;
            }
        }
    }
}
