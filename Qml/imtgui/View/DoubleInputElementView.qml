import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

TextInputElementView {
    id: root;

    property alias topValue: doubleValidator.top;
    property alias bottomValue: doubleValidator.bottom;
    property alias decimalPlaces: doubleValidator.decimals;

    property double value
    property bool popupError: false;

    onVisibleChanged: {
        if (!visible){
            ModalDialogManager.closeByComp(errorComp1);
        }
    }

    QtObject {
        id: internal;

        property string errorMessage1: qsTr("Please enter a value ranging from") + " " + root.bottomValue + " " + qsTr("to") + " " + root.topValue;
        property string errorMessage2: qsTr("Please enter a value less than or equal to") + " " + root.topValue;
        property string errorMessage3: qsTr("Please enter a value greater than or equal to") + " " + root.bottomValue;
    }

    onAcceptableInputChanged: {
        if (popupError){
            if (topValue != Number.POSITIVE_INFINITY && bottomValue != Number.NEGATIVE_INFINITY){
                errorText.text = internal.errorMessage1;
            }
            else if (topValue == Number.POSITIVE_INFINITY && bottomValue != Number.NEGATIVE_INFINITY){
                errorText.text = internal.errorMessage2;
            }
            else if (topValue != Number.POSITIVE_INFINITY && bottomValue == Number.NEGATIVE_INFINITY){
                errorText.text = internal.errorMessage3;
            }
        }
        else{
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
    }

    DoubleValidator {
        id: doubleValidator;
    }

    onTextChanged: {
        let decimal = Qt.locale().decimalPoint
        let tempText = text.replace(".", decimal)
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
        let decimal = Qt.locale().decimalPoint
        let tempText = value.toString()

        text = tempText.replace(".", decimal);
    }

    Component.onCompleted: {
        root.textInputValidator = doubleValidator;
        valueChanged()
    }

    PopupView {
        id: errorView;
        x: root.controlItem ? root.width - root.controlItem.width : 0;
        z: root.z + 1;
        width: errorText.width + 2*Style.sizeMainMargin;
        height: 30;
        noMouseArea: true;
        forceFocus: false;
        visible: !root.acceptableInput && root.popupError;

        Rectangle {
            id: background;
            anchors.fill: parent;
            color: Style.baseColor;
            radius: Style.buttonRadius;
            border.width: 1;
            border.color: Style.borderColor;

            Text {
                id: errorText;
                anchors.centerIn: parent;
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSizeNormal;
            }
        }
    }

    Component {
        id: errorComp1;

        Text {
            text: internal.errorMessage1;
            color: Style.errorTextColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSizeNormal;
        }
    }

    Component {
        id: errorComp2;

        Text {
            text: internal.errorMessage2;
            color: Style.errorTextColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSizeNormal;
        }
    }

    Component {
        id: errorComp3;

        Text {
            text: internal.errorMessage3;
            color: Style.errorTextColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSizeNormal;
        }
    }
}


