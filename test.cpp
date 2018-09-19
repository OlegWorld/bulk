#include <iostream>
#include <fstream>
#include <sstream>
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
    CommandReader reader(1, std::cin);
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
    CommandReader reader(1, std::cin);
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

TEST(BulkTest, SingleCommandStringInputTest) {
    const std::string inputString("command1\ncommand2\ncommand3\n");
    std::istringstream inputStream(inputString);

    CommandReader reader(1, inputStream);
    CommandProcessor proc;
    reader.subscribe(&proc);
    testing::internal::CaptureStdout();
    reader.scan_input();
    std::string output = testing::internal::GetCapturedStdout();

    ASSERT_EQ(output, "bulk: command1\nbulk: command2\nbulk: command3\n\n");
}

TEST(BulkTest, MulpipleCommandsStringInputTest) {
    const std::string inputString("command1\ncommand2\ncommand3\n");
    std::istringstream inputStream(inputString);

    CommandReader reader(3, inputStream);
    CommandProcessor proc;
    reader.subscribe(&proc);
    testing::internal::CaptureStdout();
    reader.scan_input();
    std::string output = testing::internal::GetCapturedStdout();

    ASSERT_EQ(output, "bulk: command1, command2, command3\n\n");
}

TEST(BulkTest, SingleCommandStringInputTestWithTwoProcessors) {
    const std::string inputString("command1\ncommand2\n");
    std::istringstream inputStream(inputString);

    CommandReader reader(1, inputStream);
    CommandProcessor proc1;
    CommandProcessor proc2;
    reader.subscribe(&proc1);
    reader.subscribe(&proc2);
    testing::internal::CaptureStdout();
    reader.scan_input();
    std::string output = testing::internal::GetCapturedStdout();

    ASSERT_EQ(output, "bulk: command1\nbulk: command1\nbulk: command2\nbulk: command2\n\n\n");
}

TEST(BulkTest, SwitchStateStringInputTest) {
    const std::string inputString("command1\n{\ncommand2\ncommand3\ncommand4\n}\n");
    std::istringstream inputStream(inputString);

    CommandReader reader(2, inputStream);
    CommandProcessor proc;
    reader.subscribe(&proc);
    testing::internal::CaptureStdout();
    reader.scan_input();
    std::string output = testing::internal::GetCapturedStdout();

    ASSERT_EQ(output, "bulk: command1\nbulk: command2, command3, command4\n\n");
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}