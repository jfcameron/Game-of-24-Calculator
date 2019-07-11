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
///     the target number "24" can be substituted for any othe real number.
///     the user can be given any number of floating point numbers, they are not limited to 4
///
/// How the calculator works:
///   1) a list of all possible permutations of operations for the given set of numbers is generated. 
///      e.g: given {1, 2, 5}, the list would be: {{+, +, +}, {+, +, -}, {+, +, *}, {+, +, /}, ..., {/, /, /}}.
///   2) each possible permutations of operations is then applied to each permutation of the input set. If the resulting expression is equal to 24 or -24, 
///      that expression is added to the set of solutions, otherwise it is discarded.
///   3) each solution is then displayed, along with the number of solutions and the amount of time it took the machine to calculate them.
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
/// \warn the output does not respect BEDMAS; order of operations is simply left to right.
///
std::vector<std::string> calculateSolutions(input_collection_type &&input)
{
    static constexpr input_type TARGET_NUMBER(24); // <! the value an expression must be equal to for it to be considered a solution.
    
    if (input.size() == 1)
    {
        if (input.front() == TARGET_NUMBER) return {std::to_string(input.front() == TARGET_NUMBER)}; 
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

    static constexpr auto operationToString = [](const Operation o)
    {
        std::string output;

        switch(o)
        {
            case Operation::Addition: output = "+"; break; 
            case Operation::Subtraction: output = "-"; break;
            case Operation::Multiplication: output = "*"; break;
            case Operation::Division: output = "/"; break;

            default: throw std::runtime_error("operationToString: invalid operation type!"); break;
        }

        return output;
    };
    
    //
    // 1. Generate list of all possible configurations of operations 
    //  
    std::vector<std::vector<Operation>> operation_permutations;

    const auto NUMBER_OF_OPERATIONS_IN_EXPRESSION(input.size() - 1);

    for (decltype(input.size()) i(0), s(static_cast<decltype(s)>(std::pow(Operation_Count, NUMBER_OF_OPERATIONS_IN_EXPRESSION))); i < s; ++i)
    {
        auto decimalValueBuffer = i;

        decltype(operation_permutations)::value_type current_operations_permutation(NUMBER_OF_OPERATIONS_IN_EXPRESSION);

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

        operation_permutations.push_back(current_operations_permutation);
    }
   
    //
    // 1.b hmm
    //
    const std::vector<std::vector<int>> order_of_operation_permutations = [&NUMBER_OF_OPERATIONS_IN_EXPRESSION]()
    {
        std::remove_const<decltype(order_of_operation_permutations)>::type buffer;

        std::vector<int> current_order_of_operations;

        current_order_of_operations.reserve(NUMBER_OF_OPERATIONS_IN_EXPRESSION);

        for (std::remove_const<decltype(NUMBER_OF_OPERATIONS_IN_EXPRESSION)>::type i = 0; i < NUMBER_OF_OPERATIONS_IN_EXPRESSION; ++i) current_order_of_operations.push_back(i);

        std::sort(current_order_of_operations.begin(), current_order_of_operations.end());

        do
        {
            buffer.push_back(current_order_of_operations);
        }
        while(std::next_permutation(current_order_of_operations.begin(), current_order_of_operations.end()));

        return buffer;
    }();

    /*for (auto &current_order_of_operations : order_of_operation_permutations)
    {
        std::stringstream ss;

        for (auto blar : current_order_of_operations) ss << blar << " ";

        std::cout << ss.str() << std::endl;
    }*/

    //operation_order_permutations.reserve(NUMBER_OF_OPERATIONS_IN_EXPRESSION);

    //
    // 2. Apply all operation configurations to all permutations of the input set. Record those expressions which equal TARGET_VALUE to the solutions array.
    //
    std::vector<std::string> solutions;

    std::sort(input.begin(), input.end()); // Initial sort must be done in order to guarantee we visit all permutations of input using std::next_permutation in the below loop.

    do // For all number permutations
    {
        const auto s(input.size());
        
        for (const auto &operations : operation_permutations) // for all operation perms
        {
//            input_type buffer = *input.begin();
//
//            for (decltype(input.size()) i(1), j(0); i < s; ++i, ++j)
//            {
//                switch(operations[j])
//                {
//                    case Operation::Addition: buffer += input[i]; break;
//                    case Operation::Subtraction: buffer -= input[i]; break;
//                    case Operation::Multiplication: buffer *= input[i]; break;
//                    case Operation::Division: buffer /= input[i]; break;
//
//                    default: throw std::runtime_error("invalid operation!");
//                }
//            }
//
//            if (/*std::abs*/(buffer) == TARGET_NUMBER)  // I was wrong. must be positive 24
//            {
//                std::stringstream ss;
//                
//                for (decltype(input.size()) i(0); i < s - 1; ++i)
//                {  
//                    ss << input[i] << operationToString(operations[i]);
//                }
//
//                ss << input.back() << " = " << buffer;
//
//                solutions.push_back(ss.str());
//            }

            for (auto &current_order_of_operations : order_of_operation_permutations) // for all orders
            {
                static constexpr auto applyOperation = [](input_type l, const input_type r, const Operation o)
                {
                    //std::cout << l << operationToString(o) << r << std::endl;
                    
                    switch(o)
                    {
                        case Operation::Addition:       l += r; break;
                        case Operation::Subtraction:    l -= r; break;
                        case Operation::Multiplication: l *= r; break;
                        case Operation::Division:       l /= r; break;

                        default: throw std::runtime_error([&]()
                        {
                            std::stringstream ss;
                            
                            ss << "invalid operation: " << static_cast<unsigned int>(o) << ", l: " << l << ", r: " << r << std::endl;

                            return ss.str();
                        }());
                    }
                    
                    return l;
                };

                /*if (true
                    && input[0] == 5
                    && input[1] == 5
                    && input[2] == 1
                    && input[3] == 5
                    && operations[0] == Operation::Division
                    && operations[1] == Operation::Subtraction
                    && operations[2] == Operation::Multiplication
                    && current_order_of_operations[0] == 2
                    && current_order_of_operations[1] == 1
                    && current_order_of_operations[2] == 0) */
                {
                    //construct the braced structure
                    //evaluate the structure
                    
                    auto input_copy = input;
                    
                    //int output;

                    {
                        decltype(input.size()) i(0); 
                        
                        input_type buffer;
                    
                        //std::set<int> used;

                        std::stringstream ss;

                        int deletionOffset = 0;
                        
                        do
                        {
                            //std::cout << ""; for (const auto a : input_copy) std::cout << a << ", "; std::cout << "\n";

                            auto order = current_order_of_operations[i] - deletionOffset;
                            
                            if (order < 0) order = 0;
                            if (order >= input_copy.size() -1) order = input_copy.size() - 1;
                            
                            ss << input_copy[order] << operationToString(operations[i]) << input_copy[order + 1] << ": ";

                            buffer = applyOperation(input_copy[order], input_copy[order + 1], operations[i]);

                            input_copy[order] = buffer;

                            input_copy.erase(input_copy.begin() + order + 1);

                            for (auto a : input_copy) ss << a << ", "; ss << "\n";

                            deletionOffset++;
                            
                            ++i;
                            
                        }
                        while(i < operations.size());

                        //std::cout << ""; for (const auto a : input_copy) std::cout << a << ", "; std::cout << "\n";

                        if (/*buffer*/ input_copy.front() == 24)
                        {
                            for (const auto a : input) std::cout << a << ", "; std::cout << "\n";
                            //for (const auto a : input_copy) std::cout << a << ", "; std::cout << "\n==\n";
                            std::cout << ss.str() <<  "result: " << buffer << "\n======\n";

                            solutions.push_back("blimblam");
                        }


                        //output = buffer;
                    }

                    //for (decltype(input.size()) i(1); i < s - 1; ++i)
                    
                    //yes!
                    /*if (buffer == TARGET_NUMBER)
                    {
                        solutions.push_back("zipzap");

                        std::stringstream ss;

                        ss << "input: "; for (const auto &i : input) ss << i << ", "; ss << "\n"; 
                        ss << "operations: "; for (const auto &i : operations) ss << operationToString(i) << ", "; ss << "\n";
                        ss << "order of operations: "; for (const auto &i : current_order_of_operations) ss << i << ", "; ss << "\n";

                        std::cout << ss.str();
                    }*/
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
    if (argc <= 1)
    {
        std::cout << *argv << "-=- requires at least one number to work with!\n";

        return EXIT_FAILURE;
    }
    else
    {
        const std::vector parameters(argv + 1, argv + argc);

        const auto start_time(std::chrono::steady_clock::now());
        
        auto solutions = calculateSolutions([&parameters]()
        {
            input_collection_type input;

            input.reserve(parameters.size());

            for (const auto &param : parameters) input.push_back(std::stod(param));

            return input;
        }()); 

        const auto end_time(std::chrono::steady_clock::now());

        const auto size = solutions.size(); 
    
        //for (auto solution : solutions) std::cout << solution << std::endl;
        
        std::cout << "-=- " << (!size ? "No solution" : [&size]()
        { 
            std::stringstream ss; 

            ss << size << " solution" << (size > 1 ? "s" : "");

            return ss.str();
        }())
        << ", time taken " << [&end_time, &start_time]()
        {
            std::stringstream ss;
            
            auto buffer = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            
            if (buffer) ss << "(miliseconds): ";
            else if ((buffer = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count())) ss << "(microseconds): ";
            else
            {
                ss << "(nanoseconds): ";
                buffer = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
            }
            
            ss << buffer;
            
            return ss.str();
        }()
        << " -=-" << std::endl;
    }

    return EXIT_SUCCESS;
}

