import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

TextInputElementView {
    id: root;

    DoubleValidator {
        id: doubleValidator;
    }

    Component.onCompleted: {
        root.textInputValidator = doubleValidator;
    }
}


