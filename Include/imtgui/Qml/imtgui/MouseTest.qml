import QtQuick 2.0
//import Acf 1.0
//import 'AuxComponents'
//import imtauthgui 1.0
//import imtlicgui 1.0
//import imtqml 1.0;
//import imtgui 1.0;
//import QtQuick.Dialogs 1.3

Rectangle
{
    id: redRect;

    anchors.fill: parent;

    width: 300;
    height: width;

    color: "red";

    ThumbnailDecorator {
        id: thumbnailDecorator;

        anchors.fill: parent;

        Component.onCompleted: {
          //  console.log("ThumbnailDecorator onCompleted", MeterEnum.ID);
            thumbnailDecorator.updateModels();
        }
    }
}
