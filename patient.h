#include "stdafx.h"

enum Sex {Male, Female};

class Patient {
private:
	std::string name;
	std::string hkid;
	std::string phoneNumber;
	std::string email;
	std::string medicalNumber;
	std::string nationality;
	std::string address;
	Sex sex;
	float height;
	float weight;

public:
	// Setters
	void setName(std::string);
	void setHKID(std::string);
	void setPhoneNumber(std::string);
	void setEmail(std::string);
	void setMedicalNumber(std::string);
	void setNationality(std::string);
	void setAddress(std::string);
	void setSex(Sex);
	void setHeight(float);
	void setWeight(float);

	// Getters 
	std::string getName();
	std::string getHKID();
	std::string getPhoneNumber();
	std::string getEmail();
	std::string getMedicalNumber();
	std::string getNationality();
	std::string getAddress();
	Sex getSex();
	float getHeight();
	float getWeight();

	bool isValid();
};
