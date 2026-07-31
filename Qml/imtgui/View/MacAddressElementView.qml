import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

TextInputElementView {
    id: rootElement;

    name: qsTr("MAC-Address");
    placeHolderText: qsTr("Enter MAC-Address");

    maximumLength: 17;

    property string prevText: "";
    property bool block: false;

    property MacAddressValidator macAddressValidator: MacAddressValidator {}

    onTextChanged: {
        if (block){
            return;
        }

        if (prevText.length < text.length){
            block = true;

            rootElement.text = macAddressValidator.convert(rootElement.text);

            block = false;
        }

        prevText = text;
    }

    onEditingFinished: {
        rootElement.bottomComp = undefined;

        if (rootElement.text.length == 0){
            rootElement.borderColor = Style.iconColorOnSelected
            rootElement.acceptableInput = true;
        }
        else if (rootElement.text.length < rootElement.maximumLength){
            rootElement.borderColor = Style.errorTextColor;
            rootElement.bottomComp = errorComp1;

            rootElement.acceptableInput = false;
        }
        else{
            let isValid = macAddressValidator.isValid(rootElement.text);

            rootElement.borderColor = isValid ? Style.iconColorOnSelected : Style.errorTextColor;
            rootElement.bottomComp = isValid ? undefined : errorComp2;

            rootElement.acceptableInput = isValid;
        }
    }

    Component {
        id: errorComp1;

        Text {
            id: macAddresInvalidText;

            text: qsTr("MAC Address must be in the format XX:XX:XX:XX:XX:XX");
            color: Style.errorTextColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSizeM;
        }
    }

    Component {
        id: errorComp2;

        Text {
            id: macAddresInvalidText;

            text: qsTr("Only the symbols <a-f> and <0 - 9> may be included");
            color: Style.errorTextColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSizeM;
        }
    }
}


