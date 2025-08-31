#include <iostream>
#include <string>
#include <map>
using namespace std;

// 🎨 Colors
#define GREEN  "\033[1;32m"
#define RED    "\033[1;31m"
#define YELLOW "\033[1;33m"
#define CYAN   "\033[1;36m"
#define RESET  "\033[0m"

// ----- WebPage Node -----
class WebPage {
public:
    string url;
    WebPage* prev;
    WebPage* next;
    WebPage(string urlVal) : url(urlVal), prev(nullptr), next(nullptr) {}
};

// ----- BrowserHistory (per user) -----
class BrowserHistory {
private:
    WebPage* current;
public:
    BrowserHistory() : current(nullptr) {}

    void visit(string url) {
        WebPage* newPage = new WebPage(url);

        if (current == nullptr) {
            current = newPage;
        } else {
            // clear forward history
            WebPage* temp = current->next;
            while (temp != nullptr) {
                WebPage* nextPage = temp->next;
                delete temp;
                temp = nextPage;
            }
            current->next = newPage;
            newPage->prev = current;
            current = newPage;
        }
        cout << GREEN << "[OK] Visited: " << url << RESET << endl;
    }

    void goBack() {
        if (current == nullptr) {
            cout << RED << "[X] No history available!" << RESET << endl;
        } else if (current->prev == nullptr) {
            cout << YELLOW << "[!] Already at the first page!" << RESET << endl;
        } else {
            current = current->prev;
            cout << CYAN << "<- Back to: " << current->url << RESET << endl;
        }
    }

    void goForward() {
        if (current == nullptr) {
            cout << RED << "[X] No history available!" << RESET << endl;
        } else if (current->next == nullptr) {
            cout << YELLOW << "[!] Already at the latest page!" << RESET << endl;
        } else {
            current = current->next;
            cout << CYAN << "-> Forward to: " << current->url << RESET << endl;
        }
    }

    void showCurrent() {
        if (current == nullptr) {
            cout << RED << "[X] No page open" << RESET << endl;
        } else {
            cout << GREEN << "[Current Page] " << current->url << RESET << endl;
        }
    }

    void showAllHistory() {
        if (current == nullptr) {
            cout << RED << "[X] No browsing history" << RESET << endl;
            return;
        }
        WebPage* temp = current;
        while (temp->prev != nullptr) temp = temp->prev;

        cout << "\n" << CYAN << "=== FULL BROWSING HISTORY ===" << RESET << endl;
        int pos = 1;
        while (temp != nullptr) {
            if (temp == current) {
                cout << pos << ". [" << GREEN << temp->url << RESET << "] <-- CURRENT";
            } else {
                cout << pos << ". [" << temp->url << "]";
            }
            cout << endl;
            temp = temp->next;
            pos++;
        }
    }

    void searchHistory(string keyword) {
        if (current == nullptr) {
            cout << RED << "[X] No browsing history" << RESET << endl;
            return;
        }
        WebPage* temp = current;
        while (temp->prev != nullptr) temp = temp->prev;

        int pos = 1;
        bool found = false;
        while (temp != nullptr) {
            if (temp->url.find(keyword) != string::npos) {
                cout << GREEN << "[OK] Found \"" << keyword 
                     << "\" at position " << pos 
                     << " (" << temp->url << ")" << RESET << endl;
                found = true;
            }
            temp = temp->next;
            pos++;
        }
        if (!found) {
            cout << RED << "[X] \"" << keyword << "\" not found in history" << RESET << endl;
        }
    }

    void clearHistory() {
        if (current == nullptr) {
            cout << YELLOW << "[!] History is already empty!" << RESET << endl;
            return;
        }
        WebPage* temp = current;
        while (temp->prev != nullptr) temp = temp->prev;

        while (temp != nullptr) {
            WebPage* nextPage = temp->next;
            delete temp;
            temp = nextPage;
        }
        current = nullptr;
        cout << GREEN << "[OK] All browsing history cleared!" << RESET << endl;
    }

    ~BrowserHistory() { clearHistory(); }
};

// ----- User class -----
class User {
public:
    string username;
    BrowserHistory history;
    User(string name) : username(name) {}
};

