#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

using namespace std;

const string DATA_FILE = "policies.txt";

struct Policy {
    int policyNo;
    string holderName;
    int age;
    string planName; // e.g. "Jeevan suraksha"
    int termYears;
    double sumAssured;
    double yearlyPremium;
};

void pauseScreen() {
    cout << "\nPress Enter to continue...";
    cin.ignore();
}

Policy inputPolicy() {
    Policy p{};
    cout << "\nEnter Policy Details" << endl;
    cout << "--------------------" << endl;

    cout << "Policy Number (integer): ";
    cin >> p.policyNo;
    cin.ignore();

    cout << "Policy Holder Name: ";
    getline(cin, p.holderName);

    cout << "Age: ";
    cin >> p.age;
    cin.ignore();

    cout << "Plan Name: ";
    getline(cin, p.planName);

    cout << "Term (in years): ";
    cin >> p.termYears;

    cout << "Sum Assured: ";
    cin >> p.sumAssured;

    cout << "Yearly Premium: ";
    cin >> p.yearlyPremium;

    return p;
}

// convert Policy to a single line of text (for file)
string policyToLine(const Policy &p) {
    string line = to_string(p.policyNo) + "|" +
                  p.holderName + "|" +
                  to_string(p.age) + "|" +
                  p.planName + "|" +
                  to_string(p.termYears) + "|" +
                  to_string(p.sumAssured) + "|" +
                  to_string(p.yearlyPremium);
    return line;
}

// convert line of text back to Policy
bool lineToPolicy(const string &line, Policy &p) {
    if (line.empty()) return false;
    size_t start = 0, pos;
    string tokens[7];
    int idx = 0;

    while ((pos = line.find('|', start)) != string::npos && idx < 6) {
        tokens[idx++] = line.substr(start, pos - start);
        start = pos + 1;
    }
    if (idx <= 6 && start <= line.size()) {
        tokens[idx++] = line.substr(start);
    }
    if (idx != 7) return false;

    try {
        p.policyNo = stoi(tokens[0]);
        p.holderName = tokens[1];
        p.age = stoi(tokens[2]);
        p.planName = tokens[3];
        p.termYears = stoi(tokens[4]);
        p.sumAssured = stod(tokens[5]);
        p.yearlyPremium = stod(tokens[6]);
    } catch (...) {
        return false;
    }
    return true;
}

// 1. Add Policy
void addPolicy() {
    ofstream fout(DATA_FILE, ios::app);
    if (!fout) {
        cout << "Error opening file for writing!" << endl;
        return;
    }

    Policy p = inputPolicy();
    string line = policyToLine(p);
    fout << line << '\n';
    fout.close();

    cout << "\nPolicy saved successfully!" << endl;
}

// print one policy in table format
void printPolicy(const Policy &p) {
    cout << left << setw(10) << p.policyNo
         << setw(20) << p.holderName.substr(0, 18)
         << setw(5)  << p.age
         << setw(15) << p.planName.substr(0, 13)
         << setw(6)  << p.termYears
         << setw(12) << fixed << setprecision(2) << p.sumAssured
         << setw(12) << fixed << setprecision(2) << p.yearlyPremium
         << endl;
}

// 2. View All Policies
void viewAllPolicies() {
    ifstream fin(DATA_FILE);
    if (!fin) {
        cout << "No data found. Add some policies first." << endl;
        return;
    }

    cout << "\nAll Policies" << endl;
    cout << "------------" << endl;
    cout << left << setw(10) << "PolicyNo"
         << setw(20) << "Holder Name"
         << setw(5)  << "Age"
         << setw(15) << "Plan"
         << setw(6)  << "Term"
         << setw(12) << "SumAssured"
         << setw(12) << "Premium" << endl;
    cout << string(90, '-') << endl;

    string line;
    Policy p;
    bool any = false;
    while (getline(fin, line)) {
        if (lineToPolicy(line, p)) {
            printPolicy(p);
            any = true;
        }
    }
    if (!any) {
        cout << "No records to display." << endl;
    }
}

