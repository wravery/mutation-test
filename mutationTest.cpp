#include "SomeSchema.h"

#include <graphqlservice/JSONResponse.h>
#include <gtest/gtest.h>

#include <iostream>

using namespace graphql;

class MockSomething : public some::object::Something
{
public:
	MockSomething() = default;

	service::FieldResult<response::StringType> getSomeId(service::FieldParams&& params) const final
	{
		return std::string("01");
	}

	service::FieldResult<response::StringType> getSomefield(service::FieldParams&& params) const final
	{
		return std::string("foobar");
	}

	service::FieldResult<response::Value> getDate(service::FieldParams&& params) const final
	{
		response::Value date(response::Type::Map);

		date.emplace_back("foo", response::Value{ "bar" });

		return date;
	}

};

class MockMutation : public some::object::Mutation
{
public:
    MockMutation() = default;

    service::FieldResult<std::shared_ptr<some::object::Something>> applyAddSomething(service::FieldParams&& params, std::optional<some::SomeInput>&& inputArg) const final
    {
        return std::make_shared<MockSomething>();
    }
};

class SomeSchemaTest : public ::testing::Test
{
public:
    static void SetUpTestCase()
    {
        operations_ = std::make_shared<some::Operations>(std::make_shared<MockMutation>());
    }

    static void TearDownTestCase()
    {
        operations_.reset();
    }

protected:
    static std::shared_ptr<some::Operations> operations_;
};

std::shared_ptr<some::Operations> SomeSchemaTest::operations_{};


TEST_F(SomeSchemaTest, queryWithVariables)
{
    auto query = R"gql(mutation variables($data: SomeInput!) {
        withVariables: addSomething(input: $data) {
            somefield
            date
        }
    })gql"_graphql;
    auto variables = response::parseJSON(R"js({ "data": { "someId": "01", "somefield": "foobar", "date": { "foo": "bar" } })js");
    auto result = operations_->resolve({}, *query.root, "variables", std::move(variables)).get();

	ASSERT_EQ(R"js({"data":{"withVariables":{"somefield":"foobar","date":{"foo":"bar"}}}})js", response::toJSON(std::move(result))) << "output matches";
}

TEST_F(SomeSchemaTest, queryWithInlineInput)
{
    auto query = R"gql(mutation inputs {
        withInlineInput: addSomething(input: { someId: "01" somefield: "foobar" date: { foo: "bar" } }) {
            somefield
            date
        }
    })gql"_graphql;
    auto result = operations_->resolve({}, *query.root, "inputs", {}).get();

	ASSERT_EQ(R"js({"data":{"withInlineInput":{"somefield":"foobar","date":{"foo":"bar"}}}})js", response::toJSON(std::move(result))) << "output matches";
}
