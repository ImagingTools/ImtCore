import QtQuick 2.0
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Rectangle {
    id: accountEditorContainer;

    color: Style.backgroundColor;

    property string itemId;
    property string itemName;
    property string commandsId;

    property int textInputHeight: 30;

    property Item rootItem;

    property alias commands: commandsDelegate;

    onVisibleChanged: {
        if (accountEditorContainer.visible){
            Events.sendEvent("CommandsModelChanged", {"Model":      commandsProvider.commandsModel,
                                                      "CommandsId": commandsProvider.commandsId});
        }
    }

    TreeItemModel {
        id: accountModel;
    }

    UndoRedoManager {
        id: undoRedoManager;

        commandsId: accountEditorContainer.commandsId;
        editorItem: accountEditorContainer;

        onModelParsed: {
            updateGui();
        }
    }

    CommandsProvider {
        id: commandsProvider;

        commandsId: accountEditorContainer.commandsId;
    }

    AccountEditorCommandsDelegate {
        id: commandsDelegate;
        commandsId: accountEditorContainer.commandsId;

        objectView: accountEditorContainer;
    }

    function updateGui(){
        console.log("AccountEditor updateGui");
        accountNameInput.text = accountModel.GetData("Name");
        accountDescriptionInput.text = accountModel.GetData("Description");

        for (let i = 0; i < accountOwnerModel.count; i++){
            let id = accountOwnerModel.get(i).Id;
            console.log("Id", id)
            if (id === "Email"){
                accountOwnerModel.setProperty(i, "Value", accountModel.GetData("Email"))
            }
            else if (id === "FirstName"){
                accountOwnerModel.setProperty(i, "Value", accountModel.GetData("FirstName"))
            }
            else if (id === "LastName"){
                accountOwnerModel.setProperty(i, "Value", accountModel.GetData("LastName"))
            }
            else if (id === "NickName"){
                accountOwnerModel.setProperty(i, "Value", accountModel.GetData("NickName"))
            }
            else if (id === "BirthDay"){
                accountOwnerModel.setProperty(i, "Value", accountModel.GetData("BirthDay"))
            }
        }
    }

    Flickable {
        anchors.fill: parent;

        contentWidth: bodyColumn.width;
        contentHeight: bodyColumn.height + 50;

        boundsBehavior: Flickable.StopAtBounds;

        Column {
            id: bodyColumn;

            anchors.left: parent.left;
            anchors.leftMargin: 20;

            width: 450;

            spacing: 7;

            Text {
                id: titleAccountName;
                text: qsTr("Account name");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            CustomTextField {
                id: accountNameInput;

                height: accountEditorContainer.textInputHeight;
                width: bodyColumn.width;

                onTextChanged: {
                    console.log("AccountEditor onTextChanged");
                    accountModel.SetData("Id", accountNameInput.text);
                    accountModel.SetData("Name", accountNameInput.text);
                }
            }

            Text {
                id: titleAccountDescription;
                text: qsTr("Account description");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            CustomTextField {
                id: accountDescriptionInput;

                height: accountEditorContainer.textInputHeight;
                width: bodyColumn.width;

                onTextChanged: {
                    accountModel.SetData("Description", accountDescriptionInput.text);
                }
            }

            Text {
                text: qsTr("Company address");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            Rectangle {
                id: companyAddressBlockBorders;

                width: parent.width;
                height: companyAddressBlock.height + 25;

                color: "transparent";

                border.width: 1;
                border.color: Style.borderColor;

                Column {
                    id: companyAddressBlock;

                    anchors.horizontalCenter: companyAddressBlockBorders.horizontalCenter;
                    anchors.verticalCenter: companyAddressBlockBorders.verticalCenter;

                    width: parent.width - 20;

                    spacing: 10;

                    ListModel {
                        id: addressModel;
                        Component.onCompleted: {
                            addressModel.append({"Id": "Country", "Name": "Country", "Value": ""});
                            addressModel.append({"Id": "City", "Name": "City", "Value": ""});
                            addressModel.append({"Id": "PostalCode", "Name": "Postal code", "Value": ""});
                            addressModel.append({"Id": "Street", "Name": "Street", "Value": ""});
                        }
                    }

                    Repeater {
                        id: repeater;

                        model: addressModel;

                        delegate: Item {
                            width: parent.width;
                            height: fieldInput.height + title.height;

                            Text {
                                id: title;

                                text: model.Name;
                                color: Style.textColor;
                                font.family: Style.fontFamily;
                                font.pixelSize: Style.fontSize_common;
                            }

                            CustomTextField {
                                id: fieldInput;
                                anchors.top: title.bottom;
                                anchors.topMargin: 7;

                                height: accountEditorContainer.textInputHeight;
                                width: companyAddressBlock.width;

                                text: model.Value;

                                onTextChanged: {
                                    accountModel.SetData(model.Id, fieldInput.text);
                                }
                            }
                        }
                    }
                }// Company address block
            }//Company address block borders

            Text {
                text: qsTr("Account Owner");
                color: Style.textColor;

                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            Rectangle {
                id: accountOwnerBlockBorders;

                width: parent.width;
                height: accountOwnerBlock.height + 25;

                color: "transparent";

                border.width: 1;
                border.color: Style.borderColor;

                Column {
                    id: accountOwnerBlock;

                    anchors.horizontalCenter: accountOwnerBlockBorders.horizontalCenter;
                    anchors.verticalCenter: accountOwnerBlockBorders.verticalCenter;

                    width: parent.width - 20;

                    spacing: 10;

//                    TreeItemModel {
//                        id: accountOwnerModel;

//                        Component.onCompleted: {
//                            let index = accountOwnerModel.InsertNewItem();

//                            accountOwnerModel.SetData("Id", "Email", index);
//                            accountOwnerModel.SetData("Name", "Email", index);
//                            accountOwnerModel.SetData("Value", "", index);

//                            index = accountOwnerModel.InsertNewItem();

//                            accountOwnerModel.SetData("Id", "BirthDay", index);
//                            accountOwnerModel.SetData("Name", "Birthday", index);
//                            accountOwnerModel.SetData("Value", "", index);

//                            index = accountOwnerModel.InsertNewItem();

//                            accountOwnerModel.SetData("Id", "FirstName", index);
//                            accountOwnerModel.SetData("Name", "First name", index);
//                            accountOwnerModel.SetData("Value", "", index);

//                            index = accountOwnerModel.InsertNewItem();

//                            accountOwnerModel.SetData("Id", "LastName", index);
//                            accountOwnerModel.SetData("Name", "Last name", index);
//                            accountOwnerModel.SetData("Value", "", index);

//                            index = accountOwnerModel.InsertNewItem();

//                            accountOwnerModel.SetData("Id", "Nickname", index);
//                            accountOwnerModel.SetData("Name", "Nickname", index);
//                            accountOwnerModel.SetData("Value", "", index);
//                        }
//                    }

                    ListModel {
                        id: accountOwnerModel;

                        Component.onCompleted: {
                            accountOwnerModel.append({"Id": "Email", "Name": "Email", "Value": ""});
                            accountOwnerModel.append({"Id": "BirthDay", "Name": "BirthDay", "Value": ""});
                            accountOwnerModel.append({"Id": "FirstName", "Name": "First name","Value": ""});
                            accountOwnerModel.append({"Id": "LastName", "Name": "Last name","Value": ""});
                            accountOwnerModel.append({"Id": "NickName", "Name": "Nickname","Value": ""});
                        }
                    }

                    Repeater {
                        id: repeaterOwnerBlock;

                        model: accountOwnerModel;

                        delegate: Item {
                            width: parent.width;
                            height: fieldInputOwnerBlock.height + titleOwnerBlock.height;

                            Text {
                                id: titleOwnerBlock;

                                text: model.Name;
                                color: Style.textColor;
                                font.family: Style.fontFamily;
                                font.pixelSize: Style.fontSize_common;
                            }

                            CustomTextField {
                                id: fieldInputOwnerBlock;

                                anchors.top: titleOwnerBlock.bottom;
                                anchors.topMargin: 7;

                                height: accountEditorContainer.textInputHeight;
                                width: accountOwnerBlock.width;

                                text: model.Value;

                                onTextChanged: {
                                    accountModel.SetData(model.Id, fieldInputOwnerBlock.text);
                                }
                            }
                        }
                    }
                } // Account owner block
            } //Account owner borders
        }//Body column
    }//Flickable
}// Account Editor container
