/**
 * @file TestScopeExit.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief Test for the ScopeObject class
 * @version 1.0
 * @date 2020-10-13
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#ifndef __TESTSCOPEEXIT_H__
#define __TESTSCOPEEXIT_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace Test
{
class ScopeExit;
}

//--------------------------------- INCLUDES ----------------------------------

#include "TestBase.h"

namespace Test
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief Test for the ScopeObject class
 */
class ScopeExit : public Test::Base
{

// constructors
public:
    // delete default constructors
    ScopeExit(const ScopeExit& other) = delete;
    ScopeExit& operator=(const ScopeExit& other) = delete;

    /**
    * @brief get singleton instance
    */
    static ScopeExit &getInstance();

private:
    ScopeExit();

    virtual void runInternal() final;

    /** singleton instance */
    static ScopeExit instance;

    struct MemberFunctionTest {
        bool changed = false;
        int value = 0;
        int reference = 0;

        MemberFunctionTest(){}
        void change() {
            changed = true;
        }

        void emplaceNumbers(int value, const int& reference) {
            this->value = value;
            this->reference = reference;
        }
    };
};
} // Test
#endif //__TESTSCOPEEXIT_H__