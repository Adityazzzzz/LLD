#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

using namespace std;

// ==========================================
// 1. OBSERVER PATTERN (Notifications)
// ==========================================
class ISubscriber {
public:
    virtual ~ISubscriber() = default;
    virtual void update(const string& message) = 0;
};

// ==========================================
// 2. CORE USER ENTITY (Publisher & Subscriber)
// ==========================================
class User : public ISubscriber {
private:
    int id;
    string name;
    vector<ISubscriber*> subscribers;

public:
    User(int id, string name) : id(id), name(name) {}

    int getId() { return id; }
    string getName() { return name; }

    // Publisher functionality
    void addSubscriber(ISubscriber* sub) {
        subscribers.push_back(sub);
    }

    void notifySubscribers(const string& postTitle) {
        string msg = "New post from " + name + ": '" + postTitle + "'";
        for (auto sub : subscribers) {
            sub->update(msg);
        }
    }

    // Subscriber functionality
    void update(const string& message) override {
        cout << "[" << name << "'s Feed] Notification: " << message << "\n";
    }
};

// ==========================================
// 3. COMMENT ENTITY
// ==========================================
class Comment {
private:
    int id;
    string text;
    User* author;
public:
    Comment(int id, string text, User* author) : id(id), text(text), author(author) {}
    string getText() { return text; }
    User* getAuthor() { return author; }
};

// ==========================================
// 4. FACTORY PATTERN (Polymorphic Posts)
// ==========================================
enum class PostType { TEXT, IMAGE };

class Post {
protected:
    int id;
    string title;
    string content;
    User* author;
    vector<Comment*> comments;
public:
    Post(int id, string title, string content, User* author) 
        : id(id), title(title), content(content), author(author) {}
    
    virtual ~Post() = default;

    virtual void display() = 0; // Pure virtual for formatting

    void addComment(Comment* comment) { comments.push_back(comment); }
    int getId() { return id; }
    string getTitle() { return title; }
    User* getAuthor() { return author; }
    vector<Comment*>& getComments() { return comments; }
};

class TextPost : public Post {
public:
    TextPost(int id, string title, string content, User* author) 
        : Post(id, title, content, author) {}
    
    void display() override {
        cout << "\n[TEXT POST] " << title << " by " << author->getName() << "\n";
        cout << "Body: " << content << "\n";
    }
};

class ImagePost : public Post {
public:
    ImagePost(int id, string title, string url, User* author) 
        : Post(id, title, url, author) {}
    
    void display() override {
        cout << "\n[IMAGE POST] " << title << " by " << author->getName() << "\n";
        cout << "Image Rendered from URL: " << content << "\n";
    }
};

class PostFactory {
public:
    static Post* createPost(PostType type, int id, string title, string content, User* author) {
        switch (type) {
            case PostType::TEXT: return new TextPost(id, title, content, author);
            case PostType::IMAGE: return new ImagePost(id, title, content, author);
            default: return nullptr;
        }
    }
};

// ==========================================
// 5. THE ORCHESTRATOR (Manager)
// ==========================================
class BloggingPlatform {
private:
    unordered_map<int, User*> users;
    unordered_map<int, Post*> posts;
    int postCounter = 1;
    int commentCounter = 1;

public:
    void registerUser(User* user) {
        users[user->getId()] = user;
    }

    void subscribeToAuthor(int readerId, int authorId) {
        if (users.count(readerId) && users.count(authorId)) {
            users[authorId]->addSubscriber(users[readerId]);
        }
    }

    Post* publishPost(int authorId, PostType type, string title, string content) {
        if (!users.count(authorId)) return nullptr;

        User* author = users[authorId];
        Post* newPost = PostFactory::createPost(type, postCounter++, title, content, author);
        posts[newPost->getId()] = newPost;

        // Trigger Observer Pattern Notification
        author->notifySubscribers(title);
        
        return newPost;
    }

    void addComment(int postId, int authorId, string text) {
        if (posts.count(postId) && users.count(authorId)) {
            Comment* comment = new Comment(commentCounter++, text, users[authorId]);
            posts[postId]->addComment(comment);
        }
    }

    void viewPost(int postId) {
        if (!posts.count(postId)) return;
        
        Post* post = posts[postId];
        post->display(); // Polymorphic render
        
        cout << "--- Comments (" << post->getComments().size() << ") ---\n";
        for (auto c : post->getComments()) {
            cout << c->getAuthor()->getName() << ": " << c->getText() << "\n";
        }
        cout << "------------------------------------\n";
    }
};

// ==========================================
// 6. MAIN DRIVER
// ==========================================
int main() {
    BloggingPlatform platform;

    // 1. Register Users
    User* alice = new User(1, "Alice (Author)");
    User* bob = new User(2, "Bob (Reader)");
    User* charlie = new User(3, "Charlie (Reader)");

    platform.registerUser(alice);
    platform.registerUser(bob);
    platform.registerUser(charlie);

    // 2. Setup Subscriptions (Observer Pattern)
    platform.subscribeToAuthor(bob->getId(), alice->getId());
    platform.subscribeToAuthor(charlie->getId(), alice->getId());

    cout << "--- PUBLISHING WORKFLOW ---\n";
    // 3. Create Posts (Factory Pattern) - This will auto-trigger notifications to Bob and Charlie
    Post* post1 = platform.publishPost(1, PostType::TEXT, "Design Patterns 101", "Let's learn Observer and Factory.");
    Post* post2 = platform.publishPost(1, PostType::IMAGE, "UML Diagram", "https://img.host/uml.png");

    // 4. Engage with Content
    if (post1) {
        platform.addComment(post1->getId(), 2, "This completely cleared up my confusion!");
        platform.addComment(post1->getId(), 3, "Can you do State Pattern next?");
    }

    // 5. Polymorphic Viewing
    platform.viewPost(post1->getId());
    platform.viewPost(post2->getId());

    return 0;
}