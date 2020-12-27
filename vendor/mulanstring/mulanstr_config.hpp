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

#pragma once
#ifndef MULANSTR_CONFIG
#define MULANSTR_CONFIG

#include <string>

namespace mls {
	
#ifndef MULANSTR_TAG_START
const std::string TAG_START = std::string("%{");
#else
const std::string TAG_START = std::string( MULANSTR_TAG_START );
#endif

#ifndef MULANSTR_TAG_END
const std::string TAG_END = std::string("}%");
#else
const std::string TAG_END = std::string( MULANSTR_TAG_END );
#endif

#ifndef MULANSTR_INNER_TAG_START
const std::string INNER_TAG_START = std::string("{");
#else
const std::string INNER_TAG_START = std::string(MULANSTR_INNER_TAG_START);
#endif

#ifndef MULANSTR_INNER_TAG_END
const std::string INNER_TAG_END = std::string("}");
#else
const std::string INNER_TAG_END = std::string(MULANSTR_INNER_TAG_END);
#endif
	
};

#endif

