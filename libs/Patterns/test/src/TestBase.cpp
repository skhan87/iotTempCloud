/**
 * @file TestBase.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief basae class for all tests written in aconno libs
 * @version 1.0
 * @date 2020-04-06
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "TestBase.h"

#include <array>

//--------------------------- STRUCTS AND ENUMS -------------------------------

std::list<Test::Base*> Test::Base::tests {};
char                   Test::Base::strBuffer[128] = {(char)0};

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

Test::Base::Base(const char* const moduleName, const char* const className)
        : moduleName(moduleName), className(className), testRan(false),
          result(false), succesfulCount(0), errorCount(0)
{
    Test::Base::tests.push_back(this);
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

bool Test::Base::run()
{
    if (testRan) {
        print("\tWas already executed");
        return result;
    }

    runInternal();
    testRan = true;

    if (errorCount + succesfulCount == 0) {
        print("\tDoes not contain a single asserts and is "
              "thereby a useless test");
        result = false;
        return result;
    } else if (errorCount > 0) {
        result = false;
    } else {
        result = true;
    }

    print("\tResult: %lu succesful and %lu failed asserts",
          succesfulCount,
          errorCount);

    return result;
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

const std::list<Test::Base*> Test::Base::getPrerequisits()
{
    static std::list<Test::Base*> lst {};
    return lst;
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------

void Test::Base::runAllTests()
{
    if (tests.size() < 1) {
        print("--------------- No Tests found, leaving ---------------");
        return;
    }

    print("--------------- Starting all tests ---------------");
    std::list<Test::Base*> toTest {tests};

    uint32_t succesfulTests = 0;
    uint32_t failedTests    = 0;

    while (toTest.size() > 0) {
        bool testedOne = false;
        // tests that will be deleted from toTest at the end of this run
        std::list<Test::Base*> testsToDelete {};

        for (auto* test : toTest) {
            // iterate over prerequisites
            bool waiting   = false;
            bool preFailed = false;
            for (auto pre : test->getPrerequisits()) {
                if (!pre->testRan) {
                    // still waiting for that test to execute
                    waiting = true;
                    break;
                } else {
                    // this prerequisite ran
                    if (!pre->result) {
                        // pre failed, cannot execute this test
                        print("%s::%s\n{\n\tcan not be executed, because "
                              "%s::%s is prerequisite and failed\n}\nskipped\n",
                              test->moduleName,
                              test->className,
                              pre->moduleName,
                              pre->className);
                        testsToDelete.push_back(test);
                        preFailed = true;
                        break;
                    }
                }
            }

            if (waiting || preFailed) {
                // still waiting for other tests
                continue;
            }

            print("%s::%s\n{", test->moduleName, test->className);
            if (test->run()) {
                print("}\nsuccesful\n");
                succesfulTests++;
            } else {
                print("}\nfailed\n");
                failedTests++;
            }

            testedOne = true;
            testsToDelete.push_back(test);
        }

        if (!testedOne) {
            print("The following leftover tests have "
                  "a circular dependency:");
            for (auto* test : toTest) {
                print("\t%s::%s", test->moduleName, test->className);
                failedTests++;
            }
            print("\n");
            break;
        }

        // delete tests that ran and tests that are missing pre from todo
        for (auto const test : testsToDelete) {
            toTest.remove(test);
        }
    }

    print("--------------- Finished all tests ---------------");

    if (failedTests > 0) {
        print("FAILED");
    } else {
        print("SUCCESS");
    }
    print("\tModules tested successully:\t%u", succesfulTests);
    print("\tModules failing tests:\t\t%u", failedTests);
}