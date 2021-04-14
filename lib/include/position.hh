/*
  SPDX-License-Identifier: MIT

*/

#ifndef POSITION_HH
#define POSITION_HH

/**
 * @file
 * Declare position_t class.
 */

#include "environment.hh"
#include "sbucket.hh"

class mmap_file_t;

/**
 * Input file position.
 * This class is intended to give position information for diagnostic
 * messages, e.g. when the scanner or parser encounters an error.
 */
class position_t {
public:
	/**
	 * Constructor for position_t.
	 * NOTE: Is is the caller's responsibility to make sure that the
	 *       object given by file argument is not destructed before this
	 *       class is.
	 */
	position_t(std::string& str,         /**< The whole input line where
					      * the position can be found. */
		   const mmap_file_t * file, /**< Object used for file input.
					      * @todo Make environment object
					      * thread local, so this parameter
					      * does not need to be given.
					      * It should be replaced with a
					      * string_idx_t so that there is
					      * no risk pointing at a destructed
					      * file object. When done, remove
					      * operator== since the class
					      * then becomes trivial. */
		   size_t line, /**< Line number in the input file that str
				 * parameter is referring to, starting from 1.
				*/
		   size_t col   /**< Column in str where the position is at,
				 * starting from 1. */
		)
		: m_str(str), m_file(file), m_line(line), m_col(col) {}

	/**
	 * Comparison operator.
	 * Compares two positions.
	 * @returns true if both positions are identical, false otherwise.
	 */
	bool operator==(
		const position_t& rhs /**< The object on the right hand side
				       * of the assignment. */
		) const noexcept
		{
			return (m_str == rhs.m_str) &&
				(m_file == rhs.m_file) &&
				(m_line == rhs.m_line) &&
				(m_col == rhs.m_col);
		}

	/**
	 * Return the code line containing the file position.
	 * The string is the line of code representing the file position.
	 * @returns One line of code without the line-feed character.
	 */
	constexpr const std::string& str(void) const noexcept { return m_str; }

	/**
	 * Return the column where the position is at.
	 * This is a byte index starting from 1, i.e. it does not consider
	 * that Unicode characters can consist of several bytes.
	 * @returns Column byte offset into the input line, starting from 1.
	 */
	constexpr size_t column(void) const noexcept
		{ return m_col; }

	/**
	 * Return the line containing the file position.
	 * The line count starts at 1. This is the line number in the input
	 * file representing this position.
	 * @returns Line number of the input line, starting from 1.
	 */
	constexpr size_t line(void) const noexcept
		{ return m_line; }

	/**
	 * Return file input reader that produced this position.
	 */
	constexpr const mmap_file_t& file(void) const noexcept
		{ return *m_file; }
	
private:
	const std::string m_str;
	const mmap_file_t * const m_file;
	const size_t m_line;
	const size_t m_col;
};

/**
 * Output file position as I/O stream.
 * Output format: file_name:line:column
 * @returns ostream object appended with position information.
 */
std::ostream &operator<<(
	std::ostream &os,   /**< ostream object to be appended. */
	position_t const &m /**< Position object to be printed. */
	);

#endif /* POSITION_HH */
