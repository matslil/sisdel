#include "error.h"

struct parser_error : public exception {
	lexer_error(const Environment& env, sbucket_idx_t file, size_t line, size_t col, size_t start_col, const std::string code, const std::string msg)
		: m_env(env), m_file(file),
		  m_start_col(col),
		  m_line(line), m_col(col), m_start_line(line),
		  m_code(code), m_msg(msg) {}
	const Environment m_env;
	const sbucket_idx_t m_file;
	const size_t m_line;
	const size_t m_col;
	const size_t m_start_col;
	const std::string m_code;
	const std::string m_msg;

	const char *what() const throw () {
		std::stringstream ss;
		ss << m_env.sbucket()[m_file] << ':' << m_line << ':' << ':' << m_col << ': error: ' << m_msg << std::endl << "        " << m_code;
		return ss.str().c_str();
	}

namespace error {
	std::error_code make_error_code(error_code_t err) {
		return std::error_code(static_cast<int>(err), error_category());
	}

	std::error_condition make_error_condition(error_condition_t econd) {
		return std::error_condition(static_cast<int>(econd), error_category());
	}
};

const char *error_category_impl::name() const {
	return "sisdel";
}

std::string error_category_impl::message(int ev) const {
	switch (ev) {
	case error::syntax_error: return "Syntax error";
	default: return "Unknown error";
	}
}

std::error_condition error_category_impl::default_error_condition(int ev) const {
	switch (ev) {
	case error::syntax_error:
		return std::error_condition(error::parser_error, *this);
	default:
		return std::error_condition(ev, *this);
	}
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
