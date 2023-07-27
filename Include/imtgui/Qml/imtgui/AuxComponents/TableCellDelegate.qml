import QtQuick 2.12
import Acf 1.0


Item {
    id: delegateContainer;

    property Item pTableDelegateContainer: parent.parent.parent !== null ? parent.parent.parent: null;

    property Item mainMouseArea: pTableDelegateContainer != null ? pTableDelegateContainer.mouseArea : null;

    height: pTableDelegateContainer ? pTableDelegateContainer.height : 0;
    width: pTableDelegateContainer ? pTableDelegateContainer.width/pTableDelegateContainer.count : 0;

    property int columnCount: pTableDelegateContainer && pTableDelegateContainer.tableItem ? pTableDelegateContainer.tableItem.columnCount : 0;

    property bool compl: false;
    property bool complCompl: columnCount && delegateContainer.compl;

    property alias contentComp: contentLoader.sourceComponent;
	property real textLeftIndent: 0
	property real textRightIndent: 0

    property int columnIndex: model.index;
    property int rowIndex: pTableDelegateContainer ? pTableDelegateContainer.rowIndex : -1;


    Component.onCompleted: {
        delegateContainer.compl = true;
    }

    onComplComplChanged: {
        if(delegateContainer.complCompl){
            delegateContainer.contentComp = delegateContainer.pTableDelegateContainer.tableItem.columnContentComps[model.index] !== null ?
                             delegateContainer.pTableDelegateContainer.tableItem.columnContentComps[model.index] : delegateContainer.defaultContentComp;

            pTableDelegateContainer.tableItem.widthRecalc.connect(delegateContainer.setCellWidth)
            delegateContainer.setCellWidth();

        }
    }

    function setCellWidth(){
        if(!delegateContainer){
            return;
        }
        if(!delegateContainer.complCompl){
            return;
        }

        var defaultWidth = delegateContainer.columnCount == 0 ? 0 : delegateContainer.pTableDelegateContainer.width/delegateContainer.columnCount;
        var widthFromModel = delegateContainer.pTableDelegateContainer.tableItem.widthDecoratorDynamic.IsValidData("Width", model.index) ?
                    delegateContainer.pTableDelegateContainer.tableItem.widthDecoratorDynamic.GetData("Width", model.index) : -1;

        if(!delegateContainer.pTableDelegateContainer.tableItem.widthDecoratorDynamic.GetItemsCount()){
            delegateContainer.width = defaultWidth;
        }
        else if(widthFromModel >= 0){
            delegateContainer.width = widthFromModel;
        }
        else{
            delegateContainer.width = defaultWidth;
        }
    }

    //borders

    Rectangle{
        id: topBorder;
        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.right: parent.right;
        height: delegateContainer.pTableDelegateContainer ? delegateContainer.pTableDelegateContainer.visibleTopBorderFirst  ? delegateContainer.pTableDelegateContainer.horizontalBorderSize : 0 : 0;
        color: delegateContainer.pTableDelegateContainer ? delegateContainer.pTableDelegateContainer.borderColorHorizontal : "transparent";
    }

    Rectangle{
        id: bottomBorder;
        anchors.bottom: parent.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
        height: delegateContainer.pTableDelegateContainer ? delegateContainer.pTableDelegateContainer.visibleBottomBorderLast ? delegateContainer.pTableDelegateContainer.horizontalBorderSize : 0 : 0;
        color:  delegateContainer.pTableDelegateContainer ? delegateContainer.pTableDelegateContainer.borderColorHorizontal : "transparent";
    }

    Rectangle{
        id: leftBorder;
        anchors.left: parent.left;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        width: delegateContainer.pTableDelegateContainer ?
                   delegateContainer.pTableDelegateContainer.isRightBorder ? delegateContainer.pTableDelegateContainer.verticalBorderSize * delegateContainer.pTableDelegateContainer.visibleLeftBorderFirst * (model.index == 0)
                                                                       : delegateContainer.pTableDelegateContainer.visibleLeftBorderFirst ? delegateContainer.pTableDelegateContainer.verticalBorderSize : model.index > 0 ? delegateContainer.pTableDelegateContainer.verticalBorderSize : 0
                                                                                                                                                                                                                                                    :0;
        color:  delegateContainer.pTableDelegateContainer ?delegateContainer.pTableDelegateContainer.borderColorVertical : "transparent";
    }

    Rectangle{
        id: rightBorder;
        anchors.right: parent.right;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        width: delegateContainer.pTableDelegateContainer ?
            !delegateContainer.pTableDelegateContainer.isRightBorder ?
                   delegateContainer.pTableDelegateContainer.verticalBorderSize * delegateContainer.pTableDelegateContainer.visibleRightBorderLast  * (model.index == (delegateContainer.pTableDelegateContainer.count -1)) * (delegateContainer.pTableDelegateContainer.count > 0) :
                   delegateContainer.pTableDelegateContainer.visibleRightBorderLast ? delegateContainer.pTableDelegateContainer.verticalBorderSize  :
                                                                                      delegateContainer.pTableDelegateContainer.verticalBorderSize * (model.index < (delegateContainer.pTableDelegateContainer.count -1)) : 0;

        color: delegateContainer.pTableDelegateContainer ? delegateContainer.pTableDelegateContainer.borderColorVertical : "transparent";
    }

    //borders
    Rectangle{
        id: mainRec;
        anchors.top: topBorder.bottom;
        anchors.left: leftBorder.right;
        anchors.right: rightBorder.left;
        anchors.bottom: bottomBorder.top;
        color: delegateContainer.pTableDelegateContainer ? delegateContainer.pTableDelegateContainer.emptyDecorCell ? "transparent" :
                                                                          delegateContainer.pTableDelegateContainer.cellDecorator.IsValidData("Color", model.index) ?
                                                                              delegateContainer.pTableDelegateContainer.cellDecorator.GetData("Color", model.index) :
                                                                              "transparent" : "transparent"

        opacity: delegateContainer.pTableDelegateContainer ? delegateContainer.pTableDelegateContainer.emptyDecorCell ? 1 :
                                                                            delegateContainer.pTableDelegateContainer.cellDecorator.IsValidData("Opacity", model.index) ?
                                                                                delegateContainer.pTableDelegateContainer.cellDecorator.GetData("Opacity", model.index) :
                                                                                1 : 0;

        radius: delegateContainer.pTableDelegateContainer ?  delegateContainer.pTableDelegateContainer.emptyDecorCell ? 0 :
                                                                           delegateContainer.pTableDelegateContainer.cellDecorator.IsValidData("CellRadius", model.index) ?
                                                                               delegateContainer.pTableDelegateContainer.cellDecorator.GetData("CellRadius", model.index) :0 : 0;

        //cornerPatches
        Rectangle{
            id: leftTopCornerPatch;
            anchors.left: parent.left;
            anchors.top: parent.top;
            width: parent.width/2;
            height: parent.height/2;
            color: parent.color;
            visible: delegateContainer.pTableDelegateContainer ? delegateContainer.pTableDelegateContainer.emptyDecorCell ? true :
                                                                                delegateContainer.pTableDelegateContainer.cellDecorator.IsValidData("LeftTopRound", model.index) ?
                                                                                    !delegateContainer.pTableDelegateContainer.cellDecorator.GetData("LeftTopRound", model.index) :true : 0;
        }

        Rectangle{
            id: rightTopCornerPatch;
            anchors.right: parent.right;
            anchors.top: parent.top;
            width: parent.width/2;
            height: parent.height/2;
            color: parent.color;
            visible: delegateContainer.pTableDelegateContainer ? delegateContainer.pTableDelegateContainer.emptyDecorCell ? true :
                                                                                delegateContainer.pTableDelegateContainer.cellDecorator.IsValidData("RightTopRound", model.index) ?
                                                                                    !delegateContainer.pTableDelegateContainer.cellDecorator.GetData("RightTopRound", model.index) :true : 0;
        }

        Rectangle{
            id: leftBottomCornerPatch;
            anchors.left: parent.left;
            anchors.bottom: parent.bottom;
            width: parent.width/2;
            height: parent.height/2;
            color: parent.color;
            visible: delegateContainer.pTableDelegateContainer ?
                         delegateContainer.pTableDelegateContainer.emptyDecorCell ? true :
                                                                                delegateContainer.pTableDelegateContainer.cellDecorator.IsValidData("LeftBottomRound", model.index) ?
                                                                                    !delegateContainer.pTableDelegateContainer.cellDecorator.GetData("LeftBottomRound", model.index) :true : 0;
        }

        Rectangle{
            id: rightBottomCornerPatch;
            anchors.right:  parent.right;
            anchors.bottom: parent.bottom;
            width: parent.width/2;
            height: parent.height/2;
            color: parent.color;
            visible: delegateContainer.pTableDelegateContainer ? delegateContainer.pTableDelegateContainer.emptyDecorCell ? true :
                                                                                delegateContainer.pTableDelegateContainer.cellDecorator.IsValidData("RightBottomRound", model.index) ?
                                                                                    !delegateContainer.pTableDelegateContainer.cellDecorator.GetData("RightBottomRound", model.index) :true : 0;
        }
        //cornerPatches
    }//mainRec

    Rectangle {
        id: selectionRec;

        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.topMargin: topBorder.height;
        anchors.bottomMargin: bottomBorder.height;

        color: delegateContainer.pTableDelegateContainer ? delegateContainer.pTableDelegateContainer.selectedColor : "transparent";
        opacity: delegateContainer.pTableDelegateContainer ? delegateContainer.pTableDelegateContainer.selectedOpacity : 0;

        radius: mainRec.radius;
        visible: delegateContainer.pTableDelegateContainer ? delegateContainer.pTableDelegateContainer.selected && mainRec.color != "transparent" : false;
    }

    Loader {
        id: contentLoader;

        anchors.verticalCenter: mainRec.verticalCenter;
        anchors.left: mainRec.left;
        anchors.right: mainRec.right;
        anchors.leftMargin: delegateContainer.pTableDelegateContainer ? delegateContainer.pTableDelegateContainer.textLeftMargin: 0;

        onLoaded: {
            if (contentLoader.item.tableCellDelegate !== undefined){
                contentLoader.item.tableCellDelegate = delegateContainer;
            }

            contentLoader.item.z = 100;
        }
    }

    function getValue(){
        if (delegateContainer.pTableDelegateContainer){
            if (delegateContainer.columnIndex >= 0){
                return delegateContainer.pTableDelegateContainer.dataModel[delegateContainer.pTableDelegateContainer.headers.GetData("Id", delegateContainer.columnIndex)];
            }
        }

        return "";
    }

    property Component defaultContentComp: defaultContent;

    Component {
        id: defaultContent;

        Text {
            id: name;

            anchors.left: parent.left;
            anchors.leftMargin:  delegateContainer.textLeftIndent;
            anchors.right: parent.right;
            anchors.rightMargin: delegateContainer.textRightIndent;

            verticalAlignment: Text.AlignVCenter;
            horizontalAlignment: delegateContainer.pTableDelegateContainer ?
                                     delegateContainer.pTableDelegateContainer.emptyDecorCell ? Text.AlignLeft :
                                                                                            delegateContainer.pTableDelegateContainer.cellDecorator.IsValidData("TextPosition", delegateContainer.columnIndex) ?
                                                                                                delegateContainer.pTableDelegateContainer.cellDecorator.GetData("TextPosition", delegateContainer.columnIndex) :
                                                                                                Text.AlignLeft: Text.AlignLeft;
            font.pixelSize: delegateContainer.pTableDelegateContainer ? delegateContainer.pTableDelegateContainer.emptyDecorCell ? Style.fontSize_common:
                                                                                       delegateContainer.pTableDelegateContainer.cellDecorator.IsValidData("FontSize", delegateContainer.columnIndex) ?
                                                                                           delegateContainer.pTableDelegateContainer.cellDecorator.GetData("FontSize", delegateContainer.columnIndex) :
                                                                                           Style.fontSize_common : Style.fontSize_common;
            font.family: Style.fontFamily;
            font.bold: delegateContainer.pTableDelegateContainer ?
                           delegateContainer.pTableDelegateContainer.emptyDecorCell ? true :
                                                                                  delegateContainer.pTableDelegateContainer.cellDecorator.IsValidData("FontBold", delegateContainer.columnIndex) ?
                                                                                      delegateContainer.pTableDelegateContainer.cellDecorator.GetData("FontBold", delegateContainer.columnIndex) :
                                                                                      true: false;
            color: delegateContainer.pTableDelegateContainer && delegateContainer.pTableDelegateContainer.enabled ?
                       (delegateContainer.pTableDelegateContainer.emptyDecorCell ? Style.textColor :
                                                                              delegateContainer.pTableDelegateContainer.cellDecorator.IsValidData("FontColor", delegateContainer.columnIndex) ?
                                                                                  delegateContainer.pTableDelegateContainer.cellDecorator.GetData("FontColor", delegateContainer.columnIndex) :
                                                                                  Style.textColor) : Style.inactive_textColor;


            elide: delegateContainer.pTableDelegateContainer ? delegateContainer.pTableDelegateContainer.elideMode : Text.ElideLeft;
            wrapMode: delegateContainer.pTableDelegateContainer ? delegateContainer.pTableDelegateContainer.wrapMode : Text.NoWrap;
            onLinkActivated: {
                Qt.openUrlExternally(link);
            }
//            text: delegateContainer.pTableDelegateContainer ? delegateContainer.columnIndex >= 0 ? delegateContainer.pTableDelegateContainer.dataModel[delegateContainer.pTableDelegateContainer.headers.GetData("Id", delegateContainer.columnIndex)] : "" : "";
//            text: delegateContainer.pTableDelegateContainer.headers.GetData("Id", 1)

            text: delegateContainer.getValue();
            onTextChanged: {
                // Multiline fit in one line
                if(wrapMode == Text.NoWrap){
                    if (name.text.includes('\n')){
                        let result = name.text.split('\n');
                        name.text = result[0] + "...";
                    }
                }
            }
            onHeightChanged: {
                if(!delegateContainer.pTableDelegateContainer.tableItem || !delegateContainer.pTableDelegateContainer.tableItem.canFitHeight ){
                    return;
                }
                if(wrapMode !== Text.NoWrap && delegateContainer.pTableDelegateContainer){
                    var height_ = name.height +
                            2*delegateContainer.pTableDelegateContainer.textMarginVer +
                            topBorder.height + bottomBorder.height;

                    delegateContainer.pTableDelegateContainer.setHeightModelElememt(delegateContainer.columnIndex, height_);

                }
            }//
        }
    }
}//delegate

