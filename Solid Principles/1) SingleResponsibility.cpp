#include <iostream>
using namespace std;

/*
Bad Design - Employee has three responsibilities:
    Store employee data.
    Calculate salary.
    Save to database.
*/
class Employee{
    string name;
    int salary;
public:
    Employee(string name,int salary){
        this->name = name;
        this->salary = salary;
    }
    void calculateSalary(){
        cout<<"Salary: "<<salary<<endl;
    }
    void saveToDatabase(){
        cout<<"Employee saved to database.\n";
    }
    void printDetails(){
        cout<<"Name: "<<name<<endl;
    }
};
int main(){
    Employee emp("Aditya",500000);
    emp.printDetails();
    emp.calculateSalary();
    emp.saveToDatabase();
    return 0;
}


// --------------------------------------------------------------------------------------------
/*
Good Design - Employee has 1 responsibility
*/

// Responsibility 1: Store employee data
class Employee{
    string name;
    int salary;
public:
    Employee(string name,int salary){
        this->name = name;
        this->salary = salary;
    }
    string getName(){
        return name;
    }
    int getSalary(){
        return salary;
    }
};

// Responsibility 2: Salary related operations
class SalaryCalculator{
public:
    void calculateSalary(Employee &emp){
        cout<<"Salary: "<<emp.getSalary()<<endl;
    }
};

// Responsibility 3: Database operations
class EmployeeDatabase{
public:
    void save(Employee &emp){
        cout<<emp.getName()<<"saved to database";
    }
};

int main(){
    Employee emp("Aditya",50000);
    SalaryCalculator salary;
    EmployeeDatabase db;

    salary.calculateSalary(emp);
    db.save(emp);

    return 0;
}