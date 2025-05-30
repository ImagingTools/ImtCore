import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0;

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

    property alias font: title.font.family;
    property alias titleFontFamily: title.font.family;
    property alias titleFontPixelSize: title.font.pixelSize;

    property string placeHolderText;

    property alias textInput: input;

    property alias spacing: contentColumn.spacing;

	property int radius: Style.radiusS;

    property alias readOnly: input.readOnly;

    signal editingFinished();

    onFocusChanged: {
        if (focus){
            input.focus = true;
        }
    }

    Column {
        id: contentColumn;

        width: parent.width;

		spacing: Style.spacingXS;

        Text {
            id: title;

            width: parent.width;

            text: root.title;
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSizeM;
        }

        CustomTextField {
            id: input;

            width: parent.width;
			height: Style.controlHeightM;

            radius: root.radius;

            placeHolderText: root.placeHolderText

            onEditingFinished: {
                root.editingFinished();
            }

            onTextChanged: {
                root.textChanged();
            }
        }
    }
}

