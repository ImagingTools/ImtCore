import QtQuick 2.12
import Acf 1.0

Item {
    id: container;

    signal checkBoxLicenseClicked(string itemId, int modelIndex, int state);
    signal checkBoxExpirationClicked(string itemId, int modelIndex, int state);
    signal textExpirationChanged(string itemId, int modelIndex, string date);

    onCheckBoxLicenseClicked: {
        console.log("InstallationLicenses onCheckBoxLicenseClicked", itemId, modelIndex, state);
        let activeLicensesModel = installationModel.GetData("ActiveLicenses");
        activeLicensesModel.SetData("LicenseState", state, modelIndex);
        activeLicensesModel.SetData("ExpirationState", 0, modelIndex);
        activeLicensesModel.SetData("Expiration", "Unlimited", modelIndex);
    }

    onCheckBoxExpirationClicked: {
        let activeLicensesModel = installationModel.GetData("ActiveLicenses");

        activeLicensesModel.SetData("ExpirationState", state, modelIndex);

        if (state === 2){
            activeLicensesModel.SetData("Expiration", "2023-01-01", modelIndex);
        }
        else if (state === 0){
            activeLicensesModel.SetData("Expiration", "Unlimited", modelIndex);
        }
    }

    onTextExpirationChanged: {
        console.log("InstallationLicenses onTextExpirationChanged",itemId, date, modelIndex);
        let activeLicensesModel = installationModel.GetData("ActiveLicenses");

        if (activeLicensesModel){
            activeLicensesModel.SetData("Expiration", date, modelIndex);
        }
    }
}
