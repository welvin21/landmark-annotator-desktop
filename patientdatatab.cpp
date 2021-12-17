#include "patientdatatab.h"

PatientDataTab::PatientDataTab(DesktopApp* parent) {
    QObject::connect(parent->ui.savePatientButton, &QPushButton::clicked, [parent]() {
        bool isPatientDataValid = true;
        
        std::string name, hkid, phone, email, medicalNumber, nationality, address;

        if ((name = parent->ui.nameInput->toPlainText().toStdString()) != "") {
            parent->patient.setName(name);
        }
        else isPatientDataValid = false;


        if ((hkid = parent->ui.idInput->toPlainText().toStdString()) != "") {
            parent->patient.setHKID(hkid);
        }
        else isPatientDataValid = false;

        if ((phone = parent->ui.phoneInput->toPlainText().toStdString()) != "") {
            parent->patient.setPhoneNumber(phone);
        }
        else isPatientDataValid = false;

        if ((email = parent->ui.phoneInput->toPlainText().toStdString()) != "") {
            parent->patient.setEmail(email);
        }
        else isPatientDataValid = false;

        if ((medicalNumber = parent->ui.phoneInput->toPlainText().toStdString()) != "") {
            parent->patient.setMedicalNumber(medicalNumber);
        }
        else isPatientDataValid = false;

        if ((nationality = parent->ui.phoneInput->toPlainText().toStdString()) != "") {
            parent->patient.setNationality(nationality);
        }
        else isPatientDataValid = false;

        if ((address = parent->ui.phoneInput->toPlainText().toStdString()) != "") {
            parent->patient.setAddress(address);
        }
        else isPatientDataValid = false;

        if (parent->ui.male->isChecked()) parent->patient.setSex(Male);
        else if (parent->ui.female->isChecked()) parent->patient.setSex(Female);
        else isPatientDataValid = false;

        try {
            std::string height = parent->ui.heightInput->toPlainText().toStdString();
            std::string weight = parent->ui.weightInput->toPlainText().toStdString();

            float parsedHeight = (float) std::stod(height, nullptr);
            float parsedWeight = (float) std::stod(weight, nullptr);
            parent->patient.setHeight(parsedHeight);
            parent->patient.setWeight(parsedWeight);
        }
        catch (std::exception& ia) {
            isPatientDataValid = false;
        }

        if (!isPatientDataValid) {
            parent->ui.patientDataValidation->setText("Please double check your input.\nNote that height and weight should be a number.");
        }
        else {
            parent->ui.patientDataValidation->setText("All good to go.");
        }

        parent->patient.setValidity(isPatientDataValid);
    });
}
