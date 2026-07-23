#include <iostream>
using namespace std;

/*
    One object notifies multiple dependent objects whenever its state changes.
    
    YouTuber uploads video
            ↓
    Subscribers get notification
*/

class Subscriber{
public:
    virtual void update() = 0;
};

class User : public Subscriber{
    string name;
public:
    User(string n) : name(n){}
    void update() override{
        cout<<name<<" received notification\n";
    }
};

class YouTubeChannel{
    vector<Subscriber*> users;
public:
    void subscribe(Subscriber* s){
        users.push_back(s);
    }
    void notify(){
        for(auto user : users){
            user->update();
        }
    }
};

int main(){
    User u1("Aditya");
    User u2("Rahul");

    YouTubeChannel yt;

    yt.subscribe(&u1);
    yt.subscribe(&u2);

    yt.notify();
}