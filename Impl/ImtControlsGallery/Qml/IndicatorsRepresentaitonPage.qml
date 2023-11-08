import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0


Rectangle {
    id: indicatorsRepresentaitonPage;

    anchors.fill: parent;

    Column{
        id: column;

        anchors.centerIn: parent;

        width: parent.width;
        spacing: 100;

        Row{
            id: progressBarRow;

            anchors.horizontalCenter: parent.horizontalCenter;

            height:  100;
            spacing: 100;

            ProgressBar{
                id: progressBar1;

                anchors.bottom:  parent.bottom;

                from: 10; to: 110;
                value: 40;
                text: value + " единиц (" + percent + " %)";
            }

            ProgressBar{
                id: progressBar2;

                anchors.bottom:  parent.bottom;


                from: 10; to: 110;
                value: 60;
                indeterminate: true;
                text: percent + " %";
            }

            ProgressBar{
                id: progressBar3;

                anchors.bottom:  parent.bottom;

                width: 150;
                from: 10; to: 110;
                value: 60;
                hasText: false;
                hasTitle: false;
                text: percent + " %";
            }

        }//progressBarRow

        Row{
            id: indicatorsRow;

            anchors.horizontalCenter: parent.horizontalCenter;

            height:  100;
            spacing: 100;

            BusyIndicator{
                id: busyIndicator;

                anchors.verticalCenter: parent.verticalCenter;

                width: parent.height;
                height: width;
                visible: true;
            }

            Rectangle{
                id: forTooltip;

                anchors.verticalCenter:  parent.verticalCenter;

                width: 150;
                height: 40;

                radius: 10;
                color: "lightblue";
                Text{
                    anchors.centerIn: parent;

                    text: "show tooltip";
                    font.pixelSize: 18;
                }

                MouseArea{
                    anchors.fill: parent;

                    hoverEnabled: true;
                    cursorShape: Qt.PointingHandCursor;

//                    onEntered: {
//                        tooltip.show(mouseX, mouseY);
//                    }
                    onPositionChanged: {
                        tooltip.show(mouseX, mouseY);
                    }

                    onExited: {
                        tooltip.hide();
                    }
                }
                CustomTooltip{
                    id: tooltip;

                    text: "Tooltip!!!"
                    fitToTextWidth: true;
                    componentHeight: 30;
                    timeout: 2000;
                    fitToCenter: true;
                }

            }


        }

    }

}

