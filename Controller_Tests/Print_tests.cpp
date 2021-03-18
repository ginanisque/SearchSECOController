/*
This program has been developed by students from the bachelor Computer Science at
Utrecht University within the Software Project course.
� Copyright Utrecht University (Department of Information and Computing Sciences)
*/

#include "pch.h"
#include "../Print.h"
#include "../Print.cpp"
#include "../Utils.cpp"
#include <regex>

#define GENERIC_STRING "teststring"
#define GENERIC_INT 1

// testing data
int test_strc = 5;
std::string* test_strs = new std::string[test_strc]
{
	"test one",
	"my test",
	"testing a function",
	"this is a random string that is very long, look at me I am a string",
	""
};

// print helper functions
TEST(tab_tests, single)
{
	std::string s = "\t";
	std::string tabstring = print::tab(1);
	EXPECT_EQ(s, tabstring);
}

TEST(tab_tests, multiple)
{
	std::string s = "\t\t\t";
	std::string tabstring = print::tab(3);
	EXPECT_EQ(s, tabstring);
}

TEST(encapsulate, succeeding)
{
	for (int i = 0; i < test_strc; ++i)
	{
		std::string word = test_strs[i];
		char c = (char)(std::rand() % 255 + 1);
		std::string encapsulated = print::encapsulate(word, c);

		EXPECT_EQ(c + word + c, encapsulated);
	}
}

TEST(encapsulate, failing)
{
	for (int i = 0; i < test_strc; ++i)
	{
		std::string word = test_strs[i];
		char c = (char)(std::rand() % 255 + 1);
		std::string encapsulated = print::encapsulate(word, c);

		EXPECT_FALSE(word == encapsulated);
		EXPECT_FALSE(std::to_string(c) == encapsulated);
	}
}

TEST(quote, succeeding)
{
	for (int i = 0; i < test_strc; ++i)
	{
		std::string word = test_strs[i];
		char c = '\"';
		std::string encapsulated = print::encapsulate(word, c);

		EXPECT_EQ(c + word + c, encapsulated);
	}
}

TEST(quote, failing)
{
	for (int i = 0; i < test_strc; ++i)
	{
		std::string word = test_strs[i];
		char c = '\'';
		std::string encapsulated = print::encapsulate(word, c);

		EXPECT_FALSE(word == encapsulated);
		EXPECT_FALSE(std::to_string(c) == encapsulated);
	}
}

TEST(plural, singular)
{
	for (int i = 0; i < test_strc; ++i)
	{
		std::string word = test_strs[i];
		std::string pluraled = print::plural(word,1);

		EXPECT_EQ(word, pluraled);
	}
}

TEST(plural, plural)
{
	for (int i = 0; i < test_strc; ++i)
	{
		std::string word = test_strs[i];
		char c = 's';
		int n= std::rand() % 200 + 2;
		std::string pluraled = print::plural(word, n);

		EXPECT_EQ(word + c, pluraled);
	}
}

TEST(plural, singular_failing)
{
	for (int i = 0; i < test_strc; ++i)
	{
		std::string word = test_strs[i];
		char c = 's';
		std::string pluraled = print::plural(word, 1);

		EXPECT_FALSE(word + c == pluraled);
	}
}

TEST(plural, plural_failing)
{
	for (int i = 0; i < test_strc; ++i)
	{
		std::string word = test_strs[i];
		int n = std::rand() % 200 + 2;
		std::string pluraled = print::plural(word, n);

		EXPECT_FALSE(word == pluraled);
	}
}

// printing functions

TEST(Print_line, print_line_test)
{
	for (int i = 0; i < test_strc; ++i)
	{
		testing::internal::CaptureStdout();
		std::string input = test_strs[i];
		print::printline(input);
		std::string output = testing::internal::GetCapturedStdout();
		EXPECT_EQ(output, input + '\n');
	}

}

TEST(version_test, version_regex)
{
	testing::internal::CaptureStdout();

	print::version_full();

	std::string output = testing::internal::GetCapturedStdout();

	ASSERT_TRUE(std::regex_match(output, std::regex("((searchseco|parser|spider|database_api) version \\d*.\\d*.\\d*\\n(\\t)?)*")));
}

// ERROR TESTING

int test_intc = 6;
int* test_ints = new int[test_intc]
{
	INT_MIN,
	-1,
	0,
	1,
	5,
	INT_MAX
};

TEST(error_testing, log)
{
	std::string output;
	for (int i = 0; i < test_strc; ++i)
	{
		testing::internal::CaptureStdout();

		error::log(test_strs[i]);
		output = testing::internal::GetCapturedStdout();

		ASSERT_EQ("L - " + test_strs[i] + '\n', output);
	}
}

TEST(error_testing, warn)
{
	std::string output;
	for (int i = 0; i < test_intc; ++i)
	{
		int code = test_ints[i];

		if (code <= 0)
		{
			ASSERT_THROW(error::warn(code), std::out_of_range);
		}
		else
		{
			testing::internal::CaptureStdout();

			error::warn(code);

			output = testing::internal::GetCapturedStdout();

			output = utils::split(output, '-')[0];
			ASSERT_EQ("W" + std::to_string(test_ints[i]) + ' ', output);
		}
	}
}

// All error throwing functions should have more or less the same behaviour: print some string to stdout,
// and then kill the program.
// It seems to be impossible to test whether this string is in the correct format, since the program terminates
// before we are able to read stdout, so for now these death tests only assert that all error functions terminate
// the program with code EXIT_FAILURE.

TEST(error_death_tests, err_insufficient_arguments)
{
	ASSERT_EXIT(error::err_insufficient_arguments(GENERIC_STRING, 1,2), ::testing::ExitedWithCode(EXIT_FAILURE), ".*");
}

TEST(error_death_tests, err_flag_not_exist)
{
	ASSERT_EXIT(error::err_flag_not_exist(GENERIC_STRING, true ), ::testing::ExitedWithCode(EXIT_FAILURE), ".*");
	ASSERT_EXIT(error::err_flag_not_exist(GENERIC_STRING, false), ::testing::ExitedWithCode(EXIT_FAILURE), ".*");
}

TEST(error_death_tests, err_flag_invalid_arg)
{
	ASSERT_EXIT(error::err_flag_invalid_arg(GENERIC_STRING, GENERIC_STRING, true ), ::testing::ExitedWithCode(EXIT_FAILURE), ".*");
	ASSERT_EXIT(error::err_flag_invalid_arg(GENERIC_STRING, GENERIC_STRING, false), ::testing::ExitedWithCode(EXIT_FAILURE), ".*");
}

TEST(error_death_tests, err_cmd_not_found)
{
	ASSERT_EXIT(error::err_cmd_not_found(), ::testing::ExitedWithCode(EXIT_FAILURE), ".*");
}

TEST(error_death_tests, err_cmd_not_exist)
{
	ASSERT_EXIT(error::err_cmd_not_exist(GENERIC_STRING), ::testing::ExitedWithCode(EXIT_FAILURE), ".*");
}

TEST(error_death_tests, err_not_implemented)
{
	ASSERT_EXIT(error::err_not_implemented(GENERIC_STRING), ::testing::ExitedWithCode(EXIT_FAILURE), ".*");
}
