import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0


Rectangle {
    id: dialogsRepresentaitonPage;

    anchors.fill: parent;

    Row{
        id: buttonsRow;

        anchors.centerIn:  parent;

        height: 40;

        spacing: 100;

        Repeater{
            model: 2;
            delegate:
                Rectangle{
                id: forDialogOpen;

                width: 200;
                height: 40;

                radius: 10;
                color: "lightblue";
                Text{
                    anchors.centerIn: parent;

                    text: model.index == 0 ? "show dialog component" :
                                             model.index == 1 ? "show dialog" : "";
                    font.pixelSize: 18;
                }

                MouseArea{
                    anchors.fill: parent;

                    hoverEnabled: true;
                    cursorShape: Qt.PointingHandCursor;

                    onClicked:{
                        if(model.index == 0){
                            var parameters = {"centered": true};
                            modalDialogManager.openDialog(dialogComp, parameters);
                        }
                        else if(model.index == 1){
                            dialog_new.open();
                        }
                    }
                }
            }//delegate
        }//Repeater
    }//buttonsRow

    Component{
        id: dialogComp;

        Dialog {
            id: dialogContainer;

            title: "Заголовок";

            buttonsModel: buttonsModelTest;
            contentComp: contentTest;

            backgroundColor: "white";
            radius: 2;
            onFinished: {
                if(buttonId == Enums.ButtonType.Cancel || buttonId == Enums.ButtonType.No){
                    console.log("Cancel!!!");
                }
                else if(buttonId == Enums.ButtonType.Ok || buttonId == Enums.ButtonType.Yes){
                    console.log("Ok!!!");
                }
            }

        }
    }//dialogComp

    DialogNew{
        id: dialog_new;

        title: "Заголовок";

        buttonsModel: buttonsModelTest;
        contentComp: contentTest;

        backgroundColor: "white";
        radius: 2;

        onFinished: {
            if(buttonId == Enums.ButtonType.Cancel || buttonId == Enums.ButtonType.No){
                console.log("Cancel!!!");
            }
            else if(buttonId == Enums.ButtonType.Ok || buttonId == Enums.ButtonType.Yes){
                console.log("Ok!!!");
            }
        }

    }//DialogNew


    //TEST DATA//
    Component{
        id: contentTest;
        Rectangle{
            width: 300;
            height: testText.height + 10;//300;

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
        //                        ListElement{Id: Enums.ButtonType.Cancel; Name:qsTr("Cancel"); Enabled: true}
        //                        ListElement{Id: Enums.ButtonType.Close; Name:qsTr("Close"); Enabled: true}
        //                        ListElement{Id: Enums.ButtonType.Apply; Name:qsTr("Apply"); Enabled: true}
    }

}
