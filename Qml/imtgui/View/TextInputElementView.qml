import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

ElementView {
    id: root;

    property string text;
    property bool textInputFocus: false;
    property var textInputMask;
    property var textInputValidator;
    property bool readOnly: false;
    property int maximumLength: -1;
    property string placeHolderText;

    signal accepted();
    signal cancelled();
    signal textEdited();
    signal editingFinished();

    onTextInputValidatorChanged: {
        if (textInputValidator && controlItem){
            controlItem.textInputValidator = textInputValidator;
        }
    }

    onMaximumLengthChanged: {
        if (controlItem){
            controlItem.maximumLength = maximumLength;
        }
    }

    onTextChanged: {
        if (controlItem){
            controlItem.text = text;
        }
    }

    controlComp: Component {
        CustomTextField {
            width: 300;
            height: 30;

            text: root.text;

            textInputFocus: root.textInputFocus;
//            textInputMask: root.textInputMask
            readOnly: root.readOnly
            placeHolderText: root.placeHolderText;

            onAccepted: {
                root.accepted();
            }

            onEditingFinished: {
                root.editingFinished();
            }

            onCancelled: {
                root.cancelled();
            }

            onTextEdited: {
                root.textEdited();
            }

            onTextChanged: {
                if(root.text !== text){
                    root.text = text
                }
            }
        }
    }
}


