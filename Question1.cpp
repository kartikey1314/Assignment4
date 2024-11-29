#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm>

class Author {
public:
    std::string name;
    std::string affiliation;

    Author(const std::string& name, const std::string& affiliation) 
        : name(name), affiliation(affiliation) {}
};

class Publication {
private:
    std::string title;
    std::string venue;
    std::vector<Author> authors;
    std::string doi; // Optional
    int year;

    // Helper function to check for duplicate authors
    bool isDuplicateAuthor(const Author& newAuthor) const {
        for (const auto& author : authors) {
            if (author.name == newAuthor.name) {
                return true;
            }
        }
        return false;
    }

public:
    Publication(const std::string& title, const std::string& venue, 
                const std::vector<Author>& authors, const std::string& doi, int year) 
        : title(title), venue(venue), authors(authors), doi(doi), year(year) {
        assert(!authors.empty() && "At least one author is required.");
        bool hasInstituteAuthor = false;
        for (const auto& author : authors) {
            if (author.affiliation == "IIIT-Delhi") {
                hasInstituteAuthor = true;
                break;
            }
        }
        assert(hasInstituteAuthor && "At least one author must have the institute's affiliation.");
    }

    void addCoAuthor(const Author& newAuthor) {
        assert(!isDuplicateAuthor(newAuthor) && "Author is already listed.");
        authors.push_back(newAuthor);
    }

    void display() const {
        std::cout << "Title: " << title << std::endl;
        std::cout << "Venue: " << venue << std::endl;
        std::cout << "Authors: " << std::endl;
        for (const auto& author : authors) {
            std::cout << " - " << author.name << " (" << author.affiliation << ")" << std::endl;
        }
        if (!doi.empty()) {
            std::cout << "DOI: " << doi << std::endl;
        }
        std::cout << "Year: " << year << std::endl;
    }
};

int main() {
    // Sample authors
    Author author1("Alice Smith", "IIIT-Delhi");
    Author author2("Bob Johnson", "Some Other Institute");
    
    // Sample publication
    std::vector<Author> authors = { author1 };
    Publication pub("Research on AI", "International Journal of AI Research", authors, "10.1234/ai.2024", 2024);
    
    // Add co-author
    pub.addCoAuthor(author2);

    // Display publication details
    pub.display();

    return 0;
}