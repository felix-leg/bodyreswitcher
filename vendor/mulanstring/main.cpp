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

#include "mulanstr_config.hpp"


#ifdef MULANSTR_USE_GETTEXT

#include "backend/gettext.cpp"

#else
#	error No defined backend for MuLan String Library
#endif


#include "basic_parse.cpp"
#include "language_features.cpp"
#include "template_object.cpp"
#include "interface.cpp"

