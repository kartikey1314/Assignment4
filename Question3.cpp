#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <cassert>
#include <cctype>
#include <algorithm>

class Publication {
public:
    std::string title;
    std::string venue;
    std::vector<std::string> authors;
    int year;
    std::string doi;

    Publication(const std::string &t, const std::string &v, const std::vector<std::string> &a, int y, const std::string &d = "")
        : title(t), venue(v), authors(a), year(y), doi(d) {}
};

class BibFileParser {
private:
    std::vector<Publication> publications;
    std::map<std::string, std::vector<Publication>> authorPublicationMap;

    // Helper function to check if a string is numeric
    bool isNumeric(const std::string &str) {
        for (char c : str) {
            if (!std::isdigit(static_cast<unsigned char>(c))) {
                return false;
            }
        }
        return true;
    }

    // Normalize author name to "Firstname Lastname" format
    std::string normalizeAuthorName(const std::string &author) const {
        size_t commaPos = author.find(',');
        if (commaPos != std::string::npos) {
            // Convert "Lastname, Firstname" to "Firstname Lastname"
            return author.substr(commaPos + 2) + " " + author.substr(0, commaPos); // Skip space after comma
        }
        return author;
    }

    void validateEntry(const std::string &entry) {
        assert(entry.find("author") != std::string::npos && "Invalid bib entry: Missing author field");
        assert(entry.find("title") != std::string::npos && "Invalid bib entry: Missing title field");
        assert(entry.find("year") != std::string::npos && "Invalid bib entry: Missing year field");

        std::set<std::string> authorsSet;
        size_t authorStart = entry.find("author");
        size_t authorEnd = entry.find("\n", authorStart);
        std::string authorsField = entry.substr(authorStart, authorEnd - authorStart);

        // Split authors by "and" and normalize each author's name
        std::istringstream authorStream(authorsField);
        std::string author;
        while (std::getline(authorStream, author, ',')) {
            author = author.substr(author.find("{") + 1, author.find("}") - author.find("{") - 1);
            // Remove any leading or trailing whitespace
            author.erase(0, author.find_first_not_of(" \t"));
            author.erase(author.find_last_not_of(" \t") + 1);

            if (author.find("and") != std::string::npos) {
                // Split authors by "and" and normalize each part
                std::istringstream authorSplitter(author);
                std::string singleAuthor;
                while (std::getline(authorSplitter, singleAuthor, ' ')) {
                    if (!singleAuthor.empty()) {
                        authorsSet.insert(normalizeAuthorName(singleAuthor));
                    }
                }
            } else {
                authorsSet.insert(normalizeAuthorName(author));
            }

            // Debugging: Print authors to ensure they are stored correctly
            std::cout << "Normalized author: " << author << std::endl;
        }
    }

    Publication parseEntry(const std::string &entry) {
        std::string title, venue, doi;
        std::vector<std::string> authors;
        int year;

        // Extract title
        size_t titleStart = entry.find("title") + 6;
        size_t titleEnd = entry.find("}", titleStart);
        title = entry.substr(titleStart, titleEnd - titleStart);

        // Extract venue (journal or conference)
        size_t venueStart = entry.find("venue") + 6;
        size_t venueEnd = entry.find("}", venueStart);
        venue = entry.substr(venueStart, venueEnd - venueStart);

        // Extract authors
        size_t authorStart = entry.find("author") + 6;
        size_t authorEnd = entry.find("\n", authorStart);
        std::string authorsField = entry.substr(authorStart, authorEnd - authorStart);

        std::istringstream authorStream(authorsField);
        std::string author;
        while (std::getline(authorStream, author, ',')) {
            author = author.substr(author.find("{") + 1, author.find("}") - author.find("{") - 1);
            authors.push_back(normalizeAuthorName(author)); // Normalize author name format
        }

        // Extract year
        size_t yearStart = entry.find("year") + 4;
        size_t yearEnd = entry.find("}", yearStart);
        std::string yearStr = entry.substr(yearStart, yearEnd - yearStart);

        // Trim any leading and trailing spaces
        yearStr.erase(0, yearStr.find_first_not_of(" \t={"));
        yearStr.erase(yearStr.find_last_not_of(" \t}") + 1);

        // Validate year string
        if (yearStr.empty() || !isNumeric(yearStr)) {
            throw std::invalid_argument("Invalid or missing year in entry: " + entry);
        }

        year = std::stoi(yearStr);

        // Extract DOI (optional)
        size_t doiStart = entry.find("doi");
        if (doiStart != std::string::npos) {
            size_t doiEnd = entry.find("}", doiStart);
            doi = entry.substr(doiStart + 5, doiEnd - doiStart - 5);  // 5 to skip "doi="
        }

        return Publication(title, venue, authors, year, doi);
    }

public:
    void parse(const std::string &filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open bib file");
        }

        std::string line, entry;
        while (std::getline(file, line)) {
            if (line.find("@") == 0) {
                if (!entry.empty()) {
                    validateEntry(entry);
                    Publication pub = parseEntry(entry);
                    publications.push_back(pub);

                    for (const auto &author : pub.authors) {
                        authorPublicationMap[author].push_back(pub);
                    }

                    entry.clear();
                }
            }
            entry += line + "\n";
        }
        if (!entry.empty()) {
            validateEntry(entry);
            Publication pub = parseEntry(entry);
            publications.push_back(pub);

            for (const auto &author : pub.authors) {
                authorPublicationMap[author].push_back(pub);
            }
        }
        file.close();
    }

    void searchByAuthor(const std::string &authorName) const {
        // Normalize the search query to ensure it matches the stored format
        std::string normalizedQuery = normalizeAuthorName(authorName);

        auto it = authorPublicationMap.find(normalizedQuery);
        if (it == authorPublicationMap.end()) {
            std::cout << "No publications found for author: " << authorName << std::endl;
            return;
        }

        const auto &publications = it->second;
        std::cout << "Publications by " << authorName << ":\n";
        for (const auto &pub : publications) {
            std::cout << "- " << pub.title << " (" << pub.year << ") in " << pub.venue;
            if (!pub.doi.empty()) {
                std::cout << " | DOI: " << pub.doi;
            }
            std::cout << "\n";
        }

        double avgCoAuthors = 0;
        for (const auto &pub : publications) {
            avgCoAuthors += pub.authors.size() - 1; // Exclude the author themselves
        }
        avgCoAuthors /= publications.size();
        std::cout << "Average co-authors per paper: " << avgCoAuthors << "\n";
    }
};

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <bib file path> <author name> [additional author names...]\n";
        return 1;
    }

    std::string bibFilePath = argv[1];

    BibFileParser parser;
    parser.parse(bibFilePath);

    for (int i = 2; i < argc; ++i) {
        parser.searchByAuthor(argv[i]);
    }

    return 0;
}
