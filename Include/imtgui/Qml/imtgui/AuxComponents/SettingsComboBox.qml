import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Item {
    id: settingsComboBoxContainer;

    height: 23;

    property var modelElements;

//    property string type;
    property string itemId;
//    property string selectedItemId;

    property int currentItemIndex: -1;

    property Item rootItem;

    onModelElementsChanged: {
        console.log("SettingsComboBox onModelElementsChanged");

        if (!settingsComboBoxContainer.modelElements){
            return;
        }

        for (var i = 0; i < settingsComboBoxContainer.modelElements.GetItemsCount(); i++){
            var id = settingsComboBoxContainer.modelElements.GetData("Id", i);
            var name = settingsComboBoxContainer.modelElements.GetData("Name", i);
            var value = settingsComboBoxContainer.modelElements.GetData("Value", i);

            settingsComboBoxContainer.modelElements.SetData("text", name, i);

            if (value === 1){
                settingsComboBox.currentText = name;
                settingsComboBox.currentIndex = i;
            }
        }
    }

    function dataChanged(){
        console.log("SettingsComboBox dataChanged");

        if (!settingsComboBoxContainer.modelElements || !settingsComboBoxContainer.rootItem){
            return;
        }

        for (var i = 0; i < settingsComboBoxContainer.modelElements.GetItemsCount(); i++){
            if (settingsComboBoxContainer.modelElements.GetData("Value", i) === 1){
                settingsComboBoxContainer.modelElements.SetData("Value", 0, i);
                break;
            }
        }

        settingsComboBoxContainer.modelElements.SetData("Value", 1, settingsComboBox.currentIndex);
        settingsComboBoxContainer.rootItem.dataChanged(settingsComboBoxContainer.currentItemIndex,
                                                                  settingsComboBoxContainer.modelElements);
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
            if (settingsComboBox.currentIndex < 0){
                return;
            }
            settingsComboBoxContainer.dataChanged();
        }
    }
}
