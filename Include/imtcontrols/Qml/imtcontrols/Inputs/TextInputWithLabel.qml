import QtQuick 2.12
import Acf 1.0;

Item {
    id: root;

    width: parent.width;
    height: contentColumn.height;

    property string title;
    property alias text: input.text;
    property alias maximumLength: input.maximumLength;
    property alias borderColor: input.borderColor;
    property alias acceptableInput: input.acceptableInput;
    property alias textInputValidator: input.textInputValidator;
    property alias echoMode: input.echoMode;

    property alias titleFontFamily: title.font.family;

    property string placeHolderText;

    property alias textInput: input;

    property alias spacing: contentColumn.spacing;

    property int radius: 3;

    signal editingFinished();

    onFocusChanged: {
        if (focus){
            input.focus = true;
        }
    }

    Column {
        id: contentColumn;

        width: parent.width;

        spacing: 5;

        Text {
            id: title;

            width: parent.width;

            text: root.title;
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        CustomTextField {
            id: input;

            width: parent.width;
            height: 30;

            radius: root.radius;

            placeHolderText: root.placeHolderText

            borderColor: Style.iconColorOnSelected;

            onEditingFinished: {
                root.editingFinished();
            }

            onTextChanged: {
                root.textChanged();
            }
        }
    }
}

