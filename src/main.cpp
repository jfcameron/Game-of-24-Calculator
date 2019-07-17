// © 2019 Joseph Cameron - All Rights Reserved
/// \brief brute force calculator for 24 game, extended to N game
///
/// 24 game rules:
///  given a set of 4 real numbers, the player must create an expression that evaluates to 24
///  each number MUST be used EXACTLY once.
///  legal operations are +, -, /, *.
///  operations can be used any number of times and in any order.
///
/// The rules have been extended so that:
///     the target number "24" can be substituted for any other real number.
///     the user can be given any number of floating point numbers, they are not limited to 4
///
/// How the calculator works:
///   1) a list of all possible permutations of operations for the given set of numbers is generated. 
///      e.g: given {1, 2, 5}, the list would be: {{+, +, +}, {+, +, -}, {+, +, *}, {+, +, /}, ..., {/, /, /}}.
///   2) a list of all possible orders of operation is generated ("braces")
///   3) each possible permutations of operations is applied to every order of operations, applied to each permutation of the input set. 
///      If the resulting expression is equal to 24, that expression is added to the set of solutions, otherwise it is discarded.
///   4) each solution is then displayed, along with the number of solutions and the amount of time it took the machine to calculate them.
///
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using input_type = double;
using input_collection_type = std::vector<input_type>;

/// \brief returns the set of solutions for Shusen's game for the given input set
///
std::vector<std::string> calculateSolutions(const input_type targetNumber, input_collection_type &&input)
{
    //
    // 0. Handle trivial cases
    //
    if (!input.size()) return {};
    else if (input.size() == 1)
    {
        if (const auto front = input.front(); front == targetNumber) return {std::string("{") + std::to_string(front) + "}\n"}; 
        else return {};
    }

    enum class Operation : decltype(input.size())
    {
        Addition,
        Subtraction,
        Multiplication,
        Division,
    };
    
    static constexpr decltype(input.size()) Operation_Count(4); // <! must be equal to the number of elements in Operation enum

    static constexpr auto Operation_ToString = [](const Operation o)
    {
        std::string output;

        switch(o)
        {
            case Operation::Addition: output = "+"; break; 
            case Operation::Subtraction: output = "-"; break;
            case Operation::Multiplication: output = "*"; break;
            case Operation::Division: output = "/"; break;

            default: throw std::runtime_error([&]()
            {
                std::stringstream ss;
                
                ss << "Operation_ToString: invalid operation: " << static_cast<std::underlying_type<Operation>::type>(o) << std::endl;

                return ss.str();
            }());
        }

        return output;
    };

    static constexpr auto Operation_PerformOperation = [](input_type l, const input_type r, const Operation o)     
    {
        switch(o)
        {
            case Operation::Addition: l += r; break;
            case Operation::Subtraction: l -= r; break;
            case Operation::Multiplication: l *= r; break;
            case Operation::Division: l /= r; break;

            default: throw std::runtime_error([&]()
            {
                std::stringstream ss;
                
                ss << "Operation_PerformOperation: invalid operation: " << static_cast<std::underlying_type<Operation>::type>(o) << std::endl;

                return ss.str();
            }());
        }
        
        return l;
    };
    
    const auto NUMBER_OF_OPERATIONS_IN_EXPRESSION(input.size() - 1);

    //
    // 1. Generate list of all possible operation configurations for an input of the given length
    //
    const std::vector<std::vector<Operation>> operation_permutations = [&NUMBER_OF_OPERATIONS_IN_EXPRESSION]()
    {
        std::remove_const<decltype(operation_permutations)>::type buffer;

        for (decltype(input.size()) i(0), s(static_cast<decltype(s)>(std::pow(Operation_Count, NUMBER_OF_OPERATIONS_IN_EXPRESSION))); i < s; ++i)
        {
            auto decimalValueBuffer = i;

            decltype(buffer)::value_type current_operations_permutation(NUMBER_OF_OPERATIONS_IN_EXPRESSION);

            for (decltype(i) j(0); decimalValueBuffer != 0; ++j)
            {
                const decltype(i) digit = decimalValueBuffer % Operation_Count;

                if (digit < 0 || digit > Operation_Count - 1) throw std::runtime_error([digit, i]()
                {
                    std::stringstream ss;

                    ss << "error: failed to convert decimal digit: " << i << " to base " << Operation_Count << " digit: " << digit;

                    return ss.str();
                }());

                current_operations_permutation[j] = static_cast<Operation>(digit);

                decimalValueBuffer /= Operation_Count;                
            }

            buffer.push_back(current_operations_permutation);
        }

        return buffer;
    }();
    
    //
    // 2. Generate a list of all possible order of operations given the length of this input
    //
    const std::vector<std::vector<int>> order_of_operation_permutations = [&NUMBER_OF_OPERATIONS_IN_EXPRESSION]() //TODO: why int
    {
        std::remove_const<decltype(order_of_operation_permutations)>::type buffer;

        decltype(order_of_operation_permutations)::value_type current_order_of_operations;

        current_order_of_operations.reserve(NUMBER_OF_OPERATIONS_IN_EXPRESSION);

        for (std::remove_const<decltype(NUMBER_OF_OPERATIONS_IN_EXPRESSION)>::type i = 0; i < NUMBER_OF_OPERATIONS_IN_EXPRESSION; ++i) 
        {
            current_order_of_operations.push_back(i);
        }

        std::sort(current_order_of_operations.begin(), current_order_of_operations.end()); //std::next_permutation requires sorted data

        do
        {
            buffer.push_back(current_order_of_operations);
        }
        while(std::next_permutation(current_order_of_operations.begin(), current_order_of_operations.end()));

        return buffer;
    }();

    //
    // 3. Apply all operation configurations to all orders of operations to all permutations of the input set. 
    // Record those expressions which equal targetNumber to the solutions array.
    //
    std::vector<std::string> solutions;

    std::sort(input.begin(), input.end()); //std::next_permutation requires sorted data

    do
    {
        const auto s(input.size());
        
        for (const auto &operations : operation_permutations)
        {
            for (auto &current_order_of_operations : order_of_operation_permutations)
            {
                auto input_copy = input;

                decltype(input.size()) i(0); 
                
                input_type buffer;

                std::stringstream ss;

                std::vector<std::pair<std::tuple<int, Operation, int>, std::vector<int>>> blar;

                std::make_signed<decltype(input_copy.size())>::type deletionOffset = 0;
                
                do
                {
                    auto order = current_order_of_operations[i] - deletionOffset;
                    
                    if (order < 0) order = 0;
                    else if (order >= input_copy.size() - 1) order = input_copy.size() - 1;
                    
                    ss << input_copy[order] << Operation_ToString(operations[i]) << input_copy[order + 1] << ": ";

                    buffer = Operation_PerformOperation(input_copy[order], input_copy[order + 1], operations[i]);

                    input_copy[order] = buffer;

                    input_copy.erase(input_copy.begin() + order + 1);

                    ss << "{";
                    
                    for (decltype(input_copy.size()) i = 0; i < input_copy.size(); ++i)
                    {
                        ss << input_copy[i]; 
                        
                        if (i != input_copy.size() - 1) ss << ", "; 
                    }

                    ss << "}\n";

                    deletionOffset++;
                    
                    ++i;
                }
                while(i < operations.size());

                if (input_copy.front() == targetNumber)
                {
                    std::stringstream ssOutput;

                    ssOutput << "{";

                    const decltype(input.size()) s = input.size();
                    
                    for (decltype(input.size()) i = 0; i < s; ++i)
                    {
                        ssOutput << input[i];

                        if (i != s - 1) ssOutput << ", "; 
                    }
                    
                    ssOutput << "}\n" << ss.str();

                    solutions.push_back(ssOutput.str());
                }
            }
        }
    } 
    while(std::next_permutation(input.begin(), input.end()));

    return solutions;
}

