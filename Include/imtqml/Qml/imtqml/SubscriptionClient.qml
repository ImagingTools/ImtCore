import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

GqlModel {
    id: container;
    property string subscriptionId;

    UuidGenerator {
        id: uuidGenerator;
    }

    Component.onCompleted: {
        subscriptionId = "1111111" //uuidGenerator.generateUUID();
    }
}
