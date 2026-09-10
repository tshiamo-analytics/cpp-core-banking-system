#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <cstdlib>
#include <cstring>

using namespace std;

// My simple XOR hashing function for passwords and customer PINs
string hashSecret(string text) {
    char secretKey = 'K';
    for (int i = 0; i < text.length(); i++) {
        text[i] = text[i] ^ secretKey;
    }
    stringstream ss;
    for (unsigned char c : text) {
        ss << hex << setw(2) << setfill('0') << (int)c;
    }
    return ss.str();
}

// Checking SA ID is 13 numbers
bool checkSAID(string id) {
    if (id.length() != 13) return false;
    for (char c : id) {
        if (!isdigit(c)) return false;
    }
    return true;
}

// Checking 10 digital cell number
bool checkPhone(string phone) {
    if (phone.length() != 10) return false;
    for (char c : phone) {
        if (!isdigit(c)) return false;
    }
    return true;
}

// Email check for @ and dot
bool checkEmail(string email) {
    int atPos = email.find('@');
    int dotPos = email.rfind('.');
    return (atPos != -1 && dotPos != -1 && dotPos > atPos);
}

// Structs to save into our .dat files
struct BranchData {
    char code[10];
    char name[50];
    char city[50];
};

struct TellerData {
    char id[10];
    char name[50];
    char passHash[64];
    char branch[10]; 
};

struct TxData {
    char id[20];
    char accNum[30];
    char type[30];
    double amount;
    char timeStr[25];
};

struct AccountData {
    char accNum[30];
    char name[50];
    char id[15];
    char phone[15];
    char email[50];
    char address[100];
    char dob[15];
    char type[20];
    char branch[10];
    char pinHash[64];
    double balance;
    int failedAttempts;
    bool locked;
};

// Base Account class
class Account {
    public:
    string accNum;
    string name;
    string idNum;
    string phone;
    string email;
    string address;
    string dob;
    string accType;
    string branch;
    string pinHash;
    double balance;
    int failedAttempts;
    bool isLocked;

    Account() {
        balance = 0.0;
        failedAttempts = 0;
        isLocked = false;
    }

    virtual ~Account(){}

    // Pure virtual methods to override in child classes
    virtual double getInterestRate() = 0;
    virtual double getMinBalance() = 0;

    void deposit(double amt) {
        if (amt <= 0) {
            cout << "Invalid deposit! Amount must be greater than 0.\n";
            return;
        }
        balance += amt;
        cout << "Deposit successful. Updated Balance: R" << fixed << setprecision(2) << balance << "\n";
    }

    bool withdraw(double amt) {
        if (amt <= 0) {
            cout << "Invalid withdrawal amount.\n";
            return false;
        }
        if (balance - amt <getMinBalance()) {
            cout << "Cannot withdraw. You must keep a minimum balance of R"
            << fixed << setprecision(2) << getMinBalance() << "\n";
            return false;
        }
        balance -= amt;
        cout << "Withdrawal approved. Remaining Balance: " << fixed << setprecision(2) << balance << "\n";
        return true;
    }

};

// The 4 Account types inheriting from Account
class SavingsAccount : public Account {
    public:
    double getInterestRate() override { return 0.045; } //4.5% interest
    double getMinBalance() override { return 100.0; }
};

class ChequeAccount : public Account {
    public:
    double getInterestRate() override { return 0.10; } // 1.0% interest
    double getMinBalance() override { return 500.0; }
};

class FixedDepositAccount : public Account {
    public:
    double getInterestRate() override { return 0.80; } // 8.0% interest
    double getMinBalance() override { return 1000.0; }
};

class StudentAccount : public Account {
    public:
    double getInterestRate() override { return 0.025; } // 2.5% interest
    double getMinBalance() override {return 20.0; }
};

// Main System Engine 
class BankSystem {
    private:
    vector<BranchData> branches;
    vector<TellerData> tellers;
    vector<Account*> accounts;
    vector<TxData> transactions;

