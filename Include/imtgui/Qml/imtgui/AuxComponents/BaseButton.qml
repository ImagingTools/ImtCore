import QtQuick 2.12

Item {
    id: baseButton;

    width: 0;//loader.width;
    height: loader.height;

    property alias decoratorSource: loader.source;
    property alias decorator: loader.sourceComponent;


    //required parameters in every decorator
    property string text: "";
    property string imageSource: "";

    property bool enabled: true;

    property bool isActive: false;
    property bool isFocused: false;
    property bool isPressed: false;
    property bool isHovered: enabled ? ma.containsMouse : false;
    property bool isMainButton: false;
    property bool isHighlighted: false;
    //

    property bool containsMouse: enabled ? ma.containsMouse : false;
    property bool selected: false;
    property string color: "";

    signal clicked();
    signal doubleClicked();
    signal pressed();
    signal released();
    signal entered();
    signal exited();

    signal loaded();

    function setItemProperties(){

        baseButton.setItemPropertiesAdd()

        loader.item.text = baseButton.text;
        loader.item.imageSource = baseButton.imageSource;
        loader.item.isActive = baseButton.isActive;
        loader.item.isFocused = baseButton.isFocused;
        loader.item.isPressed = baseButton.isPressed;
        loader.item.isHovered = baseButton.isHovered;
        loader.item.isMainButton = baseButton.isMainButton;
        loader.item.enabled = baseButton.enabled;
        loader.item.isHighlighted = baseButton.isHighlighted;
        loader.item.selected = baseButton.selected;
        loader.item.color = baseButton.color;

        loader.width = loader.item.width;
        loader.height = loader.item.height;

        if(baseButton.width > 0){
            loader.item.width = baseButton.width;
        }
        else{
            baseButton.width = loader.item.width;
        }

        loader.item.visible = baseButton.visible;

        baseButton.loaded();

    }

    function setItemPropertiesAdd(){//for web

    }

    onPressed: {
        //baseButton.isPressed = !baseButton.isPressed;
        baseButton.isHighlighted = true;
    }
    onReleased: {
        baseButton.isHighlighted = false;
    }
    onClicked: {

    }
    onDoubleClicked: {

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

    onWidthChanged: {
        if(baseButton.width !== loader.width && baseButton.width > 0){
            loader.width = baseButton.width;
            if (loader.item){
                loader.item.width = baseButton.width;
            }
        }

    }

    onHeightChanged: {
        if(baseButton.height !== loader.height){
            loader.height = baseButton.height;
            if (loader.item){
                loader.item.height = baseButton.height;
            }
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
    onIsActiveChanged: {
        if(loader.item){
            loader.item.isActive = baseButton.isActive;
        }
    }
    onIsFocusedChanged: {
        baseButton.isHighlighted = baseButton.isFocused;

        if(loader.item){
            loader.item.isFocused = baseButton.isFocused;
        }
    }
    onIsPressedChanged: {
        if(loader.item){
            loader.item.isPressed = baseButton.isPressed;
        }
    }
    onIsHoveredChanged: {
        if(loader.item){
            loader.item.isHovered = baseButton.isHovered;
        }
    }
    onIsMainButtonChanged: {
        if(loader.item){
            loader.item.isMainButton = baseButton.isMainButton;
        }
    }
    onEnabledChanged: {
        if(loader.item){
            loader.item.enabled = baseButton.enabled;
        }
    }
    onIsHighlightedChanged: {
        if(loader.item){
            loader.item.isHighlighted = baseButton.isHighlighted;
        }
    }

    onSelectedChanged: {
        if(loader.item){
            loader.item.selected = baseButton.selected;
        }

    }

    onColorChanged: {
        if(loader.item && baseButton.color !== ""){
            loader.item.color = baseButton.color;
        }
    }

    Keys.onPressed: {
        console.log("Button Key pressed!")
        if (event.key === Qt.Key_Return){
            baseButton.clicked();
        }
    }

    Component{
        id: commonButtonDecorator;

        CommonButtonDecorator{
        }
    }

    Loader{
        id: loader;

        visible: baseButton.visible;
        sourceComponent: commonButtonDecorator;

        onLoaded: {
            if(loader.item.rootItem !== undefined){
                loader.item.rootItem = baseButton;
            }
            baseButton.setItemProperties();
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
            baseButton.pressed();

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

