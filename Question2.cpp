#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include<stack>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <stdexcept>
#include <cassert>
#include <unordered_map>

using namespace std;

// Struct to hold publication details
struct Publication {
    string title;
    string venue;
    vector<string> authors;
    int year;
    string doi;

    void display() const {
        cout << "Title: " << title << endl;
        cout << "Venue: " << venue << endl;
        cout << "Year: " << year << endl;
        if (!doi.empty()) {
            cout << "DOI: " << doi << endl;
        }
        cout << "Authors: ";
        for (const auto &author : authors) {
            cout << author << "; ";
        }
        cout << endl;
    }
};

// Function to split a string by a delimiter
vector<string> split(const string &str, const string &delimiter) {
    vector<string> tokens;
    size_t start = 0, end;
    while ((end = str.find(delimiter, start)) != string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
    }
    tokens.push_back(str.substr(start));
    return tokens;
}

// Function to parse author names and format them correctly
vector<string> parseAuthors_1(const string &authorField) {
    vector<string> authorsRaw = split(authorField, " and ");
    vector<string> formattedAuthors;

    for (const auto &rawAuthor : authorsRaw) {
        size_t commaPos = rawAuthor.find(',');
        if (commaPos != string::npos) {
            string lastName = rawAuthor.substr(0, commaPos);
            string firstName = rawAuthor.substr(commaPos + 1);
            formattedAuthors.push_back(firstName + " " + lastName);
        } else {
            throw runtime_error("Invalid author format: " + rawAuthor);
        }
    }
    return formattedAuthors;
}

// Function to check if braces in a file are balanced
bool areBracesBalancedInFile(const string &filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Error: Could not open file: " << filePath << endl;
        return false;
    }

    stack<char> braceStack;
    char ch;
    while (file.get(ch)) { // Read file character by character
        if (ch == '{') {
            braceStack.push(ch);
        } else if (ch == '}') {
            if (!braceStack.empty() && braceStack.top() == '{') {
                braceStack.pop();
            } else {
                return false; // Unmatched closing brace
            }
        }
    }

    file.close();
    return braceStack.empty(); // True if all braces are matched
}


// Function to check if lines in the bib file follow proper comma rules
bool checkLineEndingsInBibFile(string filePath){
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Error: Could not open file: " << filePath << endl;
        return false;
    }
    string line;
    int lineNumber = 0;
    bool allLinesValid = true;
    while (getline(file, line)) {
        lineNumber++;
        // Remove leading and trailing whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        if (line.empty()) {
            continue; // Skip empty lines
        }
        // Check if the line ends with a comma unless it ends with "}}"
        if (line.length() >= 2 && line.substr(line.length() - 2) == "}}") {
            continue; // Line ending with "}}" is valid without a comma
        } else if (line.back() != ',') {
            cerr << "Error: Line " << lineNumber << " does not end with a comma or }}." << endl;
            allLinesValid = false;
        }
    }
    file.close();
    return allLinesValid;
}

// Function to load faculty data and return a set of IIIT-Delhi faculty names
set<string> loadFacultyData_1(const string &csvFilePath) {
    ifstream csvFile(csvFilePath);
    if (!csvFile.is_open()) {
        throw runtime_error("Could not open CSV file: " + csvFilePath);
    }

    set<string> iiitDelhiFaculty;
    string line;
    while (getline(csvFile, line)) {
        istringstream ss(line);
        string name, affiliation;
        getline(ss, name, ',');
        getline(ss, affiliation, ',');
        if (affiliation == "IIIT-Delhi") {
            iiitDelhiFaculty.insert(name);
        }
    }
    csvFile.close();
    return iiitDelhiFaculty;
}

// Function to parse a bib file and validate its content
vector<Publication> parseBibFile_1(const string &bibFilePath, const set<string> &iiitDelhiFaculty) {
    ifstream bibFile(bibFilePath);
    if (!bibFile.is_open()) {
        throw runtime_error("Could not open bib file: " + bibFilePath);
    }

    vector<Publication> publications;
    string line;
    int lineNumber = 0;
    while (getline(bibFile, line)) {
        lineNumber++;
        // Remove leading/trailing spaces
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (!areBracesBalancedInFile(bibFilePath)) {
            // cerr << "Error: Unbalanced braces found in the file" << endl;
            assert(false && "Unbalanced braces in bib file.");
        }


        if (!checkLineEndingsInBibFile(bibFilePath)) {
            // cerr << "Error: Unbalanced braces found in the file" << endl;
            assert(false && "Unbalanced braces in bib file.");
        }

    }

    bibFile.close();
    return publications;
}