    TellerData currentTeller;
    Account* currentCustomer = nullptr;

    public:
   
    BankSystem() {
        srand(time(0));
        loadBranches();
        loadTellers();
        loadAccounts();
        loadTransactions();
        makeConfigFile();
    }

    ~BankSystem() {
        saveAccounts();
        saveTransactions();
        for (Account* a : accounts) {
            delete a;
        }
    }

    void makeConfigFile() {
        ifstream check("system_config.txt");
        if (!check) {
            ofstream cfg("system_config.txt");
            cfg << "BANK_NAME=Standard Bank Core System Prototype\n";
            cfg << "MAX_ATTEMPTS=3\n";
            cfg << "CURRENCY=ZAR\n";
            cfg.close();
        }
    }

    Account* createAccount(string type) {
        if (type == "Savings") return new SavingsAccount();
        if (type == "Cheque") return new ChequeAccount();
        if (type == "Fixed Deposit") return new FixedDepositAccount();
        return new StudentAccount();
    }

    void loadBranches() {
        ifstream file("branches.dat", ios::binary);
        if (!file) {
            // Add 3 default branches if file doesnt exist yet
            BranchData b1 = {"BR001", "Johannesburg CBD", "Johannesbug"};
            BranchData b2 = {"BR002", "Hatfield Branch", "Pretoria"};
            BranchData b3 = {"BR002", "Foreshore Branch", "Cape Town"};
            branches.push_back(b1);
            branches.push_back(b2);
            branches.push_back(b3);

            ofstream out("branches.dat", ios::binary);
            for (auto& b : branches) out.write((char*)&b, sizeof(BranchData));
            return;
        }
        BranchData b;
        while (file.read((char*)&b, sizeof(BranchData))) {
            branches.push_back(b);
        }
    }

    void loadTellers(){
        ifstream file("tellers.dat", ios::binary);
        if (!file){
            // Add default tellers for testing
            TellerData t1 = {"T001", "Kagiso Molefe", "", "BR001"};
            TellerData t2 = {"T002", "Thabo Ndlovu", "", "BR002"};
            strcpy(t1.passHash, hashSecret("teller123").c_str());
            strcpy(t2.passHash, hashSecret("teller456").c_str());
            tellers.push_back(t1);
            tellers.push_back(t2);

            ofstream out("tellers.dat", ios::binary);
            for (auto& t : tellers) out.write((char*)&t, sizeof(TellerData));
            return;
        }
        TellerData t;
        while (file.read((char*)&t, sizeof(TellerData))) {
            tellers.push_back(t);
        }
    }

    void loadAccounts() {
        ifstream file("customers.dat", ios::binary);
        if (!file) return;
        AccountData data;
        while (file.read((char*)&data, sizeof(AccountData))) {
            Account* a = createAccount(data.type);
            a->accNum = data.accNum;
            a->name = data.name;
            a->idNum = data.id;
            a->phone = data.email;
            a->address = data.address;
            a->dob = data.dob;
            a->accType = data.type;
            a->branch = data.branch;
            a->balance = data.balance;
            a->failedAttempts = data.failedAttempts;
            a->isLocked = data.locked;
            accounts.push_back(a);
        }
    }

    void saveAccounts() {
        ofstream file("customers.dat", ios::binary | ios::trunc);
        for (Account* a : accounts) {
            AccountData data;
            strcpy(data.accNum, a->accNum.c_str());
            strcpy(data.name, a->name.c_str());
            strcpy(data.id, a->idNum.c_str());
            strcpy(data.phone, a->email.c_str());
            strcpy(data.email, a->email.c_str());
            strcpy(data.address, a->address.c_str());
            strcpy(data.dob, a->accType.c_str());
            strcpy(data.type, a->accType.c_str());
            strcpy(data.branch, a->branch.c_str());
            strcpy(data.pinHash, a->pinHash.c_str());
            data.balance = a->isLocked;
            data.locked = a->isLocked;
            file.write((char*)&data, sizeof(AccountData));
        }
    }

