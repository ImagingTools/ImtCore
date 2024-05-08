import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Text {
    id: name

    property TableRowDelegateBase rowDelegate
    property int columnIndex: -1;
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: rowDelegate ? rowDelegate.tableItem.emptyDecorCell ? Text.AlignLeft : rowDelegate.tableItem.cellDecorator.IsValidData("TextPosition", name.columnIndex) ? rowDelegate.tableItem.cellDecorator.GetData("TextPosition", name.columnIndex) : Text.AlignLeft : Text.AlignLeft

    font.pixelSize: rowDelegate ? rowDelegate.tableItem.emptyDecorCell ? Style.fontSize_common : rowDelegate.tableItem.cellDecorator.IsValidData("FontSize", name.columnIndex) ? rowDelegate.tableItem.cellDecorator.GetData("FontSize", name.columnIndex) : Style.fontSize_common : Style.fontSize_common
    font.family: Style.fontFamily

    color: rowDelegate
           && rowDelegate.enabled ? (rowDelegate.tableItem.emptyDecorCell ? Style.textColor : rowDelegate.tableItem.cellDecorator.IsValidData("FontColor", name.columnIndex) ? rowDelegate.tableItem.cellDecorator.GetData("FontColor", name.columnIndex) : Style.textColor) : Style.inactive_textColor

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
