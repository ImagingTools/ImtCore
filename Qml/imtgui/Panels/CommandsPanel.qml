import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0

Item {
    id: commandsPanelItem;
    height: commandsPanelLoader.height + 2 * Style.size_mainMargin;

    property alias commandsSourceComp: commandsPanelLoader.sourceComponent;

    signal loaded(var item);

    Loader {
        id: commandsPanelLoader;
        anchors.verticalCenter: parent.verticalCenter;
        width: parent.width;
        onLoaded: {
            commandsPanelItem.loaded(item);
        }
    }

    Rectangle {
        anchors.bottom: parent.bottom;
        width: parent.width;
        height: 1;
        color: Style.borderColor;
    }
}
