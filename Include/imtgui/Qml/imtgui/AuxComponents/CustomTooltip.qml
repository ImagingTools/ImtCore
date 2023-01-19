import QtQuick 2.12


Item {
    id: customTooltip;


    property string text: "";

    property int componentWidth: 200;
    property int componentHeight: 100;
    property int componentMargin: 10;

    function openTooltip(xX, yY){
        console.log("openTooltip");

        var point = mapToItem(thumbnailDecoratorContainer, xX, yY);
        if(point.x > thumbnailDecoratorContainer.width*2/3){
            point.x = point.x - customTooltip.componentWidth - customTooltip.componentMargin;
        }
        else{
            point.x = point.x + customTooltip.componentMargin;
        }

        if(point.y > thumbnailDecoratorContainer.height*2/3){
            point.y = point.y - customTooltip.componentHeight - customTooltip.componentMargin;
        }
        else{
            point.y = point.y + customTooltip.componentMargin;
        }

        modalDialogManager.openDialog(hint, {"x": point.x, "y": point.y});

        if(modalDialogManager.count == 1){
            modalDialogManager.backgroundItem.opacity = 0;
            modalDialogManager.backgroundItem.backgroundAreaItem.visible = false;
        }
    }

    function closeTooltip(){

        console.log("closeTooltip");
        if(modalDialogManager.count == 1){
            modalDialogManager.backgroundItem.opacity = 1;
            modalDialogManager.backgroundItem.backgroundAreaItem.visible = true;
        }
        modalDialogManager.closeDialog();

    }

    Component{
        id: hint;

        Rectangle{
            width: 200;
            height: 100;
            color: "violet";
            property Item root;
        }
    }

    /////////////////////////////////////////////////////

//    Rectangle{
//        id: rec;
//        //anchors.centerIn: parent;
//        x: 100;
//        y: 700;
//        width: 100;
//        height: width;
//        color:"red";

//        property int componentWidth: 200;
//        property int componentHeight: 100;
//        property int componentMargin: 10;

//        MouseArea{
//            anchors.fill: parent;
//            cursorShape: Qt.PointingHandCursor;
//            hoverEnabled: true;
//            onClicked: {

//            }
//            onEntered: {
//                console.log("Entered");

//                var point = mapToItem(thumbnailDecoratorContainer, mouseX, mouseY);
//                if(point.x > thumbnailDecoratorContainer.width*2/3){
//                    point.x = point.x - rec.componentWidth - rec.componentMargin;
//                }
//                else{
//                    point.x = point.x + rec.componentMargin;
//                }

//                if(point.y > thumbnailDecoratorContainer.height*2/3){
//                    point.y = point.y - rec.componentHeight - rec.componentMargin;
//                }
//                else{
//                    point.y = point.y + rec.componentMargin;
//                }

//                modalDialogManager.openDialog(hint, {"x": point.x, "y": point.y});

//                if(modalDialogManager.count == 1){
//                    modalDialogManager.backgroundItem.opacity = 0;
//                    modalDialogManager.backgroundItem.backgroundAreaItem.visible = false;
//                }
//            }
//            onExited: {
//                console.log("Exited");
//                if(modalDialogManager.count == 1){
//                    modalDialogManager.backgroundItem.opacity = 1;
//                    modalDialogManager.backgroundItem.backgroundAreaItem.visible = true;
//                }
//                modalDialogManager.closeDialog();

//            }
//        }

//    }

//    Component{
//        id: hint;

//        Rectangle{
//            width: 200;
//            height: 100;
//            color: "violet";
//            property Item root;
//        }
//    }

}
