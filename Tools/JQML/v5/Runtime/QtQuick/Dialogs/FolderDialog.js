const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { Dialog } = require('./Dialog')

// a folder input: the folder is the first path segment of the picked files
class FolderDialog extends Dialog {
    $show(){
        if(typeof document === 'undefined') return
        let input = document.createElement('input')
        input.type = 'file'
        input.webkitdirectory = true
        input.style.display = 'none'
        input.addEventListener('change', () => engine.batch(() => {
            let file = input.files && input.files[0]
            input.remove()
            if(!file){
                this.reject()
                return
            }
            let folder = String(file.webkitRelativePath || '').split('/')[0]
            this.$write(FolderDialog.FOLDER, folder)
            this.$write(FolderDialog.SELECTEDFOLDER, folder)
            this.$write(FolderDialog.CURRENTFOLDER, folder)
            this.accept()
        }))
        document.body.appendChild(input)
        input.click()
    }
}
defineType(FolderDialog, { props: { folder: 'var', selectedFolder: 'var', currentFolder: 'var' } })

module.exports = { FolderDialog }
