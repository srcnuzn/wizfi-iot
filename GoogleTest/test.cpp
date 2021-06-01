/*
 * test.cpp
 *
 *  Created on: 29.05.2021
 *      Author: serca
 */


#include "gtest/gtest.h"

extern "C" {

	#include "wizfi360.h"

	extern WIZFI360_HandlerTypedef wizfi360;
}

class Wizfi360Test: public ::testing::Test
{
	public:
	Wizfi360Test( ) {
	       // initialization code here
	   }

	   void SetUp( ) {
			WIZFI360_Initialize();
	   }

	   void TearDown( ) {
	       // code here will be called just after the test completes
	       // ok to through exceptions from here if need be
	   }

	   ~Wizfi360Test( )  {
	       // cleanup any pending stuff, but no exceptions allowed
	   }

	   // put in any custom data members that you need
};



