// UserEditorPage - the editor opened by UserCollectionPage's New/Edit (imtauthgui/UserView.qml,
// general block in UserGeneralEditor.qml). Password/ConfirmPassword are only visible on a NEW user
// (UserGeneralEditor.qml's passwordInput_.visible), not when editing an existing one.

const { BasePage } = require('./BasePage');
const { TextInput } = require('../controls');
const gui = require('../lib/gui');

class UserEditorPage extends BasePage {
  constructor(page) {
    super(page, 'Users');

    this.username = new TextInput(page, ['UsernameInput']);
    this.name = new TextInput(page, ['UserNameInput']);
    this.email = new TextInput(page, ['MailInput']);
    this.password = new TextInput(page, ['PasswordInput']);
    this.confirmPassword = new TextInput(page, ['ConfirmPasswordInput']);
  }

  save() { return this.runCommand('Save'); }
  undo() { return this.runCommand('Undo'); }
  redo() { return this.runCommand('Redo'); }

  async setUsername(text) { await this.username.fill(text); return this; }
  async setName(text) { await this.name.fill(text); return this; }
  async setEmail(text) { await this.email.fill(text); return this; }
  async setPassword(text) { await this.password.fill(text); return this; }
  async setConfirmPassword(text) { await this.confirmPassword.fill(text); return this; }

  expectFieldVisible(objectName) {
    return gui.expectVisible(this.page, [objectName]);
  }
}

module.exports = { UserEditorPage };
