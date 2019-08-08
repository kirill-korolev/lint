#ifndef LINT_H
#define LINT_H

#include <string>
#include <cmath>

class lint {
public:
    typedef unsigned int storage_type;

    lint();
    template <typename T>
    lint(T value);
    lint(const char* value);
    lint(const lint& other);
    ~lint();

    lint& operator=(const lint& other);
    lint& operator+=(const lint& other);

    int to_int() const;
    operator int();

    friend std::ostream& operator<<(std::ostream& os, const lint& value);
private:

    template <typename T>
    static unsigned long count_digits(T value);

    void alloc(size_t digits);
    size_t bin_pos(size_t index) const;
    void set_digit(int digit, size_t index);
    int get_digit(size_t index) const;
    int sign() const;
private:
    static const unsigned long char_bits; // Byte size in bits
    static const unsigned long digit_bits; // Bits for storing a digit [0-9]
    static const unsigned long digits_per_bin; // Number of digits in storage_type
    static const unsigned long max_per_digit; // Maximum value of a digit chunk
private:
    storage_type* buf_;
    size_t        bins_;
    size_t        digits_;
};

#endif //LINT_H
