// RoleEditorPage - the editor opened by RoleCollectionPage's New/Edit (imtauthgui/RoleView.qml).
// Role-ID is read-only (auto-derived from Role Name), so no setter is exposed for it.

const { BasePage } = require('./BasePage');
const { TextInput } = require('../controls');
const gui = require('../lib/gui');

class RoleEditorPage extends BasePage {
  constructor(page) {
    super(page, 'Roles');

    this.roleName = new TextInput(page, ['RoleNameInput']);
    this.roleId = new TextInput(page, ['RoleIdInput']);
    this.description = new TextInput(page, ['RoleDescriptionInput']);
  }

  save() { return this.runCommand('Save'); }
  undo() { return this.runCommand('Undo'); }
  redo() { return this.runCommand('Redo'); }

  async setRoleName(text) { await this.roleName.fill(text); return this; }
  async setDescription(text) { await this.description.fill(text); return this; }

  expectFieldVisible(objectName) {
    return gui.expectVisible(this.page, [objectName]);
  }
}

module.exports = { RoleEditorPage };
