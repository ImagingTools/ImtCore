import QtQuick 2.12

Item {
    id: baseButton;

    width: loader.width;
    height: loader.height;

    property alias decoratorSource: loader.source;
    property alias decorator: loader.sourceComponent;

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

    property bool containsMouse: enabled ? ma.containsMouse : false;


    signal clicked();
    signal doubleClicked();
    signal pressed();
    signal released();
    signal entered();
    signal exited();

    onClicked: {

    }
    onDoubleClicked: {

    }
    onPressed: {
        //baseButton.isPressed = !baseButton.isPressed;
        baseButton.highlighted = true;
    }
    onReleased: {
        baseButton.highlighted = false;
    }
    onEntered: {

    }
    onExited: {

    }


    onVisibleChanged: {
        if(loader.item){
            loader.item.visible = baseButton.visible;
        }
    }

    onTextChanged: {
        if(loader.item){
            loader.item.text = baseButton.text;
        }
    }
    onImageSourceChanged: {
        if(loader.item && baseButton.imageSource !== ""){
            loader.item.imageSource = baseButton.imageSource;
        }
    }
    onActiveChanged: {
        if(loader.item){
            loader.item.active = baseButton.active;
        }
    }
    onFocusedChanged: {
        if(loader.item){
            loader.item.focused = baseButton.focused;
        }
    }
    onIsPressedChanged: {
        if(loader.item){
            loader.item.isPressed = baseButton.isPressed;
        }
    }
    onHoveredChanged: {
        if(loader.item){
            loader.item.hovered = baseButton.hovered;
        }
    }
    onMainButtonChanged: {
        if(loader.item){
            loader.item.mainButton = baseButton.mainButton;
        }
    }
    onEnabledChanged: {
        if(loader.item){
            loader.item.enabled = baseButton.enabled;
        }
    }
    onHighlightedChanged: {
        if(loader.item){
            loader.item.highlighted = baseButton.highlighted;
        }
    }


    Loader{
        id: loader;

        visible: baseButton.visible;

        onLoaded: {

            loader.item.text = baseButton.text;
            loader.item.imageSource = baseButton.imageSource;
            loader.item.active = baseButton.active;
            loader.item.focused = baseButton.focused;
            loader.item.isPressed = baseButton.isPressed;
            loader.item.hovered = baseButton.hovered;
            loader.item.mainButton = baseButton.mainButton;
            loader.item.enabled = baseButton.enabled;
            loader.item.highlighted = baseButton.highlighted;

            loader.width = loader.item.width;
            loader.width = loader.item.width;

            loader.item.visible = baseButton.visible;

        }

    }

    MouseArea{
        id: ma;

        anchors.fill: parent;

        enabled: baseButton.enabled;
        hoverEnabled: enabled;
        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;

        onClicked: {
            baseButton.clicked();
        }
        onDoubleClicked: {
            baseButton.doubleClicked();
        }
        onPressed: {
            baseButton.pressed;
        }
        onReleased: {
            baseButton.released();
        }
        onEntered: {
            baseButton.entered();
        }
        onExited: {
            baseButton.exited();
        }

    }
    
}

