#include <iostream>
using namespace std;

enum class SeatType{ REGULAR,RECLINER };
enum class BookingStatus{ PENDING,CONFIRMED,CANCELLED,FAILED };
enum class PaymentType{ CARD,UPI };

class Movie{
public:
    string id;
    string title;
    int durationMin;

    Movie(string id,string title,int duration){
        this->id = id;
        this->title = title;
        this->durationMin = duration;
    }
};

class Seat{
protected:
    string id;
    double price;
public:
    Seat(string id,double price){
        this->id = id;
        this->price = price;
    }
    virtual ~Seat() = default;
    virtual SeatType getType() = 0;
    
    string getId(){ return id; }
    double getPrice(){ return price; }
};

class RegularSeat:public Seat{
public:
    RegularSeat(string id,double price) : Seat(id,price){}
    SeatType getType(){
        return SeatType::REGULAR; 
    }
};

class ReclinerSeat:public Seat{
public:
    ReclinerSeat(string id,double price) : Seat(id,price){}
    SeatType getType() override{ 
        return SeatType::RECLINER; 
    }
};

class Screen{
public:
    string id;
    unordered_map<string,Seat*> seats;

    Screen(string id){
        this->id = id;
    }
    void addSeat(Seat* st){ 
        seats[st->getId()] = st; 
    }
};

class Theatre{
public:
    string id;
    string name;
    unordered_map<string,Screen*> screens;

    Theatre(string id,string name){
        this->id = id;
        this->name = name;
    }
    void addScreen(Screen* scn){ 
        screens[scn->id] = scn; 
    }
};

class Show{
public:
    string id;
    Movie* movie;
    string startTime;
    string endTime;
    Theatre* theatre;
    Screen* screen;

    Show(string id,Movie* movie,string start,string end,Theatre* theatre,Screen* screen) : id(id),movie(movie),startTime(start),endTime(end),theatre(theatre),screen(screen){}
};
