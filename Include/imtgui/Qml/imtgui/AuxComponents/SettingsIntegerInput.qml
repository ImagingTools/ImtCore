import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

Item {
    id: settingsIntegerInputContainer;

    RegExpValidator {
        id: intValid;

        regExp: /^[1-9]\d*$/;
    }

    CustomTextField {
        id: tfcIntegerInput;

        width: settingsIntegerInputContainer.width;
        height: 30;

        textInputValidator: intValid;

        text: model.Value;

        onTextChanged: {
            console.log("SettingsIntegerInput onInputTextChanged");
            model.Value = tfcIntegerInput.text;
        }
    }
}
