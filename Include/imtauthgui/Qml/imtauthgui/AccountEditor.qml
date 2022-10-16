import QtQuick 2.0
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

DocumentBase {
    id: accountEditorContainer;

//    commandsDelegate: AccountEditorCommandsDelegate{}
    commandsDelegatePath: "../../imtauthgui/AccountEditorCommandsDelegate.qml";

    property int textInputHeight: 30;

    onDocumentModelChanged: {
        accountNameInput.focus = true;
    }

    UndoRedoManager {
        id: undoRedoManager;

        commandsId: accountEditorContainer.commandsId;
        editorItem: accountEditorContainer;

        onModelParsed: {
            updateGui();
        }
    }

    function updateGui(){
        console.log("AccountEditor updateGui");
        accountNameInput.text = documentModel.GetData("Name");
        accountDescriptionInput.text = documentModel.GetData("Description");

        for (let i = 0; i < accountOwnerModel.count; i++){
            let id = accountOwnerModel.get(i).Id;
            console.log("Id", id)
            if (id === "Email"){
                accountOwnerModel.setProperty(i, "Value", documentModel.GetData("Email"))
            }
            else if (id === "FirstName"){
                accountOwnerModel.setProperty(i, "Value", documentModel.GetData("FirstName"))
            }
            else if (id === "LastName"){
                accountOwnerModel.setProperty(i, "Value", documentModel.GetData("LastName"))
            }
            else if (id === "NickName"){
                accountOwnerModel.setProperty(i, "Value", documentModel.GetData("NickName"))
            }
            else if (id === "BirthDay"){
                accountOwnerModel.setProperty(i, "Value", documentModel.GetData("BirthDay"))
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
                    documentModel.SetData("Id", accountNameInput.text);
                    documentModel.SetData("Name", accountNameInput.text);
                }

                KeyNavigation.tab: accountDescriptionInput;
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
                    documentModel.SetData("Description", accountDescriptionInput.text);
                }

                KeyNavigation.tab: repeater;
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

                        onFocusChanged: {
                            if (focus){
                                let item  = repeater.itemAt(0)
                                item.focus = true;
                            }
                        }

                        delegate: Item {
                            id: delegateItem;

                            width: parent.width;
                            height: fieldInput.height + title.height;

                            Text {
                                id: title;

                                text: model.Name;
                                color: Style.textColor;
                                font.family: Style.fontFamily;
                                font.pixelSize: Style.fontSize_common;
                            }

                            onFocusChanged: {
                                if (focus){
                                    fieldInput.focus = true;
                                }
                            }

                            CustomTextField {
                                id: fieldInput;
                                anchors.top: title.bottom;
                                anchors.topMargin: 7;

                                height: accountEditorContainer.textInputHeight;
                                width: companyAddressBlock.width;

                                text: model.Value;

                                onTextChanged: {
                                    documentModel.SetData(model.Id, fieldInput.text);
                                }

                                Keys.onBacktabPressed: {
                                    if (model.index > 0){
                                        let item  = repeater.itemAt(model.index - 1)
                                        item.focus = true;
                                    }
                                    else{
                                        accountDescriptionInput.focus = true;
                                    }
                                }

                                Keys.onTabPressed: {
                                    if (model.index < repeater.count - 1){
                                        let item  = repeater.itemAt(model.index + 1)
                                        item.focus = true;
                                    }
                                    else{
                                        repeaterOwnerBlock.focus = true;
                                    }
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

                        onFocusChanged: {
                            if (focus){
                                let item  = repeaterOwnerBlock.itemAt(0)
                                item.focus = true;
                            }
                        }

                        delegate: Item {
                            width: parent.width;
                            height: fieldInputOwnerBlock.height + titleOwnerBlock.height;

                            onFocusChanged: {
                                if (focus){
                                    fieldInputOwnerBlock.focus = true;
                                }
                            }

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
                                    documentModel.SetData(model.Id, fieldInputOwnerBlock.text);
                                }

                                Keys.onBacktabPressed: {
                                    let item ;
                                    if (model.index > 0){
                                        item  = repeaterOwnerBlock.itemAt(model.index - 1)
                                    }
                                    else{
                                        item = repeater.itemAt(repeater.count - 1);

                                    }

                                    item.focus = true;
                                }

                                Keys.onTabPressed: {
                                    if (model.index < repeaterOwnerBlock.count - 1){
                                        let item  = repeaterOwnerBlock.itemAt(model.index + 1)
                                        item.focus = true;
                                    }
                                }
                            }
                        }
                    }
                } // Account owner block
            } //Account owner borders
        }//Body column
    }//Flickable
}// Account Editor container
