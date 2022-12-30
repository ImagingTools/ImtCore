import QtQuick 2.0
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0

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
                    console.log("databaseInputDelegate onWidthChanged", width);

                    if (settingsDatabaseInputLoader.item){
                        settingsDatabaseInputLoader.item.width = databaseInputColumn.width;
                    }
                }

                Loader {
                    id: settingsDatabaseInputLoader;

                    anchors.top: settingsDatabaseInputTitle.bottom;
                    anchors.topMargin: 5;

                    Component.onCompleted: {
                        source = model.Source;
                    }

                    onLoaded: {
                        console.log("Loader onLoaded", source);
                    }
                }
            }
        }
    }
}
