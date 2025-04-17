import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0


DecoratorBase{
    id: dec;

    property alias icon: image;
    property alias color: background.color;
    property alias border: background.border;

    Rectangle{
        id: background;

        width: dec.baseElement ? dec.baseElement.width : 0;
        height: dec.baseElement ? dec.baseElement.height : 0;
        radius: dec.baseElement ? dec.baseElement.height : 0;

        color: "transparent"
    }
    Image{
        id: image;

        anchors.centerIn:  parent;

        width: dec.baseElement ? dec.baseElement.width : 0;
        height: dec.baseElement ? dec.baseElement.height : 0;

        sourceSize.width: width;
        sourceSize.height: height;

        source: dec.baseElement.iconSource;
    }
}