#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// Helper function to trim whitespace from a string
string trim(const string &str) {
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    return (start == string::npos) ? "" : str.substr(start, end - start + 1);
}

// Helper function to parse author names and convert them to the desired format
vector<string> parseAuthors_2(const string &authorField) {
    vector<string> authors;
    stringstream ss(authorField);
    string author;

    while (getline(ss, author, 'a')) { // Parse based on "and"
        if (!author.empty()) {
            size_t commaPos = author.find(',');
            if (commaPos != string::npos) {
                string lastName = trim(author.substr(0, commaPos));
                string firstName = trim(author.substr(commaPos + 1));
                authors.push_back(firstName + " " + lastName);
            }
        }
    }
    return authors;
}

// Function to load CSV file into a map of author -> affiliation
unordered_map<string, string> loadFacultyData(const string &csvFilePath) {
    unordered_map<string, string> facultyAffiliations;
    ifstream csvFile(csvFilePath);

    if (!csvFile.is_open()) {
        cerr << "Error: Could not open CSV file: " << csvFilePath << endl;
        return facultyAffiliations;
    }

    string line, author, affiliation;
    getline(csvFile, line); // Skip header

    while (getline(csvFile, line)) {
        stringstream ss(line);
        getline(ss, author, ',');
        getline(ss, affiliation, ',');
        facultyAffiliations[trim(author)] = trim(affiliation);
    }

    csvFile.close();
    return facultyAffiliations;
}

// Function to parse the bib file and store data in the unordered map
bool parseBibFile_2(const string &bibFilePath, const string &csvFilePath) {
    unordered_map<string, vector<string>> publicationData;
    unordered_map<string, string> facultyAffiliations = loadFacultyData(csvFilePath);

    ifstream bibFile(bibFilePath);
    if (!bibFile.is_open()) {
        cerr << "Error: Could not open bib file: " << bibFilePath << endl;
        return false;
    }

    string line, key, value, title;
    bool inPublication = false;
    unordered_map<string, string> currentPublication;

    while (getline(bibFile, line)) {
        line = trim(line);

        if (line.empty()) continue;

        if (line[0] == '@') { // New publication entry
            inPublication = true;
            currentPublication.clear();
            continue;
        }

        if (inPublication) {
            size_t equalsPos = line.find('=');
            if (equalsPos != string::npos) {
                key = trim(line.substr(0, equalsPos));
                value = trim(line.substr(equalsPos + 1));
                value.erase(remove(value.begin(), value.end(), '{'), value.end());
                value.erase(remove(value.begin(), value.end(), '}'), value.end());
                value.erase(remove(value.begin(), value.end(), ','), value.end());

                currentPublication[key] = value;

                if (key == "title") title = value;
            }
        }

        if (line == "}") { // End of publication entry
            inPublication = false;

            // Parse authors and check affiliation
            string authorField = currentPublication["author"];
            vector<string> authors = parseAuthors_2(authorField);
            bool hasIIITDelhiAuthor = false;

            for (const string &author : authors) {
                if (facultyAffiliations[author] == "IIIT-Delhi") {
                    hasIIITDelhiAuthor = true;
                }
            }

            if (!hasIIITDelhiAuthor) {
                cerr << "Error: Publication \"" << title << "\" has no author affiliated with IIIT-Delhi." << endl;
                return false;
            }

            // Store publication details in the map
            vector<string> fields = {
                currentPublication["venue"],
                currentPublication["author"],
                currentPublication["year"],
                currentPublication["doi"]
            };
            publicationData[title] = fields;
        }
    }

    bibFile.close();
    return true;
}




int main() {
    try {
        // File paths
        string bibFilePath = "C:/Users/kartikey singh/OneDrive/Desktop/Assignment_4_OOPD/Assignment4/publist.bib";
        string csvFilePath = "C:/Users/kartikey singh/OneDrive/Desktop/Assignment_4_OOPD/Assignment4/faculty.csv";

        // Load IIIT-Delhi faculty data
        set<string> iiitDelhiFaculty = loadFacultyData_1(csvFilePath);

        // Parse bib file and validate
        vector<Publication> publications = parseBibFile_1(bibFilePath, iiitDelhiFaculty);

        if (parseBibFile_2(bibFilePath, csvFilePath)) {
            cout << "All publications have at least one author affiliated with IIIT-Delhi." << endl;
        } else {
            cout << "Some publications lack authors affiliated with IIIT-Delhi." << endl;
        }

        // Display parsed publications
        for (const auto &pub : publications) {
            pub.display();
            cout << "-----------------------------" << endl;
        }

    } catch (const exception &e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}