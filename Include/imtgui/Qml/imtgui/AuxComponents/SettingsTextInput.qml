import QtQuick 2.0
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: settingsTextInputContainer;

    property string value;
    property string itemId;

    property Item rootItem;
    property Item repeaterItem;

    property int index: 0; // index element in the data model, default - 0

    onItemIdChanged: {
        if (settingsTextInputContainer.itemId === "Password"){
            tfcTextInput.echoMode = TextInput.Password;
        }
    }

    Keys.onPressed: {
        console.log("SettingsTextInput keys pressed")
        if (event.key === Qt.Key_Tab){
            console.log('Key tab was pressed');

            //tfcTextInput.setFocus(false);
            //settingsTextInputContainer.repeaterItem.focusOnNextItem();
        }
    }

    Component.onCompleted: {
        console.log("SettingsTextInput onCompleted", settingsTextInputContainer.itemId);
    }

    CustomTextField {
        id: tfcTextInput;

        width: settingsTextInputContainer.width;
        height: 30;

        text: settingsTextInputContainer.value;

        onTextChanged: {
            console.log("SettingsTextInput onInputTextChanged");
            if (settingsTextInputContainer.rootItem){
                settingsTextInputContainer.rootItem.dataChanged(settingsTextInputContainer.index, tfcTextInput.text);
            }
        }
    }
}
