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
        switch(type){
            case PostType::TEXT: return new TextPost(id,title,content,author);
            case PostType::IMAGE: return new ImagePost(id,title,content,author);
            default: return nullptr;
        }
    }
};

class BloggingPlatform{
private:
    unordered_map<int,User*> users;
    unordered_map<int,Post*> posts;
    int postCounter = 1;
    int commentCounter = 1;
public:
    void registerUser(User* user){
        users[user->getId()] = user;
    }

    void subscribeToAuthor(int readerId,int authorId){
        if(users.count(readerId) && users.count(authorId)){
            users[authorId]->addSubscriber(users[readerId]);
        }
    }

    Post* publishPost(int authorId,PostType type,string title,string content){
        if(!users.count(authorId)) return nullptr;

        User* author = users[authorId];
        Post* newPost = PostFactory::createPost(type,postCounter++,title,content,author);
        posts[newPost->getId()] = newPost;

        // Trigger Observer Pattern Notification
        author->notifySubscribers(title);
        
        return newPost;
    }

    void addComment(int postId,int authorId,string text){
        if(posts.count(postId) && users.count(authorId)){
            Comment* comment = new Comment(commentCounter++,text,users[authorId]);
            posts[postId]->addComment(comment);
        }
    }

    void viewPost(int postId){
        if(!posts.count(postId)) return;
        
        Post* post = posts[postId];
        post->display();
        
        for(auto it:post->getComments()){
            cout << it->getAuthor()->getName() << ": " << it->getText() << "\n";
        }
    }
};

int main() {
    BloggingPlatform platform;

    User* alice = new User(1,"Alice (Author)");
    User* bob = new User(2,"Bob (Reader)");
    User* charlie = new User(3,"Charlie (Reader)");

    platform.registerUser(alice);
    platform.registerUser(bob);
    platform.registerUser(charlie);

    //Setup Subscriptions (Observer Pattern)
    platform.subscribeToAuthor(bob->getId(),alice->getId());
    platform.subscribeToAuthor(charlie->getId(),alice->getId());

    //Create Posts (Factory Pattern) - This will auto-trigger notifications to Bob and Charlie
    Post* post1 = platform.publishPost(1,PostType::TEXT,"Design Patterns 101","Let's learn Observer and Factory.");
    Post* post2 = platform.publishPost(1,PostType::IMAGE,"UML Diagram","https://img.host/uml.png");

    if(post1){
        platform.addComment(post1->getId(),2,"This completely cleared up my confusion!");
        platform.addComment(post1->getId(),3,"Can you do State Pattern next?");
    }

    platform.viewPost(post1->getId());
    platform.viewPost(post2->getId());

    return 0;
}