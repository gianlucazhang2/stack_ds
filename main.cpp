#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stack>
#include <queue>
#include <ios>
#include "notification.h"

class User {
public: 
	User(std::string ID, std::string u, 
	bool a, bool l, bool t, bool c, bool f, bool m) {//parse out the id, username, and notification settings
		id = ID; 
		user = u; 
		all = a; 
		likes = l; 
		tags = t; 
		comments = c; 
		follow = f; 
		msgs = m; 
	}
	
	User() {}
	
	User& operator=(const User& other) {
    if (this != &other) {
        id = other.id;
        user = other.user;
        all = other.all;
		likes = other.likes;  
		tags = other.tags; 
		comments = other.comments; 
		follow = other.follow; 
		msgs = other.msgs; 
    }
    return *this;
	}
	
	std::string getName() {return user;}
	std::string getID() {return id;}
	bool getPauseAll() {return all;}
	bool getLikes() {return likes;}
	bool getTags() {return tags;}
	bool getComments() {return comments;}
	bool getFollow() {return follow;}
	bool getMessages() {return msgs;}
	
private: 
	std::string id, user; 
	bool all, likes, tags, comments, follow, msgs; 
};

std::string extractValue(const std::string& str, int pos) {
    size_t startQuote = str.find('"', pos);
    size_t endQuote = str.find('"', startQuote + 1);
    pos = endQuote;
	return str.substr(startQuote + 1, endQuote - startQuote - 1);
}

bool strToBool (std::string str) {
	if (str == "true") {return true;}
	else {return false; }
}

void printAggregate(std::string outputFile, std::vector<Notification*> group) {
	std::ofstream out(outputFile, std::ios::app); 
	out << group[0]->getUser() << ", "  << group[1]->getUser() << " and " << group.size() - 2 << " others"; 
	if (group[0]->getType() == "likes") {out << " liked your post." << std::endl; }
	if (group[0]->getType() == "tags"){out << " tagged you in a post." << std::endl; }
	if (group[0]->getType() == "comments_on") {out << " commented on your post." << std::endl; }
	if (group[0]->getType() == "follows") {out << " started to follow you." << std::endl; }
	if (group[0]->getType() == "messageRequests") {out << " wants to send you a message." << std::endl; }
}

