import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0


Rectangle {
    id: viewsRepresentaitonPage;

    anchors.fill: parent;
    clip: true;
	color: Style.baseColor

    Column{
        id: column;

        anchors.centerIn: parent;

        width: parent.width;
        spacing: 100;

        Rectangle{
            id: splitViewContainer;

            anchors.horizontalCenter: parent.horizontalCenter;

            width: splitView.orientation == Qt.Horizontal ? 800 : 200;
            height: splitView.orientation == Qt.Horizontal ? 200 : 500;
            color: "transparent";
            border.color: "lightgreen";


            SplitView{
                id: splitView;

                anchors.centerIn: parent;

                width: parent.width - 8;
                height: parent.height -8;

                //orientation: Qt.Vertical;//!!!

                Component.onCompleted: {
                    //sizeModel.setData("MinimumWidth", 50, 0);
                    sizeModel.setData("FillWidth", true, 0);
                }

                Rectangle{
                    id: red;

                    width: splitView.orientation == Qt.Horizontal ? 100 : 0;
                    height: splitView.orientation == Qt.Vertical ? 70 : 0;

                    color: "red";
                    clip: true;
                    Text {
                        anchors.centerIn: parent;
                        font.pixelSize: 14;
                        text: "SplitView red";
                    }
                }

                Rectangle{
                    id: green;

                    width: splitView.orientation == Qt.Horizontal ? 200 : 0;
                    height: splitView.orientation == Qt.Vertical ? 100 : 0;
                    color: "green";
                    clip: true;
                    Text {
                        anchors.centerIn: parent;
                        font.pixelSize: 14;
                        text: "SplitView green";
                    }
                }

                Rectangle{
                    id: yellow;

                    width: splitView.orientation == Qt.Horizontal ? 400 : 0;
                    height: splitView.orientation == Qt.Vertical ? 200 : 0;
                    color: "yellow";
                    clip: true;
                    Text {
                        anchors.centerIn: parent;
                        font.pixelSize: 14;
                        text: "SplitView yellow";
                    }
                }


            }//SplitView

        }//splitViewContainer

        Rectangle{
            id: swipeViewContainer;

            anchors.horizontalCenter: parent.horizontalCenter;

            width: 300;
            height: 300;
            color: "transparent";
            border.color: "lightgreen";
            SwipeView{
                id: swipeView;

                anchors.centerIn: parent;

                width: parent.width - 8;
                height: parent.height - 8;

                //orientation: Qt.Vertical;

                Rectangle{
                    color: "red";
                    Text {
                        anchors.centerIn: parent;
                        font.pixelSize: 14;
                        text: "SwipeView red";
                    }
                }

                Rectangle{
                    color: "green";
                    Text {
                        anchors.centerIn: parent;
                        font.pixelSize: 14;
                        text: "SwipeView green";
                    }
                }

                Rectangle{
                    color: "yellow";
                    Text {
                        anchors.centerIn: parent;
                        font.pixelSize: 14;
                        text: "SwipeView yellow";
                    }
                }
            }

            Button{
                id: leftButton;

                anchors.verticalCenter:  parent.verticalCenter;
                anchors.right: parent.left;
                anchors.rightMargin: 70;

                width: 50;
                height: width;

                decorator: Style.roundButtonDecorator;

                iconSource: "../../../" + Style.theme + "/Icons" + "/Left.svg";
                onClicked: {
                    if(swipeView.currentIndex > 0){
                        swipeView.setCoordAnimFunc(swipeView.currentIndex * swipeView.width, (swipeView.currentIndex - 1) * swipeView.width)
                        swipeView.currentIndex--;
                    }
                }
            }

            Button{
                id: rightButton;

                anchors.verticalCenter:  parent.verticalCenter;
                anchors.left: parent.right;
                anchors.leftMargin: 70;

                width: 50;
                height: width;

                decorator: Style.roundButtonDecorator;

                iconSource: "../../../" + Style.theme + "/Icons" + "/Right.svg";
                onClicked: {
                    if(swipeView.currentIndex < (swipeView.count - 1)){
                        swipeView.setCoordAnimFunc(swipeView.currentIndex * swipeView.width, (swipeView.currentIndex + 1) * swipeView.width)
                        swipeView.currentIndex++;
                    }
                }
            }
        }//SwipeView

    }//Column

    DrawingContainer{
        anchors.verticalCenter: parent.verticalCenter;
        content : contentRec;

    }

    DrawingContainer{
        anchors.verticalCenter: parent.verticalCenter;
        edge: Qt.RightEdge;
        content : contentRec;
        Component.onCompleted: {
            addStep(100)
            addStep(200)
            addStep(300)
        }
    }

    DrawingContainer{
        anchors.horizontalCenter: parent.horizontalCenter;
        edge: Qt.TopEdge;
        content : contentRec;

    }

    DrawingContainer{
        anchors.horizontalCenter:  parent.horizontalCenter;
        edge: Qt.BottomEdge;
        content : contentRec;

    }

    Component{
        id: contentRec
        Rectangle{
            width: 300;
            height: 300;
            border.color: "lightgray";
            radius: 2;
            Rectangle{
                anchors.centerIn: parent;
                width: parent.width - 20;
                height: parent.height - 20;
                radius: 8;
                border.color: Style.textColor;
                Text{
                    anchors.centerIn: parent;

                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSizeXL;
                    color: Style.textColor;
                    width: parent.width - 20;
                    wrapMode: Text.WordWrap;
                    horizontalAlignment: Text.AlignHCenter;

                    text: "Vita nostra brevis est, Brevi finietur. Venit mors velociter, Rapit nos atrociter, Nemini parcetur!";
                }

            }
        }
    }

}
