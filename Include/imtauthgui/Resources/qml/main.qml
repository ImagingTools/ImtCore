import QtQuick 2.0
//import ACF 1.0

Item {
    width: 300
    height: 500
    
    TreeItemModel {
        id:contactInfoTreeItemModel
        source: "../../Models/__ContactInfo__"
        target: "../../Models/__ContactInfo__"
    }

    ContactInfoEditor {
        width: parent.width
        height: 300
        id: contactEditor
        contactInfoModel: contactInfoTreeItemModel
    }

}


