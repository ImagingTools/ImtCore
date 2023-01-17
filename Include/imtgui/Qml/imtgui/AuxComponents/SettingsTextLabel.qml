import QtQuick 2.0
import Acf 1.0

Item {
    id: settingsTextLabelContainer;

    width: 270;
    height: 30;

    Text {
        id: text;

        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;

        text: model.Value;
    }
}
