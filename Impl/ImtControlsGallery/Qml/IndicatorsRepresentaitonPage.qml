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


            ProgressRoundIndicator{
                id: roundIndicator;

                anchors.bottom:  parent.bottom;
                width: 70;
                height: width;
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

                    onPositionChanged: {
                        if(tooltip.text !== ""){
                            tooltip.show(mouseX, mouseY);
                        }
                    }

                    onExited: {
                        if(tooltip.text !== ""){
                            tooltip.hide();
                        }
                    }
                }
                CustomTooltip{
                    id: tooltip;

                    text: "Tooltip!!!"
                    fitToTextWidth: true;
                    componentHeight: 30;
                    timeout: 2000;
                    fitToHCenter: true;
                }

            }

        }//indicatorsRow

        Row{
            id: scrollBarsRow;

            anchors.horizontalCenter: parent.horizontalCenter;

            height:  300;
            spacing: 100;

            Rectangle{
                id: listViewVertContainer;

                width: parent.height;
                height: width;

                radius: 4;
                border.color: "lightgreen";

                ListView{
                    id: listVert;

                    anchors.centerIn: parent;

                    width: parent.width - 8;
                    height: parent.height - 8;

                    clip: true;
                    boundsBehavior: Flickable.StopAtBounds;
                    spacing: 20;

                    model: 10;


                    delegate: Rectangle{
                        width: listVert.width - (scrollVert.width + 2);
                        height: 50;
                        radius: 2;
                        color: "lightblue";
                        Text{
                            anchors.centerIn: parent;

                            font.pixelSize: 18;
                            text: model.index + 1;
                        }
                    }

                 }//listVert

                CustomScrollbar{
                    id: scrollVert;

                    anchors.right: listVert.right;
                    anchors.rightMargin: 1;

                    anchors.bottom: listVert.bottom;

                    secondSize: 8;

                    targetItem: listVert;
                }

            }//listViewVertContainer

            Rectangle{
                id: listViewHorizContainer;

                width: parent.height;
                height: width;

                radius: 4;
                border.color: "lightgreen";

                ListView{
                    id: listHoriz;

                    anchors.centerIn: parent;

                    width: parent.width - 8;
                    height: parent.height - 8;

                    clip: true;
                    boundsBehavior: Flickable.StopAtBounds;
                    orientation: Qt.Horizontal;
                    spacing: 20;

                    model: 10;
                    delegate: Rectangle{
                        width: 50;
                        height: listHoriz.height - (scrollHoriz.height + 2);
                        radius: 2;
                        color: "lightblue";
                        Text{
                            anchors.centerIn: parent;

                            font.pixelSize: 18;
                            text: model.index + 1;
                        }
                    }

                }

                CustomScrollbar{
                    id: scrollHoriz;

                    anchors.right: listHoriz.right;
                    anchors.bottom: listHoriz.bottom;
                    anchors.bottomMargin: 1;

                    secondSize: 8;

                    vertical: false;
                    targetItem: listHoriz;
                }

            }//listViewHorizContainer
        }//scrollBarsRow

    }//Column

    Timer{
        id: timer;

        running: true;
        triggeredOnStart: true;
        onTriggered: {
            roundIndicatorAnim.start();
        }
        interval: 4000;
        repeat: true;
    }


    NumberAnimation {
        id: roundIndicatorAnim;

        target: roundIndicator;
        property: "percent";
        duration: 3000;
        from: 0; to: 100;
    }

}

