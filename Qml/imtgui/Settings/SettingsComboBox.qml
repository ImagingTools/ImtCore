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

        textCentered: false;

        onCurrentIndexChanged: {
            console.log("onCurrentIndexChanged", settingsComboBoxContainer.modelData);
            let modelValue = settingsComboBoxContainer.modelData.Value;
            console.log("modelValue", modelValue);
             console.log("settingsComboBox.currentIndex", settingsComboBox.currentIndex);
            if (modelValue != settingsComboBox.currentIndex){
                settingsComboBoxContainer.modelData.Value = settingsComboBox.currentIndex;
            }

            console.log("settingsComboBoxContainer.modelData.Value", settingsComboBoxContainer.modelData.Value);
        }
    }
}
