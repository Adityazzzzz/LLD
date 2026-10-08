#include <iostream>
using namespace std;

//observer
class ISubscriber{
public:
    virtual ~ISubscriber() = default;
    virtual void update(const string& msg) = 0;
};

class User:public ISubscriber{
private:
    int id;
    string name;
    vector<ISubscriber*> subscribers;
public:
    User(int id,string name){
        this->id = id;
        this->name = name;
    }
    int getId(){ return id; }
    string getName(){ return name; }

    void addSubscriber(ISubscriber* sub){
        subscribers.push_back(sub);
    }
    void notifySubscriber(string &postTitle){
        string msg = "message";
        for(auto it:subscribers){
            it->update(msg);
        }
    }

    void update(string &msg) override{
        // cout
    }
};

class Comment{
private:
    int id;
    string text;
    User* author;
public:
    Comment(int id,string text,User* author){
        this->id = id;
        this->text = text;
        this->author = author;
    }
    string getText(){ return text; }
    User* getAuthor(){ return author; }
};

//factory
enum class PostType{ TEXT,IMAGE };

class Post{
protected:
    int id;
    string title;
    string content;
    User* author;
    vector<Comment*> comments;
public:
    Post(int id,string title,string content,User* author){
        this->id = id;
        this->title = title;
        this->content = content;
        this->author = author;
    }
    virtual ~Post() = default;
    virtual void display() = 0; 

    void addComment(Comment* comment){ 
        comments.push_back(comment); 
    }
    int getId(){ return id; }
    string getTitle(){ return title; }
    User* getAuthor(){ return author; }
    vector<Comment*>& getComments(){ 
        return comments;
    }
};

class TextPost:public Post{
public:
    TextPost(int id,string title,string content,User* author) : Post(id,title,content,author){}
    
    void display() override{
        cout << author->getName() << "\n";
        cout << "Body: " << content << "\n";
    }
};

class ImagePost:public Post{
public:
    ImagePost(int id,string title,string url,User* author) : Post(id,title,url,author){}
    
    void display() override{
        cout << author->getName() << "\n";
        cout << "Image Rendered from URL: " << content << "\n";
    }
};

class PostFactory{
public:
    static Post* createPost(PostType type,int id,string title,string content,User* author){
        switch (type){
            case PostType::TEXT: return new TextPost(id,title,content,author);
            case PostType::IMAGE: return new ImagePost(id,title,content,author);
            default: return nullptr;
        }
    }
};