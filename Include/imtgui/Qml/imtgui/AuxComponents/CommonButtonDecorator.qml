import QtQuick 2.12
import Acf 1.0
import QtGraphicalEffects 1.0

Item {
    id: commonButtonDecorator;


    //required parameters in every decorator
    property string text: "";
    property string imageSource: "";

    property bool active: false;
    property bool focused: false;
    property bool isPressed: false;
    property bool hovered: containsMouse;
    property bool mainButton: false;
    property bool highlighted: false;
    //



}
