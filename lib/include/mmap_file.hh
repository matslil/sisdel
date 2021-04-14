/*
  SPDX-License-Identifier: MIT

*/

#ifndef MMAP_FILE_H
#define MMAP_FILE_H

/**
 * @file
 * Memory mapped file for the token parser.
 * @todo The public part of the class interface should be made into a base
 *       class which mmap_file_t then inherits from. Then a cstream-class
 *       could be implemented to allow really big files and piping from
 *       cin.
 */

#include "file.hh"
#include "sbucket.hh"
#include "environment.hh"
#include "error.hh"
#include "position.hh"

class position_t;

/**
 * Read a memory mapped file.
 * The file reader only supports streamed reading to make it possible to
 * implement true stream reading file reader in the future.
 * Memory mapped file is used for sake of performance, but the tradeoff is
 * harder to support large files and lack of possibility to read from
 * cin.
 * @todo Should use Unicode characters rather than bytes.
 */
class mmap_file_t {
public:
	/**
	 * Constructor.
	 */
	mmap_file_t(
		environment_t &env, /**< [in] Environment object containing
				     * string bucket where to store the file
				     * name. */
		const char *name    /**< [in] Path to file to read. This name
				     * will be stored in the string bucket in
				     * the environment object. */
		);

	/**
	 * Move assignment operator.
	 */
	mmap_file_t& operator=(
		mmap_file_t &&from /**< [in] Object to transfer information
				    * from. */
		);

	/**
	 * Peek at current character.
	 * @returns Current character, or '\0' if end of file.
	 * @todo Should return a Unicode 32-bit character, uchar32_t.
	 */
	constexpr char peek(void) const noexcept
		{ return eof() ? '\0' : *m_buff; }

	/**
	 * Return end of file status.
	 * @returns true if no more characters can be read, false otherwise.
	 */
	constexpr bool eof(void) const noexcept
		{ return m_buff >= m_end; }

	/**
	 * Skip until a non-matching character.
	 * Skip will stop when either a character not matching skip_ch
	 * is encountered, or until end of file.
	 * @returns Number of bytes skipped.
	 * @todo skip_ch should be of type uchar32_t.
	 */
	size_t skip(
		char skip_ch /**< [in] Character to be skipped. */
		);

	/**
	 * Skip until any character not in given string.
	 * Skip will stop when either a character not matching any characters
	 * in skip_str is encountered, or until end of file.
	 * @returns Number of bytes skipped.
	 * @todo skip_str should be of type uchar32_t*.
	 */
	size_t skip(
		const char *skip_str /**< [in] Array of characters to be
				      * skipped. */
		);

	/**
	 * Skip until matching character.
	 * Skip will stop when either a character matching until_ch is
	 * encountered, or until end of file.
	 * @todo until_ch should be of type uchar32_t.
	 */
	void skip_until(
		char until_ch /**< [in] Character to find. */
		);

	/**
	 * Skip until matching character, calculate hash for skipped characters.
	 * @returns Number of bytes skipped.
	 * @todo Return string_idx_t of string being skipped. Remove hash
	 *       parameter and marker_start() and marker_end(). This will make
	 *       a stream implementation easier.
	 * @todo unil_ch should be of type uchar32_t.
	 */
	size_t skip_until_hashed(
		char until_ch, /**< [in] Character to find. */
		hash_t& hash   /**< [out] Hash of skipped characters. */
		);

	/**
	 * Skip until matching any character in given string, calculate hash
	 * for skipped characters.
	 * @returns Number of bytes skipped.
	 * @todo Return string_idx_t of string being skipped. Remove hash
	 *       parameter and marker_start() and marker_end(). This will make
	 *       a stream implementation easier.
	 * @todo until_str should be of type uchar32_t*.
	 */
	size_t skip_until_hashed(
		const char* until_str, /**< [in] Array of characters to be
					* found. */
		hash_t& hash           /**< [out] Hash of skipped characters. */
		);

	/**
	 * Skip a single character.
	 */
	void skip(void);

	/**
	 * Get current file position.
	 * @returns Copy of a position_t object containing the current file
	 *          position.
	 * @seealso position_t
	 */
	const position_t get_position(void) const noexcept;

	/**
	 * Get name of input file.
	 * @returns Pointer to a C string which is the name of the input file,
	 *          including path. This pointer is valid as long as the
	 *          environment object given in the construtor is valid.
	 * @todo Should return string_idx_t, when environment object has
	 *       become thread local.
	 */
	constexpr const char * filename(void) const
		{ return m_env.sbucket()[m_filename]; }

	/**
	 * Set starting position for the marker.
	 * @seealso marker_end
	 * @todo Remove once skip_until_hashed() has been re-designed.
	 */
	void marker_start(void) { m_marker_start = m_buff; }

	/**
	 * Creating string based on marker selection.
	 * @note Must have used marker_start() to start the selection.
	 * @returns String with characters starting at position when
	 *          marker_start() was called, and ending at current
	 *          position.
	 * @seealso marker_start
	 * @todo Remove once skip_until_hashed() has been re-designed.
	 */
	std::string marker_end(void);

private:
	
	// Forbidden methods

	// Need parameters to construct this class.
	mmap_file_t() = delete;

	// Having multiple references to the same file is not supported.
	mmap_file_t(const mmap_file_t &) = delete;

	// Having multiple references to the same file is not supported.
	mmap_file_t& operator=(const mmap_file_t &) = delete;
	
	// Class to wrap map()/munmap() to make deallocation work with
	// exceptions
	class mmap_t {
	public:
		mmap_t(const char *name);
		~mmap_t();
		constexpr const char *map(void) const noexcept
			{ return m_map; }
		constexpr size_t file_size(void) const noexcept
			{ return m_file.size(); }

		// Forbidden methods
		mmap_t() = delete;
		mmap_t(const mmap_t&) = delete;
		mmap_t& operator=(const mmap_t&) = delete;

	private:
		const file_t m_file;
		const char * const m_map;
	};

	// Environment reference as given by constructor.
	environment_t& m_env;

	// Memory mapped file.
	mmap_t m_map;

	// Current file position.
	const char * m_buff;

	// Start of current line.
	const char * m_start;

	// First character past end of file.
	const char * const m_end;

	// Column count.
	size_t m_col;

	// Line count.
	size_t m_line;

	// File name, index to string bucket.
	const string_idx_t m_filename;

	// File position for last call to marker_start().
	const char * m_marker_start;
};


#endif // MMAP_FILE_H
