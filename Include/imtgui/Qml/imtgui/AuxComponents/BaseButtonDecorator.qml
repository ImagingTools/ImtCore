import QtQuick 2.12


Item {
    id: baseButtonDecorator;

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
