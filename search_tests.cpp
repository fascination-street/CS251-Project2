#include <gtest/gtest.h>

#include "search.h"
#include "tests/build_index.h"
#include "tests/clean_token.h"
#include "tests/find_query_matches.h"
#include "tests/gather_tokens.h"

// TODO_STUDENT: write your tests here!

TEST(CleanToken, ExampleEmptyTest) {
    cout << "Hi! This shows up in the terminal." << endl;
}

TEST(CleanToken, PunctuationAtBeginningAndEnd) {
    map<string, set<string>> tests;

    tests["hello"].insert(".hello!");
    tests["hello"].insert("..hello??");
    tests["we"].insert("...we...?");
    tests["winter"].insert(".winter.!");
    tests["collect"].insert(".!.!collect.?");
    tests["great"].insert(".gREAt????");

    testCleanToken(tests);
}

TEST(CleanToken, PunctuationInMiddle) {
    map<string, set<string>> tests;

    tests["hey!bro"].insert("hey!bro");
    tests["what??isup"].insert("what??isup");

    tests["what??really"].insert("What??Really??");
    tests["yes!yes"].insert("!Yes!Yes!");
    tests["s.back"].insert("s.back()");
    tests["isalpha(a"].insert("!isalpha(a)");
    tests["wh?at"].insert("??wh?at");
    tests["ye!p"].insert("...ye!p");

    testCleanToken(tests);
}

TEST(CleanToken, NumbersAtEnd) {
    map<string, set<string>> tests;

    tests["thelegend27"].insert("thelegend27");
    tests["hello1"].insert("hello1");
    tests["le1337"].insert("Le1337");
    tests["crazybignumbers!!!!1001232"].insert("CRAZYBIGNUMBERS!!!!1001232");
    tests["hugewin3"].insert("!HugeWin3");

    testCleanToken(tests);
}

TEST(GatherTokens, SpacesBetweenWords) {
    string text = "  Hey   there   buddy      of             mine!";

    set<string> expected = {"hey","there","buddy","of","mine"};
    set<string> studentResult = gatherTokens(text);

    string testFeedback = "gatherTokens(\"" + text + "\") incorrect\n";

    EXPECT_EQ(expected, studentResult) << testFeedback;
}

TEST(GatherTokens, JustOneWord) {
    string text = "word";

    set<string> expected = {"word"};
    set<string> studentResult = gatherTokens(text);

    string testFeedback = "gatherTokens(\""+ text + "\") incorrect\n";

    EXPECT_EQ(expected, studentResult) << testFeedback;
}

TEST(GatherTokens, LeadingSpaces) {
    string text = "      Ummm, check please!?";

    set<string> expected = {"ummm","check","please"};
    set<string> studentResult = gatherTokens(text);

    string testFeedback = "gatherTokens(\"" + text + "\") incorrect\n";

    EXPECT_EQ(expected, studentResult) << testFeedback;
}

TEST(GatherTokens, TrailingSpaces) {
    string text = "Look at just how long I can truly be!                     ";

    set<string> expected = {"look","at","just","how","long","i","can","truly","be"};
    set<string> studentResult = gatherTokens(text);

    string testFeedback = "gatherTokens(\"" + text + "\") incorrect\n";

    EXPECT_EQ(expected, studentResult) << testFeedback;
}

TEST(BuildIndex, TinyTxt) {
    string filename = "tiny.txt";
    map<string, set<string>> expected = {
        {"eggs", {"www.shoppinglist.com"}},
        {"milk", {"www.shoppinglist.com"}},
        {"fish", {"www.shoppinglist.com", "www.dr.seuss.net"}},
        {"bread", {"www.shoppinglist.com"}},
        {"cheese", {"www.shoppinglist.com"}},
        {"red",{"www.rainbow.org", "www.dr.seuss.net"}},
        {"green",{"www.rainbow.org"}},
        {"orange", {"www.rainbow.org"}},
        {"yellow", {"www.rainbow.org"}},
        {"blue", {"www.rainbow.org", "www.dr.seuss.net"}},
        {"indigo", {"www.rainbow.org"}},
        {"violet", {"www.rainbow.org"}},
        {"one", {"www.dr.seuss.net"}},
        {"two", {"www.dr.seuss.net"}},
        {"i'm", {"www.bigbadwolf.com"}},
        {"not", {"www.bigbadwolf.com"}},
        {"trying", {"www.bigbadwolf.com"}},
        {"to", {"www.bigbadwolf.com"}},
        {"eat", {"www.bigbadwolf.com"}},
        {"you", {"www.bigbadwolf.com"}},
    };

    map<string, set<string>> studentIndex;
    int studentNumProcesed = buildIndex(filename, studentIndex);
    

    string indexTestFeedback = "buildIndex(\"" + filename + "\", ...) index incorrect\n";
    EXPECT_EQ(expected, studentIndex) << indexTestFeedback;

    string retTestFeedback = "buildIndex(\"" + filename + "\", ...) return value incorrect\n";
    EXPECT_EQ(4, studentNumProcesed) << retTestFeedback;
}

TEST(BuildIndex, MissingFile) {
    string filename = "tinoy.txt";

    map<string, set<string>> expected;

    map<string, set<string>> studentIndex;
    int studentNumProcesed = buildIndex(filename, studentIndex);

    string indexTestFeedback = "buildIndex(\"" + filename + "\", ...) index incorrect\n";
    EXPECT_EQ(expected, studentIndex) << indexTestFeedback;

    string retTestFeedback = "buildIndex(\"" + filename + "\", ...) return value incorrect\n";
    EXPECT_EQ(0, studentNumProcesed) << retTestFeedback;
}

TEST(FindQueryMatches, FirstQueryInvalid) {
    set<string> expected;

    expected = {};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "tough"));

    expected = {"uic.edu"};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "tough to"));

    expected = {};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "tough +all"));

    expected = {};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "tough -aviation"));
}

TEST(FindQueryMatches, LaterQueryInvalid) {
    set<string> expected;

    expected = {"example.com", "uic.edu", "random.org"};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "all me"));

    expected = {"random.org"};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "aviation -me"));

    expected = {};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "to +me"));

    expected = {};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "to -aviation +me"));
}