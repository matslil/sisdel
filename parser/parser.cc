#include <boost/multiprecision/gmp.hpp>

using namespace boost::multiprecision;

// Parser for integer
template< typename Iterator >
bool parse_integer( Iterator first, Iterator last, mpz_int value )
{
    using qi::phrase_parse;
    using qi::_1;
    using ascii::space;

    bool result = phrase_parse( first, last,
            {
                -char_('-')
}

