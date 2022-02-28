import QtQuick 2.0
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0

Item {
    id: settingsDatabaseInputContainer;

    property string value;
    property string itemId;

    property Item rootItem;

    property var modelDatabase;

    Component.onCompleted: {
        console.log("SettingsDatabaseInput onCompleted");
        console.log("width", settingsDatabaseInputContainer.width);
    }

    onModelDatabaseChanged: {
        settingsDatabaseInputRepeater.model = settingsDatabaseInputContainer.modelDatabase;
    }

    function dataChanged(id, value){
        console.log("DatabaseSettingsInput dataChanged", id, value);

        var parameters = {}

        parameters["dbId"] = settingsDatabaseInputContainer.itemId;
        parameters["itemId"] = id;
        parameters["itemValue"] = value;

        settingsDatabaseInputContainer.rootItem.databaseDataChanged(parameters);
    }

    Column {
        width: settingsDatabaseInputContainer.width;

        Repeater {
            id: settingsDatabaseInputRepeater;

            delegate: Item {
                id: databaseInputDelegate;

                width: settingsDatabaseInputContainer.width;
                height: 60;

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
                    }
                }
            }
        }
    }
}
