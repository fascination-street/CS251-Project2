#pragma once

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <fstream>
#include <algorithm>

using namespace std;

/**
 * Cleans a specific token by:
 * - Removing leading and trailing punctuation
 * - Converting to lowercase
 * If the token has no letters, returns the empty string.
 */
string cleanToken(string s) {

    //Detect if the string has letters
    //Convert every letter to lowercase counterpart
    bool noLetters = true;
    for (unsigned int i = 0; i < s.length(); i++) {
        if(isalpha(s[i])) {
            noLetters = false;
        }
        s[i] = tolower(s[i]);
    }
    
    if (noLetters) {
        return "";
    }
    //

    //Remove trailing punctuation
    while (ispunct(s.back())) {
        s.pop_back();
    }
    //

    //Remove leading punctuation
    while (ispunct(s.front())) {
        s.erase(0,1);
    }
    //

    return s;
}

/**
 * Returns a set of the cleaned tokens in the given text.
 */
set<string> gatherTokens(string text) {
    //establishing required variables
    set<string> tokens; //return set
    string word = ""; //string var for addition to set

    //add a space to make the loop function properly
    text.push_back(' ');
    //

    size_t spacePos = text.find(' '); 

    int initPos = 0;
    //

    //seperate the words and add them to the set
    while (spacePos != string::npos) {
        //setting the word var to be a substring of the characters in between spaces
        word = text.substr(initPos, spacePos - initPos); 
        //

        //remove leading and trailing non-alpha chars, then convert to lowercase
        word = cleanToken(word);
        //

        //add to set as long as it's not an empty string
        if (word != "") {
            tokens.insert(word);
        }
        //
        
        //move to the next whitespaces in the string
        initPos = spacePos + 1;
        spacePos = text.find(' ', spacePos + 1);
        //
    }
    //

    return tokens;
}

/**
 * Builds the inverted index from the given data file. The existing contents
 * of `index` are left in place.
 */
int buildIndex(string filename, map<string, set<string>>& index) {
    //open file
    fstream file;
    int numFilesProcessed = 0;

    file.open(filename);
    
    if (!file.is_open()) {
        cout << "Invalid filename." << endl;
        return numFilesProcessed;
    }
    //

    //read through the file, take in two lines at a time. for each word, we add it as a key to map
    //then we populate the map's set with the sites that have the word in them.
    while (!file.eof()) {
        string text,site;

        //input site and text string from file
        getline(file, site);
        getline(file, text);
        
        //end the loop if there's empty strings (offsets return int)
        if (site == "" || text == "") {
            break;
        }
        
        //break the text string into tokens that we can fill our inverted index with
        set<string> tokenSet = gatherTokens(text);

        //populate the inverted index
        for (string word : tokenSet) {
            //if the word is not part of the map, then create it
            if (index.count(word) == 0) {
                set<string> newSiteSet = {site};
                index.emplace(word, newSiteSet);
            }
            //otherwise insert the site in the word's set
            else {
                index.at(word).insert(site);
            } 
        }
        
        numFilesProcessed++;
    }

    file.close();

    return numFilesProcessed;
}
//

/**
 * Runs a search query on the provided index, and returns a set of results.
 *
 * Search terms are processed using set operations (union, intersection, difference).
 */
set<string> findQueryMatches(map<string, set<string>>& index, string sentence) {

    size_t queryIndex, whiteSpaceIndex;
    sentence.push_back(' ');

    whiteSpaceIndex = sentence.find(' ');
    queryIndex = 0;

    set<string> result = {};

    while (queryIndex < sentence.length()) {
        //get query (our search term with prefix) and cleanQuery (search term without prefix)
        string query = sentence.substr(queryIndex, whiteSpaceIndex - queryIndex);
        string cleanQuery = cleanToken(query);

        if (query == "") {
            break;
        }
        //

        
        //populate the sites set
        //if cleanQuery is not in index, sites is the empty set
        set<string> sites;
        if (index.find(cleanQuery) != index.end()) {
           sites = index.find(cleanQuery)->second;
        } 
        else {
            sites = {};
        }
        //

        //determine which set operation to use and then perform it between the result and sites sets
        set<string> tempResult = {};

        char prefix = query.at(0);
        if (prefix == '+') {
            set_intersection(result.begin(), result.end(),
                             sites.begin(), sites.end(),
                             inserter(tempResult, tempResult.begin()));
        }
        else if (prefix == '-') {
            set_difference(result.begin(), result.end(),
                           sites.begin(), sites.end(),
                           inserter(tempResult, tempResult.begin()));
        }
        else {
            set_union(sites.begin(), sites.end(),
                      result.begin(), result.end(),
                      inserter(tempResult, tempResult.begin()));
        }
        result.swap(tempResult);
        //

        //moves along through sentence
        queryIndex = whiteSpaceIndex + 1;
        whiteSpaceIndex = sentence.find(' ', queryIndex);
        //
    }

    return result;
}

/**
 * Runs the main command loop for the search program
 */
void searchEngine(string filename) {
    map<string, set<string>> index;

    int numPages = buildIndex(filename, index);

    cout << "Stand by while building index..." << endl;

    cout << "Indexed " << numPages << " pages containing " << index.size() << " unique terms." << endl;

    string search;

    while (true) {
        cout << "Enter query sentence (press enter to quit): ";
        getline(cin, search);
        cout << "\n";

        if (search == "") {
            cout << "Thank you for searching!" << endl;
            break;
        }

        set<string> sites = findQueryMatches(index, search);

        cout << "Found " << sites.size() << " matching pages" << endl;
        
        for (string site : sites) {
            cout << site << endl;
        }
    }
}
