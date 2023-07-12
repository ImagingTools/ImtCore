import QtQuick 2.12
import Acf 1.0

Item {
    id: root;

    function logout(){
        Events.sendEvent("Logout");
    }
}

