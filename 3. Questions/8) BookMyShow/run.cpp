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

class Booking{
public:
    string bookingId;
    string showId;
    string userId;
    vector<string> seatIds;
    BookingStatus status;
    PaymentType paymentType;
    double amount;

    Booking(string bId,string sId,string uId,vector<string> seats,double amt) : bookingId(bId),showId(sId),userId(uId),seatIds(seats),amount(amt),status(BookingStatus::PENDING){}
};

//-------------------------------------------------
class BookingRepo{
private:
    unordered_map<string,Booking*> bookingDB;
    mutex m;
public:
    void save(Booking* booking){
        lock_guard<mutex> lock(m);

        bookingDB[booking->bookingId] = booking;
    }

    Booking* get(string id){
        lock_guard<mutex> lock(m);

        if(bookingDB.find(id) != bookingDB.end()){
            return bookingDB[id];
        }
        return nullptr;
    }
};

// Concurrency
struct LockData{
    string userId;
    chrono::system_clock::time_point expiryTime;
    bool isPermanent;
};

class LockProvider{
public:
    virtual ~LockProvider() = default;

    virtual bool tryLock(string key,string userId,int ttlMinutes) = 0;
    virtual void unlock(string key) = 0;
    virtual bool isLockedBy(string key,string userId) = 0;
    virtual void makeLockPermanent(string key) = 0;
};

class InMemoryLockProvider:public LockProvider{
private:
    unordered_map<string,LockData> locks;
    mutex m;

    // Background Thread Management for TTL cleanup
    thread cleanerThread;
    atomic<bool> isRunning;
    condition_variable cv;

    void cleanupExpiredLocks(){
        while(isRunning){
            unique_lock<mutex> lk(m);

            // Waits 1 minute or wakes up instantly if the destructor triggers 'cv.notify_all()'
            cv.wait_for(lk,chrono::minutes(1),[this]{ 
                return !isRunning.load(); 
            });

            if(!isRunning) break;
            
            auto now = chrono::system_clock::now();
            for(auto it = locks.begin(); it != locks.end(); ){
                if(!it->second.isPermanent && now > it->second.expiryTime){
                    cout << "[TTL EXPIRY] Auto-releasing lock for seat key: " << it->first << "\n";
                    it = locks.erase(it);
                } 
                else{
                    ++it;
                }
            }
        }
    }

public:
    InMemoryLockProvider() : isRunning(true){
        cleanerThread = thread( &InMemoryLockProvider::cleanupExpiredLocks,this);
    }
    ~InMemoryLockProvider(){
        isRunning = false;
        cv.notify_all();
        if(cleanerThread.joinable()) cleanerThread.join();
    }

    bool tryLock(string key,string userId,int ttlMinutes) override{
        lock_guard<mutex> lock(lockMutex);
        auto now = chrono::system_clock::now();
        
        // If locked by someone else and it hasn't expired
        if(locks.find(key) != locks.end()){
            if(locks[key].isPermanent || now < locks[key].expiryTime){
                return locks[key].userId == userId; // Allowed if it's the same user retrying
            }
        }

        // Grant lock
        locks[key] ={userId,now + chrono::minutes(ttlMinutes),false};
        return true;
    }

    void unlock(string key) override{
        lock_guard<mutex> lock(lockMutex);
        locks.erase(key);
    }

    bool isLockedBy(string key,string userId) override{
        lock_guard<mutex> lock(lockMutex);
        if(locks.find(key) != locks.end()){
            if(locks[key].userId == userId && (locks[key].isPermanent || chrono::system_clock::now() < locks[key].expiryTime)){
                return true;
            }
        }
        return false;
    }

    void makeLockPermanent(string key) override{
        lock_guard<mutex> lock(lockMutex);
        if(locks.find(key) != locks.end()){
            locks[key].isPermanent = true;
        }
    }

};

class PaymentStrategy{
public:
    virtual ~PaymentStrategy() = default;
    virtual bool pay(Booking* booking) = 0;
};

class CardPayment : public PaymentStrategy{
public:
    bool pay(Booking* booking) override{
        cout << "[CARD] Processing Rs. " << booking->amount << " for Booking: " << booking->bookingId << "\n";
        return true; // Assume success
    }
};
class UpiPayment : public PaymentStrategy{
public:
    bool pay(Booking* booking) override{
        cout << "[UPI] Processing Rs. " << booking->amount << " for Booking: " << booking->bookingId << "\n";
        return true; // Assume success
    }
};