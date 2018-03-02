#include <iostream>
class Student {
public:
	Student(const char *_name) {
		name = strdup(_name);
	}
	virtual ~Student() {
		free(name);
	}
	void printName() {
		std::cout << "My name is " << name << "\n";
	}
private:
	Student() : name(nullptr) {};
	char *name;
};
class SmartStudent {
public:
	SmartStudent(const char *_name) {
		name = strdup(_name);
	}
	SmartStudent(const SmartStudent &stud) {
		name = strdup(stud.name);
	}
	SmartStudent(SmartStudent &&stud) {
		name = stud.name;
		stud.name = nullptr;
	}
	SmartStudent & operator=(const SmartStudent &stud) {
		name = strdup(stud.name);
		return *this;
	}
	SmartStudent & operator=(SmartStudent &&stud) {
		name = stud.name;
		stud.name = nullptr;
		return *this;
	}

	virtual ~SmartStudent() {
		free(name);
	}
	void printName() {
		std::cout << "My name is " << name << "\n";
	}
private:
	SmartStudent() : name(nullptr) {};
	char *name;
};

/*int main(int argc, const char * argv[]) {
	Student fred("Fred");
	SmartStudent barney("Barney");
	/* This will cause the app to crash:
	{
	Student anotherFred = fred;
	anotherFred.printName();
	}
	*/
/*
	fred.printName();
	{
		SmartStudent barn = barney;
		barn.printName();
	}
	barney.printName();
	return 0;
}*/