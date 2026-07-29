// GroupEditorPage - the editor opened by GroupCollectionPage's New/Edit (imtauthgui/UserGroupView.qml).

const { BasePage } = require('./BasePage');
const { TextInput } = require('../controls');
const gui = require('../lib/gui');

class GroupEditorPage extends BasePage {
  constructor(page) {
    super(page, 'Groups');

    this.name = new TextInput(page, ['GroupNameInput']);
    this.description = new TextInput(page, ['GroupDescriptionInput']);
  }

  save() { return this.runCommand('Save'); }
  undo() { return this.runCommand('Undo'); }
  redo() { return this.runCommand('Redo'); }

  async setName(text) { await this.name.fill(text); return this; }
  async setDescription(text) { await this.description.fill(text); return this; }

  expectFieldVisible(objectName) {
    return gui.expectVisible(this.page, [objectName]);
  }
}

module.exports = { GroupEditorPage };
