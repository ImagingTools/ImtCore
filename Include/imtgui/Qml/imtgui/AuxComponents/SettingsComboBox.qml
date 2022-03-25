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

    property int currentItemIndex: -1;
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
            var name = settingsComboBoxContainer.modelElements.GetData("Name", i);
            //settingsComboBoxContainer.modelElements.SetData("name", name, i);
            settingsComboBoxModelList.append({'name': name})

            if (i == settingsComboBoxContainer.currentValue){
                settingsComboBox.currentText = name;
            }
        }

        settingsComboBox.currentIndex = settingsComboBoxContainer.currentValue;
    }

    function dataChanged(){
        console.log("SettingsComboBox dataChanged");

        if (!settingsComboBoxContainer.modelElements || !settingsComboBoxContainer.rootItem){
            return;
        }

//        for (var i = 0; i < settingsComboBoxContainer.modelElements.GetItemsCount(); i++){
//            if (settingsComboBoxContainer.modelElements.GetData("Value", i) === 1){
//                settingsComboBoxContainer.modelElements.SetData("Value", 0, i);
//                break;
//            }
//        }

//        settingsComboBoxContainer.modelElements.SetData("Value", 1, settingsComboBox.currentIndex);
        settingsComboBoxContainer.currentValue = settingsComboBox.currentIndex;
        settingsComboBoxContainer.rootItem.dataChanged(settingsComboBoxContainer.currentItemIndex,
                                                       settingsComboBoxContainer.modelElements,
                                                       settingsComboBoxContainer.currentValue);
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
            if (settingsComboBox.currentIndex < 0){
                return;
            }
            settingsComboBoxContainer.dataChanged();
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