// ----- Global users database -----
map<string, User*> users;
User* currentUser = nullptr;

// ----- Menus -----
void showMainMenu() {
    cout << CYAN << "\n=== LOGIN MENU ===" << RESET << endl;
    cout << "[1] Sign up\n";
    cout << "[2] Log in\n";
    cout << "[3] Exit\n";
    cout << "Choice: ";
}

void showUserMenu(string username) {
    cout << CYAN << "\n=== USER MENU (" << username << ") ===" << RESET << endl;
    cout << "[1] Visit Website\n";
    cout << "[2] <- Go Back\n";
    cout << "[3] -> Go Forward\n";
    cout << "[4] Show Current Page\n";
    cout << "[5] Show Full History\n";
    cout << "[6] Clear History\n";
    cout << "[7] Search in History\n";
    cout << "[8] Logout\n";
    cout << "Choice: ";
}

void showAdminMenu() {
    cout << CYAN << "\n=== ADMIN MENU ===" << RESET << endl;
    cout << "[1] View all users\n";
    cout << "[2] View history of a specific user\n";
    cout << "[3] Logout\n";
    cout << "Choice: ";
}

// ----- Main program -----
int main() {
    int choice;
    string uname, url, keyword;

    while (true) {
        if (currentUser == nullptr) {
            // Not logged in
            showMainMenu();
            cin >> choice;

            if (choice == 1) { // signup
                cout << "Enter new username: ";
                cin >> uname;
                if (users.count(uname)) {
                    cout << RED << "[X] Username already exists!" << RESET << endl;
                } else {
                    users[uname] = new User(uname);
                    cout << GREEN << "[OK] User created. You can now log in." << RESET << endl;
                }
            }
            else if (choice == 2) { // login
                cout << "Enter username: ";
                cin >> uname;
                if (uname == "admin") {
                    currentUser = new User("admin"); // admin is special
                    cout << GREEN << "[OK] Logged in as ADMIN" << RESET << endl;
                }
                else if (users.count(uname)) {
                    currentUser = users[uname];
                    cout << GREEN << "[OK] Welcome, " << uname << RESET << endl;
                } else {
                    cout << RED << "[X] No such user. Please sign up first." << RESET << endl;
                }
            }
            else if (choice == 3) {
                cout << GREEN << "[OK] Exiting program. Bye!" << RESET << endl;
                break;
            }
            else {
                cout << RED << "[X] Invalid choice!" << RESET << endl;
            }
        }
        else if (currentUser->username == "admin") {
            // ADMIN MENU
            showAdminMenu();
            cin >> choice;
            if (choice == 1) {
                cout << CYAN << "Registered Users:" << RESET << endl;
                for (auto &p : users) {
                    cout << "- " << p.first << endl;
                }
            }
            else if (choice == 2) {
                cout << "Enter username to view history: ";
                cin >> uname;
                if (users.count(uname)) {
                    users[uname]->history.showAllHistory();
                } else {
                    cout << RED << "[X] No such user." << RESET << endl;
                }
            }
            else if (choice == 3) {
                delete currentUser; // free admin session
                currentUser = nullptr;
                cout << YELLOW << "[!] Logged out from ADMIN" << RESET << endl;
            }
        }
        else {
            // NORMAL USER MENU
            showUserMenu(currentUser->username);
            cin >> choice;
            switch (choice) {
                case 1:
                    cout << "Enter website URL: ";
                    cin >> url;
                    currentUser->history.visit(url);
                    break;
                case 2: currentUser->history.goBack(); break;
                case 3: currentUser->history.goForward(); break;
                case 4: currentUser->history.showCurrent(); break;
                case 5: currentUser->history.showAllHistory(); break;
                case 6: currentUser->history.clearHistory(); break;
                case 7:
                    cout << "Enter keyword to search: ";
                    cin >> keyword;
                    currentUser->history.searchHistory(keyword);
                    break;
                case 8:
                    cout << YELLOW << "[!] Logged out from " << currentUser->username << RESET << endl;
                    currentUser = nullptr;
                    break;
                default:
                    cout << RED << "[X] Invalid choice!" << RESET << endl;
            }
        }
    }
    return 0;
}