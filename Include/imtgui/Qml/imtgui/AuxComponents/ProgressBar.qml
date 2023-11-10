import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: progressBar;

    width: 300;
    height: progressContainer.y + progressContainer.height;
    property int barHeight: 30;

    property bool hasText: true;
    property bool hasTitle: true;

    property string title: "Прогресс:";
    property string text: ""; //+ progressBar.percent + "%";

    property int percent: position * 100;
    property real from: 0.0;
    property real to: 1.0;
    property real value: 0.0;
    property real position: (value - from)/(to - from);
    property bool indeterminate: false;


    property int barRadius: 4;
    property string barColor: "lightgreen";
    property string borderColor: "lightgrey";
    property string backgroundColor: "#ffffff";

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

