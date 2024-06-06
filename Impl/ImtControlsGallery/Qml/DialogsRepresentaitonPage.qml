import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0


Rectangle {
    id: dialogsRepresentaitonPage;

    anchors.fill: parent;
    clip: true;


    Column{
        id: column;

        anchors.centerIn: parent;

        spacing: 100;

        Row{
            id: buttonsRow;

            anchors.horizontalCenter:  parent.horizontalCenter;

            spacing: 80;

            Button{
                id: messageDialogButton;

                anchors.verticalCenter:  parent.verticalCenter;

                text: "Message";
                onClicked: {
                    var parameters = {"centered": true};
                    ModalDialogManager.openDialog(messageDialogComp, parameters);
                }
            }

            Button{
                id: confirmationDialogButton;

                anchors.verticalCenter:  parent.verticalCenter;

                text: "Confirmation";
                onClicked: {
                    var parameters = {"centered": true};
                    ModalDialogManager.openDialog(confirmationDialogComp, parameters);
                }
            }

            Button{
                id: contentDialogButton;

                anchors.verticalCenter:  parent.verticalCenter;

                text: "Content";
                onClicked: {
                    var parameters = {"centered": true};
                    ModalDialogManager.openDialog(contentDialogComp, parameters);
                }
            }

            Button{
                id: inputDialogButton;

                anchors.verticalCenter:  parent.verticalCenter;

                text: "Input";
                onClicked: {
                    var parameters = {"centered": true};
                    ModalDialogManager.openDialog(inputDialogComp, parameters);
                }
            }
        }//row1

        Row{
            id: buttonsRowNew;

            anchors.horizontalCenter:  parent.horizontalCenter;

            spacing: 80;

            Button{
                id: newDialogButton;

                anchors.verticalCenter:  parent.verticalCenter;

                text: "Open new dialog";
                onClicked: {
                    dialog_new.open();
                }
            }

            Button{
                id: windowDialogButton;

                anchors.verticalCenter:  parent.verticalCenter;

                visible: Style.styleId == "StyleQt";
                text: "window style";
                onClicked: {
                    //var parameters = {"title": "Confirmation(window)", "modality": Qt.WindowModal};
                    var parameters = {"title": "Confirmation(window)"};
                    ModalDialogManager.openDialog(confirmationDialogComp, parameters, "External");
                }
            }
        }

    }//Column


    //Components
//    Component{
//        id: dialogComp;

//        Dialog {
//            id: dialogContainer;

//            title: "Заголовок";

//            buttonsModel: buttonsModelTest;
//            contentComp: contentTest;

//            backgroundColor: "white";
//            radius: 2;
//            onFinished: {
//                if(buttonId == Enums.cancel || buttonId == Enums.no){
//                    console.log("Cancel!!!");
//                }
//                else if(buttonId == Enums.ok || buttonId == Enums.yes){
//                    console.log("Ok!!!");
//                }
//            }

//        }
//    }//dialogComp


        Component{
            id: messageDialogComp;

            Dialog {

                title: "Message";

                contentComp: Component{ Item{
                    width: 200;
                    height: messageText.height + 20;

                    Text {
                        id: messageText;

                        anchors.horizontalCenter: parent.horizontalCenter;
                        width: parent.width - 10;

                        text: "Lorem ipsum dolor sit amet...";
                        font.pixelSize: 18;
                        wrapMode: Text.WordWrap;
                    }

                }}

                backgroundColor: "white";
                radius: 2;

            }
        }//messageDialogComp

        Component{
            id: confirmationDialogComp;

            Dialog {

                title: "Confirmation";

                contentComp: Component{ Item{
                    width: 300;
                    height: checkBox.y + checkBox.height + 20;

                    Text {
                        id: confirmationText;

                        width: parent.width - 10;

                        text: "The document has been modified.\nDo you want to save your changes?";
                        font.pixelSize: 18;
                        wrapMode: Text.WordWrap;
                    }

                    CheckBox{
                        id: checkBox;

                        anchors.top: confirmationText.bottom;
                        anchors.topMargin: 50;

                        text: "Do not ask again";

                        onClicked: {
                            if(checkBox.checkState !== Qt.Checked){
                                checkBox.checkState = Qt.Checked;
                            }
                            else {
                                checkBox.checkState = Qt.Unchecked;
                            }
                        }

                    }

                }}

                buttonsModel: ListModel{
                    ListElement{Id: Enums.ButtonType.Yes; Name:qsTr("Yes"); Enabled: true}
                    ListElement{Id: Enums.ButtonType.No; Name:qsTr("No"); Enabled: true}
                }


                backgroundColor: "white";
                radius: 2;

                onFinished: {
                    if(buttonId == Enums.ok || buttonId == Enums.yes){
                        console.log("Dialog Finished::Yes");
                    }
                    else if(buttonId == Enums.cancel || buttonId == Enums.no){
                        console.log("Dialog Finished::No");
                    }
                }
            }
        }//confirmationDialogComp

        Component{
            id: contentDialogComp;

            Dialog {

                title: "Content";
                contentComp: Component{ Rectangle{
                    width: 300;
                    height: contentText.height + 10;

                    border.color: "lightgray";
                    radius: 2;

                    Text {
                        id: contentText;

                        anchors.centerIn: parent;
                        width: parent.width - 10;

                        text: "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nunc finibus "
                              + "in est quis laoreet. Interdum et malesuada fames ac ante ipsum primis "
                              + "in faucibus. Curabitur eget justo sollicitudin enim faucibus bibendum. "
                              + "Suspendisse potenti. Vestibulum cursus consequat mauris id sollicitudin. "
                              + "Duis facilisis hendrerit consectetur. Curabitur sapien tortor, efficitur "
                              + "id auctor nec, efficitur et nisl. Ut venenatis eros in nunc placerat, "
                              + "eu aliquam enim suscipit."

                        font.pixelSize: 18;
                        wrapMode: Text.WordWrap;
                    }
                }}

                buttonsModel: ListModel{
                    ListElement{Id: Enums.ButtonType.Close; Name:qsTr("Close"); Enabled: true}
                }

                backgroundColor: "white";
                radius: 2;

            }
        }//contentDialogComp

        Component{
            id: inputDialogComp;

            Dialog {
                id: inputDialog;

                title: "Input";

                contentComp: Component{ Item{
                    width: 300;
                    height: column.height + 20;

                    property alias username: userNameTextField.text;
                    property alias password: passwordTextField.text;

                    Component.onCompleted: {
                        userNameTextField.forceActiveFocus();
                    }

                    onUsernameChanged: {
                        if(inputDialog.rootItem.username !==undefined){
                            inputDialog.rootItem.username = username;
                        }
                    }
                    onPasswordChanged: {
                        if(inputDialog.rootItem.password !==undefined){
                            inputDialog.rootItem.password = password;
                        }
                    }

                    Column{
                        id: column;

                        anchors.centerIn: parent;

                        width: parent.width;

                        spacing: 30;

                        Text {
                            id: inputText;

                            anchors.horizontalCenter: parent.horizontalCenter;
                            width: parent.width - 10;

                            text: "Please enter the credentials:";
                            font.pixelSize: 18;
                            wrapMode: Text.WordWrap;
                        }

                        TextField{
                            id: userNameTextField;

                            placeHolderText: "Username";
                        }

                        TextField{
                            id: passwordTextField;

                            placeHolderText: "Password";
                        }
                    }

                }}

//                buttonsModel: ListModel{
//                    ListElement{Id: Enums.ok; Name:qsTr("Ok"); Enabled: true}
//                    ListElement{Id: Enums.cancel; Name:qsTr("Cancel"); Enabled: true}
//                }

                backgroundColor: "white";
                radius: 2;

                Component.onCompleted: {
                    buttonsModel.append({Id: Enums.ok, Name:qsTr("Ok"), Enabled: true})
                    buttonsModel.append({Id: Enums.cancel, Name:qsTr("Cancel"), Enabled: true})
                }
            }
        }//inputDialogComp


    /*************************new Dialog******************************/
    DialogNew{
        id: dialog_new;

        title: "Заголовок";

        buttonsModel: buttonsModelTest;
        contentComp: contentTest;

        backgroundColor: "white";
        radius: 2;

        onFinished: {
            if(buttonId == Enums.cancel || buttonId == Enums.no){
                console.log("Cancel!!!");
            }
            else if(buttonId == Enums.ok || buttonId == Enums.yes){
                console.log("Ok!!!");
            }
        }

    }//DialogNew


    //TEST DATA//
    Component{
        id: contentTest;
        Rectangle{
            width: 300;
            height: testText.height + 10;

            border.color: "lightgray";
            radius: 2;

            Text {
                id: testText;

                anchors.centerIn: parent;
                width: parent.width - 10;

                text: "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nunc finibus "
                      + "in est quis laoreet. Interdum et malesuada fames ac ante ipsum primis "
                      + "in faucibus. Curabitur eget justo sollicitudin enim faucibus bibendum. "
                      + "Suspendisse potenti. Vestibulum cursus consequat mauris id sollicitudin. "
                      + "Duis facilisis hendrerit consectetur. Curabitur sapien tortor, efficitur "
                      + "id auctor nec, efficitur et nisl. Ut venenatis eros in nunc placerat, "
                      + "eu aliquam enim suscipit."

                font.pixelSize: 18;
                wrapMode: Text.WordWrap;
            }
        }

    }

    ListModel{
        id: buttonsModelTest;

        ListElement{Id: Enums.ButtonType.Yes; Name:qsTr("Yes"); Enabled: true}
        ListElement{Id: Enums.ButtonType.No; Name:qsTr("No"); Enabled: true}
        //                        ListElement{Id: Enums.ButtonType.Save; Name:qsTr("Save"); Enabled: true}
        //                        ListElement{Id: Enums.cancel; Name:qsTr("Cancel"); Enabled: true}
        //                        ListElement{Id: Enums.ButtonType.Close; Name:qsTr("Close"); Enabled: true}
        //                        ListElement{Id: Enums.apply; Name:qsTr("Apply"); Enabled: true}
    }

}
