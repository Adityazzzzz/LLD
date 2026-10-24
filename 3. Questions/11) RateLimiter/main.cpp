#include<iostream>
using namespace std;

enum class RateLimitType{
    TOKEN_BUCKET,
    FIXED_WINDOW,
    SLIDING_WINDOW_LOG,
    SLIDING_WINDOW_COUNTER
};

class RateLimitConfig{
public:
    int maxRequests;
    int windowInSeconds;

    RateLimitConfig(int maxReq,int windowSec){
        this->maxRequests = maxReq;
        this->windowInSeconds = windowSec;
    }
};

class User{
public:
    string id;
    string tier; // e.g.,"BASIC","PREMIUM"
    User(string id,string tier){
        this->id = id;
        this->tier = tier;
    }
};

class RateLimiter{
protected:
    RateLimitConfig config;
    RateLimitType type;
public:
    RateLimiter(RateLimitConfig cfg,RateLimitType type) : config(cfg),type(type){}
    virtual ~RateLimiter() = default;

    virtual bool allowRequest(string userId) = 0;
};

class FixedWindow:public RateLimiter{
private:
    struct WindowData{
        time_t windowStartTime;
        int counter;
    };
    unordered_map<string,WindowData> userWindows;
public:
    FixedWindow(RateLimitConfig cfg) : RateLimiter(cfg,RateLimitType::FIXED_WINDOW){}
    
    bool allowRequest(string userId) override{
        time_t now = time(nullptr);
        if(userWindows.find(userId) == userWindows.end()){
            userWindows[userId] = {now,1};
            return true;
        }

        WindowData& data = userWindows[userId];

        // If the current time has moved past the active window
        if(now - data.windowStartTime >= config.windowInSeconds){
            data.windowStartTime = now;
            data.counter = 1;
            return true;
        }
        // If within the window,check the limit
        if(data.counter < config.maxRequests){
            data.counter++;
            return true;
        }
        return false;
    }
};

class SlidingWindowLogs:public RateLimiter{
private:
    unordered_map<string,queue<time_t>> userLogs;
public:
    SlidingWindowLogs(RateLimitConfig cfg) : RateLimiter(cfg,RateLimitType::SLIDING_WINDOW_LOG){}

    bool allowRequest(string userId) override{
        time_t now = time(nullptr);
        queue<time_t>& logs = userLogs[userId];

        // Remove outdated timestamps from the front of the queue
        while(!logs.empty() && (now - logs.front() >= config.windowInSeconds)){
            logs.pop();
        }
        // Check if there is capacity in the current sliding window
        if(logs.size() < config.maxRequests){
            logs.push(now);
            return true;
        }
        return false;
    }
};

class TokenBucket:public RateLimiter{
private:
    struct Bucket{
        int tokens;
        time_t lastRefillTime;
    };
    unordered_map<string,Bucket> userBuckets;
public:
    TokenBucket(RateLimitConfig cfg) : RateLimiter(cfg,RateLimitType::TOKEN_BUCKET){}

    bool allowRequest(string userId) override{
        time_t now = time(nullptr);
        int refillRate = config.maxRequests / config.windowInSeconds; // Tokens per second
        if(refillRate == 0) refillRate = 1;

        if(userBuckets.find(userId) == userBuckets.end()){
            userBuckets[userId] = {config.maxRequests - 1,now};
            return true;
        }

        Bucket& bucket = userBuckets[userId];
        time_t timePassed = now - bucket.lastRefillTime;
        
        // Refill tokens
        bucket.tokens = min(config.maxRequests,bucket.tokens + (int)(timePassed * refillRate));
        bucket.lastRefillTime = now;

        if(bucket.tokens > 0){
            bucket.tokens--;
            return true;
        }
        return false;
    }
};

class SlidingWindowCounter:public RateLimiter{
public:
    SlidingWindowCounter(RateLimitConfig cfg) : RateLimiter(cfg,RateLimitType::SLIDING_WINDOW_COUNTER){}
    bool allowRequest(string userId) override{
        return true;
    }
};

class RateLimiterFactory{
public:
    static RateLimiter* createRateLimiter(RateLimitType type,RateLimitConfig config){
        switch (type){
            case RateLimitType::FIXED_WINDOW: return new FixedWindow(config);
            case RateLimitType::SLIDING_WINDOW_LOG: return new SlidingWindowLogs(config);
            case RateLimitType::TOKEN_BUCKET: return new TokenBucket(config);
            case RateLimitType::SLIDING_WINDOW_COUNTER: return new SlidingWindowCounter(config);
            default: return nullptr;
        }
    }
};

class RateLimiterService{
private:
    // Maps User Tier (e.g.,"PREMIUM") to a specific RateLimiter implementation
    unordered_map<string,RateLimiter*> rateLimitersByTier;

public:
    ~RateLimiterService(){
        for(auto const& [tier,limiter]:rateLimitersByTier){
            delete limiter;
        }
    }

    void addTier(string tier,RateLimitType type,RateLimitConfig config){
        rateLimitersByTier[tier] = RateLimiterFactory::createRateLimiter(type,config);
    }

    bool allowRequest(User* user){
        if(rateLimitersByTier.find(user->tier) == rateLimitersByTier.end()){
            cout << "[ERROR] Unknown tier: " << user->tier << "\n";
            return false;
        }

        RateLimiter* limiter = rateLimitersByTier[user->tier];
        bool isAllowed = limiter->allowRequest(user->id);

        if(isAllowed){
            cout << "[ALLOWED] Request processed for " << user->id << " (" << user->tier << ")\n";
        } 
        else{
            cout << "[BLOCKED] Rate limit exceeded for " << user->id << " (" << user->tier << ")\n";
        }

        return isAllowed;
    }
};

int main(){
    RateLimiterService service;

    // Basic tier: 2 requests per 10 seconds (Fixed Window)
    service.addTier("BASIC",RateLimitType::FIXED_WINDOW,RateLimitConfig(2,10));
    // Premium tier: 5 requests per 10 seconds (Sliding Window Logs)
    service.addTier("PREMIUM",RateLimitType::SLIDING_WINDOW_LOG,RateLimitConfig(5,10));

    User* alice = new User("U1","BASIC");
    User* bob = new User("U2","PREMIUM");

    //Test Basic Tier (Fixed Window) - 2 allowed,3rd blocked
    service.allowRequest(alice);
    service.allowRequest(alice);
    service.allowRequest(alice); // Should be BLOCKED

    //Test Premium Tier (Sliding Logs) - 5 allowed,6th blocked
    for(int i=0;i<6;i++){
        service.allowRequest(bob); 
    }

    delete alice;
    delete bob;
    return 0;
}