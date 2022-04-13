import QtQuick 2.0
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0

Item {
    id: settingsTextInputContainer;

    property string value;
    property string itemId;

    property Item rootItem;
    property Item repeaterItem;

    onFocusChanged: {
        console.log("SettingsTextInput onFocusChanged", settingsTextInputContainer.focus);
        if (settingsTextInputContainer.focus){
            tfcTextInput.setFocus(true);
        }
    }

    onItemIdChanged: {
        if (settingsTextInputContainer.itemId === "Password"){
            tfcTextInput.echoMode = TextInput.Password;
        }
    }

    Keys.onPressed: {
        console.log("SettingsTextInput keys pressed")
        if (event.key === Qt.Key_Tab){
            console.log('Key tab was pressed');

            tfcTextInput.setFocus(false);
            settingsTextInputContainer.repeaterItem.focusOnNextItem();
        }
    }

    function dataChanged(id, value){
        console.log("SettingsTextInput dataChanged", id, value);

        if (id === "ServerUrl"){
            settingsTextInputContainer.rootItem.dataChanged(0, null , value);
        }
        else{
            settingsTextInputContainer.rootItem.dataChanged(id, value);
        }
    }

    Component.onCompleted: {
        console.log("SettingsTextInput onCompleted", settingsTextInputContainer.itemId);
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
