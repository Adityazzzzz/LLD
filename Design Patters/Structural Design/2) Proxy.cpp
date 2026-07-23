#include <iostream>
using namespace std;

class Internet{
public:
    virtual void connect() = 0;
    virtual ~Internet(){}
};

class ProxyInternet : public Internet{
    bool isAdmin;
public:
    ProxyInternet(bool admin){
        isAdmin = admin;
    }
    void connect() override{
        if(isAdmin) realInternet.connect();
        else cout<<"Access Denied";
    }
};

int main(){
    ProxyInternet user(false);
    user.connect();

    ProxyInternet admin(true);
    admin.connect();

    return 0;
}