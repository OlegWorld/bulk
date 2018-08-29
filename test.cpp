#include <iostream>
#include <fstream>
#include <gtest/gtest.h>
#include "command_handler.h"

TEST(BulkTest, CommandProcessorSingleTest) {
    CommandProcessor proc;
    proc.add("command");
    testing::internal::CaptureStdout();
    proc.process();
    std::string output = testing::internal::GetCapturedStdout();

    ASSERT_EQ(output, "bulk: command\n");
}

TEST(BulkTest, CommandProcessorMultipleTest) {
    CommandProcessor proc;
    proc.add("command1");
    proc.add("command2");
    proc.add("command3");
    proc.add("three words command");
    testing::internal::CaptureStdout();
    proc.process();
    std::string output = testing::internal::GetCapturedStdout();

    ASSERT_EQ(output, "bulk: command1, command2, command3, three words command\n");
}

TEST(BulkTest, CommandReaderTest) {
    CommandReader reader(1);
    CommandProcessor proc1;
    CommandProcessor proc2;
    reader.subscribe(&proc1);
    reader.subscribe(&proc2);
    reader.push_string("command");
    testing::internal::CaptureStdout();
    reader.notify();
    std::string output = testing::internal::GetCapturedStdout();

    ASSERT_EQ(output, "bulk: command\nbulk: command\n");
}

TEST(BulkTest, CommandReaderAbortTest) {
    CommandReader reader(1);
    CommandProcessor proc1;
    CommandProcessor proc2;
    reader.subscribe(&proc1);
    reader.subscribe(&proc2);
    reader.push_string("command");
    reader.abort();
    testing::internal::CaptureStdout();
    reader.notify();
    std::string output = testing::internal::GetCapturedStdout();

    ASSERT_EQ(output, "\n\n");
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}