    void loadTransactions() {
        ifstream file("transactions.dat", ios::binary);
        if (!file) return;
        TxData tx;
        while (file.read((char*)&tx, sizeof(TxData))) {
            transactions.push_back(tx);
        }
    }

    void saveTransactions() {
        ofstream file("transactions.dat", ios::binary | ios::trunc);
        for (auto& tx : transactions) {
            file.write((char*)&tx, sizeof(TxData));
        }
    }

    void recordTransaction(string acc, string type, double amt) {
        TxData tx;
        string id = "TX" + to_string(rand()% 90000 + 10000);
        strcpy(tx.id, id.c_str());
        strcpy(tx.accNum, acc.c_str());
        strcpy(tx.type, type.c_str());
        tx.amount = amt;

        time_t t = time(0);
        string dt = ctime(&t);
        if (!dt.empty() && dt[dt.length() - 1] == '\n') dt.erase(dt.length() - 1);
        strcpy(tx.timeStr, dt.c_str());

        transactions.push_back(tx);
        saveTransactions();
    }

    Account* findAccount(string accNum) {
        for (Account* a : accounts) {
            if (a->accNum == accNum) return a;
        }
        return nullptr;
    }

    // Teller actions
    void tellerLogin() {
        string id, pass;
        cout << "\nEnter Teller ID: ";
        cin >> id;
        cout << "Enter Password: ";
        cin >> pass;

        string hashed = hashSecret(pass);
        bool found = false;
        for (auto& t : tellers) {
            if (t.id == id && t.passHash == hashed) {
                currentTeller = t;
                found = true;
                break;
            }
        }

        if (!found){
            cout << "Invalid login credentials!\n";
            return;
        }

        cout << "\nWelcome" << currentTeller.name << " (Branch: " << currentTeller.branch << ")\n";
        tellerMenu();
    }

    void openCustomerAccount() {
        string name, id, phone, email, address, dob;
        int typeChoice;
        double depositAmt;

        cin.ignore();
        cout << "\n--- New Customer Registration ---\n";
        cout << "Enter Full Name: ";
        getline(cin, name);

        do {
            cout << "Enter SA ID (13 digits): ";
            cin >> id;
        }while (!checkSAID(id));

        do {
            cout << "Enter Phone Number (10 digits): ";
            cin >> phone;
        } while (!checkPhone(phone));

        do {
            cout << "Enter Email Address: ";
            cin >> email;
        } while (!checkEmail(email));

        cin.ignore();
        cout << "Enter Address: ";
        getline(cin, address);
        cout << "Enter DOB (DD/MM/YYYY): ";
        cin >> dob;

        cout << "\nChoose Account Type:\n";
        cout << "1. Savings\n2. Cheque\n3. Fixed Deposit\n4. Student\nChoice ";
        cin >> typeChoice;

        string type = "Savings";
        double minDep = 100.0;
        if (typeChoice == 2) { type = "Cheque"; minDep = 500.0; }
        else if (typeChoice == 3) { type = "Fixed Deposit"; minDep = 1000.0; }
        else if (typeChoice == 4) { type = "Student"; minDep = 20.0; }

        do {
            cout << "Initial Deposiy (Minimum R " << minDep << "): R";
            cin >> depositAmt;
        } while (depositAmt < minDep);

        // Auto generate random 5 digit pin & account number
        int randomPin = rand() % 90000 + 10000;
        string pinStr = to_string(randomPin);
        string newAcc = "ACC-" + string(currentTeller.branch) + "-" + to_string(rand() % 90000 + 10000);

        Account* a = createAccount(type);
        a->accNum = newAcc;
        a->name = name;
        a->idNum = id;
        a->phone = phone;
        a->email = email;
        a->address = address;
        a->dob = dob;
        a->accType = type;
        a->branch = currentTeller.branch;
        a->pinHash = hashSecret(pinStr);
        a->balance = depositAmt;
        accounts.push_back(a);

        recordTransaction(newAcc, "Initial Deposit", depositAmt);
        saveAccounts();

        cout << "\n========================================\n";
        cout << "ACCOUNTT CREATED SUCCESSFULLY!\n";
        cout << "Account Number:" << newAcc << "\n";
        cout << "Generated PIN : " << pinStr << " (Show to customer once)\n";
        cout << "========================================";
    }

