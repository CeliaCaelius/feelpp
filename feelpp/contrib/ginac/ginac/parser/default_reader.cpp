/** @file default_reader.cpp
 *
 *  Implementation of the default and builtin readers (part of GiNaC's parser).
 **/

/*
 *  GiNaC Copyright (C) 1999-2016 Johannes Gutenberg University Mainz, Germany
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <cmath>
#include "parse_context.h"
#include "power.h"
#include "lst.h"
#include "operators.h"
#include "inifcns.h"
#include "function.h"
#include <cstdint> // for uintptr_t

namespace GiNaC
{
DECLARE_FUNCTION_2P(mod)
static ex mod_eval(const ex & x, const ex& y ) 
{ 
	if (is_exactly_a<numeric>(y) && is_exactly_a<numeric>(x))
		return mod(ex_to<numeric>(x), ex_to<numeric>(y));
	
	return mod(x, y).hold();
}
static ex mod_evalf(const ex & x, const ex& y ) 
{ 
	if (is_exactly_a<numeric>(y) && is_exactly_a<numeric>(x))
		return mod(ex_to<numeric>(x), ex_to<numeric>(y));
	
	return mod(x, y).hold();	
}
                                                                                
static void mod_print_latex(const ex & arg1, const ex&  arg2,const print_context & c)
{
    c.s << "{"; arg1.print(c); c.s << "\%"; arg2.print(c); c.s << "|}";
}
                                                                                
static void mod_print_csrc_float(const ex & arg1, const ex & arg2, const print_context & c)
{
    c.s << "fmod("; arg1.print(c); c.s << ","; arg2.print(c); c.s << ")";
}

                                                                                
REGISTER_FUNCTION(mod, eval_func(mod_eval).
                       evalf_func(mod_evalf).
                       print_func<print_latex>(mod_print_latex).
                       print_func<print_csrc_float>(mod_print_csrc_float).
                       print_func<print_csrc_double>(mod_print_csrc_float));



static ex mod_reader(const exvector& ev)
{
	return GiNaC::mod(ev[0],ev[1]);
}

static ex sqrt_reader(const exvector& ev)
{
	return GiNaC::sqrt(ev[0]);
}

static ex pow_reader(const exvector& ev)
{
	return GiNaC::pow(ev[0], ev[1]);
}
static ex min_reader(const exvector& ev)
{
	return (ev[0]+ev[1])/2-GiNaC::abs(ev[0]-ev[1])/2;//GiNaC::min(ev[0], ev[1]);
}
static ex max_reader(const exvector& ev)
{
	return (ev[0]+ev[1])/2+GiNaC::abs(ev[0]-ev[1])/2; //GiNaC::max(ev[0], ev[1]);
}

static ex power_reader(const exvector& ev)
{
	return GiNaC::power(ev[0], ev[1]);
}

static ex lst_reader(const exvector& ev)
{
	return GiNaC::lst(ev.begin(), ev.end());
}


// function::registered_functions() is protected, but we need to access it
// TODO: add a proper const method to the `function' class, so we don't
// need this silly hack any more.
class registered_functions_hack : public function
{
public:
	static const std::vector<function_options>& get_registered_functions()
	{
		return function::registered_functions();
	}
private:
	registered_functions_hack();
	registered_functions_hack(const registered_functions_hack&);
	registered_functions_hack& operator=(const registered_functions_hack&);
};

// Encode an integer into a pointer to a function. Since functions
// are aligned (the minimal alignment depends on CPU architecture)
// we can distinguish between pointers and integers.
static reader_func encode_serial_as_reader_func(unsigned serial)
{
	uintptr_t u = (uintptr_t)serial;
	u = (u << 1) | (uintptr_t)1;
	reader_func ptr = (reader_func)((void *)u);
	return ptr;
}

const prototype_table& get_default_reader()
{
	using std::make_pair;
	static bool initialized = false;
	static prototype_table reader;
	if (!initialized) {
		
		reader[make_pair("sqrt", 1)] = sqrt_reader;
		reader[make_pair("pow", 2)] = pow_reader;
		reader[make_pair("mod", 2)] = mod_reader;
		reader[make_pair("power", 2)] = power_reader;
        reader[make_pair("min", 2)] = min_reader;
        reader[make_pair("max", 2)] = max_reader;
		reader[make_pair("lst", 0)] = lst_reader;
		unsigned serial = 0;
		for (auto & it : registered_functions_hack::get_registered_functions()) {
			prototype proto = make_pair(it.get_name(), it.get_nparams());
			reader[proto] = encode_serial_as_reader_func(serial);
			++serial;
		}
		initialized = true;
	}
	return reader;
}

const prototype_table& get_builtin_reader()
{
	using std::make_pair;
	static bool initialized = false;
	static prototype_table reader;
	if (!initialized) {
		
		reader[make_pair("sqrt", 1)] = sqrt_reader;
		reader[make_pair("pow", 2)] = pow_reader;
		reader[make_pair("mod", 2)] = mod_reader;
		reader[make_pair("power", 2)] = power_reader;
        reader[make_pair("min", 2)] = min_reader;
        reader[make_pair("max", 2)] = max_reader;
		reader[make_pair("lst", 0)] = lst_reader;
		enum {
			log,
			exp,
			sin,
			cos,
			tan,
			asin,
			acos,
			atan,
			sinh,
			cosh,
			tanh,
			asinh,
			acosh,
			atanh,
			atan2,
			Li2,
			Li3,
			zetaderiv,
			Li,
			S,
			H,
			lgamma,
			tgamma,
			beta,
			factorial,
			binomial,
			Order,
			NFUNCTIONS
		};
		auto it = registered_functions_hack::get_registered_functions().begin();
		unsigned serial = 0;
		for ( ; serial<NFUNCTIONS; ++it, ++serial ) {
			prototype proto = make_pair(it->get_name(), it->get_nparams());
			reader[proto] = encode_serial_as_reader_func(serial);
		}
		initialized = true;
	}
	return reader;
}

} // namespace GiNaC
extern template GiNaC::registered_class_info GiNaC::container<std::vector>::reg_info;
extern template GiNaC::registered_class_info GiNaC::container<std::list>::reg_info;
