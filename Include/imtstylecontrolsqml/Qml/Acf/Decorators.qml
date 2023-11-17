pragma Singleton
import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Item {
    property Component topButtonDecorator: topButtonDecoratorComp
    property Component topPanelDecorator: topPanelDecoratorComp

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


    Component {
        id: topButtonDecoratorComp

        TopButtonDecoratorNew{

        }
    }

    Component{
        id: topPanelDecoratorComp;

        TopPanelDecorator{

        }

    }

    Component{
        id: buttonDecoratorComp;

        ButtonDecorator{
            width: 160;
            height: 30;
            radius: 4;

        }
    }

    Component{
        id: roundButtonDecoratorComp;

        ButtonDecorator{
            width: 30;
            height: width;
            radius: width;
        }
    }

    Component{
        id: textFieldDecoratorComp;

        Item{
            id: textFieldItem;

            width: 200;
            height: 30;
            z: -1;

            property var baseElement;

            property string text: "";


            Rectangle {
                id: mainRect;

                anchors.fill: textFieldItem;

                color: !textFieldItem.baseElement ? "transparent" : textFieldItem.baseElement.readOnly ? Style.alternateBaseColor : Style.baseColor;

                radius: !textFieldItem.baseElement ? 0 : textFieldItem.baseElement.radius;

                border.color: !textFieldItem.baseElement ? "transparent" : textFieldItem.baseElement.borderColorConst !== "" ? textFieldItem.baseElement.borderColorConst: textFieldItem.baseElement.activeFocus ? textFieldItem.baseElement.borderColor : Style.hover;

                border.width: 1;
            }

            Text {
                id: placeHolder;

                anchors.left: textFieldItem.left;
                anchors.leftMargin: !textFieldItem.baseElement ? 0 : textFieldItem.baseElement.margin;
                anchors.verticalCenter: textFieldItem.verticalCenter;


                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;

                //TODO -> Style
                color: !textFieldItem.baseElement ? "transparent" : textFieldItem.baseElement.placeHolderFontColor;
                opacity: !textFieldItem.baseElement ? 0 : textFieldItem.baseElement.placeHolderOpacity;

                visible: !textFieldItem.baseElement ? false : textFieldItem.baseElement.text == "";

                text: !textFieldItem.baseElement ? "" : textFieldItem.baseElement.placeHolderText;


            }

            MouseArea {
                id: mouseArea;

                anchors.fill: parent;
                cursorShape: !textFieldItem.baseElement ? Qt.ArrowCursor : textFieldItem.baseElement.readOnly ? Qt.ArrowCursor : Qt.IBeamCursor;

                visible: !textFieldItem.baseElement ? false : !textFieldItem.baseElement.readOnly;

                acceptedButtons: Qt.RightButton;

                onClicked: {
                    let point = mapToItem(null, mouse.x, mouse.y);
                    console.log("inputActions");
                    if(textFieldItem.baseElement){
                        textFieldItem.baseElement.openInputAction(point.x, point.y)
                    }
                }
            }
        }
    }

    Component{
        id: textEditDecoratorComp;

        Item{
            id: textEditItem;

            width: 200;
            height: 30;
            z: -1;

            property var baseElement;
            property string text: "";

            function setHeight(){
                //console.log("setHeight");
            }

            Rectangle {
                id: mainRect;

                anchors.fill: parent;

                color: !textEditItem.baseElement ? "transparent" : textEditItem.baseElement.readOnly ? Style.alternateBaseColor : Style.baseColor;

                radius: !textEditItem.baseElement ? 0 : textEditItem.baseElement.radius;

                border.color: !textEditItem.baseElement ? "transparent" : textEditItem.baseElement.borderColorConst !== "" ? textEditItem.baseElement.borderColorConst: textEditItem.baseElement.activeFocus ? textEditItem.baseElement.borderColor : Style.hover;

                border.width: 1;
            }

            Text {
                id: placeHolder;

                anchors.left: parent.left;
                anchors.leftMargin: !textEditItem.baseElement ? 0 : textEditItem.baseElement.margin;
                anchors.verticalCenter: parent.verticalCenter;


                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;

                //TODO -> Style
                color: !textEditItem.baseElement ? "transparent" : textEditItem.baseElement.placeHolderFontColor;
                opacity: !textEditItem.baseElement ? 0 : textEditItem.baseElement.placeHolderOpacity;

                visible: !textEditItem.baseElement ? false : textEditItem.baseElement.text == "";

                text: !textEditItem.baseElement ? "" : textEditItem.baseElement.placeHolderText;
            }

            MouseArea {
                id: mouseArea;

                anchors.fill: parent;
                cursorShape: !textEditItem.baseElement ? Qt.ArrowCursor : textEditItem.baseElement.readOnly ? Qt.ArrowCursor : Qt.IBeamCursor;

                visible: !textEditItem.baseElement ? false : !textEditItem.baseElement.readOnly;

                acceptedButtons: Qt.RightButton;

                onClicked: {
                    let point = mapToItem(null, mouse.x, mouse.y);
                    console.log("inputActions");
                    if(textEditItem.baseElement){
                        textEditItem.baseElement.openInputAction(point.x, point.y)
                    }
                }
            }
        }
    }

    Component{
        id: comboBoxDecoratorComp;

        Rectangle {
            id: cbMainRect;

            width: 200;
            height: 30;

            property var baseElement;
            property var model;
            property int currentIndex: -1;


            border.color: !baseElement ? "transparent" : baseElement.borderColor;
            border.width: 1;

            radius: !baseElement ? 0 : baseElement.radius;
            color: !baseElement ? "transparent" : baseElement.backgroundColor;

            gradient: Gradient {
                id: grad;
                property bool isColor: !baseElement ? false : baseElement.isColor;

                GradientStop { position: 0.0; color: grad.isColor ? cbMainRect.color : Style.imagingToolsGradient1; }
                GradientStop { position: 0.97; color: grad.isColor ? cbMainRect.color : Style.imagingToolsGradient2; }
                GradientStop { position: 0.98; color: grad.isColor ? cbMainRect.color : Style.imagingToolsGradient3; }
                GradientStop { position: 1.0; color: grad.isColor ? cbMainRect.color : Style.imagingToolsGradient4; }

            }

            Text {
                id: cbTitleTxt;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: !baseElement ? 0 : baseElement.textCentered ? parent.width/2 - width/2 : 10;
                anchors.right: parent.right;
                anchors.rightMargin: 10;

                color: !baseElement ? "transparent" : baseElement.fontColorTitle;
                text: !baseElement ? "" : baseElement.currentText !== "" ? baseElement.currentText : baseElement.placeHolderText;
                font.family: Style.fontFamily;
                font.pixelSize: !baseElement ? 10 : baseElement.textSize;

                elide: Text.ElideRight;
            }

            Image {
                id: cbArrowIcon;

                anchors.right: cbMainRect.right;
                anchors.verticalCenter: cbMainRect.verticalCenter;
                anchors.rightMargin: 5;

                width: 12;
                height: 10;
                rotation: !baseElement ? 0 : baseElement.isOpen ? 180 : 0
                source: "../../../" + Style.theme + "/Icons/" + "/" + "Down" + "_On_Normal.svg";

                sourceSize.width: width;
                sourceSize.height: height;
            }
        }

    }

    Component{
        id: checkBoxDecoratorComp;

        Item{
            id: checkBoxItem;

            width: checkBoxText.text == "" ? checkRect.width : checkRect.width + checkBoxText.width + checkBoxItem.mainMargin;
            height: 20;

            property var baseElement: null;

            property int mainMargin: !checkBoxItem.baseElement ? 0 : 8;
            property string text: !baseElement ? "" : baseElement.text == undefined ? "" : baseElement.text;
            property string borderColor: !baseElement ? "" : baseElement.borderColor == undefined ? "" : baseElement.borderColor;
            property int fontPixelSize: !baseElement ? 10 : baseElement.fontPixelSize == undefined ? 10 : baseElement.fontPixelSize;
            property bool fontBold: !baseElement ? false : baseElement.fontBold == undefined ? false : baseElement.fontBold;

            Rectangle {
                id: checkRect;

                anchors.left: parent.left;
                anchors.leftMargin: !checkBoxItem.baseElement ? 0 : !checkBoxItem.baseElement.isLeftText ? 0 : checkBoxText.width + checkBoxItem.baseElement.mainMargin;
                anchors.verticalCenter: parent.verticalCenter;

                width: height;
                height: parent.height;

                color: "transparent";

                border.width: 1;
                border.color: !checkBoxItem.baseElement ? "transparent" : checkBoxItem.borderColor !=="" ? checkBoxItem.borderColor  : checkBoxItem.baseElement.isActive ? Style.borderColor : Style.disabledInActiveTextColor;

                Image {
                    id: image;

                    anchors.centerIn: parent;

                    height: checkRect.height - 1;
                    width: height;

                    sourceSize.width: width;
                    sourceSize.height: height;

                    visible: !checkBoxItem.baseElement ? false : checkBoxItem.baseElement.checkState != Qt.PartiallyChecked;
                    source: !checkBoxItem.baseElement ? "" : checkBoxItem.baseElement.isActive ? checkBoxItem.baseElement.checkState == Qt.Checked ? checkBoxItem.baseElement.imageSourceActive : "" :
                    checkBoxItem.baseElement.checkState == Qt.Checked ? checkBoxItem.baseElement.imageSourceNotActive : "";
                }
            }

            Rectangle {
                id: rect;

                anchors.centerIn: checkRect;

                height: checkRect.height - 4;
                width: height;

                color: Style.textColor;

                visible: !checkBoxItem.baseElement ? false : checkBoxItem.baseElement.checkState == Qt.PartiallyChecked;
            }



            Text {
                id: checkBoxText;

                anchors.left: parent.left;
                anchors.leftMargin: !checkBoxItem.baseElement ? 0 : checkBoxItem.baseElement.isLeftText ? 0 : checkRect.width + checkBoxItem.baseElement.mainMargin;
                anchors.verticalCenter: parent.verticalCenter;

                color: !checkBoxItem.baseElement ? "transparent" :  checkBoxItem.baseElement.enabled ? Style.buttonText : Style.inactive_buttonText;

                font.pixelSize: checkBoxItem.fontPixelSize;
                font.family: Style.fontFamily;
                font.bold: checkBoxItem.fontBold;

                text: checkBoxItem.text;
            }

        }

    }

    Component{
        id: switchDecoratorComp;

        Item{
            id: switchItem;

            width: switchText.text == "" ? backgroundRec.width : backgroundRec.width + switchText.width + switchItem.mainMargin;
            height: 20;

            property var baseElement: null;
            property bool checked: !baseElement ? false : baseElement.checked;

            property int mainMargin: !baseElement ? 0 : 8;
            property string text: !baseElement ? "" : baseElement.text == undefined ? "" : baseElement.text;
            property string borderColor: !baseElement ? "" : baseElement.borderColor == undefined ? "" : baseElement.borderColor;
            property int fontPixelSize: !baseElement ? 10 : baseElement.fontPixelSize == undefined ? 10 : baseElement.fontPixelSize;
            property bool fontBold: !baseElement ? false : baseElement.fontBold == undefined ? false : baseElement.fontBold;

            property real backgroundWidth: 40;

            Rectangle{
                id: backgroundRec;

                anchors.left: parent.left;
                anchors.verticalCenter: parent.verticalCenter;

                width: switchItem.backgroundWidth;
                height: !switchItem.baseElement ? 0 : switchItem.baseElement.backgroundHeight;
                color: !switchItem.baseElement ? "transparent" : switchItem.checked ? switchItem.baseElement.backgroundColor_checked :
                                                                                      switchItem.baseElement.backgroundColor;
                radius: height;

                opacity: !switchItem.baseElement ? 0 : switchItem.baseElement.backgroundOpacity;

            }

            Rectangle{
                id: controlRec;

                anchors.verticalCenter: parent.verticalCenter;

                x: !switchItem.baseElement ? 0 : switchItem.baseElement.controlRecX;
                width: parent.height;
                height: width;
                radius: width;
                color: !switchItem.baseElement ? "transparent" : switchItem.checked ? switchItem.baseElement.controlColor_checked :
                                                                                      switchItem.baseElement.controlColor;
            }

            Text {
                id: switchText;

                anchors.left: parent.left;
                anchors.leftMargin: !switchItem.baseElement ? 0 : switchItem.baseElement.isLeftText ? 0 : backgroundRec.width + switchItem.baseElement.mainMargin;
                anchors.verticalCenter: parent.verticalCenter;

                color: !switchItem.baseElement ? "transparent" :  switchItem.baseElement.enabled ? Style.buttonText : Style.inactive_buttonText;

                font.pixelSize: switchItem.fontPixelSize;
                font.family: Style.fontFamily;
                font.bold: switchItem.fontBold;

                text: switchItem.text;
            }
        }

    }

    Component{
        id: sliderDecoratorComp;

        Item{
            id: sliderItem;

            width: 150;
            height: 20;

            property var baseElement: null;
            property real value: 0;

            property string borderColor: !baseElement ? "" : baseElement.borderColor == undefined ? "" : baseElement.borderColor;

            Rectangle{
                id: backgroundRec;

                anchors.left: parent.left;
                anchors.verticalCenter: parent.verticalCenter;

                width: parent.width;
                height: !sliderItem.baseElement ? 0 : sliderItem.baseElement.backgroundHeight;
                color: !sliderItem.baseElement ? "transparent" : sliderItem.baseElement.backgroundColor;

                radius: height;

                opacity: !sliderItem.baseElement ? 0 : sliderItem.baseElement.backgroundOpacity;

                Rectangle{
                    width: !sliderItem.baseElement ? 0 : sliderItem.baseElement.controlRecX + sliderItem.baseElement.height/2;
                    height: parent.height;
                    radius: parent.radius;
                    color: !sliderItem.baseElement ? "transparent" : sliderItem.baseElement.controlColor;
                }

            }

            Rectangle{
                id: controlRec;

                anchors.verticalCenter: parent.verticalCenter;

                x: !sliderItem.baseElement ? 0 : sliderItem.baseElement.controlRecX;
                width: parent.height;
                height: width;
                radius: width;
                color: !sliderItem.baseElement ? "transparent" : sliderItem.baseElement.controlColor;

            }

        }
    }

    Component{
        id: rangeSliderDecoratorComp;

        Item{
            id: rangeSliderItem;

            width: 150;
            height: 20;

            property var baseElement: null;
            property real value: 0;

            property string borderColor: !baseElement ? "" : baseElement.borderColor == undefined ? "" : baseElement.borderColor;

            Rectangle{
                id: backgroundRec;

                anchors.left: parent.left;
                anchors.verticalCenter: parent.verticalCenter;

                width: parent.width;
                height: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.backgroundHeight;
                color: !rangeSliderItem.baseElement ? "transparent" : rangeSliderItem.baseElement.backgroundColor;

                radius: height;

                opacity: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.backgroundOpacity;

                Rectangle{
                    x: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.controlRecXFirst + rangeSliderItem.baseElement.height/2
                    width: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.controlRecXSecond - rangeSliderItem.baseElement.controlRecXFirst;
                    height: parent.height;
                    radius: parent.radius;
                    color: !rangeSliderItem.baseElement ? "transparent" : rangeSliderItem.baseElement.controlColor;
                }

            }

            Rectangle{
                id: controlRecFirst;

                anchors.verticalCenter: parent.verticalCenter;

                x: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.controlRecXFirst;
                width: parent.height;
                height: width;
                radius: width;
                color: !rangeSliderItem.baseElement ? "transparent" : rangeSliderItem.baseElement.controlColor;

            }

            Rectangle{
                id: controlRecSecond;

                anchors.verticalCenter: parent.verticalCenter;

                x: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.controlRecXSecond;
                width: parent.height;
                height: width;
                radius: width;
                color: "green"//!rangeSliderItem.baseElement ? "transparent" : rangeSliderItem.baseElement.controlColor;

            }

        }
    }

    Component{
        id: radioButtonDecoratorComp;

        Item{
            id: radioButtonItem;

            width: radioButtonItem.text == "" ? radioRect.width :
                                                radioRect.width + radioButtonText.width + radioButtonText.anchors.leftMargin;
            height: Math.max(checkSize, radioButtonText.height);

            property var baseElement: null;
            property int checkSize: !baseElement ? 0 : baseElement.checkSize;
            property string text: !baseElement ? "" : baseElement.text;

            Rectangle {
                id: radioRect;

                anchors.verticalCenter: parent.verticalCenter;

                height: radioButtonItem.checkSize;
                width: height;
                radius: height;

                color: "transparent";

                border.width: 2;
                border.color: !radioButtonItem.baseElement ? "transparent" : radioButtonItem.baseElement.selectedColor;


                Rectangle{
                    id: circle

                    anchors.centerIn: parent;

                    height: radioRect.height - 2*radioRect.border.width;
                    width: height;
                    radius: width;
                    border.color: Style.borderColor;
                    border.width: 1;
                    color: "lightgrey";//Style.color_shadow;

                    Rectangle{
                        id: selectCircle

                        anchors.centerIn: parent;

                        height: circle.height - 8;
                        width: height;
                        radius: width;
                        color: !radioButtonItem.baseElement ? "transparent" : radioButtonItem.baseElement.selectedColor;
                        visible: !radioButtonItem.baseElement ? false : radioButtonItem.baseElement.checkState == Qt.Checked;
                    }
                }

            }

            Text {
                id: radioButtonText;

                anchors.left: radioRect.right;
                anchors.leftMargin: !radioButtonItem.baseElement ? 0 : radioButtonItem.baseElement.mainMargin;
                anchors.verticalCenter: radioButtonItem.verticalCenter;

                color: !radioButtonItem.baseElement ? "transparent" : radioButtonItem.baseElement.enabled ? Style.buttonText : Style.inactive_buttonText;

                font.pixelSize: !radioButtonItem.baseElement ? 8 : radioButtonItem.baseElement.fontPixelSize;
                font.family: Style.fontFamily;
                font.bold: !radioButtonItem.baseElement ? false : radioButtonItem.baseElement.fontBold;

                text: radioButtonItem.text;
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
        SpinBoxComp{}
    }


    Component {
        id: progressBarDecoratorComp;
        Item {
            id: progressBar;

            width: 300;
            height: progressContainer.y + progressContainer.height;

            property int barHeight: !baseElement ? 30 : baseElement.barHeight;

            property bool hasText: !baseElement ? true : baseElement.hasText;
            property bool hasTitle: !baseElement ? true : baseElement.hasTitle;

            property string title: !baseElement ? qsTr("Прогресс:") : baseElement.title;
            property string text: !baseElement ? "" : baseElement.text;

            property int percent: !baseElement ? 0 : baseElement.percent;
            property real from: !baseElement ? 0.0 : baseElement.from;
            property real to: !baseElement ? 1.0 : baseElement.to;
            property real value: !baseElement ? 0.0 : baseElement.value;
            property real position: !baseElement ? 0.0 : baseElement.position;
            property bool indeterminate: !baseElement ? false : baseElement.indeterminate;

            property int barRadius: 4;
            property string barColor: "lightgreen";
            property string borderColor: "lightgrey";
            property string backgroundColor: "#ffffff";

            property var baseElement: null;

            Component.onCompleted: {
                if(indeterminate){
                    animX.start();
                }
                if(barHeight > height){
                    barHeight = height;
                }
            }

            onIndeterminateChanged: {
                if(indeterminate){
                    animX.start();
                }
                else {
                    animX.stop();
                    animWidthTo.stop();
                    animWidthPause.stop();
                    animWidthFrom.stop();
                    animX2.stop();
                    animWidthTo2.stop();
                    animWidthPause2.stop();
                    animWidthFrom2.stop();
                    barIndeterminate1.x = 0;
                    barIndeterminate2.x = 0;
                    barIndeterminate1.width = 0;
                    barIndeterminate2.width = 0;
                }
            }

            onHeightChanged: {
                if(barHeight > height){
                    barHeight = height;
                }
            }

            Text {
                id: mainTitle;

                anchors.top: parent.top;

                color: Style.color_text_common !==undefined ? Style.color_text_common : "#000000";
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_subtitle;
                visible: !progressBar.hasTitle ? false : !progressBar.title ? false : true;

                text: qsTr(progressBar.title);

            }

            Text {
                id: progressText;

                anchors.top: parent.top;
                anchors.topMargin: mainTitle.visible * (mainTitle.height + 10);

                color: Style.color_text_common !==undefined ? Style.color_text_common : "#000000";
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_subtitle;
                visible: !progressBar.hasText ? false : !progressBar.text ? false : true;

                text: !progressBar.text ? "" : !progressBar.indeterminate ? qsTr("Выполнено") + ": " + progressBar.text :
                                                                            qsTr("Выполнено") + "...";

            }

            Rectangle{
                id: progressContainer;

                anchors.top: parent.top;
                anchors.topMargin: mainTitle.visible * mainTitle.height +
                                   progressText.visible * (progressText.height + mainTitle.visible * 10)
                                   + !onlyBar * 10;
                anchors.left: parent.left;
                anchors.right: parent.right;

                height: progressBar.barHeight;
                radius: progressBar.barRadius;
                color: progressBar.backgroundColor;
                border.width: 1;
                border.color: progressBar.borderColor;
                property bool onlyBar: !mainTitle.visible && !progressText.visible;

                Rectangle{
                    id: bar;

                    anchors.left: parent.left;
                    anchors.leftMargin: progressContainer.border.width;
                    anchors.verticalCenter: parent.verticalCenter;

                    height: parent.height - 2* anchors.leftMargin;
                    width: progressBar.percent < 0 ? 0 : progressBar.percent > 100 ?
                                                         (parent.width - 2* anchors.leftMargin) :
                                                         (parent.width - 2* anchors.leftMargin) * progressBar.percent/100;

                    radius: parent.radius;
                    color: progressBar.barColor;
                    visible: !progressBar.indeterminate;

                }

                Rectangle{
                    id: barIndeterminate1;

                    anchors.verticalCenter: parent.verticalCenter;

                    height: parent.height - 2* progressContainer.border.width;
                    width: 0;

                    radius: parent.radius;
                    color: progressBar.barColor;
                    visible: progressBar.indeterminate;
                    property real maxWidth: parent.width/5;

                }

                Rectangle{
                    id: barIndeterminate2;

                    anchors.verticalCenter: parent.verticalCenter;

                    height: parent.height - 2* progressContainer.border.width;
                    width: 0;

                    radius: parent.radius;
                    color: progressBar.barColor;
                    visible: progressBar.indeterminate;
                    property real maxWidth: parent.width/5;

                }

            }

            //animations
            NumberAnimation {
                id: animX;

                target: barIndeterminate1;
                property: "x";
                duration: 1200;
                from: 0;
                to: barIndeterminate1.parent.width;
                onStarted: {
                    if(progressBar.indeterminate){
                        animWidthTo.start();
                    }
                }
            }

            NumberAnimation {
                id: animWidthTo;

                target: barIndeterminate1;
                property: "width";
                duration: animX.duration/4;
                from: barIndeterminate1.maxWidth/2;
                to: barIndeterminate1.maxWidth;
                onFinished: {
                    if(progressBar.indeterminate){
                        animWidthPause.start();
                    }
                }

            }

            PauseAnimation {
                id: animWidthPause;

                duration: animX.duration/2;
                onFinished: {
                    if(progressBar.indeterminate){
                        animWidthFrom.start();
                        animX2.start();
                    }
                }
            }

            NumberAnimation {
                id: animWidthFrom;

                target: barIndeterminate1;
                property: "width";
                duration: animX.duration/4;
                from: barIndeterminate1.maxWidth;
                to: 0;

                onFinished: {
                    if(progressBar.indeterminate){
                        animX2.start();
                    }
                }

            }

            //for second rectangle
            NumberAnimation {
                id: animX2;

                target: barIndeterminate2;
                property: "x";
                duration: animX.duration/2;
                from: 0;
                to: barIndeterminate2.parent.width;
                onStarted: {
                    if(progressBar.indeterminate){
                        animWidthTo2.start();
                    }
                }
            }

            NumberAnimation {
                id: animWidthTo2;

                target: barIndeterminate2;
                property: "width";
                duration: animX2.duration/3;
                from: barIndeterminate2.maxWidth/2;
                to: barIndeterminate2.maxWidth;
                onFinished: {
                    if(progressBar.indeterminate){
                        animWidthPause2.start();
                    }
                }

            }

            PauseAnimation {
                id: animWidthPause2;

                duration: animX2.duration/3;
                onFinished: {
                    if(progressBar.indeterminate){
                        animWidthFrom2.start();
                    }
                }
            }

            NumberAnimation {
                id: animWidthFrom2;

                target: barIndeterminate2;
                property: "width";
                duration: animX2.duration/3;
                from: barIndeterminate2.maxWidth;
                to: 0;
                onFinished: {
                    if(progressBar.indeterminate){
                        animX.start();
                    }
                }

            }

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

            Item { // square
                id: square;

                anchors.centerIn: parent
                property double minimum: Math.min(busyIndicatorItem.width, busyIndicatorItem.height);
                width: 50;
                height: 50;

                Repeater {
                    id: repeater;

                    model: 8;

                    delegate: Rectangle{
                        color: Style.textColor;

                        property double b: 0.1;
                        property double a: 0.25;

                        width: ((b - a) / repeater.count * index + a) * square.height;  height: width;
                        radius: 0.5 * height;

                        x: 0.5 * square.width  + 0.5 * (square.width  - width )  * Math.cos(2 * Math.PI / repeater.count * model.index) - 0.5 * width;
                        y: 0.5 * square.height - 0.5 * (square.height - height)  * Math.sin(2 * Math.PI / repeater.count * model.index) - 0.5 * height;
                    }
                }
            }

            MouseArea {
                anchors.fill: parent;
            }

            Timer {
                interval: 10;
                running: busyIndicatorItem.visible;
                repeat:  true;

                onTriggered: {
                    square.rotation += 2; // degrees
                }
            }
        }

    }

    Component {
        id: tooltipDecoratorComp;
        Item {
            id: tooltipItem;

            property var baseElement: null;
        }
    }

    Component {
        id: dialogDecoratorComp;
        Item {
            id: dialogItem;

            property var baseElement: null;
        }
    }

}
