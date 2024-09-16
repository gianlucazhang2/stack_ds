#include <string>
#include <iostream>
#include <fstream>
#include <ios>

class Notification {

public:
    Notification(std::string u, std::string t) : user(u), type(t) {}

    std::string getUser() { return user; }
    std::string getType() { return type; }
	virtual void print(std::string output) {
		std::ofstream out(output, std::ios::app); 
		out << user << " " << type << std::endl; 
	}	
	virtual ~Notification() {}

protected:
    std::string user, type;
};

//===============================================//

class Like : public Notification {

public:
    Like(std::string u, std::string t, std::string id)
        : Notification(u, t), post_id(id) {}

    std::string getID() { return post_id; }
	
	void print(std::string output) {
		std::ofstream out(output, std::ios::app); 
		out << user << " liked your post." << std::endl;
	}

private:
    std::string post_id;
};

//===============================================//

class Tag : public Notification {

public:
    Tag(std::string u, std::string t, std::string o)
        : Notification(u, t), other(o) {}

    std::string getOtherUser() { return other; }
	
	void print(std::string output) {
		std::ofstream out(output, std::ios::app); 
		out << user <<" tagged you in a post." << std::endl;
	}

private:
    std::string other;
};

//===============================================//

class Comment : public Notification {

public:
    Comment(std::string u, std::string t, std::string id)
        : Notification(u, t), post_id(id) {}

    std::string getID() { return post_id; }
	
	void print(std::string output) {
		std::ofstream out(output, std::ios::app); 
		out << user << " commented on your post." << std::endl;
	}

private:
    std::string post_id;
};

//===============================================//

class Follow : public Notification {

public:
    Follow(std::string u, std::string t, std::string o)
        : Notification(u, t), other(o) {}

    std::string getOtherUser() { return other; }
	
	void print(std::string output) {
		std::ofstream out(output, std::ios::app); 
		out << user << " started following you." << std::endl;
	}

private:
    std::string other;
};

//===============================================//

class Message : public Notification {

public:
    Message(std::string u, std::string t, std::string o)
        : Notification(u, t), other(o) {}

    std::string getOtherUser() { return other; }
	
	void print(std::string output) {
		std::ofstream out(output, std::ios::app); 
		out << user << " wants to send you a message." << std::endl;
	}

private:
    std::string other;
};