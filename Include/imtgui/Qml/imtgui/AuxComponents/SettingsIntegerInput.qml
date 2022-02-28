import QtQuick 2.0
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0

Item {
    id: settingsIntegerInputContainer;

    property string value;
    property string itemId;

    property Item rootItem;

    function dataChanged(id, value){
        console.log("SettingsIntegerInput dataChanged", id, value);
        settingsIntegerInputContainer.rootItem.dataChanged(id, value);
    }

    Component.onCompleted: {
        console.log("SettingsIntegerInput onCompleted");
        console.log("width", settingsIntegerInputContainer.width);
    }

    TextFieldCustom {
        id: tfcIntegerInput;

        width: settingsIntegerInputContainer.width;
        height: 30;

        text: settingsIntegerInputContainer.value;

        onInputTextChanged: {
            console.log("SettingsIntegerInput onInputTextChanged");

            settingsIntegerInputContainer.dataChanged(settingsIntegerInputContainer.itemId,
                                                   tfcIntegerInput.text);
        }
    }
}
