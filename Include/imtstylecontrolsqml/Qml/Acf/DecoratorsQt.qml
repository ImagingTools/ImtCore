pragma Singleton
import QtQuick 2.12
import Acf 1.0
import QtQuick.Controls 2.15

Item {
    property Component buttonDecorator: buttonDecoratorComp
    property Component roundButtonDecorator: roundButtonDecoratorComp

    property Component textFieldDecorator: textFieldDecoratorComp
    property Component textEditDecorator: textEditDecoratorComp

    property Component comboBoxDecorator: comboBoxDecoratorComp
    property Component checkBoxDecorator: checkBoxDecoratorComp
    property Component switchDecorator: switchDecoratorComp
    property Component sliderDecorator: sliderDecoratorComp
    property Component rangeSliderDecorator: rangeSliderDecoratorComp
    property Component radioButtonDecorator: radioButtonDecoratorComp

    property Component textAreaDecorator: textAreaDecoratorComp
    property Component spinBoxDecorator: spinBoxDecoratorComp

    property Component dialogDecorator: dialogDecoratorComp

    property Component progressBarDecorator: progressBarDecoratorComp
    property Component busyIndicatorDecorator: busyIndicatorDecoratorComp
    property Component tooltipDecorator: tooltipDecoratorComp

    property Component splitViewDecorator: splitViewDecoratorComp
    property Component stackViewDecorator: stackViewDecoratorComp
    property Component swipeViewDecorator: swipeViewDecoratorComp


    Component {
        id: buttonDecoratorComp

        Button{
            width: 160;
            height: 40;
            text: !baseElement ? "" : baseElement.text;
            icon.source: !baseElement ? "" : baseElement.iconSource;
            checkable: !baseElement ? false : baseElement.checkable;
            checked: !baseElement ? false : !baseElement.checkable ? false : baseElement.checked;
            highlighted: !baseElement ? false : baseElement.focus;
            property var baseElement;
            onClicked: {
                //console.log("Qt Button clicked!!!")
                if(baseElement){
                    baseElement.clicked()
                }
            }
            onToggled: {
                if(checked){
                    console.log("Qt Button toggled!!!")
                    if(baseElement){
                        baseElement.toggled()
                    }
                }
            }

        }
    }

    Component {
        id: roundButtonDecoratorComp

        RoundButton{
            width: 40;
            height: 40;
            radius: width;
            text: !baseElement ? "" : baseElement.text;
            icon.source: !baseElement ? "" : baseElement.iconSource;
            highlighted: !baseElement ? false : baseElement.focus;
            property var baseElement;
            onClicked: {
                //console.log("Qt Button clicked!!!")
                if(baseElement){
                    baseElement.clicked()
                }
            }

        }
    }

    Component {
        id: textFieldDecoratorComp


        TextField {
            width: 200;
            height: 40;

            z: baseElement ? 1 : -1;

            placeholderText: baseElement ? baseElement.placeHolderText : "";

            property var baseElement;

            onTextChanged: {
                if(baseElement){
                    baseElement.text = text;
                }
            }
            onAccepted: {
                if(baseElement){
                    baseElement.accepted();
                }
            }
            onEditingFinished: {
                if(baseElement){
                    baseElement.editingFinished();
                }
            }
            onTextEdited: {
                if(baseElement){
                    baseElement.textEdited();
                }
            }

            onFocusChanged: {
                console.log("focus", focus)
                if(focus){
                    forceActiveFocus();
                }
            }

        }
    }

    Component{
        id: textEditDecoratorComp;

        Flickable {
            id: flick;

            width: 200;
            height: 30;

            contentWidth: edit.paintedWidth;
            contentHeight: edit.paintedHeight;
            clip: true;
            z: baseElement ? 1 : -1;

            property var baseElement;
            property alias text: edit.text;

            onHeightChanged: {
                if(flick.baseElement){
                    flick.baseElement.height = height;
                }
            }

            onFocusChanged: {
                if(focus){
                    edit.forceActiveFocus();
                }
            }

            function ensureVisible(r)
            {
                if (contentX >= r.x)
                    contentX = r.x;
                else if (contentX+width <= r.x+r.width)
                    contentX = r.x+r.width-width;
                if (contentY >= r.y)
                    contentY = r.y;
                else if (contentY+height <= r.y+r.height)
                    contentY = r.y+r.height-height;
            }

            function setHeight(){
                //console.log("setHeight")
                height = 30;
            }

            TextEdit {
                id: edit

                width: flick.width;
                focus: true;
                wrapMode: TextEdit.Wrap;
                font.pixelSize: 15;
                clip: true;

                onCursorRectangleChanged: {
                    flick.ensureVisible(cursorRectangle);
                }

                onTextChanged: {
                    if(flick.baseElement){
                        flick.baseElement.text = text;
                    }
                }

                onEditingFinished: {
                    if(flick.baseElement){
                        flick.baseElement.editingFinished();
                    }
                }

            }
        }

    }

    Component{
        id: comboBoxDecoratorComp;

        ComboBox{
            width: 200;
            height: 30;

            topInset: 0;
            bottomInset: 0;

            textRole: "Name"
            displayText: currentIndex >= 0 ? currentText : "ComboBox"


            model: !baseElement ? 0 : baseElement.model;

            property var baseElement;

            onModelChanged: {
                if(model){
                    currentIndex = -1;
                }
            }

            onActivated: {
                var id = model.GetData("Id", currentIndex);
                console.log("Activated", currentIndex, id)
                if(baseElement){
                    baseElement.finished(id, currentIndex);
                }
            }

        }
    }

    Component{
        id: checkBoxDecoratorComp;

        CheckBox{
            height: 30;

            indicator.width: height - 10;
            indicator.height: height - 10;

            text: !baseElement ? "" : baseElement.text;
            checkState: !baseElement ? 0 : baseElement.checkState;

            property var baseElement;

            onCheckStateChanged: {
                if(baseElement){
                    baseElement.checkState = checkState;
                }
            }

        }

    }

    Component{
        id: switchDecoratorComp;

        Switch{
            height: 30;

            text: !baseElement ? "" : baseElement.text;
            checked: !baseElement ? false : baseElement.checked;

            property real backgroundWidth: contentItem.width;
            property var baseElement;

            onCheckedChanged: {
                if(baseElement){
                    baseElement.checked = checked;
                }
            }
        }
    }

    Component{
        id: sliderDecoratorComp;

        Slider{
            property var baseElement;
            property bool compl: false;

            onPositionChanged: {
                if(baseElement && compl && baseElement.from !== undefined && baseElement.to !== undefined){
                    baseElement.position = position;
                    baseElement.value = value;
                }
            }
        }
    }

    Component{
        id: rangeSliderDecoratorComp;

        RangeSlider{
            property var baseElement;
            property real positionFirst: first.position;
            property real positionSecond: second.position;
            property real valueFirst: first.value;
            property real valueSecond: second.value;
            property bool compl: false;

            onPositionFirstChanged: {
                if(baseElement && compl && baseElement.from !== undefined && baseElement.to !== undefined){
                    baseElement.positionFirst = positionFirst;
                    baseElement.valueFirst = valueFirst;
                }
            }
            onPositionSecondChanged: {
                if(baseElement && compl && baseElement.from !== undefined && baseElement.to !== undefined){
                    baseElement.positionSecond = positionSecond;
                    baseElement.valueSecond = valueSecond;
                }
            }

        }
    }

    Component{
        id: radioButtonDecoratorComp;

        RadioButton{
            id: radioButtonItem;

            property var baseElement: null;

            autoExclusive: !baseElement ? true : !baseElement.autoExclusive;
            text: !baseElement ? "" : baseElement.text;
            onCheckedChanged: {
                if(baseElement){
                    baseElement.checked = checked;
                }
            }
        }
    }

    Component {
        id: textAreaDecoratorComp;
        Item {
            id: textAreaItem;

            property var baseElement: null;
        }
    }


    Component {
        id: spinBoxDecoratorComp;
        Item {
            id: spinBoxItem;

            property var baseElement: null;
        }
    }


    Component {
        id: progressBarDecoratorComp;
        ProgressBar {
            id: progressBarItem;

            property var baseElement: null;

            indeterminate: !baseElement ? false : baseElement.indeterminate;
            from: !baseElement ? 0.0 : baseElement.from;
            to: !baseElement ? 0.0 : baseElement.to;
            value: !baseElement ? 0.0 : baseElement.value;

        }
    }

    Component {
        id: busyIndicatorDecoratorComp;

        Rectangle {
            id: busyIndicatorItem;

            anchors.fill: parent;

            color: Style.backgroundColor;
            visible: !busyIndicatorItem.baseElement ? false : busyIndicatorItem.baseElement.visible;

            property var baseElement: null;

            BusyIndicator{
                anchors.centerIn: parent;
                running: busyIndicatorItem.visible;
            }

        }

    }

    Component {
        id: tooltipDecoratorComp;
        ToolTip {
            id: tooltipItem;

            property var baseElement: null;

            parent: baseElement;

            delay: !baseElement ? 0 : baseElement.delay;
            timeout: !baseElement ? -1 : baseElement.timeout;
            text: !baseElement ? "" : baseElement.text;
        }
    }

    Component {
        id: dialogDecoratorComp;
        Item {
            id: dialogItem;

            property var baseElement: null;
        }
    }

    Component {
        id: splitViewDecoratorComp;
        Item {
            id: splitViewItem;

            width: 800;
            height: 200;

            property var baseElement: null;

            SplitView {
                anchors.fill: parent
                orientation: Qt.Horizontal

                Rectangle {
                    implicitWidth: 200
                    SplitView.maximumWidth: 400
                    color: "lightblue"
                    Label {
                        text: "View 1"
                        anchors.centerIn: parent
                    }
                }
                Rectangle {
                    id: centerItem

                    SplitView.minimumWidth: 50
                    SplitView.fillWidth: true

                    color: "lightgray"
                    Label {
                        text: "View 2"
                        anchors.centerIn: parent
                    }
                }
                Rectangle {
                    implicitWidth: 200
                    color: "lightgreen"
                    Label {
                        text: "View 3"
                        anchors.centerIn: parent
                    }
                }
            }

        }
    }


    Component {
        id: stackViewDecoratorComp;
        Item {
            id: stackViewItem;

            property var baseElement: null;
        }
    }


    Component {
        id: swipeViewDecoratorComp;
        Item {
            id: swipeViewItem;

            property var baseElement: null;
        }
    }


}
