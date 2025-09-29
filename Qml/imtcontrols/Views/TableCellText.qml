import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Text {
    id: name

    property TableRowDelegateBase rowDelegate: null
    property int columnIndex: -1;
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: rowDelegate ? rowDelegate.tableItem.emptyDecorCell ? Text.AlignLeft : rowDelegate.tableItem.cellDecorator.isValidData("TextPosition", name.columnIndex) ? rowDelegate.tableItem.cellDecorator.getData("TextPosition", name.columnIndex) : Text.AlignLeft : Text.AlignLeft

    font.pixelSize: rowDelegate ? rowDelegate.tableItem.emptyDecorCell ? Style.fontSizeM : rowDelegate.tableItem.cellDecorator.isValidData("FontSize", name.columnIndex) ? rowDelegate.tableItem.cellDecorator.getData("FontSize", name.columnIndex) : Style.fontSizeM : Style.fontSizeM
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
        if (wrapMode == Text.NoWrap){
            if (name.text.includes('\n')){
                let result = name.text.split('\n')
                name.text = result[0] + "..."
            }
        }
    }
    onHeightChanged: {
        if (!rowDelegate || !rowDelegate.tableItem
                || !rowDelegate.tableItem.canFitHeight){
            return
        }
        if (wrapMode !== Text.NoWrap && rowDelegate){
            var height_ = name.height + 2 * rowDelegate.textMarginVer
            rowDelegate.setHeightModelElememt(name.columnIndex, height_)
        }
    }


}
