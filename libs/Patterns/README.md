# Patterns

Library for implementing generalized patterns.

## Error

This is the class that manages most basic error handling.
All errors shall be forwarded here if possible. This allows for a 
single library to handle all recoverable and non recoverable errors and simplify their debugging.

Error return values from this Error::Code enumeration shall be used throughout all libraries.
They have to be checked by the app developer and either handled or escalated. Every handling has
to be postponed by a CHECK_ERROR() call in the default (unhandled) case. This will result in
normal execution when no error is returned, but in restart of the controller otherwise.

## Singleton

There is no Singleton implementation in this lib, only this info.

### Description

Pattern for implenting classes that only can have on instance.
Always name the function for getting the instance _getInstance()_!
Also allows for building adapters to C callbacks that do not have any "void* context" argument (see CCallable).

There is two ways to write a singleton:

### Lazy Loading
This will call the constructor the first time the _getInstance()_ function is called.
```cpp
class Singleton
{
    // delete default constructors
    Singleton(const Singleton& other) = delete;
    Singleton& operator=(const Singleton& other) = delete;

    private:
    // constructor is private
    Singleton();

    public:
    // actual function to get the instance
    static Singleton& getInstance()
    {
        static Singleton instance{};
        return instance;
    }
};

// other code part
auto& myInstance = Singleton::getInstance();
```

### Eager Loading
This will call the constructor on program start.
```cpp
class Singleton
{
    // delete default constructors
    Singleton(const Singleton& other) = delete;
    Singleton& operator=(const Singleton& other) = delete;

    private:
    // constructor is private
    Singleton();

    // instance of the singleton
    static Singleton instance{};

    public:
    // actual function to get the instance
    static Singleton& getInstance()
    {
        return instance;
    }
};

// other code part
auto& myInstance = getInstance();
```

## Utility class

There is no Utility implementation in this code, only information.

### Usage

A utility class is a purely static class.
It can contain static functions and other class definitions, but can not be instantiated.
All of its implementation is private. Thereby it is not accessible to anyone except classes and functions declared as friend.
This results in a list of those entities. A permission list. Thereby internal functions and classes can be hidden from application developers, while still well organized for those writing the module.
Every module should only contain one Utility class.

### Example
```cpp
class Utility
{
    // permission list (forward declaration)
    friend class OtherClass;

    // purely static
    Utility() = delete;
    Utility(const Utility& other) = delete;
    Utility& operator=(const Utility& other) = delete;

private:
    class InternalClass{};

    static void doInternalLibStuff()
    {

    }
};

class OtherClass
{
public:
    void someFunction()
    {
        Utility::doInternalLibStuff();
        Utility::InternalClass instance{};
    }
}

```

## CCallable

### Functions

-   wrapCCallable
-   wrapSingleton

### Description

wrapCCallable is a function forwarding C callbacks with environment (or context) pointer
to object member functions.

### Example

```cpp
typedef void (CCallback*) (int a, void* environment, char b);

// c libn function to hook the callback
void hookCallback(CCallback cb, void* environment);

// your class
class Foo
{
    // member function callback handler
    void cbHandler(int a, char b);

    /**
     * more application stuff
     */
};

// class instance
Foo f1{};

// somewhere in your code
hookCallback(Patterns::CCallable<Foo, void(int, char), &Foo::cbHandler, int, char>, &f1);
```

## State Pattern

### Classes

-   StateMachine

### Description

Pattern for implementing state machines. Implementation roughly follows a finite
state machine.

No multi threading included here!
* For async functions please use RTOS::StateMachineTask class based on these.
* States only read and write to the Context, never to the Container.
* getNextState() decides which state to use next based on context and currentState only.

## Observer Pattern

### Classes

-   Observable
-   Observer

### Description

Allows any class that extends and implements "Observable" to notify any "Observers"
that register to them on something.
Observable and Observer that will be connected need to have the same template arguments.

### Usage

1.  Write a class A<> that implements Observable<T>
2.  Write any ammount of classes B<> that implement Observer<T>
3.  Pass an instance of your A<> class to every B<> instance on construction

## LifetimeList

LifetimeList is a generic collection that can be used without dynamic memory.
New elements are added through one of the factory functions. In the case of the
dynamically generated nodes, smart pointers are used to ensure proper lifetime.
Dynamic and static nodes can be mixed in the same list.
Elements get deleted when they go out of scope with the help of their destructor.
A list has to be emptied before it is destroyed.

