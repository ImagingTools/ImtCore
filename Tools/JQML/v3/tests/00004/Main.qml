import QtQuick

Item {
    readonly property var _devicesDict: ({
     "Barbieri Spectro Swing"    : "SWING",
     "LFP"                       : "LFP",
     "Barbieri Spectropad"       : "SPECT",
     "Scan"                      : "Scan",
     "SP62"                      : "SP62",
     "SCP5000"                   : "SCP5000",
     "SCP7000"                   : "SCP7000",
     "ILS30+iSis"                : "ILS30+iSis",
     "ILS20"                     : "ILS20",
     "FD-9"                      : "FD-9",
     "FD-7"                      : "FD-7",
     "iSis"                      : "iSis",
     "i1"                        : "i1",
     "Pro3"                      : "Pro3",
     "Pro3 Plus"                 : "Pro3+",
     "X-Rite eXact"              : "eXact",
     "X-Rite eXact2"             : "eXact2",
     "SpectroDens"               : "DENS"
    })

    Component.onCompleted: {
        console.log(Object.keys(_devicesDict).join(", "))
        Qt.quit()
    }
}