    void searchCustomer() {
        cout << "\n1. Search by Account Number\n2. Search by SA ID\nChoice: ";
        int choice;
        cin >> choice;
        string term;
        cout << "Enter search value: ";
        cin >> term;

        bool found = false;
        for (Account* a : accounts) {
            if ((choice == 1 && a->accNum == term) || (choice == 2 && a->idNum == term)) {
                cout << "\n--- Customer Found ---\n";
                cout << "Account: " << a->accNum << " | Name: " << a->name
                << " | Type: " << a->accType << " | Balance: R" << a->balance
                << " | Branch: " << a->branch
                << " | Status: " << (a->isLocked ? "LOCKED" : "ACTIVE") << "\n";
                found = true;
                break;
            }
        }
        if (!found) cout << "No account matches that information.\n";
    }

    void showBranchReport() {cout << "\n--- Branch Comparison Report ---\n";
        for (auto& b : branches) {
            double total = 0;
            int count = 0;
            for (Account* a : accounts) {
                if (a->branch == b.code) {
                    total += a->balance;
                    count++;
                }
            }
            cout << b.code << " (" << b.name << " - " << b.city << ")\n";
            cout << "Accounts: " << count << " | Total Deposits: R" << fixed << setprecision(2) << total << "\n";
        }
    }

    void runMonthlyInterest() {
        int count = 0;
        for (Account* a : accounts) {
            double interest = a->balance * a->getInterestRate();
            if (interest > 0) {
                a->balance += interest;
                recordTransaction(a->accNum, "Monthly Interest", interest);
                count++;
            }
        }
        saveAccounts();
        cout << "\nApplied interest to " << count << " accounts successfully.\n";
    }

    void exportCSV() {
        ofstream csv("customers_export.csv");
        csv << "AccountNumber,Fullname, IDNumber,Type,Branch,Balance,Locked\n";
        for (Account* a : accounts) {
            csv << a->accNum << ","
            << a->name << ","
            << a->idNum << ","
            << a->accType << ","
            << a->branch << ","
            << fixed << setprecision(2) << a->balance << ","
            << (a->isLocked ? "YES" : "NO") << "\n";
        }
        csv.close();
        cout << "\nSaved customer database to 'customers_export.csv'.\n";
    }

    void tellerMenu() {
        int choice;
        do {
            cout << "\n--- Teller Menu (" << currentTeller.branch <<") ---\n";
            cout << "1. Open New Customer Account\n";
            cout << "2. Search for Customer\n";
            cout << "3. Branch Comparison Report\n";
            cout << "4. Run Monthly Interest Calculation\n";
            cout << "5. Export Customer Data to CSV\n";
            cout << "6. Logout\n";
            cout << "Enter option: ";
            cin >> choice;

            if (choice == 1) openCustomerAccount();
            else if (choice == 2) searchCustomer();
            else if (choice == 3) showBranchReport();
            else if (choice == 4) runMonthlyInterest();
            else if (choice == 5) exportCSV();
        } while (choice !=6);
    }

