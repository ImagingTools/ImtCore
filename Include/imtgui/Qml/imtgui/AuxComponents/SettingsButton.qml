import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Item {
    id: settingsButtonContainer;

    height: 25;

    property string textButton;
    property string itemId;
    property string value;

    property Item rootItem;

    AuxButton {
        id: button;

        width: settingsButtonContainer.width;
        height: settingsButtonContainer.height;

        hasText: true;
        hasIcon: false;

        textButton: settingsButtonContainer.value;
        borderColor: (button.highlighted || button.focus) ? Style.iconColorOnSelected : Style.buttonColor;
        backgroundColor: Style.imagingToolsGradient1;

        onClicked: {
            settingsButtonContainer.rootItem.openFileDialog();
        }
    }
}
