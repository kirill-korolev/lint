#include "lint.hpp"

const unsigned long lint::char_bits = 8;
const unsigned long lint::digit_bits = 4;
const unsigned long lint::digits_per_bin = sizeof(storage_type) * char_bits / digit_bits;
const unsigned long lint::max_per_digit = static_cast<unsigned long>(std::pow(2, digit_bits) - 1);

template <typename T>
unsigned long lint::count_digits(T value){
    if(value == 0){
        return 1;
    }
    unsigned long result = 0;
    while(value){
        value /= 10;
        ++result;
    }
    return result;
}

void lint::alloc(size_t digits) {
    bins_ = digits / digits_per_bin + 1;
    buf_ = new storage_type[bins_]();
}

size_t lint::bin_pos(size_t index) const {
    return digit_bits * (index % digits_per_bin);
}

void lint::set_digit(int digit, size_t index){
    size_t pos = bin_pos(index);
    buf_[index / digits_per_bin] &= ~(max_per_digit << pos);
    buf_[index / digits_per_bin] |= digit << bin_pos(index);
}

int lint::get_digit(size_t index) const {
    size_t pos = bin_pos(index);
    return (buf_[index / digits_per_bin] & (max_per_digit << pos)) >> pos;
}

int lint::sign() const {
    return get_digit(digits_);
}

lint::lint(): lint(0) {}

template <typename T>
lint::lint(T value){
    static_assert(std::is_integral<T>::value, "lint supports only integral types initialization");
    digits_ = count_digits(value);
    alloc(digits_);
    T abs_value = std::abs(value);

    for(size_t i = 0; i < digits_; ++i){
        set_digit(abs_value % 10, i);
        abs_value /= 10;
    }

    set_digit(value < 0, digits_);
}

lint::lint(const char* value) {

    if(!value || !*value){
        throw std::invalid_argument("lint: a string is empty");
    }

    const char* v = value;
    int is_negative = 0;

    if(*v == '-'){
        is_negative = 1;
        ++v;
    }

    while(*v){
        if(!std::isdigit(*v++)){
            throw std::invalid_argument("lint: cannot convert string to long integer");
        }
    }

    digits_ = v - value - is_negative;
    alloc(digits_);

    for(size_t i = 0; i < digits_; ++i){
        set_digit(*--v - '0', i);
    }

    set_digit(is_negative, digits_);
}

lint::lint(const lint& other) {
    digits_ = other.digits_;
    bins_ = other.bins_;
    std::copy(other.buf_, other.buf_ + bins_, buf_);
}

lint::~lint(){
    delete[] buf_;
}

lint& lint::operator=(const lint& other) {
    if(this != &other){
        delete[] buf_;
        digits_ = other.digits_;
        bins_ = other.bins_;
        std::copy(other.buf_, other.buf_ + bins_, buf_);
    }
    return *this;
}

lint& lint::operator+=(const lint& other){
    size_t digits = std::min(digits_, other.digits_);
    int carry = 0;

    //TODO: Negative values

    for(size_t i = 0; i < digits; ++i){
        int d = get_digit(i) + other.get_digit(i);
        set_digit((d % 10) + carry, i);
        carry = d / 10;
    }

    if(carry){
        if(digits < digits_){
            set_digit(get_digit(digits) + carry, digits);
        } else {
            //TODO: Realloc buffer
        }
    }

    return *this;
}

int lint::to_int() const {
    int result = 0;

    for(size_t i = digits_; i > 0; --i){
        result = 10 * result + get_digit(i - 1);
    }

    if(sign())
        return -result;
    return result;
}

lint::operator int() {
    return to_int();
}

std::ostream& operator<<(std::ostream& os, const lint& value){
    if(value.sign()){
        os << '-';
    }

    for(size_t i = value.digits_; i > 0; --i){
        os << value.get_digit(i - 1);
    }
    return os;
}