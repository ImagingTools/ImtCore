import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
    id: settingsIntegerInputContainer;

    width: 270;
    height: 30;

    RegularExpressionValidator {
        id: intValid;

        regularExpression: /^[1-9]\d*$/;
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
