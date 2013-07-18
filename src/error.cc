#include "error.hh"

namespace error {
	std::error_code make_error_code(error_code_t err) {
		return std::error_code(static_cast<int>(err), error_category());
	}

	std::error_condition make_error_condition(error_condition_t econd) {
		return std::error_condition(static_cast<int>(econd), error_category());
	}
}

const char *error_category_impl::name() const {
	return "sisdel";
}

std::string error_category_impl::message(int ev) const {
	switch (ev) {
	case error::syntax_error: return "Syntax error";
	case error::unexpected_eof: return "Unexpected end of file";
	case error::overflow_error: return "Overflow";
	case error::illegal_character: return "Illegal character";
	case error::internal_error: return "Internal error";
	}

	// Should never end up here, this is just to keep compiler happy
	return "Unknown error";
}

std::error_condition error_category_impl::default_error_condition(int ev) const {
	switch (ev) {
		// Parser error
	case error::syntax_error:
		return std::error_condition(error::parser_error, *this);
	case error::unexpected_eof:
		return std::error_condition(error::parser_error, *this);
	case error::overflow_error:
		return std::error_condition(error::parser_error, *this);
	case error::illegal_character:
		return std::error_condition(error::parser_error, *this);

		// Implementation error
	case error::internal_error:
		return std::error_condition(error::implementation_error, *this);
	}

	return std::error_condition(ev, *this);
}

const std::error_category& error_category() {
	static error_category_impl instance;
	return instance;
}

#ifdef UNIT_TEST

#include <cassert>

int main(int argc, char *argv[]) {
	std::error_code ec = error::syntax_error;

	assert(ec == error::parser_error);
	assert(ec == error::syntax_error);
	assert(ec.category().name() == "sisdel");
	assert(ec.message() == "Syntax error");

	return 0;
}


#endif // UNIT_TEST
