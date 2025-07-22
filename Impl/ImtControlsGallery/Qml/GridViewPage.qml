import QtQuick 2.12

import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtcolgui 1.0


Rectangle {
    id: gridViewPage;

    anchors.fill: parent;
    clip: true;
	color: Style.baseColor

    Text{
        id: title;

        anchors.top: parent.top;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.topMargin: 20;

        font.family: Style.fontFamily;
        font.pixelSize: 20;
        color: Style.textColor;

        text: "Please click on the cell!";
    }

    GridCollectionViewBase{
        id: gridBase;

        anchors.top: title.bottom;
        anchors.bottom: parent.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.margins:  20;

        gridCellWidth: 110;
        gridAddHeight: 110*3;

        extendingInfoComp: detailComp;
        gridIndicatorMainColor: "lightblue";

        gridElementsModel : 160;


        gridElementsDelegate:
            Component{
            GridExtendingBaseDelegate{
                id: gridItem;
                rootItem : gridBase;
                Rectangle{
                    width: parent.width;
                    height: gridBase.gridCellHeightMin - 10;
                    color:  "lightblue";
                    border.color: "gray";
                    border.width: 2;
                    radius: 10;

                    Text{
                        anchors.centerIn: parent;

                        font.family: Style.fontFamily;
                        font.pixelSize: 14;
                        color: Style.textColor;
                        text: "Cell " + String(model.index + 1);
                    }
                    MouseArea{
                        anchors.fill: parent;

                        cursorShape: Qt.PointingHandCursor;
                        onClicked: {
                            gridItem.setOpenST();
                            gridBase.selectedIndex = model.index;
                        }
                    }
                }



            }//delegate
        }

        Component{
            id: detailComp;

            Rectangle{
                width: 1000;
                height: 300;

                color: "lightblue";
                radius: 10;
                border.color: "gray";
                border.width: 2;
                clip: true;
                Text{
                    anchors.centerIn: parent;

                    font.family: Style.fontFamily;
                    font.pixelSize: 30;
                    color: Style.textColor;
                    width: parent.width - 40;
                    horizontalAlignment: Text.AlignHCenter;
                    wrapMode: Text.WordWrap;

                    text: "Vita nostra brevis est, Brevi finietur. Venit mors velociter, Rapit nos atrociter, Nemini parcetur!";
                }
            }
        }

    }

   
}
