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
    Movie(string id,string title,int duration):id(id),title(title),durationMin(duration){}
};

class Seat{
protected:
    string id;
    double price;
public:
    Seat(string id,double price):id(id),price(price){}
    virtual ~Seat() = default;
    virtual SeatType getType() = 0;
    string getId(){ return id; }
    double getPrice(){ return price; }
};

class RegularSeat:public Seat{
public:
    RegularSeat(string id,double price):Seat(id,price){}
    SeatType getType() override{ return SeatType::REGULAR; }
};

class ReclinerSeat:public Seat{
public:
    ReclinerSeat(string id,double price):Seat(id,price){}
    SeatType getType() override{ return SeatType::RECLINER; }
};

class Screen{
public:
    string id;
    unordered_map<string,Seat*> seats;
    Screen(string id):id(id){}
    void addSeat(Seat* seat){ seats[seat->getId()] = seat; }
};

class Theatre{
public:
    string id;
    string name;
    unordered_map<string,Screen*> screens;
    Theatre(string id,string name):id(id),name(name){}
    void addScreen(Screen* screen){ screens[screen->id] = screen; }
};

class Show{
public:
    string id;
    Movie* movie;
    string startTime;
    string endTime;
    Theatre* theatre;
    Screen* screen;

    Show(string id,Movie* movie,string start,string end,Theatre* theatre,Screen* screen)
       :id(id),movie(movie),startTime(start),endTime(end),theatre(theatre),screen(screen){}
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

    Booking(string bId,string sId,string uId,vector<string> seats,double amt)
       :bookingId(bId),showId(sId),userId(uId),seatIds(seats),amount(amt),status(BookingStatus::PENDING){}
};

class BookingRepo{
private:
    unordered_map<string,Booking*> bookingDB;
    mutex repoMutex;
public:
    void save(Booking* booking){
        lock_guard<mutex> lock(repoMutex);
        bookingDB[booking->bookingId] = booking;
    }
    Booking* get(string id){
        lock_guard<mutex> lock(repoMutex);
        if(bookingDB.find(id) != bookingDB.end()) return bookingDB[id];
        return nullptr;
    }
};

struct LockData{
    string userId;
    chrono::system_clock::time_point expiryTime;
    bool isPermanent; // Used when a booking is confirmed
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
    mutex lockMutex;
    
    // Background Thread Management for TTL cleanup
    thread cleanerThread;
    atomic<bool> isRunning;
    condition_variable cv;

    void cleanupExpiredLocks(){
        while(isRunning){
            unique_lock<mutex> lk(lockMutex);
            cv.wait_for(lk,chrono::minutes(1),[this]{ return !isRunning.load(); });
            
            if(!isRunning) break;

            auto now = chrono::system_clock::now();
            for(auto it = locks.begin(); it != locks.end(); ){
                if(!it->second.isPermanent && now > it->second.expiryTime){
                    cout << "[TTL EXPIRY] Auto-releasing lock for seat key: " << it->first << "\n";
                    it = locks.erase(it);
                } else{
                    ++it;
                }
            }
        }
    }

public:
    InMemoryLockProvider():isRunning(true){
        cleanerThread = thread(&InMemoryLockProvider::cleanupExpiredLocks,this);
    }

    ~InMemoryLockProvider(){
        isRunning = false;
        cv.notify_all();
        if(cleanerThread.joinable()) cleanerThread.join();
    }

