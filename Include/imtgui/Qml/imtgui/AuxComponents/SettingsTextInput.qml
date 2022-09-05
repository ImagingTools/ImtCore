import QtQuick 2.0
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: settingsTextInputContainer;

    signal textInputFocusChanged();

    CustomTextField {
        id: tfcTextInput;

        width: settingsTextInputContainer.width;
        height: 30;

        text: model.Value;

        echoMode: model.Id === "Password" ? TextInput.Password: TextInput.Normal;

        onTextChanged: {
            model.Value = tfcTextInput.text;
        }

        onTextInputFocusChanged: {
            settingsTextInputContainer.textInputFocusChanged();
        }
    }
}
