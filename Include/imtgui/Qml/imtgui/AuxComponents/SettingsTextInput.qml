import QtQuick 2.0
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0

Item {
    id: settingsTextInputContainer;

    property string value;
    property string itemId;

    property Item rootItem;

    function dataChanged(id, value){
        console.log("SettingsTextInput dataChanged", id, value);
        settingsTextInputContainer.rootItem.dataChanged(id, value);
    }

    Component.onCompleted: {
        console.log("SettingsTextInput onCompleted");
        console.log("width", settingsTextInputContainer.width);
    }

    TextFieldCustom {
        id: tfcTextInput;

        width: settingsTextInputContainer.width;
        height: 30;

        text: settingsTextInputContainer.value;

        onInputTextChanged: {
            console.log("SettingsTextInput onInputTextChanged");

            settingsTextInputContainer.dataChanged(settingsTextInputContainer.itemId,
                                                   tfcTextInput.text);
        }
    }
}
