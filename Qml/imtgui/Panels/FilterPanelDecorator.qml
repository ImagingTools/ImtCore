import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

DecoratorBase {
    id: filterPanelDecorator;

    height: content.height;

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", onLocalizationChanged)

        updateText();
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", onLocalizationChanged)
    }

    function onLocalizationChanged(language){
        updateText();
    }

    function updateText(){
        tfc.placeHolderText = qsTr("Enter some text to filter the item list");
    }

    Row {
        id: content;

        anchors.right: filterPanelDecorator.right;

        height: tfc.height;
        spacing: Style.size_mainMargin;

        CustomTextField {
            id: tfc;

            anchors.verticalCenter: content.verticalCenter;

            //textFieldRightMargin: iconClear.width + 2 * margin;

            width: 270;
            height: 30;

            onTextChanged: {
                timer.restart();
            }

            Timer {
                id: timer;

                interval: 500;

                onTriggered: {
                    filterPanelDecorator.baseElement.filterChanged("TextFilter", tfc.text);
                }
            }

            ToolButton {
                id: iconClear;

                z: tfc.z + 1;

                anchors.verticalCenter: tfc.verticalCenter;
                anchors.right: tfc.right;
                anchors.rightMargin: Style.margin;

                width: Style.buttonWidthSmall;
                height: width;

                visible: tfc.text != "";

                iconSource: "../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);

                onClicked: {
                    tfc.text = "";
                }
            }
        }

        ToolButton {
            id: closeButton;

            anchors.verticalCenter: content.verticalCenter;

            width: Style.buttonWidthMedium;
            height: width;

            iconSource: "../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);

            onClicked: {
                tfc.text = "";
                filterPanelDecorator.baseElement.close();
            }
        }
    }
}

