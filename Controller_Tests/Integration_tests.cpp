﻿/*
This program has been developed by students from the bachelor Computer Science at
Utrecht University within the Software Project course.
� Copyright Utrecht University (Department of Information and Computing Sciences)
*/
#include "pch.h"

#include "Flags.h"
#include "Commands.h"




TEST(IntegrationTest, Crawler)
{
    EXPECT_TRUE(true);
}

TEST(IntegrationTest, Spider)
{
    EXPECT_TRUE(true);
}

TEST(IntegrationTest, Parser)
{
    // Dummy variables.
    Flags flags;

    Commands::parseRepository("https://github.com/zavg/linux-0.01.git", flags);
    EXPECT_TRUE(true);
}

TEST(IntegrationTest, DatabaseAPI)
{
    EXPECT_TRUE(true);
}
