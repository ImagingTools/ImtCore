import QtQuick 2.12
import Acf 1.0

Item {
    id: documentData;

    property string uuid;
    property string documentTypeId;
    property bool isDirty: false;
    property TreeItemModel documentModel: TreeItemModel {};
    property UndoRedoManager undoManagerPtr: null;

    property var documentManagerPtr: null;
}
