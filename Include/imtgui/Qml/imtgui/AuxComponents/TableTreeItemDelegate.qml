import QtQuick 2.0
import Acf 1.0

Item {
    id: treeItemDelegate;

    height: childrenColumn.visible ? childrenColumn.height + rowBody.height : rowBody.height;

    property int itemHeight: 30;

    signal clicked(string id, int index, var model);
    signal doubleClicked();

    signal itemRemoved(var itemData);

    property int level: model.Level;

    property int headersCount: 4;

    property TreeItemModel childModel: model.ChildModel ? model.ChildModel : null;

    property ModelIndex modelIndex: ModelIndex {};

    Component.onCompleted: {
        console.log("TreeItemDelegate onCompleted");
        modelIndex.itemData = model;
        modelIndex.index = model.index;
    }

    onChildModelChanged: {
        console.log("treeItemDelegate onChildModelChanged", model.ChildModel);

        if (model.ChildModel){
            childModelRepeater.model = model.ChildModel;
        }
    }

    function getSelectedId(){
        return model.Id;
    }

    function getSelectedName(){
        return model.Name;
    }

    Rectangle {
        id: rowBody;

        width: parent.width;
        height: treeItemDelegate.itemHeight;

        color: modelIndex.equal(selectedIndex) ? Style.selectedColor : "transparent";

        MouseArea {
            id: rowMouseArea;

            anchors.fill: parent;

            onClicked: {
                treeItemDelegate.clicked(model.Id, model.index, model);
            }

            onDoubleClicked: {
                treeItemDelegate.doubleClicked();
            }
        }

        Item {
            id: optionalRect;

            anchors.right: parent.right;

            width: parent.width / headersCount;
            height: parent.height;

            CheckBox {
                id: optional;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: 10;

                width: 15;
                height: width;

                checkState: model.Optional ? Qt.Checked : Qt.Unchecked;

                onClicked: {
                    model.Optional = !model.Optional;
                }
            }
        }

        Item {
            id: descriptionRect;

            anchors.right: optionalRect.left;

            width: parent.width / headersCount;
            height: parent.height;

            Text {
                id: descriptionTitle;

                anchors.left: parent.left;
                anchors.leftMargin: 10;
                anchors.verticalCenter: parent.verticalCenter;

                width: parent.width - 10;

                text: model.Description ? model.Description : "";
//                color: model.Active ? Style.textColor : Style.disabledInActiveTextColor;

                color: Style.textColor;

                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;

                elide: Text.ElideRight;

                MouseArea {
                    anchors.fill: parent;

                    cursorShape: Qt.IBeamCursor;

                    onClicked: {
                        treeItemDelegate.clicked(model.Id, model.index, model);
                    }

                    onDoubleClicked: {
                        treeItemDelegate.doubleClicked();
                        inputDescription.visible = true;
                        inputDescription.focus = true;
                    }
                }
            }

            CustomTextField {
                id: inputDescription;

                anchors.verticalCenter: parent.verticalCenter;

                width: descriptionTitle.width;
                height: parent.height - 7;

                text: model.Description ? model.Description : "";

                visible: false;

                onTextChanged: {
                    if (inputDescription.text != model.Description){
                        model.Description = inputDescription.text;
                    }
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
            id: nameRect;

            anchors.right: idRect.left;

            width: parent.width / headersCount - model.Level * 10 + 10;
            height: parent.height;

            AuxButton {
                id: arrowButton;

                anchors.left: parent.left;
//                anchors.leftMargin: 10;
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
                id: titleName;

                anchors.left: arrowButton.right;
                anchors.leftMargin: 10;
                anchors.verticalCenter: parent.verticalCenter;

                width: parent.width - 10;

                text: model.Name ? model.Name : "";
//                color: model.Active ? Style.textColor : Style.disabledInActiveTextColor;
                color: Style.textColor;

                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;

                visible: !inputName.visible;

                elide: Text.ElideRight;

                MouseArea {
                    anchors.fill: parent;

                    cursorShape: Qt.IBeamCursor;

                    onClicked: {
                        treeItemDelegate.clicked(model.Id, model.index, model);
                    }

                    onDoubleClicked: {
                        treeItemDelegate.doubleClicked();
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

                width: titleName.width;
                height: parent.height - 7;

                text: model.Name ? model.Name : "";

                visible: false;

                onTextChanged: {                    
                    if (inputName.text != model.Name){
                        model.Name = inputName.text;
                    }
                }

                onTextInputFocusChanged: {
                    if (!textInputFocus){
                        visible = false;
                    }
                }

                onAccepted: {
                    visible = false;

                    if (model.Id == ""){
                        let id = model.Name.replace(/\s+/g, '');
                        model.Id = id;
                    }
                }
            }
        }

        Item {
            id: idRect;

            anchors.right: descriptionRect.left;

            width: parent.width / headersCount;

            height: parent.height;

            Text {
                id: titleId;

                anchors.left: parent.left;
                anchors.leftMargin: 10;
                anchors.verticalCenter: parent.verticalCenter;

                width: parent.width - 10;

                text: model.Id ? model.Id : "";
//                color: model.Active ? Style.textColor : Style.disabledInActiveTextColor;
                color: Style.textColor;

                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;

                visible: !inputId.visible;

                elide: Text.ElideRight;

                MouseArea {
                    anchors.fill: parent;

                    cursorShape: Qt.IBeamCursor;

                    onClicked: {
                        treeItemDelegate.clicked(model.Id, model.index, model);
                    }

                    onDoubleClicked: {
                        treeItemDelegate.doubleClicked();
                        inputId.visible = true;
                        inputId.focus = true;
                    }
                }
            }

            CustomTextField {
                id: inputId;

                anchors.verticalCenter: parent.verticalCenter;

                width: titleId.width;
                height: parent.height - 7;

                text: model.Id ? model.Id : "";

                visible: false;

                onTextChanged: {
                    if (inputId.text != model.Id){
                        model.Id = inputId.text;
                    }
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
                item.modelIndex.parentIndex = modelIndex;
                modelIndex.children.push(item.modelIndex);
            }
        }
    }
}
