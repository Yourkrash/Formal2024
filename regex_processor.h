#include <iostream>
#include <string>
#include <stack>
#include <stdexcept>
#include <limits>
#include <cctype>
#include <unordered_set>

class RegexNode {
public:
    bool zero_included;
    int min_count;
    int max_count;
    int counts_mod;

    RegexNode(bool zero_included = false, int min_count = 0, int max_count = 0, int counts_mod = 0)
        : zero_included(zero_included), min_count(min_count), max_count(max_count), counts_mod(counts_mod) {}
};

int gcd(int a, int b) {
    if (b == 0)
        return a;
    return gcd(b, a % b);
}

class RegexProcessor {
public:
    RegexProcessor(const std::string& expression, char x)
        : expression(expression), x(x) {}

    RegexNode process() {
        std::stack<RegexNode> stack;
        for (size_t i = 0; i < expression.size(); ++i) {
            char token = expression[i];

            if (token == 'a' || token == 'b' || token == 'c' || token == '1') {
                RegexNode node = handleOperand(token);
                stack.push(node);
            } else if (token == '+' || token == '.' || token == '*') {
                if (token == '*') {
                    if (stack.empty()) {
                        throw std::invalid_argument("Error at position " + std::to_string(i) + ": '*' operator requires one operand.");
                    }
                    RegexNode operand = stack.top();
                    stack.pop();
                    RegexNode result = handleKleeneStar(operand);
                    stack.push(result);
                } else {
                    if (stack.size() < 2) {
                        throw std::invalid_argument("Error at position " + std::to_string(i) + ": Operator '" + token + "' requires two operands.");
                    }
                    RegexNode right = stack.top();
                    stack.pop();
                    RegexNode left = stack.top();
                    stack.pop();
                    RegexNode result = (token == '+') ? handleUnion(left, right) : handleConcatenation(left, right);
                    stack.push(result);
                }
            } else if (std::isspace(token)) {
                continue;
            } else {
                throw std::invalid_argument("Error at position " + std::to_string(i) + ": Unknown symbol '" + std::string(1, token) + "'.");
            }
        }

        if (stack.size() != 1) {
            throw std::invalid_argument("Error: Invalid regular expression. Stack size is not 1 after processing.");
        }

        return stack.top();
    }

private:
    std::string expression;
    char x;

    RegexNode handleOperand(char token) {
        if (token == '1') {
            return RegexNode(true, 0, 0, 0);
        } else if (token == x) {
            return RegexNode(false, 1, 1, 1);
        } else {
            return RegexNode(false, 0, 0, 0);
        }
    }

    RegexNode handleUnion(const RegexNode& left, const RegexNode& right) {
        int min_count = std::min(left.min_count, right.min_count);
        int max_count = std::max(left.max_count, right.max_count);
        bool zero_included = left.zero_included || right.zero_included;
        int counts_mod = gcd(left.counts_mod, right.counts_mod);
        return RegexNode(zero_included, min_count, max_count, counts_mod);
    }

    RegexNode handleConcatenation(const RegexNode& left, const RegexNode& right) {
        int min_count = left.min_count + right.min_count;
        int max_count = (left.max_count == std::numeric_limits<int>::max() || right.max_count == std::numeric_limits<int>::max())
                            ? std::numeric_limits<int>::max()
                            : left.max_count + right.max_count;
        bool zero_included = left.zero_included && right.zero_included;
        int counts_mod;
        if (left.counts_mod == 0 && right.counts_mod == 0) {
            counts_mod = 0;
        } else if (left.counts_mod == 0) {
            counts_mod = right.counts_mod;
        } else if (right.counts_mod == 0) {
            counts_mod = left.counts_mod;
        } else {
            counts_mod = gcd(left.counts_mod, right.counts_mod);
        }
        return RegexNode(zero_included, min_count, max_count, counts_mod);
    }

    RegexNode handleKleeneStar(const RegexNode& operand) {
        bool zero_included = true;
        int min_count = 0;
        int max_count = (operand.zero_included && operand.min_count == 0) ? 0 : std::numeric_limits<int>::max();
        int counts_mod = operand.zero_included ? 0 : operand.counts_mod;
        return RegexNode(zero_included, min_count, max_count, counts_mod);
    }
};

bool checkKInCounts(const RegexNode& node, int k) {
    if (k < node.min_count)
        return false;
    if (node.max_count != std::numeric_limits<int>::max() && k > node.max_count)
        return false;
    if (node.counts_mod == 0)
        return true;
    return (k - node.min_count) % node.counts_mod == 0;
}

bool isKOccurrencesPossible(const std::string& expression, char x, int k) {
    RegexProcessor processor(expression, x);
    RegexNode root = processor.process();
    return checkKInCounts(root, k);
}

#ifndef UNIT_TEST
int main() {
    std::string expression;
    char x;
    int k;
    try {
        std::getline(std::cin, expression, ' ');
        std::cin >> x >> k;
        bool result = isKOccurrencesPossible(expression, x, k);
        std::cout << (result ? "Yes" : "No") << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
#endif