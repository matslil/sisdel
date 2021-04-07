/*

  Parsing Sisdel language elements.
  This file declares the objects used to give tokens structure. They are then
  used to provide byte code.

  Copyright (C) 2013-2017 Mats G. Liljegren
  SPDX-License-Identifier: Apache-2.0

  This file is part of Sisdel.

  Licensed under the Apache License, Version 2.0 (the "License"); you may not
  use this file except in compliance with the License. You may obtain a copy
  of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
  License for the specific language governing permissions and limitations
  under the License.
*/

#ifndef PARSER_ELEMENTS_HH
#define PARSER_ELEMENTS_HH

#include "__cplusplus.h"

#include "file.h"
#include "sbucket.h"

namespace sisdel {

	//
	// This is the base type for all Sisdel language elements.
	//
	
	struct data_t {
	public:
		data_t(const token_t& token);
		virtual ~data() noexcept {}
		
		string_idx_t name(void) const noexcept
			{ return typeid(*this).name(); }
		const type_t& type(void) const noexcept
			{ return *m_type; }
		void type(std::shared_ptr<type_t> type)
			{ m_type = type; }
		const position_t& declared_at(void) const noexcept
			{ return m_declared_at; }

		// Check if constraints are fullfilled given thread scope
		bool is_valid(const scope_t& thread_scope) const
			{
				for (const constraint_expression_t ce : m_constraints)
					if (!ce.is_valid(thread_scope))
						return false;
				return true;
			}
					

		// Calculate hash. Uses member function hash_next().
		// Inheriting classes needs to override this method, and
		// use hash_next() below for calculating the contents for
		// this class.
		virtual hash_t hash(void) const noexcept
			{
				return hash_finish(hash_next(0));
			}

	protected:
		// Calculate hash, used by hash() function.
		// For inheriting classes, this function is used to include
		// member variables for this class.
		virtual hash_t hash_next(hash_t hash) const noexcept
			{
				hash = hash_next(m_declared_at.hash(), hash);
				hash = hash_next(m_type.hash(), hash);
			}
		
		// NOTE: Remember to updated hash_next() function above if
		//       there are any changes to member variables!
		const position_t m_declared_at;

		// NOTE: If this object is a type, m_type will refer to
		//       itself. FIXME: Is this a problem?
		std::shared_ptr<type_t> m_type;

		std::vector<constraint_expression_t> m_constraints;
	};

	struct type_constraint_t : public data_t {
	public:
		type_constraint_t(const token_string_t& token);
		constexpr string_idx_t name(void) const noexcept
			{ return m_name; }

	private:
		const string_idx_t m_name;
	};

	struct unit_t : public data_t {
	public:
		virtual ~unit_t() {}
		virtual bool compatible_with(const unit_t& unit) const = 0;
	};

	struct unit_specifier_t : public data_t {
	public:
		unit_t(string_idx_t name);
		bool compatible_with(const unit_t& unit) const;

	private:
		const string_idx_t m_name;
	};

	struct unit_expression_t : public data_t {
	public:
		unit_t(std::shared_ptr<data_t> expression);
		bool compatible_with(const unit_t& unit) const;

	private:
		shared_ptr<data_t> m_expression;
	};
	
	//
	// Describes a Sisdel type. Note that a type object might refer to
	// type itself, since types are first class citizens in the Sisdel
	// language.
	//
	
	// Remember:
	// - Add constraint iterator
	struct type_t : public data_t {
	public:
		type_t(const token_identifier_t& token,
		       const type_info& base_type);
		bool compatible_with(const type_t& rhs) const noexcept
			= 0;
		void add_constraint(
			std::shared_ptr<constraint_t> constraint) = 0;
		const unit_t& unit(void) const noexcept = 0;
		shared_ptr<unit_t> unit(void) noexcept = 0;
		void unit(shared_ptr<unit_t> new_unit) noexcept = 0;

		// From data_t

		hash_t hash(void) const noexcept = 0;

	private:
		const type_info m_base_type;
		std::vector<type_constraint_t> m_type_constraints;
	};

	// Scope is either a lexical scope, unit scope or a thread scope. The
	// thread scope is reached from lexical scope by referring to the
	// identifier name "thread".
	struct scope_t : public data_t {
	public:
		scope_t(const position_t& position,
			std::shared_ptr<scope_t> parent);

		// From data_t

		hash_t hash(void) const noexcept;

		// Unique members

		const data_t& operator[](string_idx_t idx) const;
		shared_ptr<data_t> operator[](string_idx_t idx);
		std::shared_ptr<scope_t> parent(void) const noexcept
			{ return m_parent; }

	private:
		const position_t m_position;
		const std::shared_ptr<scope_t> m_parent;
		std::map<string_idx_t, shared_ptr<data_t> > m_symbols;
	};

	struct operator_t : public data_t {
	public:
		opeator_t(const position_t& position,
			  std::shared_ptr<scope_t> scope,
			  std::shared_ptr<data_t> code);
		std::shared_ptr<data_t> run(const scope_t& thread_scope,
					    std::shared_ptr<data_t> lhs,
					    std::shared_ptr<data_t> rhs);

	struct real_number_t : public data_t {
	public:
		real_number_t(const position_t& position,
			      const token_float_t& token);

	private:
		const mp_float m_value;
	};

	struct integer_number_t : public data_t {
	public:
		integer_number_t(const position_t& position,
				 const token_integer_t& token);

	private:
		const mp_int m_value;
	};

	struct boolean_t : public data_t {
	public:
		boolean_t(const position_t& position,
			  boolean b);

	private:
		const boolean m_boolean;
	};

	struct string_t : public data_t {
	public:
		string_t(const position_t& position,
			 string_idx_t str);

	private:
		const string_idx_t m_string;
	};

	struct map_t : public data_t {
	public:
		map_t(const position_t& position);
		add(std::shared_ptr<data_t> key,
		    std::shared_ptr<data_t> value);

	private:
		std::map<std::shared_ptr<data_t>, std::shared_ptr<data_t> > m_map;
	};

	// Expression describes how a number of data objects can calculate
	// a new data object.
	//
	// Expression is a tree of expressions with functions or immediate
	// data as its leafs. Each expression has two children, lhs and rhs
	// (left hand side and right hand side).
	struct expression_t : public data_t {
	public:
		expression_t(const position_t& position,
			     shared_ptr<data_t> lhs,
			     shared_ptr<operator_t> operator,
			     shared_ptr<data_t> rhs);
		
		// Calculate value of expression, and return the result
		shared_ptr<data_t> value(void);

	private:
		shared_ptr<data_t> m_lhs;
		shared_ptr<operator_t> m_operator;
		shared_ptr<data_t> m_rhs;
	};

	struct constraint_expression_t : public data_t {
	public:
		constraint_expression_t(const position_t& position,
					std::shared_ptr<data_t> lhs,
					std::shared_ptr<operator_t> operator,
					std::shared_ptr<data_t> rhs);
		bool is_valid(const scope_t& thread_scope);

	private:
		std::shared_ptr<data_t> m_lhs;
		std::shared_ptr<operator_t> m_operator;
		std::shared_ptr<data_t> m_rhs;
	};

	private:
		std::shared_ptr<scope_t> m_scope;
		std::shared_ptr<data_t> m_code;
	};
	
	struct block_t : public data_t {
	public:
		block_t(const position_t& position);
		void append(std::shared_ptr<data_t> data);

	private:
		std::vector<std::shared_ptr<data_t> > m_entries;
	};
	
};

#endif /* PARSER_ELEMENTS_HH */
