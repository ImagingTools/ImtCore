import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtguigql 1.0

QtObject {
    id: root

    property string permissionPath: ""

    GqlSdlRequestSender {
        permissionPath: root.permissionPath
    }
}



