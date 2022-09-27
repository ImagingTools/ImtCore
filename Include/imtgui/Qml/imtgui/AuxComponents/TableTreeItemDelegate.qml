import QtQuick 2.0
import Acf 1.0

Item {
    id: treeItemDelegate;

    height: childrenColumn.visible ? childrenColumn.height + rowBody.height : rowBody.height;

    property int itemHeight: 30;

    property TreeItemModel childModel: model.ChildModel ? model.ChildModel : null;

    signal doubleClicked;

    property int firstRowWidth: treeItemDelegate.width / 3/* - model.Level* 15*/;

    function selectItem(){
        resetSelectedItem(modelItems);
        model.Selected = true;
    }

    onFirstRowWidthChanged: {
        console.log("onFirstRowWidthChanged");

        console.log()
    }

    onChildModelChanged: {
        console.log("onChildModelChanged");
        childModelRepeater.model = model.ChildModel;
    }

    Rectangle {
        id: rowBody;

        width: parent.width;
        height: model.Visible ? treeItemDelegate.itemHeight : 0;

        visible: model.Visible;

        color: model.Selected ? Style.selectedColor : "transparent";

        MouseArea {
            id: rowMouseArea;

            anchors.fill: parent;

            onClicked: {
                selectItem();
            }

            onDoubleClicked: {
                treeItemDelegate.doubleClicked();
            }
        }

        Item {
            id: descriptionRect;

            anchors.right: parent.right;

            width: parent.width / 3;
            height: parent.height;

            Text {
                id: descriptionTitle;

                anchors.left: parent.left;
                anchors.leftMargin: 10;
                anchors.verticalCenter: parent.verticalCenter;

                text: model.Description;
                color: model.Active ? Style.textColor : Style.disabledInActiveTextColor;

                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;
            }
        }

        Item {
            id: nameRect;

            anchors.right: descriptionRect.left;

            width: parent.width / 3;
            height: parent.height;

            Text {
                id: titleName;

                anchors.left: parent.left;
                anchors.leftMargin: 10;
                anchors.verticalCenter: parent.verticalCenter;

                text: model.Name;
                color: model.Active ? Style.textColor : Style.disabledInActiveTextColor;

                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;

                visible: !inputName.visible;

                MouseArea {
                    anchors.fill: parent;

                    onClicked: {
                        selectItem();

                    }

                    onDoubleClicked: {
                        console.log("onDoubleClicked");
                        inputName.visible = true;
                        inputName.focus = true;
                    }
                }
            }

            CustomTextField {
                id: inputName;

                anchors.left: parent.left;
                anchors.leftMargin: 10;
                anchors.verticalCenter: parent.verticalCenter;

                width: titleName.width + 40;
                height: parent.height - 7;

                text: model.Name;

                visible: false;

                onTextChanged: {
                    model.Name = inputName.text;
                }

                onTextInputFocusChanged: {
                    if (!textInputFocus){
                        visible = false;
                    }
                }

                onAccepted: {
                    visible = false;
                }
            }
        }

        Item {
            id: idRect;

            anchors.right: nameRect.left;

            width: firstRowWidth;
            height: parent.height;

            AuxButton {
                id: arrowButton;

                anchors.left: parent.left;
                anchors.leftMargin: 5;
                anchors.verticalCenter: parent.verticalCenter;

                width: 15;
                height: width;

                iconSource: model.Opened ? "../../../" + "Icons/" + Style.theme + "/" + "Down" + "_On_Normal.svg" :
                                       "../../../" + "Icons/" + Style.theme + "/" + "Right" + "_On_Normal.svg";

                visible: childModelRepeater.count > 0;

                onClicked: {
                    model.Opened = !model.Opened;
                }
            }

            Text {
                id: titleId;

                anchors.left: arrowButton.right;
                anchors.leftMargin: 10;
                anchors.verticalCenter: parent.verticalCenter;

                text: model.Id;
                color: model.Active ? Style.textColor : Style.disabledInActiveTextColor;

                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;

                visible: !inputId.visible;

                MouseArea {
                    anchors.fill: parent;

                    onClicked: {
                        selectItem();
                    }

                    onDoubleClicked: {
                        console.log("onDoubleClicked");
                        inputId.visible = true;
                        inputId.focus = true;
                    }
                }
            }

            CustomTextField {
                id: inputId;

                anchors.left: arrowButton.right;
                anchors.leftMargin: 10;
                anchors.verticalCenter: parent.verticalCenter;

                width: titleId.width + 40;
                height: parent.height - 7;

                text: model.Id;

                visible: false;

                onTextChanged: {
                    model.Id = inputId.text;
                }

                onTextInputFocusChanged: {
                    if (!textInputFocus){
                        visible = false;
                    }
                }

                onAccepted: {
                    visible = false;
                }
            }
        }
    }

    Column {
        id: childrenColumn;

        anchors.top: rowBody.bottom;
        anchors.right: parent.right;

        visible: model.Opened;

        Repeater {
            id: childModelRepeater;

            delegate: delegateComp;

            onItemAdded: {
                console.log("item onItemAdded", item);
                item.firstRowWidth = treeItemDelegate.firstRowWidth - 15;
                console.log("item.firstRowWidth", item.firstRowWidth);
            }
        }
    }
}