    // Customer actions
    void customerLogin() {
        string acc, pin;
        cout << "\nEnter Account Number: ";
        cin >> acc;
        
        Account* a = findAccount(acc);
        if (!a) {
            cout << "Account not found.\n";
            return;
        }

        if (a->isLocked) {
            cout << "Account is locked after 3 failed tries. Please see a teller.\n";
            return;
        }

        cout << "Enter 5-digit PIN: ";
        cin >> pin;

        if (a->pinHash == hashSecret(pin)) {
            a->failedAttempts = 0;
            currentCustomer = a;
            cout << "\nLogin successful! Welcome " << currentCustomer->name << "\n";
            customerMenu();
        } else {
            a->failedAttempts++;
            cout << "Wrong PIN! Attempts left: " << (3 - a->failedAttempts) << "\n";
            if (a->failedAttempts >= 3) {
                a->isLocked = true;
                cout << "Account is now locked for security.\n";
            }
            saveAccounts();
        }
    }
        void customerMenu() {
            int choice;
            do {
                cout << "\n--- Customer Menu ---\n";
                cout << "1. View Balance\n";
                cout << "2. Deposit Money\n";
                cout << "3. Withdraw Money\n";
                cout << "4. Transfer to Another Account\n";
                cout << "5. Mini Statement\n";
                cout << "6. Change PIN\n";
                cout << "7. Logout\n";
                cin >> choice;

                if (choice == 1) {
                    cout << "Your Balance: R" << setprecision(2) << currentCustomer->balance << "\n";
                } else if (choice == 2) {
                    double amt;
                    cout << "Amount to deposit: R";
                    cin >> amt;
                    currentCustomer->deposit(amt);
                    recordTransaction(currentCustomer->accNum, "Deposit", amt);
                    saveAccounts();
                } else if (choice == 3) {
                    double amt;
                    cout << "Amount to withdraw: R";
                    cin >> amt;
                    if (currentCustomer->withdraw(amt)) {
                        recordTransaction(currentCustomer->accNum, "Withdrawal", amt);
                        saveAccounts();
                    }
                }else if (choice == 4) {
                    string targetAcc;
                    double amt;
                    cout << "Enter recipient Account Number: ";
                    cin >> targetAcc;
                    Account* target = findAccount(targetAcc);
                    if (!target) {
                        cout << "Target account does not exist.\n";
                        continue;
                    }
                    cout << "Amount to transfer: R";
                    cin >> amt;
                    if (currentCustomer->withdraw(amt)) {
                        target->deposit(amt);
                        recordTransaction(currentCustomer->accNum, "Transfer Out", amt);
                        recordTransaction(target->accNum, "Transfer In", amt);
                        saveAccounts();
                        cout << "Transfer complete!\n";
                    }
                } else if (choice == 5) {
                    cout << "\n--- Statement for " << currentCustomer->accNum << " ---\n";
                    for (auto& tx : transactions) {
                        if (string(tx.accNum) == currentCustomer->accNum) {
                            cout << tx.id << " | " << tx.timeStr << " | "
                            << setw(15) << left << tx.type
                            << " | R" << fixed << setprecision(2) << tx.amount << "\n";
                        }
                    }
                } else if (choice == 6) {
                    string oldP, newP;
                    cout << "Enter current PIN: ";
                    cin >> oldP;
                    if (currentCustomer->pinHash == hashSecret(oldP)) {
                        cout << "Enter new 5-digit PIN ";
                        cin >> newP;
                        if (newP.length() == 5) {
                            currentCustomer->pinHash = hashSecret(newP);
                            saveAccounts();
                            cout << "PIN updated successfully!\n";
                        } else {
                            cout << "PIN must be 5 digits.\n";
                        }
                    } else {
                        cout << "Current PIN incorrect.\n";
                    }
                }
            } while (choice != 7);
            currentCustomer = nullptr;
        }

        void start() {
            int choice;
            do {
                cout << "\n========================================\n";
                cout << " STANDARD BANK CORE SYSTEM PROTOTYPE\n";
                cout << "========================================\n";
                cout << "1. Bank Teller Login\n";
                cout << "2. Customer Login\n";
                cout << "3. Exit System\n";
                cout << "Select option: ";
                if (!(cin >> choice)) {
                    cin.clear();
                    cin.ignore(1000, '\n');
                    continue;
                }

                if (choice == 1) tellerLogin();
                else if (choice == 2) customerLogin();
                else if (choice == 3) cout << "Shutting down system. Goodbye!\n";
            } while (choice !=3);
        }
    };

    int main() {
        BankSystem app;
        app.start();
        return 0;
    }