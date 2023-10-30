import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0

Item {
    id: progressBar;

    width: 400;
    height: mainTitle.height + progressText.height + progressText.anchors.topMargin +
            progressContainer.height+ progressContainer.anchors.topMargin;

    property int percent: 25;

    property int barHeight: 40;
    property int barRadius: 4;
    property string barColor: "lightgreen";
    property string borderColor: "lightgrey";

    Text {
        id: mainTitle;

        anchors.top: parent.top;
        anchors.left: parent.left;

        color: Style.color_text_common;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;

        text: qsTr("Прогресс:");

    }//

    Text {
        id: progressText;

        anchors.top: mainTitle.bottom;
        anchors.topMargin: 10;
        anchors.left: parent.left;

        color: Style.color_text_common;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_subtitle;

        text: qsTr("Выполнено") + " " + progressBar.percent + "%";

    }//

    Rectangle{
        id: progressContainer;

        anchors.top: progressText.bottom;
        anchors.topMargin: 10;
        anchors.left: parent.left;
        anchors.right: parent.right;

        height: progressBar.barHeight;
        radius: progressBar.barRadius;
        border.width: 1;
        border.color: progressBar.borderColor;

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

        }

    }

}
