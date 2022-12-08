import QtQuick 2.12


Item {
    id: baseButtonDecorator;

    //required parameters in every decorator
    property string text: "";
    property string imageSource: "";

    property bool isActive: false;
    property bool isFocused: false;
    property bool isPressed: false;
    property bool isHovered: containsMouse;
    property bool isMainButton: false;
    property bool isHighlighted: false;

    property bool selected: false;
    property string color: "";
    //
    
}
