import QtQuick 2.0
import Acf 1.0

Item {
    id: settingsDatabaseInputContainer;

    width: 225;
    height: databaseInputColumn.height;

    property var parameters: null;

    onParametersChanged: {
        settingsDatabaseInputRepeater.model = settingsDatabaseInputContainer.parameters;
    }

    Component.onCompleted: {
        console.log("databaseInputDelegate onCompleted");
    }

    Column {
        id: databaseInputColumn;

        width: settingsDatabaseInputContainer.width;

        spacing: 10;

        Repeater {
            id: settingsDatabaseInputRepeater;

            delegate: Item {
                id: databaseInputDelegate;

                width: databaseInputColumn.width;
                height: 60;

                Text {
                    id: settingsDatabaseInputTitle;

                    font.pixelSize: Style.fontSize_common;
                    color: Style.textColor;
                    font.family: Style.fontFamily;

                    text: model.Name;
                }

                onWidthChanged: {
                    console.log("databaseInputDelegate onWidthChanged", databaseInputDelegate.width);

                    if (settingsDatabaseInputLoader.item){
                        settingsDatabaseInputLoader.item.width = databaseInputColumn.width;
                    }
                }

                Loader {
                    id: settingsDatabaseInputLoader;

                    anchors.top: settingsDatabaseInputTitle.bottom;
                    anchors.topMargin: 5;

                    Component.onCompleted: {
                        settingsDatabaseInputLoader.source = model.Source;
                    }

                    onLoaded: {
                        console.log("Loader onLoaded", settingsDatabaseInputLoader.source);

                        if (settingsDatabaseInputLoader.item.parameters !== undefined){
                            settingsDatabaseInputLoader.item.parameters = model.Parameters;
                        }
                    }
                }
            }
        } // Repeater

        DatabaseBackupController {
            width: parent.width;
        }
    }
}


