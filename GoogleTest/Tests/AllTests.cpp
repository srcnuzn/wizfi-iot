/*
 * main.cpp
 *
 *  Created on: 29.05.2021
 *      Author: serca
 */

#include "../gtest/gtest.h"

// https://wiki.eclipse.org/Linux_Tools_Project/GCov/User_Guide
// https://stackoverflow.com/questions/16682606/how-to-resolve-gcov-init-undefined-reference-issue-when-linking

int main (int argc, char **argv) {
	:: testing:: InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
