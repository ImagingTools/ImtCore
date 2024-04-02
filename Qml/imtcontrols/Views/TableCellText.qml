import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Text {
    id: name;

    // anchors.left: parent.left;
    // anchors.leftMargin:  rowDelegate.textLeftIndent;
    // anchors.right: parent.right;
    // anchors.rightMargin: rowDelegate.textRightIndent;

    property TableRowDelegateBase rowDelegate

    verticalAlignment: Text.AlignVCenter;
    horizontalAlignment: rowDelegate ?
                             rowDelegate.table.emptyDecorCell ? Text.AlignLeft :
                                                    rowDelegate.table.cellDecorator.IsValidData("TextPosition", delegateContainer.columnIndex) ?
                                                        rowDelegate.table.cellDecorator.GetData("TextPosition", delegateContainer.columnIndex) :
                                                        Text.AlignLeft: Text.AlignLeft;


    font.pixelSize: rowDelegate ? rowDelegate.table.emptyDecorCell ? Style.fontSize_common:
                                                   rowDelegate.table.cellDecorator.IsValidData("FontSize", delegateContainer.columnIndex) ?
                                                       rowDelegate.table.cellDecorator.GetData("FontSize", delegateContainer.columnIndex) :
                                                       Style.fontSize_common : Style.fontSize_common;
    font.family: Style.fontFamily;
    font.bold: rowDelegate ?
                   rowDelegate.table.emptyDecorCell ? true :
                                                  rowDelegate.table.cellDecorator.IsValidData("FontBold", delegateContainer.columnIndex) ?
                                                      rowDelegate.table.cellDecorator.GetData("FontBold", delegateContainer.columnIndex) :
                                                      true: false;
    color: rowDelegate && rowDelegate.enabled ?
               (rowDelegate.table.emptyDecorCell ? Style.textColor :
                                                  rowDelegate.table.cellDecorator.IsValidData("FontColor", delegateContainer.columnIndex) ?
                                                      rowDelegate.table.cellDecorator.GetData("FontColor", delegateContainer.columnIndex) :
                                                      Style.textColor) : Style.inactive_textColor;

    elide: rowDelegate ? rowDelegate.elideMode : Text.ElideLeft;
    wrapMode: rowDelegate ? rowDelegate.wrapMode : Text.NoWrap;
    onLinkActivated: {
        Qt.openUrlExternally(link);
    }

    // text: delegateContainer.getValue();
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
        if(!rowDelegate || !rowDelegate.table || !rowDelegate.table.canFitHeight ){
            return;
        }
        if(wrapMode !== Text.NoWrap && rowDelegate){
            var height_ = name.height +
                    2*rowDelegate.textMarginVer;

            rowDelegate.setHeightModelElememt(delegateContainer.columnIndex, height_);

        }
    }
}

