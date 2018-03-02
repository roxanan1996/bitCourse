#include <iostream>
#include <string>

class Person {
public:
	Person(std::string name_) :name(name_) {
		std::cout << "Creating A person named " << name << "\n";
	}
	virtual ~Person() {
		std::cout << "Destroying THE person named " << name << "\n";
	}
protected:
	Person() {}
	std::string name;
};
class SpecialisationList {
public:
	SpecialisationList() {
		std::cout << "Creating A specialisation list\n";
	}
	virtual ~SpecialisationList() {
		std::cout << "Distroying THE specialisation list\n";
	}
};
class Student : public Person {
public:
	Student(std::string name_, int year_) : Person(name_) {
		std::cout << "Creating a student named " << name_ << " in year " << year_ << "\n";
			if (year < 1 || year > 4) {
				std::cout << "The student must be in year 1 to 4! Throwing exception!\n";
					throw std::string("The student must be in year 1 to 4!");
			}
		year = year_;
	}
	virtual ~Student() {
		std::cout << "Destroying the student named " << name << " in year " << year << "\n";
	}
private:
	Student() {}
	SpecialisationList specialisationList;
	int year;
};
int main(int argc, const char * argv[]) {
	try {
		Student john("John", 2);
		Student jack("Jack", 0);
	}
	catch (std::string ex) {
		std::cout << "EXCEPTION: " << ex << "\n";
	}
	return 0;
}