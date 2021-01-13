/**
 * @file TestBase.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief abstract base class for all tests written in aconno libraries
 * @version 1.0
 * @date 2020-04-06
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __TESTBASE_H__
#define __TESTBASE_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace Test
{
class Base;
}

//--------------------------------- INCLUDES ----------------------------------

#include <PatternsPort.h>

#include <cstdint>
#include <cstdio>
#include <list>

namespace Test
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief abstract base class for all tests written in aconno libraries
 */
class Base
{
    // constructors
public:
    // delete default constructors
    Base() = delete;
    Base(const Base &other) = delete;
    Base &operator=(const Base &other) = delete;

    /**
     * @brief Construct a new Base object
     *
     * @param module
     */
    Base(const char *const moduleName, const char *const className);

    // exposed functions
public:
    /** get module name */
    const char *const getModule();
    /** get class name */
    const char *const getClass();
    /**
     * @brief runs a test
     *
     * @return true test succesfully
     * @return false at least one error
     */
    bool run();

    /**
     * @brief function runs all test instances in the project.
     *
     */
    static void runAllTests();

    // interface implementation
public:
    // private variables
private:
    /** module of the class */
    const char *const moduleName;
    /** class beeing tested */
    const char *const className;
    /** saves, whether test already ran */
    bool testRan;
    /** saves the result of the test */
    bool result;
    /** number of succesful assignments */
    uint32_t succesfulCount;
    /** number of errors detected */
    uint32_t errorCount;

    /**
     * @brief list of all tests
     *
     */
    static std::list<Base *> tests;
    /** internal buffer for print functions */
    static char strBuffer[128];

protected:
    // functions used by classes implementing tests
    /**
     * @brief call this function with a value to evaluate.
     * If evaluation is false, error is detected and errorMessage is triggered.
     *
     * @param eval value to evaluate to true
     * @param errorMessage message to print on failure
     */
    template<class... ArgTypes>
    void assert(bool eval, const char *const errorMessage, ArgTypes... args)
    {
        if (eval) {
            ++succesfulCount;
        } else {
            ++errorCount;
            int i = snprintf(strBuffer, sizeof(strBuffer), "\t");
            if (i < 0)
                return;
            i += snprintf(&strBuffer[i],
                          sizeof(strBuffer) - 1,
                          errorMessage,
                          args...);
            if (i < 0)
                return;
            if (i < static_cast<int>(sizeof(strBuffer) - 1)) {
                snprintf(&strBuffer[i], sizeof(strBuffer) - i, "\n");
            }
            Port::logInfo(strBuffer);
        }
    }
    /**
     * @brief prints to printFct
     *
     * @tparam ArgTypes
     * @param str
     * @param args
     */
    template<class... ArgTypes>
    static void print(const char *const str, ArgTypes... args)
    {
        uint32_t i = snprintf(strBuffer, sizeof(strBuffer), str, args...);
        if (i < sizeof(strBuffer) - 1) {
            snprintf(&strBuffer[i], 2, "\n");
        }
        Port::logInfo(strBuffer);
    }

    // functions implemented by tests
    /**
     * @brief implement the test in this function
     *
     */
    virtual void runInternal() = 0;
    /**
     * @brief implement and pass enumerable start / end to set prerequisits.
     * These will be run before this test is.
     *
     * Default implementation returns empy array (no prerequisits).
     *
     * @param iterableBegin start of an iterable
     * @param iterableEnd end of an iterable
     */
    virtual const std::list<Base *> getPrerequisits();
};
}  // namespace Test
#endif  //__TESTBASE_H__