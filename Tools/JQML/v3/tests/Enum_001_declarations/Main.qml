import QtQuick
import "."

Item {
    id: root

    enum Local { One = 11, Two = 22 }
    enum Signed { Minus = -1, Zero, Hex = 0x10, Next }

    property int boundValue: Holder.Kind.View
    property int storedValue: Holder.Kind.External

    function show(label, getter){
        try {
            console.log(label, "=", String(getter()))
        }
        catch (e) {
            console.log(label, "THROW", e.message)
        }
    }

    Component.onCompleted: {
        show("plain-singleton-property", function(){ return SingletonHolder.plainValue })
        show("type-explicit", function(){ return Holder.Explicit.First })
        show("type-explicit-flat", function(){ return Holder.First })
        show("type-implicit", function(){ return Holder.Kind.Own })
        show("type-implicit-assigned", function(){ return Holder.Kind.View })
        show("type-implicit-after", function(){ return Holder.Kind.External })
        show("singleton-explicit", function(){ return SingletonHolder.Explicit.Second })
        show("singleton-explicit-flat", function(){ return SingletonHolder.Second })
        show("singleton-implicit", function(){ return SingletonHolder.Kind.Own })
        show("own-file", function(){ return Main.Local.Two })
        show("own-file-flat", function(){ return Main.Two })
        show("signed", function(){ return Main.Signed.Minus + "," + Main.Signed.Zero + "," + Main.Signed.Hex + "," + Main.Signed.Next })
        show("derived", function(){ return Derived.Kind.View })
        show("derived-flat", function(){ return Derived.External })
        show("bound", function(){ return root.boundValue })
        show("stored", function(){ return root.storedValue })
        console.log("done")
        Qt.quit()
    }
}
