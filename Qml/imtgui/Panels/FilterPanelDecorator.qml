import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0

DecoratorBase {
    id: filterPanelDecorator;

    height: content.height;

    property alias contentWidth: content.width;

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", onLocalizationChanged)
        updateText();
        checkWidth()
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", onLocalizationChanged)
    }

    onWidthChanged: {
        checkWidth();
    }

    function checkWidth(){
//        rect.visible = width - rect.width <= contentWidth + Style.size_largeMargin;
//        tfc.visible = width - tfc.width <= contentWidth + Style.size_largeMargin;
    }

    function onLocalizationChanged(language){
        updateText();
    }

    function updateText(){
        tfc.placeHolderText = qsTr("Enter some text to filter the item list");
    }

    Component {
        id: timeFilterParamComp;

        TimeFilterParamView {
            onAccepted: {
                filterPanelDecorator.baseElement.filterChanged("TimeFilter", this.model);
                ModalDialogManager.closeByComp(timeFilterParamComp);
                rect.filterEnabled = true;

                buttonText.text = name;
            }

            onCancelled: {
                clearButton.clicked();
            }
        }
    }

    TreeItemModel {
        id: filterModel;
    }

    Row {
        id: content;
        anchors.right: filterPanelDecorator.right;
        height: tfc.height;
        spacing: Style.size_mainMargin;

        Rectangle {
            id: rect;
            width: item.width;
            height: parent.height;
            color: filterEnabled ? Style.iconColorOnSelected : "transparent";
            border.width: 1;
            border.color: Style.borderColor;
            radius: 3;
            property bool filterEnabled: false;

            MouseArea {
                id: ma;
                anchors.fill: parent;
                cursorShape: Qt.PointingHandCursor;
                hoverEnabled: true;
                onClicked: {
                    var point = mapToItem(null, x - width, y + height);
                    ModalDialogManager.openDialog(timeFilterParamComp, {"x": point.x, "y": point.y, "model": filterModel});
                }
            }

            Item {
                id: item;
                anchors.centerIn: parent;
                width: row.width + 2 * Style.size_mainMargin;
                height: row.height;

                Row {
                    id: row;
                    anchors.horizontalCenter: parent.horizontalCenter;
                    spacing: Style.size_mainMargin;

                    BaseText {
                        id: buttonText;
                        anchors.verticalCenter: parent.verticalCenter;
                        text: !rect.filterEnabled ? qsTr("Date") : "";
                        color: rect.filterEnabled ? Style.baseColor : Style.textColor;
                    }

                    ToolButton {
                        id: clearButton;
                        anchors.verticalCenter: parent.verticalCenter;
                        width: 15;
                        height: width;
                        iconSource: "../../../" + Style.getIconPath("Icons/Close", Icon.State.Off, Icon.Mode.Disabled)
                        visible: rect.filterEnabled;
                        onClicked: {
                            rect.filterEnabled = false;
                            buttonText.text = qsTr("Date");
                            filterModel.clear();
                            filterPanelDecorator.baseElement.filterChanged("TimeFilter", filterModel);
                        }
                        decorator: Component {
                            ToolButtonDecorator {
                                color: "transparent";
                                icon.width: 16;
                                textColor: "white";
                            }
                        }
                    }

                    ToolButton {
                        id: downButton;
                        anchors.verticalCenter: parent.verticalCenter;
                        width: 15;
                        height: width;
                        iconSource: "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal);
                        visible: !rect.filterEnabled;
                        onClicked: {
                            ma.clicked();
                        }
                        decorator: Component {
                            ToolButtonDecorator {
                                color: "transparent";
                                icon.width: 16;
                            }
                        }
                    }
                }
            }
        }

        CustomTextField {
            id: tfc;
            anchors.verticalCenter: content.verticalCenter;
            textFieldRightMargin: iconClear.width + 2 * margin;
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

                z: 999;

                anchors.verticalCenter: tfc.verticalCenter;
                anchors.right: tfc.right;
                anchors.rightMargin: Style.margin;

                width: Style.buttonWidthSmall;
                height: width;

                visible: tfc.text != "";

                iconSource: "../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);
                decorator: Component {
                    ToolButtonDecorator {
                        color: "transparent";
                        icon.width: 16;
                    }
                }

                onClicked: {
                    tfc.text = "";
                }
            }
        }

        Item {
            width: Style.buttonWidthMedium;
            height: width;
        }
    }
}

