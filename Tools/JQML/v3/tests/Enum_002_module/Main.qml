import QtQuick
import EnumModule 1.0
import EnumModule 1.0 as Aliased

Item {
    ModuleUser {
        id: user
    }

    Component.onCompleted: {
        console.log("module-type", ModuleType.Kind.First, ModuleType.Kind.Second, ModuleType.Kind.Third, ModuleType.Third)
        console.log("module-derived", DerivedType.Kind.Third, DerivedType.Extra.Added, DerivedType.Added)
        console.log("module-singleton", ModuleSingleton.Own.Mine, ModuleSingleton.Mine, ModuleSingleton.Kind.Second, ModuleSingleton.Added)
        console.log("module-user", user.bound, user.read())
        console.log("module-alias", Aliased.ModuleType.Kind.Third, Aliased.ModuleType.Second, Aliased.ModuleSingleton.Own.Mine)
        Qt.quit()
    }
}
