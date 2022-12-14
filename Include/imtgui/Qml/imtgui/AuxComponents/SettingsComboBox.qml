import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: settingsComboBoxContainer;

    width: 150;
    height: 23;

    property var parameters: null;

    property bool active: false;

    onParametersChanged: {
        console.log("onParametersChanged", parameters);
        settingsComboBox.model = parameters;
        settingsComboBox.currentIndex = model.Value;
    }

    ListModel {
        id: settingsComboBoxModelList;
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
            parent.currentValueChanged();
        }
    }

    function currentValueChanged(){
        model.Value = settingsComboBox.currentIndex;
    }
}