### Notes
-   LifetimeList is std::iterable<...>.
-   the factory functions use perfect forwarding. That means, if T of LifetimeList<T> is constructable, you can pass any arguments that the constructor of T accepts.
-   Do not use stack allocated Nodes, they will be swithed out of scope.
-   When using statically allocated lists, use lazy loading. This way the constructor is guarenteed to be called before list is used the first time

### Example

The first example shows how to use LifetimeList to make a list of all objects of a class:
```cpp
#include "LifetimeList.h"

/**
 * class that has a registry of all instances
 */
class MyClass
{
    public:
        MyClass() : node(getInstances().appendStatic(*this))
        {
        }

        /**
         * lazy loading getter function needs to be used.
         * A static list inside this class would not be able to guarantee,
         * that the list is constructed before it is used the first time.
         */
        static Collections::LifetimeList<MyClass&>& getInstance()
        {
            static Collections::LifetimeList<MyClass&> instances{};
            return instances;
        }   

        /**
         * DO NOT DO THIS!
         * 
         * It does not guarantee order of construction!
         */
        // static Collections::LifetimeList<MyClass&> instances;

    private:
        Collections::LifetimeList<MyClass&>::Node node;         /**< the node belongs to the object */
};

Collections::LifetimeList<MyClass&> MyClass::instances{};

// instantiate as many as you want, they will all be registered in instances.
static MyClass myObjects[100];
```

The second example shows how to use it as a stand alone list
```cpp
#include "LifetimeList.h"

Collections::LifetimeList<int> iList{};     /**< generic list of integers */

void main()
{
    for (int i = 0; i < 10; ++i) {
        // static gets created in this scope
        auto nodeStatic = list.appendStatic(1);
        // dynamic gets created to a smart pointer
        auto nodeDynamic = list.pushDynamic(2);
        // destroy dynamic node
        nodeDynamic.reset();
        // make another dynamic node
        nodeDynamic = list.pushDynamic(3);
        // scope ends here, object and smart pointer get destroyed
    }
}
```

## PatternsPort

In order to execute error handling and testing properly, a few forward declarations
have to be made. The developer of the abstraction layer has to supply a PatternsPort.cpp
that translates these functions for her platform. 

## Testing

### Classes

-   Test::Base

### Usage

To use the testing pattern following has to be done:

All modules that implement testing, must contain a "Makefile.test". This
must include all testing files and headers. It will be called by the "Makefile.common"
when running test targets.
A highlevel module has to contain a call to _void Test::Base::runAllTests()_ in its test source files. The function has to be supplied with a callback to print output to the tester (null terminated strings).

-   There should be one test class per class that is tested.
-   All test classes should be in the Test namespace.
-   Do not build parrallel namespace trees (e.g. Patterns::MyClass and Test::Patterns::MyClass). Otherwise it will be difficult to get the Patterns::MyClass from the test class context.
-   Use the eager loading singleton pattern descried above. This way, you do not have to bother about instantiating the test. It also enables other tests to reference your test as a prerequisite.
-   Extend the Test::Base class to create your test.
-   The testing is done in the _void runInternal()_ virtual function. Implement that. Call the assign function whenever you want your test to check a value. The errMsg is written when the check failes.
-   If you need other modules to work, you can implement _const std::list<Test::Base*> getPrerequisits()_ to tell the testing framework which tests need to succeed before your test can be run.

### Example

```cpp
// Testing class
class YourTest : public Test::Base
{
    // constructors
public:
    // delete default constructors
    FlashRecord(const FlashRecord &other) = delete;
    FlashRecord &operator=(const FlashRecord &other) = delete;

    /**
     * @brief get singleton instance
     */
    static FlashRecord &getInstance()
    {
        return instance;
    }

private:
    FlashRecord() : Test::Base("IO", "YourClassToTest")
    {
    }

    // actual testing method
    virtual void runInternal() final
    {
        assign(1==1, "weird stuff went wrong");
        
        // your tests
    }

    // optional prerequisits
    virtual const std::list<Base *> getPrerequisits() final
    {
        return std::list<Base*>({&Test::OtherTest::getInstance()});
    }

    // member you might need for testing go here

    /** singleton instance */
    static FlashRecord instance;
};
```

## Authors

* **Joshua Lauterbach** - *joshua@aconno.de*

## License

This project is made by aconno.de
