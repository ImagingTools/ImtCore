import QtQuick 2.0
import Acf 1.0

Item {
    id: settingsTextLabelContainer;

    Text {
        id: text;

        text: model.Value;

        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;
    }
}