int main(int argc, char* argv[]) {
	if (argc != 6) {return 1; }
	
	std::string postJson = argv[1]; 
	std::string userJson = argv[2]; 
	std::string eventText = argv[3]; 
	std::string outputFile = argv[4]; 
	std::string username = argv[5]; 
	User user;
	
	std::ofstream out(outputFile); 
	out << ""; 

	
	//find user and their settings based on 
	std::ifstream userFile(userJson); 
	std::string userLine; 
	while (std::getline(userFile, userLine)) {
		int pos = userLine.find("\"id\":"); 
		std::string id = extractValue(userLine, pos+5); 
		
		pos = userLine.find("\"username\":"); 
		std::string un = extractValue(userLine, pos+11); 
		
		
		if (un == username) {
		
		std::string boolStr; 
		
		pos = userLine.find("\"pauseAll\":"); 
		boolStr = extractValue(userLine, pos+11);
		bool all = strToBool(boolStr); 
		
		pos = userLine.find("\"likes\":"); 
		boolStr = extractValue(userLine, pos+8); 
		bool likes = strToBool(boolStr);
		
		pos = userLine.find("\"tags\":"); 
		boolStr = extractValue(userLine, pos+7); 
		bool tags = strToBool(boolStr);
		
		pos = userLine.find("\"comments\":"); 
		boolStr = extractValue(userLine, pos+11); 
		bool comments = strToBool(boolStr);
		
		pos = userLine.find("\"newFollowers\":"); 
		boolStr = extractValue(userLine, pos+15); 
		bool follow = strToBool(boolStr);
		
		pos = userLine.find("\"messageRequests\":"); 
		boolStr = extractValue(userLine, pos+18); 
		bool msgs = strToBool(boolStr);
		
		User* userPtr = new User(id, un, all, likes, tags, comments, follow, msgs); 
		user = *userPtr;
		delete userPtr; 
		break;
		}
	}
	
	//find all posts created by the user
	std::vector<std::string> posts; 
	std::ifstream postFile(postJson); 
	std::string postLine; 
	while (std::getline(postFile, postLine)) {
		int pos = postLine.find("\"id\":");
		std::string id = extractValue(postLine, pos+4);
		
		pos = postLine.find("\"ownerUsername\":"); 
		std::string un = extractValue(postLine, pos+15); 
		
		if (un == username) {
			posts.push_back(id); 
		}
	}
	
	//create stack of notifications pertaining to the specified user
	std::stack<Notification*> notifs; 
	Notification* notif = nullptr; 
	std::ifstream eventFile(eventText); 
	std::string eventLine; 
	while (std::getline(eventFile,eventLine)) {
		std::vector<std::string> arguments; 
		std::stringstream ss(eventLine); 
		std::string argument; 
		while (ss >> argument) {
			arguments.push_back(argument); 
		}
		//find events that only pertain to the specified user or their posts
		if (arguments[2] == username || std::find(posts.begin(), posts.end(), arguments[2]) != posts.end()) {
			try {
                if (arguments[1] == "likes" && user.getLikes()) {
                    notif = new Like(arguments[0], arguments[1], arguments[2]);
                } else if (arguments[1] == "tags" && user.getTags()) {
                    notif = new Tag(arguments[0], arguments[1], arguments[2]);
                } else if (arguments[1] == "comments_on" && user.getComments()) {
                    notif = new Comment(arguments[0], arguments[1], arguments[2]);
                } else if (arguments[1] == "follows" && user.getFollow()) {
                    notif = new Follow(arguments[0], arguments[1], arguments[2]);
                } else if (arguments[1] == "messageRequests" && user.getMessages()) {
                    notif = new Message(arguments[0], arguments[1], arguments[2]);
                }
				else {continue;}
                notifs.push(notif);
            } catch(std::bad_alloc& e) {
                std::cerr << "Memory allocation failed: " << e.what() << std::endl;
            }
		}
	}
	
	if (user.getPauseAll()) {//pause all case
		while (!notifs.empty()) {
			delete notifs.top();
			notifs.pop();
		}
		return 1; 
	}
	
	//print out first 100 notification for first 100 events
	int count = 0;
	std::vector<Notification*> group;
	if (!notifs.empty()) {
		group.push_back(notifs.top());
	}
	
	while (count < 100 && !notifs.empty()) {
		Notification* temp = notifs.top();
		if (count == 0) {
			notifs.pop(); 
			if (!notifs.empty()) {
				temp = notifs.top(); 
			}
			else {
				group[0]->print(outputFile); 
				group.clear(); 
				break; 
			}
		} 
		if (group.empty() || temp->getType() != group[0]->getType()) {
			if (group.size() < 4) {
				for (Notification* n: group) {
					n->print(outputFile);
					delete n;  
				}
			}
			else {
				printAggregate(outputFile, group);  
				count -= group.size();  
				count++; 
			}
			group.clear();
		}
		group.push_back(temp); 
		notifs.pop();
		++count;
	}
	//get line count from the outputfile
	//sees if there is extra space for the 100 notifications
	if (!group.empty()) {
        std::ifstream file(outputFile); 
		std::string line; 
		int lineCount=0; 
		while (std::getline(file, line)) {lineCount++;}
		if (lineCount < 100) {
			for (int i = 0; i < 100-lineCount; i++) {
				if (group.size() < 4) {
					group[i]->print(outputFile); 
				}
				else {
					printAggregate(outputFile, group); 
					break;
				}
			}
		}
    }
	
	while (!notifs.empty()) {
		delete notifs.top();
		notifs.pop();
	}
	for (Notification* n: group) {
		delete n; 
	}
	group.clear(); 
	
	return 0; 
}