// helper: search by number and return policy
bool searchPolicyByNumber(int searchNo, Policy &foundPolicy) {
    ifstream fin(DATA_FILE);
    if (!fin) return false;

    string line;
    Policy p;
    while (getline(fin, line)) {
        if (lineToPolicy(line, p)) {
            if (p.policyNo == searchNo) {
                foundPolicy = p;
                return true;
            }
        }
    }
    return false;
}

// 3. Search Policy
void searchPolicy() {
    cout << "\nEnter policy number to search: ";
    int searchNo;
    cin >> searchNo;

    Policy p;
    if (searchPolicyByNumber(searchNo, p)) {
        cout << "\nPolicy found:\n";
        cout << left << setw(10) << "PolicyNo"
             << setw(20) << "Holder Name"
             << setw(5)  << "Age"
             << setw(15) << "Plan"
             << setw(6)  << "Term"
             << setw(12) << "SumAssured"
             << setw(12) << "Premium" << endl;
        cout << string(90, '-') << endl;
        printPolicy(p);
    } else {
        cout << "\nNo policy found with this number." << endl;
    }
}

// 4. Update Policy
void updatePolicy() {
    cout << "\nEnter policy number to update: ";
    int searchNo;
    cin >> searchNo;

    ifstream fin(DATA_FILE);
    if (!fin) {
        cout << "No data file found." << endl;
        return;
    }

    ofstream temp("temp.txt");
    if (!temp) {
        cout << "Error creating temp file." << endl;
        return;
    }

    string line;
    Policy p;
    bool updated = false;

    while (getline(fin, line)) {
        if (lineToPolicy(line, p)) {
            if (p.policyNo == searchNo) {
                cout << "\nExisting details:" << endl;
                printPolicy(p);
                cout << "\nEnter new details for this policy:" << endl;
                Policy newP = inputPolicy();
                temp << policyToLine(newP) << '\n';
                updated = true;
            } else {
                temp << line << '\n';
            }
        }
    }

    fin.close();
    temp.close();

    remove(DATA_FILE.c_str());
    rename("temp.txt", DATA_FILE.c_str());

    if (updated) {
        cout << "\nPolicy updated successfully!" << endl;
    } else {
        cout << "\nPolicy number not found." << endl;
    }
}

// 5. Delete Policy
void deletePolicy() {
    cout << "\nEnter policy number to delete: ";
    int searchNo;
    cin >> searchNo;

    ifstream fin(DATA_FILE);
    if (!fin) {
        cout << "No data file found." << endl;
        return;
    }

    ofstream temp("temp.txt");
    if (!temp) {
        cout << "Error creating temp file." << endl;
        return;
    }

    string line;
    Policy p;
    bool deleted = false;

    while (getline(fin, line)) {
        if (lineToPolicy(line, p)) {
            if (p.policyNo == searchNo) {
                deleted = true; // skip writing this record
            } else {
                temp << line << '\n';
            }
        }
    }

    fin.close();
    temp.close();

    remove(DATA_FILE.c_str());
    rename("temp.txt", DATA_FILE.c_str());

    if (deleted) {
        cout << "\nPolicy deleted successfully!" << endl;
    } else {
        cout << "\nPolicy number not found." << endl;
    }
}

// Main menu loop
void menuLoop() {
    int choice;
    do {
        cout << "\n==============================" << endl;
        cout << "      LIC Management System" << endl;
        cout << "==============================" << endl;
        cout << "1. Add New Policy" << endl;
        cout << "2. View All Policies" << endl;
        cout << "3. Search Policy" << endl;
        cout << "4. Update Policy" << endl;
        cout << "5. Delete Policy" << endl;
        cout << "6. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                addPolicy();
                break;
            case 2:
                viewAllPolicies();
                break;
            case 3:
                searchPolicy();
                break;
            case 4:
                updatePolicy();
                break;
            case 5:
                deletePolicy();
                break;
            case 6:
                cout << "Exiting..." << endl;
                break;
            default:
                cout << "Invalid choice. Try again." << endl;
        }

        if (choice != 6) {
            cin.ignore();
            pauseScreen();
        }
    } while (choice != 6);
}

int main() {
    menuLoop();
    return 0;
}