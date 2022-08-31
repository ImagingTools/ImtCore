import QtQuick 2.0
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0

Item {
    id: settingsDatabaseInputContainer;

    height: databaseInputColumn.height;

    property var parameters: 1; // Start initialize

    onParametersChanged: {
        settingsDatabaseInputRepeater.model = settingsDatabaseInputContainer.parameters;
    }

    onWidthChanged: {
        databaseInputColumn.width = width;
    }

    Column {
        id: databaseInputColumn;

        Repeater {
            id: settingsDatabaseInputRepeater;

            delegate: Item {
                id: databaseInputDelegate;

                width: databaseInputColumn.width;
                height: 60;

                Text {
                    id: settingsDatabaseInputTitle;
                    text: model.Name;

                    font.pixelSize: Style.fontSize_common;
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                }

                onWidthChanged: {
                    settingsDatabaseInputLoader.item.width = databaseInputDelegate.width;
                }

                Loader {
                    id: settingsDatabaseInputLoader;

                    anchors.top: settingsDatabaseInputTitle.bottom;
                    anchors.topMargin: 5;

                    Component.onCompleted: {
                        var componentType = model.ComponentType;

                        if (componentType === "TextInput"){
                            settingsDatabaseInputLoader.source = "SettingsTextInput.qml";
                        }
                        else if (componentType === "IntegerInput"){
                            settingsDatabaseInputLoader.source = "SettingsIntegerInput.qml";
                        }
                    }
                }
            }
        }
    }
}
