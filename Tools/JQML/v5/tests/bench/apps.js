// The real applications the benchmarks compile, with the arguments their CMake build gives the compiler
// (Config/CMake/WebCompiler.cmake). An application whose sources are not on this machine is skipped.
const fs = require('fs')
const path = require('path')

const IMAGING = path.resolve(__dirname, '../../../../../..')

const APPS = [
    { name: 'ProLife', config: 'ProLife/Qml/Include/prolifeqml/prolife.json', entry: 'ProLife/Qml/Include/prolifeqml/ProLifeWeb.qml', root: '/ProLife/Views/' },
    { name: 'Lisa', config: 'Lisa/Include/lisaqml/lisa.json', entry: 'Lisa/Include/lisaqml/Qml/LisaWeb.qml', root: '/Lisa/Views/' },
    // Puma builds with v3; compiled here with v5 as one more real body of QML
    { name: 'Puma', config: 'Puma/Include/pumaqml/puma.json', entry: 'Puma/Include/pumaqml/Qml/PumaWeb.qml', root: '/Puma/Views/' },
    // IotPlatform: not on this machine
]

// the variables the configs read, as the build sets them (TARGETNAME: the generated SDL of the build)
function setEnv(){
    for(let [key, dir] of [['IMTCOREDIR', 'ImtCore'], ['IMTCOREDIR_BUILD', 'ImtCore'], ['PROLIFEDIR_BUILD', 'ProLife'], ['LISADIR_BUILD', 'Lisa'], ['PUMADIR_BUILD', 'Puma'], ['PROLIFEDIR', 'ProLife'], ['LISADIR', 'Lisa'], ['PUMADIR', 'Puma'], ['ACFDIR', 'Acf']]){
        process.env[key] = path.join(IMAGING, dir)
    }
    process.env.TARGETNAME = process.env.TARGETNAME || 'Qt6_ClangOSX_x64'
}

function available(){
    setEnv()
    return APPS.map(app => ({ ...app, config: path.join(IMAGING, app.config), entry: path.join(IMAGING, app.entry) }))
        .filter(app => fs.existsSync(app.config) && fs.existsSync(app.entry))
}

module.exports = { APPS, available, IMAGING }
