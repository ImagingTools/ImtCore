import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Item {
    id: settingsComboBoxContainer;

    height: 23;

    property var modelElements;

    property bool active: false;

//    property string type;
    property string itemId;
//    property string selectedItemId;

    property int index: 0;
    property int currentValue: 0;

    property Item rootItem;
    property Item delegateItem;

    onFocusChanged: {
        console.log("SettingsComboBox onFocusChanged", settingsComboBoxContainer.focus)

//        if (!settingsComboBoxContainer.focus){
//            settingsComboBoxContainer.active = false;
//        }
//        else{
//            settingsComboBoxContainer.active = true;
//        }
    }

    Keys.onPressed: {
        console.log("SettingsComboBox keys pressed")
        if (event.key === Qt.Key_Tab){
            console.log('Key tab was pressed');
            settingsComboBoxContainer.delegateItem.focusOnNextItem();
        }
        else if (event.key === Qt.Key_Up){
            console.log('Key up was pressed');
        }
        else if (event.key === Qt.Key_Down){
            console.log('Key down was pressed');
        }
        else if (event.key === Qt.Key_Return){
            console.log('Key return was pressed');
        }
        else if (event.key === Qt.Key_Space){
            console.log('Key space was pressed');
            settingsComboBox.clicked();
        }
    }

    onModelElementsChanged: {
        console.log("SettingsComboBox onModelElementsChanged", settingsComboBox.currentIndex);

        if (!settingsComboBoxContainer.modelElements){
            return;
        }

        for (var i = 0; i < settingsComboBoxContainer.modelElements.GetItemsCount(); i++){
            var id = settingsComboBoxContainer.modelElements.GetData("Id", i);
            var name = settingsComboBoxContainer.modelElements.GetData("Name", i);

            settingsComboBoxModelList.append({'name': name, 'id': id})

            if (i == settingsComboBoxContainer.currentValue){
                settingsComboBox.currentText = name;
            }
        }

        settingsComboBox.currentIndex = settingsComboBoxContainer.currentValue;
    }

    ListModel {
        id: settingsComboBoxModelList;
    }

    ComboBox {
        id: settingsComboBox;

        width: settingsComboBoxContainer.width;
        height: settingsComboBoxContainer.height;

        radius: 3;

        //model: settingsComboBoxContainer.modelElements;
        model: settingsComboBoxModelList;

        borderColor: settingsComboBoxContainer.active ? Style.iconColorOnSelected :
                                                        Style.alternateBaseColor;


        textCentered: false;

        onCurrentIndexChanged: {
            if (settingsComboBoxContainer.rootItem){
                settingsComboBoxContainer.rootItem.dataChanged(settingsComboBoxContainer.index, settingsComboBox.currentIndex);
            }
        }

        onClicked: {
            settingsComboBox.openContextMenu();
        }

        onDialogResultChanged: {
            console.log("SettingsComboBox ComboBox onDialogResultChanged");
            settingsComboBoxContainer.rootItem.forceActiveFocus();
        }
    }
}
