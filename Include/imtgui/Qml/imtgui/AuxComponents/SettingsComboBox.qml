import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Item {
    id: settingsComboBoxContainer;

    height: 30;

    property var modelElements;

    property string type;
    property string itemId;
    property string selectedItemId;

    property Item rootItem;

    onModelElementsChanged: {
        console.log("SettingsComboBox onModelElementsChanged");
        for (var i = 0; i < settingsComboBoxContainer.modelElements.GetItemsCount(); i++){
            var id = settingsComboBoxContainer.modelElements.GetData("Id", i);
            var name = settingsComboBoxContainer.modelElements.GetData("Name", i);
            var value = settingsComboBoxContainer.modelElements.GetData("Value", i);

            settingsComboBoxContainer.modelElements.SetData("text", name, i);

            if (value === 1){
                settingsComboBox.currentText = name;
                settingsComboBox.currentIndex = i;

                settingsComboBoxContainer.selectedItemId = id;
            }
        }
    }

    function dataChanged(value){
        console.log("SettingsComboBox dataChanged");
        var paramaters = {}

        paramaters["itemId"] = settingsComboBoxContainer.itemId;
        paramaters["selectedId"] = value;
        paramaters["parentModel"] = settingsComboBoxContainer.modelElements.GetParent();

        if (settingsComboBoxContainer.rootItem){
            settingsComboBoxContainer.rootItem.generalSettingsChanged(paramaters);
        }
    }

    ComboBox {
        id: settingsComboBox;

        width: settingsComboBoxContainer.width;
        height: settingsComboBoxContainer.height;

        radius: 3;

        model: settingsComboBoxContainer.modelElements;

        borderColor: Style.theme == "Dark" ? "#565757" : "#a4a4a6";
        textCentered: false;

        onCurrentIndexChanged: {
            var value = settingsComboBoxContainer.modelElements.GetData("Id", settingsComboBox.currentIndex);
            settingsComboBoxContainer.dataChanged(value);
        }
    }
}
