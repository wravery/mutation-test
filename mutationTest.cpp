#include "SomeSchema.h"

#include <graphqlservice/JSONResponse.h>

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

void queryWithVariables(const std::shared_ptr<some::Operations>& operations)
{
    auto query = R"gql(mutation variables($data: SomeInput!) {
        withVariables: addSomething(input: $data) {
            somefield
        }
    })gql"_graphql;
    auto variables = response::parseJSON(R"js({ "data": { "someId": "01", "somefield": "foobar", } })js");
    auto result = operations->resolve({}, *query.root, "variables", std::move(variables)).get();

    std::cout << response::toJSON(std::move(result)) << std::endl;
}

void queryWithInlineInput(const std::shared_ptr<some::Operations>& operations)
{
    auto query = R"gql(mutation inputs {
        withInlineInput: addSomething(input: { someId: "01" somefield: "foobar" }) {
            somefield
        }
    })gql"_graphql;
    auto result = operations->resolve({}, *query.root, "inputs", {}).get();

    std::cout << response::toJSON(std::move(result)) << std::endl;
}

int main(int argc, char** argv)
{
    auto operations = std::make_shared<some::Operations>(std::make_shared<MockMutation>());
    
    queryWithVariables(operations);
    queryWithInlineInput(operations);

    return 0;
}