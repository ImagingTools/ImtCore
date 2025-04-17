// pragma Singleton
import QtQuick 2.12
// import Acf 1.0
import com.imtcore.imtqml 1.0
import QtQuick.Controls 2.15

Item {
    property string styleId: "StyleQt"

    property Component buttonDecorator: buttonDecoratorComp
    property Component toolButtonDecorator: toolButtonDecoratorComp
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
    property Component scrollBarDecorator: scrollBarDecoratorComp
    property Component itemDelegateDecorator: itemDelegateDecoratorComp

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

    Component{
        id: toolButtonDecoratorComp;

        ToolButton{
            width: 160;
            height: 40;
            text: !baseElement ? "" : baseElement.text;
            icon.source: !baseElement ? "" : baseElement.iconSource;
            checkable: !baseElement ? false : baseElement.checkable;
            checked: !baseElement ? false : !baseElement.checkable ? false : baseElement.checked;
            highlighted: !baseElement ? false : baseElement.focus;

            property var baseElement;
            onClicked: {
                if(baseElement){
                    baseElement.clicked()
                }
            }

            onToggled: {
                if(checked){
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
        id: itemDelegateDecoratorComp;

        ItemDelegate {
            width: parent.width;
            height: 30;

            text: !baseElement ? "" : baseElement.text;
            highlighted: !baseElement ? false : baseElement.highlighted;
            checkable: !baseElement ? false : baseElement.checkable;
            checked: !baseElement ? false : !baseElement.checkable ? false : baseElement.checked;
            down: !baseElement ? false : baseElement.down;

            property var baseElement;
            onClicked: {
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

            textRole: !baseElement ? "name" : baseElement.nameId;
            displayText: currentIndex >= 0 ? currentText : "ComboBox"


            model: !baseElement ? 0 : baseElement.model;

            property var baseElement;

            onModelChanged: {
                if(model){
                    currentIndex = -1;
                }
            }

            onActivated: {
                var id = model.getData("id", currentIndex);
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

            TextArea {
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


    Component {
        id: spinBoxDecoratorComp;
        SpinBox {
            id: spinBoxItem;

            property var baseElement: null;

            value: !baseElement ? 0 : baseElement.startValue;

            from: !baseElement ? 0 : baseElement.from;
            to: !baseElement ? 99 : baseElement.to;
            stepSize: !baseElement ? 1 : baseElement.stepSize;

            editable: !baseElement ? true : baseElement.editable;
            wrap: !baseElement ? false : baseElement.wrap;

            onValueChanged: {
                if(baseElement){
                    baseElement.value = value;
                }
            }
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

            color: "transparent";//Style.backgroundColor;
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

        Dialog {
            id: dialogContainer

            title: !baseElement ? "Title" : baseElement.title;
            standardButtons: buttonIds;

            visible: true;
            modal: baseElement ? baseElement.modal : false;

            property int buttonIds: 0;
            property var baseElement: null;
            property Item rootItem: null;
            property Item content_: null;

            property var buttonsModel: baseElement ? baseElement.buttonsModel : null;
            property int buttonsModelCount: baseElement ? baseElement.buttonsModelCount : 0;

            onBaseElementChanged: {
                if(baseElement){
                    addContentItemToDecorator();
                }
            }
            onButtonsModelCountChanged: {
                //dialogContainer.buttonIds = dialogContainer.baseElement.buttonIds;
                //dialogContainer.setDecoratorSize(dialogContainer.content_)
                buttonIdsPause.restart();
            }

            function addContentItemToDecorator(){

                var content_ = dialogContainer.baseElement.contentComp.createObject(dialogContainer);
                dialogContainer.content_ = content_
                dialogContainer.contentItem.children.push(content_);
                dialogContainer.baseElement.decoratorItem = content_;
                dialogContainer.rootItem = dialogContainer.baseElement;

                //buttonIdsPause.restart();
                //dialogContainer.buttonIds = dialogContainer.baseElement.buttonIds;
                dialogContainer.setDecoratorSize(content_)


            }


            function setDecoratorSize(content_){
                if(!content_){
                    return;
                }
                let width_  = content_.width +
                    dialogContainer.leftPadding +
                    dialogContainer.rightPadding;
                dialogContainer.width = Math.max(width_, dialogContainer.footer.width);

                let hasButtons = dialogContainer.buttonIds !== 0;
                dialogContainer.height = content_.height +
                        dialogContainer.header.height +
                        dialogContainer.footer.height * hasButtons +
                        dialogContainer.topPadding +
                        dialogContainer.bottomPadding;

                content_.x = (dialogContainer.width - content_.width)/2 -
                        dialogContainer.leftPadding;
            }


            PauseAnimation {
                id: buttonIdsPause;

                duration: 20;
                onFinished: {
                    dialogContainer.buttonIds = dialogContainer.baseElement.buttonIds;
                    dialogContainer.setDecoratorSize(dialogContainer.content_)

                }
            }

        }
    }

    Component {
        id: scrollBarDecoratorComp;

        ScrollBar{
            id: scrollBarItem;

            anchors.top : !baseElement ? undefined : orientation == Qt.Vertical ? baseElement.top : undefined;
            anchors.left : !baseElement ? undefined : orientation == Qt.Horizontal ? baseElement.left : undefined;
            anchors.bottom : !baseElement ? undefined : baseElement.bottom;
            anchors.right : !baseElement ? undefined : baseElement.right;

            active: false;
            orientation: !baseElement ? Qt.Vertical : baseElement.vertical ? Qt.Vertical : Qt.Horizontal;
            size: !targetItem || targetItem.contentHeight === 0 || targetItem.contentWidth === 0 ? 0 : orientation == Qt.Vertical ? targetItem.height / targetItem.contentHeight : targetItem.width / targetItem.contentWidth;
            policy: ScrollBar.AlwaysOn;


            property var baseElement: null;
            property var targetItem: !baseElement ? null : baseElement.targetItem;

            onPositionChanged: {
                if((scrollBarItem.pressed || scrollBarItem.hovered) && baseElement){
                    baseElement.setContentPositionFunc(position);
                }
            }
        }
    }

}

