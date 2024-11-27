import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

TextInputElementView {
    id: root;

    property alias topValue: doubleValidator.top;
    property alias bottomValue: doubleValidator.bottom;
    property double value
    property int decimalPlaces: 1000

    onAcceptableInputChanged: {
        if (topValue != Number.POSITIVE_INFINITY && bottomValue != Number.NEGATIVE_INFINITY){
            root.bottomComp = acceptableInput ? undefined : errorComp1;
        }
        else if (topValue == Number.POSITIVE_INFINITY && bottomValue != Number.NEGATIVE_INFINITY){
            root.bottomComp = acceptableInput ? undefined : errorComp3;
        }
        else if (topValue != Number.POSITIVE_INFINITY && bottomValue == Number.NEGATIVE_INFINITY){
            root.bottomComp = acceptableInput ? undefined : errorComp2;
        }
    }

    DoubleValidator {
        id: doubleValidator;
        decimals: root.decimalPlaces
    }

    onTextChanged: {
        console.log("!@# OnTextChanged: ", text)

        let decimal = Qt.locale().decimalPoint
        let tempText = text.replace(".", decimal)
        console.log("tempText", tempText)
        if (text != tempText){
            text = tempText

            return
        }
        if (acceptableInput){
            tempText = text.replace(decimal, ".")

            value = Number(tempText)
        }
    }

    onValueChanged: {
        console.log("!@# OnValueChanged: ", value)

        let decimal = Qt.locale().decimalPoint
        let tempText = value.toString()

        text = tempText.replace(".", decimal);
    }

    Component.onCompleted: {
        root.textInputValidator = doubleValidator;
        valueChanged()
    }

    Component {
        id: errorComp1;

        Text {
            text: qsTr("Please enter a value ranging from") + " " + root.bottomValue + " " + qsTr("to") + " " + root.topValue;
            color: Style.errorTextColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }
    }

    Component {
        id: errorComp2;

        Text {
            text: qsTr("Please enter a value less than or equal to") + " " + root.topValue;
            color: Style.errorTextColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }
    }

    Component {
        id: errorComp3;

        Text {
            text: qsTr("Please enter a value greater than or equal to") + " " + root.bottomValue;
            color: Style.errorTextColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }
    }
}


