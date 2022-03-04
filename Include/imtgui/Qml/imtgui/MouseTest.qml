import QtQuick 2.0
import Acf 1.0
import 'AuxComponents'
import imtauthgui 1.0
import imtlicgui 1.0
import imtqml 1.0;
import imtgui 1.0;
import QtQuick.Dialogs 1.3

Rectangle
{
    id: redRect;

    width: 300;
    height: width;

    color: "red";

//    FileDialog {
//        id: fileDialog;
//        title: "Выберите файл";
//        folder: shortcuts.home;
//        onAccepted: {
//            console.log(fileDialog.fileUrl)
//        }
//        onRejected: {
//            console.log("Canceled")
//        }
//    }

//    Rectangle{
//        width: 500;
//        height: 500;
//        x: 500;
//        Rectangle{
//            width: 50;
//            height: 50;
//            color: 'blue';
//            MouseArea{
//                anchors.fill: parent;
//                onClicked:{
//                    fileDialog.visible = true
//                    //this._context.location.changeHref('/newurl/')
//                }
//            }
//        }
//        Rectangle{
//            width: 50;
//            height: 50;
//            x: 60;
//            color: 'blue';
//            MouseArea{
//                anchors.fill: parent;
//                onClicked:{
//                    //this._context.location.changeHref('newurl/')
//                }
//            }
//        }
//        Rectangle{
//            width: 50;
//            height: 50;
//            color: 'blue';
//            x: 120;
//            MouseArea{
//                anchors.fill: parent;
//                onClicked:{
//                    //this._context.location.changeHref('../newurl/')
//                }
//            }
//        }
//    }
}
