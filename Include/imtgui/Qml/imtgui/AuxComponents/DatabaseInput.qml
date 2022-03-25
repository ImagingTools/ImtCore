import QtQuick 2.0
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0

Item {
    id: settingsDatabaseInputContainer;

//    property string value;
    property string itemId;

    property Item rootItem;
    property Item delegateItem;

    property var modelDatabase;

    property int currentItemIndex: -1;

    property bool active: false;

    onFocusChanged: {
        if (settingsDatabaseInputContainer.focus){
            settingsDatabaseInputRepeater.focusOnNextItem();
        }
    }

    Component.onCompleted: {
        console.log("SettingsDatabaseInput onCompleted");
        console.log("width", settingsDatabaseInputContainer.width);
    }

    onModelDatabaseChanged: {
        settingsDatabaseInputRepeater.model = settingsDatabaseInputContainer.modelDatabase;
    }

    function dataChanged(id, value){
        console.log("DatabaseSettingsInput dataChanged", id, value);

        if (!settingsDatabaseInputContainer.modelDatabase){
            return;
        }

        for (var i = 0; i < settingsDatabaseInputContainer.modelDatabase.GetItemsCount(); i++){
            var curId = settingsDatabaseInputContainer.modelDatabase.GetData("Id", i);

            if (curId === id){
                settingsDatabaseInputContainer.modelDatabase.SetData("Value", value, i);
            }
        }

        settingsDatabaseInputContainer.rootItem.dataChanged(settingsDatabaseInputContainer.currentItemIndex,
                                                                    settingsDatabaseInputContainer.modelDatabase);
    }

    Column {
        id: databaseInputColumn;

        width: settingsDatabaseInputContainer.width;

        property int currentFocusIndex: -1;
        property Item activeItem;

        Repeater {
            id: settingsDatabaseInputRepeater;

            function focusOnNextItem(){
                console.log("DatabaseInput Repeater focusOnNextItem");

                databaseInputColumn.currentFocusIndex++;

                if (databaseInputColumn.currentFocusIndex == settingsDatabaseInputRepeater.count){
                    databaseInputColumn.currentFocusIndex = -1;
                    settingsDatabaseInputContainer.rootItem.focusOnButtons();
                }
                else{
                    databaseInputColumn.activeItem = settingsDatabaseInputRepeater.itemAt(databaseInputColumn.currentFocusIndex);
                    databaseInputColumn.activeItem.setFocus(true);
                }
            }

            delegate: Item {
                id: databaseInputDelegate;

                width: settingsDatabaseInputContainer.width;
                height: 60;

                function setFocus(state){
                    console.log("DatabaseInput Item setFocus");
                    if (state === true){
                       settingsDatabaseInputLoader.item.forceActiveFocus();
                    }
                }

                Text {
                    id: settingsDatabaseInputTitle;
                    text: model.Name;

                    font.pixelSize: Style.fontSize_common;
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                }

                Loader {
                    id: settingsDatabaseInputLoader;

                    anchors.top: settingsDatabaseInputTitle.bottom;
                    anchors.topMargin: 5;

                    Component.onCompleted: {
                        var componentType = model.ComponentType;
                        var itemId = model.Id;

                        var value = model.Value;

                        if (componentType === "TextInput"){
                            settingsDatabaseInputLoader.source = "SettingsTextInput.qml";
                        }
                        else if (componentType === "IntegerInput"){
                            settingsDatabaseInputLoader.source = "SettingsIntegerInput.qml";
                        }

                        settingsDatabaseInputLoader.item.itemId = itemId;
                        settingsDatabaseInputLoader.item.width = databaseInputDelegate.width;
                        settingsDatabaseInputLoader.item.value = value;
                        settingsDatabaseInputLoader.item.rootItem = settingsDatabaseInputContainer;
                        settingsDatabaseInputLoader.item.repeaterItem = settingsDatabaseInputRepeater;
                    }
                }
            }
        }
    }
}
