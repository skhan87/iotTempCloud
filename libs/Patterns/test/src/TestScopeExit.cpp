/**
 * @file TestScopeExit.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief Test foor the ScopeExit class
 * @version 1.0
 * @date 2020-10-13
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

//--------------------------------- INCLUDES ----------------------------------

#include "TestScopeExit.h"
#include "ScopeExit.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

Test::ScopeExit Test::ScopeExit::instance{};

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

Test::ScopeExit::ScopeExit() : Base("Patterns", "ScopeExit") {}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

void Test::ScopeExit::runInternal() {
    bool called = false;
    {
        auto se = Patterns::make_scopeExit([&called](){called = true;});
    }
    assert(called == true, "failed to run simple ScopeExit");

    int x = 0;
    {
        auto se = Patterns::make_scopeExit([](int& number, int otherNumber){number = otherNumber;}, x, 5);
    }
    assert(x == 5, "Lambda function with arguments did not work");

    MemberFunctionTest mft1{};
    {
        auto se = Patterns::make_scopeExit(&MemberFunctionTest::change, mft1);
    }
    assert(mft1.changed == true, "Member function not triggered");

    MemberFunctionTest mft2{};
    {
        auto se = Patterns::make_scopeExit(&MemberFunctionTest::change, mft2);
        se.deactivate();
    }
    assert(mft2.changed == false, "ScopeExit triggered when deactived");

    MemberFunctionTest mft3{};
    constexpr int referencedI = 15;
    {
        auto se = Patterns::make_scopeExit(&MemberFunctionTest::emplaceNumbers, mft3, 10, referencedI);
    }
    assert((mft3.value == 10) && (mft3.reference == 15), "Member function with parameters failed");
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------

Test::ScopeExit& Test::ScopeExit::getInstance() {
    return instance;
}