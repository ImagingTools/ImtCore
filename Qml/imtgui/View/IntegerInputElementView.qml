import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

TextInputElementView {
    id: root;

    textInputValidator: intValidator.intRegExp;

    IntValidator {
        id: intValidator;
    }
}


