/*
mulan-string stands for MUltiLANguage-STRING: an i18n and l10n library.
Copyright (C) 2020 felix-leg <felix.leg@gmail.comu>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef MULANSTR_MAIN_HPP
#define MULANSTR_MAIN_HPP




#include "mulanstr_config.hpp"


#ifdef MULANSTR_USE_GETTEXT

#include "backend/gettext.hpp"

#else
#	error No defined backend for MuLan String Library
#endif


#include "basic_parse.hpp"
#include "language_features.hpp"
#include "template_object.hpp"
#include "interface.hpp"

#ifndef MULANSTR_DONT_USE_UNDERSCORE
#define _(STRING) mls::translate(STRING)
#define _c(CAT,STRING) mls::translate(CAT,STRING)
#define _l(STRING,LOC) mls::translateWithLocale(STRING,LOC)
#define _cl(CAT,STRING,LOC) mls::translateWithLocale(CAT,STRING,LOC)
#endif

#endif