/// Program entry, input sanitization, output display
///
/// Shusen's set: 1, 5, 5, 5
/// Yuhao's set:  1, 2, 5, 6
///
int main(int argc, char **argv)
{
    try
    {
        const std::vector<std::string> parameters(argv + 1, argv + argc);

        const auto start_time(std::chrono::steady_clock::now());
        
        auto solutions = calculateSolutions(24, [&parameters]()
        {
            input_collection_type input;

            input.reserve(parameters.size());

            for (const auto &param : parameters) 
            {
                try
                {
                    input.push_back(std::stod(param));
                }
                catch (std::invalid_argument)
                {
                    std::cerr << "input contains invalid parameter: \"" << param << "\". All inputs must be integer or floating point numbers" << std::endl;

                    return decltype(input)();
                }
            }

            return input;
        }()); 

        const auto end_time(std::chrono::steady_clock::now());

        const auto size = solutions.size(); 

        for (auto solution : solutions) std::cout << solution << "==========" << std::endl;
        
        std::cout << (!size ? "No solution" : [&size]()
        { 
            std::stringstream ss; 

            ss << size << " solution" << (size > 1 ? "s" : "");

            return ss.str();
        }())
        << ", time taken " << [&end_time, &start_time]()
        {
            std::stringstream ss;
            
            auto buffer = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            
            if (buffer) ss << "(milliseconds): ";
            else
            {
                ss << "(microseconds): ";
                buffer = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
            }
            
            ss << buffer;
            
            return ss.str();
        }()
        << std::endl;
    }
    catch (std::runtime_error e)
    {
        std::cerr << "fatal error: " << e.what() << std::endl;
    }
    catch (std::exception)
    {
        std::cerr << "fatal error: something went terribly wrong." << std::endl;
    }

    return EXIT_SUCCESS;
}

