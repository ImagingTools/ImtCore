import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

Item {
    id: settingsIntegerInputContainer;

    property string value;
    property string itemId;

    property Item rootItem;
    property Item repeaterItem;

    property int index: 0; // index element in the data model, default - 0

    onFocusChanged: {
        if (settingsIntegerInputContainer.focus){
//            tfcIntegerInput.setFocus(true);
        }
    }

    Keys.onPressed: {
        console.log("SettingsIntegerInput keys pressed")
        if (event.key === Qt.Key_Tab){
            console.log('Key tab was pressed');

//            tfcIntegerInput.setFocus(false);
//            settingsIntegerInputContainer.repeaterItem.focusOnNextItem();
        }
    }

    Component.onCompleted: {
        console.log("SettingsIntegerInput onCompleted");
        console.log("width", settingsIntegerInputContainer.width);
    }

    CustomTextField {
        id: tfcIntegerInput;

        width: settingsIntegerInputContainer.width;
        height: 30;

        text: settingsIntegerInputContainer.value;

        onTextChanged: {
            console.log("SettingsIntegerInput onInputTextChanged");
            if (settingsIntegerInputContainer.rootItem){
                settingsIntegerInputContainer.rootItem.dataChanged(settingsIntegerInputContainer.index, tfcIntegerInput.text);
            }
        }
    }
}