    bool tryLock(string key,string userId,int ttlMinutes) override{
        lock_guard<mutex> lock(lockMutex);
        auto now = chrono::system_clock::now();
        
        if(locks.find(key) != locks.end()){
            if(locks[key].isPermanent || now < locks[key].expiryTime){
                return locks[key].userId == userId;
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
            if(locks[key].userId == userId &&(locks[key].isPermanent || chrono::system_clock::now() < locks[key].expiryTime)){
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

class CardPayment:public PaymentStrategy{
public:
    bool pay(Booking* booking) override{
        cout << "[CARD] Processing Rs. " << booking->amount << " for Booking: " << booking->bookingId << "\n";
        return true; // Assume success
    }
};

class UpiPayment:public PaymentStrategy{
public:
    bool pay(Booking* booking) override{
        cout << "[UPI] Processing Rs. " << booking->amount << " for Booking: " << booking->bookingId << "\n";
        return true; // Assume success
    }
};

class PaymentStrategyFactory{
public:
    static PaymentStrategy* getStrategy(PaymentType type){
        switch(type){
            case PaymentType::CARD: return new CardPayment();
            case PaymentType::UPI: return new UpiPayment();
            default: return nullptr;
        }
    }
};

class BookingService{
private:
    BookingRepo* bookingRepo;
    LockProvider* lockProvider;
    int bookingCounter = 1;
    mutex serviceMutex; // For ID generation

    string generateLockKey(string showId,string seatId){
        return showId + "_" + seatId;
    }

public:
    BookingService(BookingRepo* repo,LockProvider* lockProv) 
       :bookingRepo(repo),lockProvider(lockProv){}

    Booking* createBooking(string userId,Show* show,vector<string> seatIds){
        int TTL_MINUTES = 8; // Business requirement constraint
        double totalAmount = 0.0;
        vector<string> lockedSeats;

        //Attempt to lock all requested seats concurrently
        for(string seatId:seatIds){
            string lockKey = generateLockKey(show->id,seatId);
            if(lockProvider->tryLock(lockKey,userId,TTL_MINUTES)){
                lockedSeats.push_back(seatId);
                totalAmount += show->screen->seats[seatId]->getPrice();
            } else{
                cout << "[FAILED] User " << userId << " could not lock seat " << seatId << ". It is occupied.\n";
                // Rollback: Release any seats locked during this transaction loop
                for(string locked:lockedSeats){
                    lockProvider->unlock(generateLockKey(show->id,locked));
                }
                return nullptr;
            }
        }

        //Generate Booking object
        serviceMutex.lock();
        string bId = "BKG-" + to_string(bookingCounter++);
        serviceMutex.unlock();

        Booking* booking = new Booking(bId,show->id,userId,lockedSeats,totalAmount);
        bookingRepo->save(booking);

        cout << "[SUCCESS] User " << userId << " locked " << seatIds.size() << " seat(s) for 8 mins. Booking ID: " << bId << "\n";
        return booking;
    }

    void confirmBooking(Booking* booking,PaymentType paymentType){
        //Validate locks haven't expired
        for(string seatId:booking->seatIds){
            string lockKey = generateLockKey(booking->showId,seatId);
            if(!lockProvider->isLockedBy(lockKey,booking->userId)){
                cout << "[EXPIRED] Payment failed. TTL expired for booking " << booking->bookingId << "\n";
                booking->status = BookingStatus::FAILED;
                return;
            }
        }

        //Execute Strategy Payment
        PaymentStrategy* paymentStrat = PaymentStrategyFactory::getStrategy(paymentType);
        bool success = paymentStrat->pay(booking);

        //Process Result
        if(success){
            booking->status = BookingStatus::CONFIRMED;
            booking->paymentType = paymentType;
            
            // Convert temporary locks to permanent bookings
            for(string seatId:booking->seatIds){
                lockProvider->makeLockPermanent(generateLockKey(booking->showId,seatId));
            }
            cout << ">>> Booking " << booking->bookingId << " CONFIRMED!\n";
        } 
        else{
            // High Concurrency requirement: failed payments free the seat immediately
            booking->status = BookingStatus::FAILED;
            for(string seatId:booking->seatIds){
                lockProvider->unlock(generateLockKey(booking->showId,seatId));
            }
            cout << ">>> Payment failed. Locks released for " << booking->bookingId << "\n";
        }
        delete paymentStrat;
    }
};

int main(){
    BookingRepo* repo = new BookingRepo();
    LockProvider* lockProv = new InMemoryLockProvider();
    BookingService* service = new BookingService(repo,lockProv);

    Movie* inception = new Movie("M1","Inception",148);
    Theatre* pvr = new Theatre("T1","PVR Cinemas");
    Screen* screen1 = new Screen("SCR1");
    
    screen1->addSeat(new ReclinerSeat("A1",500.0));
    screen1->addSeat(new RegularSeat("A2",300.0));
    screen1->addSeat(new RegularSeat("A3",300.0));
    pvr->addScreen(screen1);

    Show* eveningShow = new Show("SH-001",inception,"18:00","21:00",pvr,screen1);

    //Thread Safe Double-Booking Test
    // Alice and Bob try to book seat A1 at the exact same millisecond
    thread t1([&](){ service->createBooking("User-Alice",eveningShow,{"A1","A2"}); });
    thread t2([&](){ service->createBooking("User-Bob",eveningShow,{"A1","A3"}); });

    t1.join();
    t2.join();

    //Payment Flow Test
    cout << "\n--- PAYMENT TEST --- \n";
    Booking* aliceBooking = repo->get("BKG-1"); // Assuming Alice won the race
    if(aliceBooking){
        service->confirmBooking(aliceBooking,PaymentType::UPI);
    }

    //Cleanup
    //(Destructor of InMemoryLockProvider safely kills the background TTL cleaner thread)
    delete service; delete lockProv; delete repo;
    return 0;
}