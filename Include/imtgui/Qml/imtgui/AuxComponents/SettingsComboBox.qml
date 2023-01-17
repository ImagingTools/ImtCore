import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: settingsComboBoxContainer;

    width: 150;
    height: 23;

    property var modelData: model;

    property var parameters: null;

    property bool active: false;

    onParametersChanged: {
        console.log("onParametersChanged", settingsComboBoxContainer.parameters);
        settingsComboBox.model = settingsComboBoxContainer.parameters;
        settingsComboBox.currentIndex = modelData.Value;
    }

    ComboBox {
        id: settingsComboBox;

        width: settingsComboBoxContainer.width;
        height: settingsComboBoxContainer.height;

        radius: 3;

        borderColor: settingsComboBoxContainer.active ? Style.iconColorOnSelected :
                                                        Style.alternateBaseColor;

        textCentered: false;

        onCurrentIndexChanged: {
            let modelValue = settingsComboBoxContainer.modelData.Value;
            if (modelValue != settingsComboBox.currentIndex){
                settingsComboBoxContainer.modelData.Value = settingsComboBox.currentIndex;
            }
        }
    }
}
