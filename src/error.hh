#include <iostream>
#include <system_error>

namespace error {

	// Sisdel error codes
	enum error_code_t {
		syntax_error = 1
	};

	// Sisdel error conditions
	enum error_condition_t {
		parser_error = 1
	};

	std::error_code make_error_code(error_code_t err);
	std::error_condition make_error_condition(error_condition_t econd);
}

class error_category_impl : public std::error_category {
public:
	virtual const char *name() const;
	virtual std::string message(int ev) const;
	virtual std::error_condition default_error_condition(int ev) const;
};

const std::error_category& error_category();

namespace std {
	template <> struct is_error_code_enum<error::error_code_t>
		: public true_type {};

	template <> struct is_error_condition_enum<error::error_condition_t>
		: public true_type {};
}

