const { defineType } = require('../Core/type')
const { QtObject } = require('../Core/QtObject')

// StandardPaths of QtCore (QQmlStandardPaths, Qt 6.2): the places of a platform's files. A page has no file system of
// the user: every location is empty (writableLocation "", standardLocations [], locate "", locateAll []); the names
// are those Qt gives the locations (QStandardPaths::displayName)
const TYPES = {
    DesktopLocation: 0, DocumentsLocation: 1, FontsLocation: 2, ApplicationsLocation: 3, MusicLocation: 4,
    MoviesLocation: 5, PicturesLocation: 6, TempLocation: 7, HomeLocation: 8, AppLocalDataLocation: 9, CacheLocation: 10,
    GenericDataLocation: 11, RuntimeLocation: 12, ConfigLocation: 13, DownloadLocation: 14, GenericCacheLocation: 15,
    GenericConfigLocation: 16, AppDataLocation: 17, AppConfigLocation: 18, PublicShareLocation: 19, TemplatesLocation: 20,
    StateLocation: 21, GenericStateLocation: 22,
}
const NAMES = ['Desktop', 'Documents', 'Fonts', 'Applications', 'Music', 'Movies', 'Pictures', 'Temporary Items', 'Home',
    'Application Support', 'Caches', 'Application Support', 'Runtime', 'Preferences', 'Downloads', 'Caches', 'Preferences',
    'Application Support', 'Preferences', 'Public', 'Templates', 'State', 'Shared State']

class StandardPaths extends QtObject {}
defineType(StandardPaths)
Object.assign(StandardPaths, TYPES, {
    LocateFile: 0, LocateDirectory: 1,
    displayName: (type) => NAMES[type] || '',
    findExecutable: () => '',
    locate: () => '',
    locateAll: () => [],
    standardLocations: () => [],
    writableLocation: () => '',
})

module.exports = { StandardPaths }
