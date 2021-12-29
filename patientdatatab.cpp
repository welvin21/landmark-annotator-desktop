#include "patientdatatab.h"

PatientDataTab::PatientDataTab(DesktopApp* parent) {
    this->parent = parent;

    QObject::connect(parent->ui.savePatientButton, &QPushButton::clicked, [this]() {
        bool isPatientDataValid = true;
        
        std::string name, hkid, phone, email, medicalNumber, nationality, address;

        if ((name = this->parent->ui.nameInput->toPlainText().toStdString()) != "") {
            this->parent->patient.setName(name);
        }
        else isPatientDataValid = false;


        if ((hkid = this->parent->ui.idInput->toPlainText().toStdString()) != "") {
            this->parent->patient.setHKID(hkid);
        }
        else isPatientDataValid = false;

        if ((phone = this->parent->ui.phoneInput->toPlainText().toStdString()) != "") {
            this->parent->patient.setPhoneNumber(phone);
        }
        else isPatientDataValid = false;

        if ((email = this->parent->ui.phoneInput->toPlainText().toStdString()) != "") {
            this->parent->patient.setEmail(email);
        }
        else isPatientDataValid = false;

        if ((medicalNumber = this->parent->ui.phoneInput->toPlainText().toStdString()) != "") {
            this->parent->patient.setMedicalNumber(medicalNumber);
        }
        else isPatientDataValid = false;

        if ((nationality = this->parent->ui.phoneInput->toPlainText().toStdString()) != "") {
            this->parent->patient.setNationality(nationality);
        }
        else isPatientDataValid = false;

        if ((address = this->parent->ui.phoneInput->toPlainText().toStdString()) != "") {
            this->parent->patient.setAddress(address);
        }
        else isPatientDataValid = false;

        if (this->parent->ui.male->isChecked()) this->parent->patient.setSex(Male);
        else if (this->parent->ui.female->isChecked()) this->parent->patient.setSex(Female);
        else isPatientDataValid = false;

        try {
            // Height and weight should be a number
            std::string height = this->parent->ui.heightInput->toPlainText().toStdString();
            std::string weight = this->parent->ui.weightInput->toPlainText().toStdString();

            float parsedHeight = (float) std::stod(height, nullptr);
            float parsedWeight = (float) std::stod(weight, nullptr);
            this->parent->patient.setHeight(parsedHeight);
            this->parent->patient.setWeight(parsedWeight);
        }
        catch (std::exception& ia) {
            isPatientDataValid = false;
        }

        if (!isPatientDataValid) {
            this->parent->ui.patientDataValidation->setText("Please double check your input.\nNote that height and weight should be a number.");
        }
        else {
            if (this->savePatientData()) {
                this->parent->ui.patientDataValidation->setText("All good to go!");
            }
            else {
                this->parent->ui.patientDataValidation->setText("Something went wrong while saving patient data.");
            }
        }

        this->parent->patient.setValidity(isPatientDataValid);
    });
}

bool PatientDataTab::savePatientData() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save patient data"), QString(), tr("Text (*.txt)"));
        
    if (!fileName.isEmpty()) {
        this->parent->savePath = QFileInfo(fileName).dir();

        QFile file(fileName);
        file.open(QIODevice::WriteOnly | QIODevice::Text);

        QTextStream out(&file);
        out << "Full name: " << QString::fromStdString(this->parent->patient.getName()) << "\n";
        out << "HKID: " << QString::fromStdString(this->parent->patient.getHKID()) << "\n";
        out << "Phone number: " << QString::fromStdString(this->parent->patient.getPhoneNumber()) << "\n";
        out << "Email: " << QString::fromStdString(this->parent->patient.getEmail()) << "\n";
        out << "Medical number: " << QString::fromStdString(this->parent->patient.getMedicalNumber()) << "\n";
        out << "Nationality: " << QString::fromStdString(this->parent->patient.getNationality()) << "\n";
        out << "Address: " << QString::fromStdString(this->parent->patient.getAddress()) << "\n";

        std::string sex = this->parent->patient.getSex() == Male ? "Male" : "Female";
        out << "Sex: " << QString::fromStdString(sex) << "\n";

        float height = this->parent->patient.getHeight();
        out << "Height: " << QString::fromStdString(std::to_string(height)) << " cm\n";

        float weight = this->parent->patient.getWeight();
        out << "Weight: " << QString::fromStdString(std::to_string(weight)) << " kg\n";

        file.close();

        return true;
    }
    else return false;
}
