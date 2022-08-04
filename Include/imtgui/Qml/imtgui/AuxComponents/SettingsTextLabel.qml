import QtQuick 2.0
import Acf 1.0

Item {
    id: settingsTextLabelContainer;

    property string value;
    property string itemId;

    property Item rootItem;
    property Item repeaterItem;

    property int index: 0; // index element in the data model, default - 0

    Component.onCompleted: {

    }

    Text {
        id: text;

        text:settingsTextLabelContainer.value;

        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;
    }
}
