import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

Item {
    id: settingsIntegerInputContainer;

    width: 270;
    height: 30;

    RegExpValidator {
        id: intValid;

        regExp: /^[1-9]\d*$/;
    }

    CustomTextField {
        id: tfcIntegerInput;

        anchors.fill: parent;

        textInputValidator: intValid;

        text: model.Value;

        onEditingFinished: {
            if (model.Value != tfcIntegerInput.text){
                model.Value = tfcIntegerInput.text;
            }
        }
    }
}
