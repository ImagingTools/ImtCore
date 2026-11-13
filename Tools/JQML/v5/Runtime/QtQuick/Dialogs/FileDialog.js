const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { Dialog } = require('./Dialog')

// ---- dialogs (QtQuick.Dialogs / Qt.labs.platform) ----------------------------------------------

// On the web the picked file is the File itself; QML treats it like a url (toString, replace, split)
function fileAsUrl(file){
    file.toString = () => file
    file.replace = () => file
    file.split = (delimiter) => file.name.split(delimiter)
    return file
}

class FileDialog extends Dialog {
    $show(){
        if(this.$v[FileDialog.FILEMODE] === FileDialog.SaveFile){
            // no save dialog on the web: the name is decided by currentFile, FileIO.write downloads
            engine.queueCall(() => this.accept())
            return
        }
        if(typeof document === 'undefined') return
        let input = document.createElement('input')
        input.type = 'file'
        input.multiple = this.$v[FileDialog.FILEMODE] === FileDialog.OpenFiles
        let filters = (this.$v[FileDialog.NAMEFILTERS] || []).join(',').match(/\*\.\w+/g)
        if(filters) input.accept = filters.map(f => f.slice(1)).join(',')
        input.style.display = 'none'
        input.addEventListener('change', () => engine.batch(() => {
            let files = Array.from(input.files || []).map(fileAsUrl)
            input.remove()
            if(!files.length){
                this.reject()
                return
            }
            this.$write(FileDialog.FILE, files[0])
            this.$write(FileDialog.FILES, files)
            this.$write(FileDialog.SELECTEDFILE, files[0])
            this.$write(FileDialog.SELECTEDFILES, files)
            this.$write(FileDialog.CURRENTFILE, files[0])
            this.accept()
        }))
        input.addEventListener('cancel', () => engine.batch(() => {
            input.remove()
            this.reject()
        }))
        document.body.appendChild(input)
        input.click()
    }
}
defineType(FileDialog, {
    props: {
        fileMode: 'int',
        nameFilters: { kind: 'var', initial: [] },
        selectedNameFilter: 'var',
        file: 'var',
        files: 'var',
        selectedFile: 'var',
        selectedFiles: 'var',
        currentFile: 'var',
        currentFiles: 'var',
        folder: 'var',
        currentFolder: 'var',
        defaultSuffix: 'string',
    },
})
Object.assign(FileDialog, { OpenFile: 0, OpenFiles: 1, SaveFile: 2, DontResolveSymlinks: 1, DontConfirmOverwrite: 2, ReadOnly: 4, HideNameFilterDetails: 8, DontUseNativeDialog: 16 })

module.exports = { FileDialog }
