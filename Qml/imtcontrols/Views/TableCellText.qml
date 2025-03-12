import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

// Canvas {
//     id: canvas;

//     property TableRowDelegateBase rowDelegate: null;
//     property int columnIndex: -1;
//     property string text
//     height: rowDelegate.tableItem.emptyDecorCell ? Style.fontSizeNormal : rowDelegate.tableItem.cellDecorator.isValidData("FontSize", columnIndex) ? rowDelegate.tableItem.cellDecorator.getData("FontSize", columnIndex) : Style.fontSizeNormal

//     Component.onCompleted: {
//         console.log("onCompleted", text)
//     }

//     onTextChanged: {
//         console.log("onTextChanged", text)
//         canvas.requestPaint();
//     }

//     Connections {
//         target: canvas.rowDelegate.tableItem;
//         function onWidthRecalc(){
//             canvas.requestPaint();
//         }
//     }

//     function trimTextToFit(ctx, text_, maxWidth){
//         if (ctx.measureText(text_).width <= maxWidth){
//             return text_;
//         }

//         let ellipsis = "...";
//         let ellipsisWidth = ctx.measureText(ellipsis).width;

//         while (text_.length > 0 && ctx.measureText(text_).width + ellipsisWidth > maxWidth){
//             text_ = text_.slice(0, -1);
//         }

//         return text_ + ellipsis;
//     }


//     function drawText(ctx, x, y, cellWidth, text){
//         let maxTextWidth = cellWidth - Style.sizeSmallMargin * 2;
//         let trimmedText;
//         if (text !== ""){
//             trimmedText = trimTextToFit(ctx, text, maxTextWidth);
//         }

//         ctx.textBaseline = "middle";
//         ctx.textAlign = "left";

//         ctx.fillText(trimmedText, x, y);
//     }

//     onPaint: {
//         console.log("onPaint")

//         var ctx = canvas.getContext('2d');
//         ctx.reset();
//         ctx.clearRect(0, 0, width, height);

//         if (!rowDelegate || text == ""){
//             return
//         }


//         ctx.strokeStyle =  rowDelegate.tableItem.borderColorVertical
//         let fontPixelSize = height // rowDelegate.tableItem.emptyDecorCell ? Style.fontSizeNormal : rowDelegate.tableItem.cellDecorator.isValidData("FontSize", columnIndex) ? rowDelegate.tableItem.cellDecorator.getData("FontSize", columnIndex) : Style.fontSizeNormal
//         ctx.font = '' + fontPixelSize + "px " + Style.fontFamily
//         // ctx.fillStyle = rowDelegate.tableItem.emptyDecorCell ? Style.textColor : rowDelegate.tableItem.cellDecorator.isValidData("FontColor", columnIndex) ? rowDelegate.tableItem.cellDecorator.getData("FontColor", columnIndex) : Style.textColor

//          ctx.fillStyle = Style.textColor

//         drawText(ctx, Style.sizeSmallMargin, height / 2, width, text)


//         ctx.stroke();

//         ctx.fill();
//     }


// }

Text {
    id: name

    property TableRowDelegateBase rowDelegate: null
    property int columnIndex: -1;
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: rowDelegate ? rowDelegate.tableItem.emptyDecorCell ? Text.AlignLeft : rowDelegate.tableItem.cellDecorator.isValidData("TextPosition", name.columnIndex) ? rowDelegate.tableItem.cellDecorator.getData("TextPosition", name.columnIndex) : Text.AlignLeft : Text.AlignLeft

    font.pixelSize: rowDelegate ? rowDelegate.tableItem.emptyDecorCell ? Style.fontSizeNormal : rowDelegate.tableItem.cellDecorator.isValidData("FontSize", name.columnIndex) ? rowDelegate.tableItem.cellDecorator.getData("FontSize", name.columnIndex) : Style.fontSizeNormal : Style.fontSizeNormal
    font.family: Style.fontFamily

    color: rowDelegate
           && rowDelegate.enabled ? (rowDelegate.tableItem.emptyDecorCell ? Style.textColor : rowDelegate.tableItem.cellDecorator.isValidData("FontColor", name.columnIndex) ? rowDelegate.tableItem.cellDecorator.getData("FontColor", name.columnIndex) : Style.textColor) : Style.inactiveTextColor

    elide: rowDelegate ? rowDelegate.elideMode : Text.ElideLeft
    wrapMode: (!rowDelegate
               || !rowDelegate.tableItem) ? Text.NoWrap : rowDelegate.tableItem.wrapMode_deleg
    onLinkActivated: {
        Qt.openUrlExternally(link)
    }

    onTextChanged: {
        // Multiline fit in one line
        if (wrapMode == Text.NoWrap) {
            if (name.text.includes('\n')) {
                let result = name.text.split('\n')
                name.text = result[0] + "..."
            }
        }
    }
    onHeightChanged: {
        if (!rowDelegate || !rowDelegate.tableItem
                || !rowDelegate.tableItem.canFitHeight) {
            return
        }
        if (wrapMode !== Text.NoWrap && rowDelegate) {
            var height_ = name.height + 2 * rowDelegate.textMarginVer
            rowDelegate.setHeightModelElememt(name.columnIndex, height_)
        }
    }


}
