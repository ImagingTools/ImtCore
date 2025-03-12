import QtQuick 2.12
import Acf 1.0


Item {
    id: delegateContainer;

    property Item pTableDelegateContainer: null;

    property Item mainMouseArea: pTableDelegateContainer != null ? pTableDelegateContainer.mouseArea : null;

    height: pTableDelegateContainer ? pTableDelegateContainer.height : 0;
    width: pTableDelegateContainer ? pTableDelegateContainer.width/pTableDelegateContainer.count : 0;

    property int columnCount: pTableDelegateContainer && pTableDelegateContainer.tableItem ? pTableDelegateContainer.tableItem.columnCount : 0;

    property bool compl: false;
    property bool complCompl: columnCount && delegateContainer.compl && ready;

    property alias contentComp: contentLoader.sourceComponent;
	property real textLeftIndent: 0
	property real textRightIndent: 0

    property int columnIndex: model.index;
    property int rowIndex: pTableDelegateContainer ? pTableDelegateContainer.rowIndex : -1;

    property Item parent_temp: (parent == null || parent.parent == null || parent.parent.parent == null) ? null : parent.parent.parent;

    property bool  ready: parent_temp !==null;

    onParent_tempChanged: {
        if(parent_temp !== null){
            pTableDelegateContainer = parent_temp;
        }
    }

    Component.onCompleted: {
        delegateContainer.compl = true;
    }

    Component.onDestruction: {
        if(delegateContainer.complCompl) pTableDelegateContainer.tableItem.widthRecalc.disconnect(delegateContainer.setCellWidth)
    }

    onComplComplChanged: {
        if(delegateContainer.complCompl){
            let contents = delegateContainer.pTableDelegateContainer.tableItem.columnContentComps;
            let contentComp = delegateContainer.defaultContentComp;

            if (Object.keys(contents).includes(model.Id)){
                if (contents[model.Id]){
                    contentComp = contents[model.Id];
                }
            }
            delegateContainer.contentComp = contentComp;

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
        if(!delegateContainer.pTableDelegateContainer){
            return;
        }

        var defaultWidth = delegateContainer.columnCount == 0 ? 0 : delegateContainer.pTableDelegateContainer.width/delegateContainer.columnCount;
        var widthFromModel = delegateContainer.pTableDelegateContainer.tableItem.widthDecoratorDynamic.isValidData("Width", model.index) ?
                    delegateContainer.pTableDelegateContainer.tableItem.widthDecoratorDynamic.getData("Width", model.index) : -1;

        if(!delegateContainer.pTableDelegateContainer.tableItem.widthDecoratorDynamic.getItemsCount()){
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
                                                                          delegateContainer.pTableDelegateContainer.cellDecorator.isValidData("Color", model.index) ?
                                                                              delegateContainer.pTableDelegateContainer.cellDecorator.getData("Color", model.index) :
                                                                              "transparent" : "transparent"

        opacity: delegateContainer.pTableDelegateContainer ? delegateContainer.pTableDelegateContainer.emptyDecorCell ? 1 :
                                                                            delegateContainer.pTableDelegateContainer.cellDecorator.isValidData("Opacity", model.index) ?
                                                                                delegateContainer.pTableDelegateContainer.cellDecorator.getData("Opacity", model.index) :
                                                                                1 : 0;

        radius: delegateContainer.pTableDelegateContainer ?  delegateContainer.pTableDelegateContainer.emptyDecorCell ? 0 :
                                                                           delegateContainer.pTableDelegateContainer.cellDecorator.isValidData("CellRadius", model.index) ?
                                                                               delegateContainer.pTableDelegateContainer.cellDecorator.getData("CellRadius", model.index) :0 : 0;

        //cornerPatches
        Rectangle{
            id: leftTopCornerPatch;
            anchors.left: parent.left;
            anchors.top: parent.top;
            width: parent.width/2;
            height: parent.height/2;
            color: parent.color;
            visible: delegateContainer.pTableDelegateContainer ? delegateContainer.pTableDelegateContainer.emptyDecorCell ? true :
                                                                                delegateContainer.pTableDelegateContainer.cellDecorator.isValidData("LeftTopRound", model.index) ?
                                                                                    !delegateContainer.pTableDelegateContainer.cellDecorator.getData("LeftTopRound", model.index) :true : 0;
        }

        Rectangle{
            id: rightTopCornerPatch;
            anchors.right: parent.right;
            anchors.top: parent.top;
            width: parent.width/2;
            height: parent.height/2;
            color: parent.color;
            visible: delegateContainer.pTableDelegateContainer ? delegateContainer.pTableDelegateContainer.emptyDecorCell ? true :
                                                                                delegateContainer.pTableDelegateContainer.cellDecorator.isValidData("RightTopRound", model.index) ?
                                                                                    !delegateContainer.pTableDelegateContainer.cellDecorator.getData("RightTopRound", model.index) :true : 0;
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
                                                                                delegateContainer.pTableDelegateContainer.cellDecorator.isValidData("LeftBottomRound", model.index) ?
                                                                                    !delegateContainer.pTableDelegateContainer.cellDecorator.getData("LeftBottomRound", model.index) :true : 0;
        }

        Rectangle{
            id: rightBottomCornerPatch;
            anchors.right:  parent.right;
            anchors.bottom: parent.bottom;
            width: parent.width/2;
            height: parent.height/2;
            color: parent.color;
            visible: delegateContainer.pTableDelegateContainer ? delegateContainer.pTableDelegateContainer.emptyDecorCell ? true :
                                                                                delegateContainer.pTableDelegateContainer.cellDecorator.isValidData("RightBottomRound", model.index) ?
                                                                                    !delegateContainer.pTableDelegateContainer.cellDecorator.getData("RightBottomRound", model.index) :true : 0;
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

        property var cellDelegate: delegateContainer;

        onLoaded: {
            if (contentLoader.item.tableCellDelegate !== undefined){
                contentLoader.item.tableCellDelegate = delegateContainer;
            }

            contentLoader.item.z = delegateContainer.mainMouseArea.z + 1;
        }
    }

    function getValue(){
        if (delegateContainer.complCompl){
            if (delegateContainer.columnIndex >= 0){
                if(delegateContainer.pTableDelegateContainer !== null && delegateContainer.pTableDelegateContainer.dataModel !==null){
                    return delegateContainer.pTableDelegateContainer.dataModel[delegateContainer.pTableDelegateContainer.headers.getData("Id", delegateContainer.columnIndex)];
                }
            }
        }

        return "";
    }

    function setValue(value){
        if (delegateContainer.complCompl){
            if (delegateContainer.columnIndex >= 0){
                if(delegateContainer.pTableDelegateContainer !== null && delegateContainer.pTableDelegateContainer.tableItem !==null){
                    let tableItem = delegateContainer.pTableDelegateContainer.tableItem;
                    let elements = tableItem.elements;
                    let headerId = delegateContainer.pTableDelegateContainer.headers.getData("Id", delegateContainer.columnIndex);

                    elements.setData(headerId, value, delegateContainer.rowIndex);
                }
            }
        }
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
                                                                                            delegateContainer.pTableDelegateContainer.cellDecorator.isValidData("TextPosition", delegateContainer.columnIndex) ?
                                                                                                delegateContainer.pTableDelegateContainer.cellDecorator.getData("TextPosition", delegateContainer.columnIndex) :
                                                                                                Text.AlignLeft: Text.AlignLeft;
            font.pixelSize: delegateContainer.pTableDelegateContainer ? delegateContainer.pTableDelegateContainer.emptyDecorCell ? Style.fontSizeNormal:
                                                                                       delegateContainer.pTableDelegateContainer.cellDecorator.isValidData("FontSize", delegateContainer.columnIndex) ?
                                                                                           delegateContainer.pTableDelegateContainer.cellDecorator.getData("FontSize", delegateContainer.columnIndex) :
                                                                                           Style.fontSizeNormal : Style.fontSizeNormal;
            font.family: Style.fontFamily;
//            font.bold: delegateContainer.pTableDelegateContainer ?
//                           delegateContainer.pTableDelegateContainer.emptyDecorCell ? true :
//                                                                                  delegateContainer.pTableDelegateContainer.cellDecorator.isValidData("FontBold", delegateContainer.columnIndex) ?
//                                                                                      delegateContainer.pTableDelegateContainer.cellDecorator.getData("FontBold", delegateContainer.columnIndex) :
//                                                                                      true: false;
            color: delegateContainer.pTableDelegateContainer && delegateContainer.pTableDelegateContainer.enabled ?
                       (delegateContainer.pTableDelegateContainer.emptyDecorCell ? Style.textColor :
                                                                              delegateContainer.pTableDelegateContainer.cellDecorator.isValidData("FontColor", delegateContainer.columnIndex) ?
                                                                                  delegateContainer.pTableDelegateContainer.cellDecorator.getData("FontColor", delegateContainer.columnIndex) :
                                                                                  Style.textColor) : Style.inactiveTextColor;


            elide: delegateContainer.pTableDelegateContainer ? delegateContainer.pTableDelegateContainer.elideMode : Text.ElideLeft;
            wrapMode: delegateContainer.pTableDelegateContainer ? delegateContainer.pTableDelegateContainer.wrapMode : Text.NoWrap;
            onLinkActivated: {
                Qt.openUrlExternally(link);
            }
//            text: delegateContainer.pTableDelegateContainer ? delegateContainer.columnIndex >= 0 ? delegateContainer.pTableDelegateContainer.dataModel[delegateContainer.pTableDelegateContainer.headers.getData("Id", delegateContainer.columnIndex)] : "" : "";
//            text: delegateContainer.pTableDelegateContainer.headers.getData("Id", 1)

            text: delegateContainer.getValue() !== undefined ? delegateContainer.getValue() : "";
            onTextChanged: {
                // Multiline fit in one line
                if(wrapMode == Text.NoWrap){
                    if (name.text && name.text.includes('\n')){
                        let result = name.text.split('\n');
                        name.text = result[0] + "...";
                    }
                }
            }
            onHeightChanged: {
                if(!delegateContainer.pTableDelegateContainer || !delegateContainer.pTableDelegateContainer.tableItem || !delegateContainer.pTableDelegateContainer.tableItem.canFitHeight ){
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

