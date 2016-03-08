# Title 

## Building 
Navigate to the database/tests folder and run 

	make

## Running 

	./tests

## Creating a test

Right now to change what test is being done, you will need to change the test directly in GraphTest.

Currently there are 3 modes to choose from:

* MODE_CREATE
* MODE_DROP
* MODE_INSERT

Mode insert is very early on, it is only set up but doesn't actually work.

Drop test example: 

``` c++
    TestResult result = dataStoreTest.with(MODE_CREATE)
                                    .generateCases(0b0000)
                                    .test();
```

## Threads

* Threads are created using the C++11 thread library within DataStoreTest::test()

## Test creation flow

1. The first function to be called is DataStoreTest::with(int mode). This function sets up the test we are going to use according to the struct in Modes.h
	* Eventually we want to allow setting manually made sql statements here

2. DataStoreTest::generateCases(int complexity) is called
	* DataStoreTest::generateCases(int complexity) in a complexity bitmask and parses it to get information about the test. The information stored in the bitmask is as follows

		dasdsa

	* The Tervel object is created right after DataStoreTest::parseComplexity() since thats when we find out how many threads there is going to be

	* Then the test statements are created based on the mode. Currently they are just the same statement for performance testing but they will also be randomized in the future

3. Finally we call DataStoreTest::test(). 