#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "../Lights/RuleInterpreter.h"

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::Invoke;
using ::testing::_;

using Bytes = std::vector<byte>;

// 4 inputs, 4 outputs to be used in tests
namespace {
	enum Inputs {
		input_zero_zero,
		input_zero_one,
		input_one_zero = 1 << SHIFT_PORT,
		input_one_one
	};

	enum Outputs {
		output_zero_zero,
		output_zero_one,
		output_one_zero = 1 << SHIFT_PORT,
		output_one_one
	};
}

class FakeSource : public RulesSource
{
public:
	FakeSource() {}

	void use(const Bytes& rules) {
		m_rules = rules;
	}

	byte readByte(byte offset) const {
		return m_rules.at(offset);
	}

	unsigned size() const {
		return m_rules.size();
	}
private:
	Bytes m_rules;
};

class MockRulesSource : public RulesSource
{
public:
	MockRulesSource() {}
	MOCK_CONST_METHOD1(readByte, byte(byte offset));
	MOCK_CONST_METHOD0(size, unsigned());

	void useRules(const Bytes& rules) {
		fakeSource.use(rules);
		ON_CALL(*this, size()).WillByDefault(Invoke(&fakeSource, &FakeSource::size));
		ON_CALL(*this, readByte(_)).WillByDefault(Invoke(&fakeSource, &FakeSource::readByte));
	}
private:
	FakeSource fakeSource;
};

TEST(RuleInterpreter, returns_noop_if_there_are_no_rules)
{
	MockRulesSource source;	
	RuleInterpreter interpreter(source);

	EXPECT_CALL(source, size()).Times(1).WillOnce(Return(0));
	ASSERT_EQ(noop, interpreter.pressedFor(1, 2, 3));
}

TEST(RuleInterpreter, skips_one_byte_after_reading_a_byte_with_a_null_trigger)
{
	MockRulesSource source;
	RuleInterpreter interpreter(source);

	const auto size = 42;
	const auto byteWithNoTrigger = 0x3F;
	EXPECT_CALL(source, size()).WillRepeatedly(Return(size));
	EXPECT_CALL(source, readByte(_)).Times(size/2).WillRepeatedly(Return(byteWithNoTrigger));
	ASSERT_EQ(noop, interpreter.pressedFor(1, 2, 3));
}

TEST(RuleInterpreter, reads_every_bytes_from_rules_if_there_are_no_null_triggers_and_no_matching_rules)
{
	MockRulesSource source;
	RuleInterpreter interpreter(source);

	const auto noMatch = 0xFF;
	const auto size = 42;
	EXPECT_CALL(source, size()).WillRepeatedly(Return(size));
	EXPECT_CALL(source, readByte(_)).Times(size).WillRepeatedly(Return(noMatch));
	ASSERT_EQ(noop, interpreter.pressedFor(1, 2, 3));
}

TEST(RuleInterpreter, returns_noop_if_no_matching_rule_is_found)
{
	MockRulesSource source;
	RuleInterpreter interpreter(source);

	source.useRules({
		WHEN_PRESSED_SHORT(zero_zero), TOGGLE(one_zero)
	});

	ASSERT_EQ(noop, interpreter.pressedFor(1, 1, ms_to_ticks(LongPressMs)));
}

TEST(RuleInterpreter, returns_correct_command_when_a_matching_rule_is_found)
{
	MockRulesSource source;
	RuleInterpreter interpreter(source);

	source.useRules({
		WHEN_PRESSED_SHORT(zero_zero), TOGGLE(one_zero)
	});

	auto command = interpreter.pressedFor(0, 0, ms_to_ticks(ShortPressMs));
	ASSERT_NE(noop, command);
	ASSERT_EQ(command.address, 1);
	ASSERT_EQ(command.output, 0);
	ASSERT_EQ(command.effect, Toggle);
}

TEST(RuleInterpreter, returns_correct_command_when_more_rules_are_defined)
{
	MockRulesSource source;
	RuleInterpreter interpreter(source);

	source.useRules({
		WHEN_PRESSED_SHORT(zero_zero), TOGGLE(one_zero),
		WHEN_PRESSED_SHORT(one_zero), TOGGLE(zero_one)
	});

	auto command = interpreter.pressedFor(1, 0, ms_to_ticks(ShortPressMs));
	ASSERT_NE(noop, command);
	ASSERT_EQ(command.address, 0);
	ASSERT_EQ(command.output, 1);
	ASSERT_EQ(command.effect, Toggle);
}

TEST(RuleInterpreter, pressed_handler_fails_fast_if_there_are_more_rules_for_the_input)
{
	MockRulesSource source;
	RuleInterpreter interpreter(source);

	source.useRules({
		WHEN_PRESSED_MEDIUM(zero_zero), TOGGLE(zero_zero),
		WHEN_PRESSED_SHORT(zero_zero), TOGGLE(one_zero)
	});

	EXPECT_CALL(source, size()).Times(AtLeast(1));
	EXPECT_CALL(source, readByte(_)).Times(2);
	ASSERT_EQ(noop, interpreter.pressedFor(0, 0, ms_to_ticks(ShortPressMs)));
}

TEST(RuleInterpreter, released_handler_returns_command_for_short_press_when_releasing_after_short_press_time)
{
	MockRulesSource source;
	RuleInterpreter interpreter(source);

	source.useRules({
		WHEN_PRESSED_MEDIUM(zero_zero), TOGGLE(zero_zero),
		WHEN_PRESSED_SHORT(zero_zero), TOGGLE(one_zero)
	});

	EXPECT_CALL(source, size()).Times(AtLeast(1));
	EXPECT_CALL(source, readByte(_)).Times(4);

	auto command = interpreter.releasedAfter(0, 0, ms_to_ticks(ShortPressMs));
	ASSERT_NE(noop, command);
	ASSERT_EQ(command.address, 1);
	ASSERT_EQ(command.output, 0);
	ASSERT_EQ(command.effect, Toggle);
}

TEST(RuleInterpreter, released_handler_returns_command_for_medium_press_when_releasing_after_medium_press_time)
{
	MockRulesSource source;
	RuleInterpreter interpreter(source);

	source.useRules({
		WHEN_PRESSED_MEDIUM(zero_zero), TOGGLE(zero_zero),
		WHEN_PRESSED_SHORT(zero_zero), TOGGLE(one_zero)
	});

	EXPECT_CALL(source, size()).Times(AtLeast(1));
	EXPECT_CALL(source, readByte(_)).Times(2);

	auto command = interpreter.releasedAfter(0, 0, ms_to_ticks(MediumPressMs));
	ASSERT_NE(noop, command);
	ASSERT_EQ(command.address, 0);
	ASSERT_EQ(command.output, 0);
	ASSERT_EQ(command.effect, Toggle);
}

TEST(RuleInterpreter, last_value_saved_with_store_command_can_be_read_in_context)
{
	MockRulesSource source;
	RuleInterpreter interpreter(source);

	source.useRules({
		WHEN_PRESSED_SHORT(zero_zero), STORE(11), STORE(22), STORE(33), TOGGLE(one_zero)
	});

	EXPECT_CALL(source, size()).Times(AtLeast(1));
	EXPECT_CALL(source, readByte(_)).Times(5);

	auto command = interpreter.pressedFor(0, 0, ms_to_ticks(ShortPressMs));
	ASSERT_NE(noop, command);
	ASSERT_EQ(33, interpreter.context());
}