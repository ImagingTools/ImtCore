import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

TextInputElementView {
    id: root;

    property alias topValue: intValidator.top;
    property alias bottomValue: intValidator.bottom;

    IntValidator {
        id: intValidator;
    }

    Component.onCompleted: {
        root.textInputValidator = intValidator;
    }
}


