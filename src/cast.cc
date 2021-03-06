/*
 * Copyright (C) 2017 deipi.com LLC and contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "cast.h"

#include "schema.h"


MsgPack
Cast::cast(const MsgPack& obj)
{
	if (obj.size() == 1) {
		const auto str_key = obj.begin()->str();
		switch ((Hash)xxh64::hash(str_key)) {
			case Hash::INTEGER:
				return integer(obj.at(str_key));
			case Hash::POSITIVE:
				return positive(obj.at(str_key));
			case Hash::FLOAT:
				return _float(obj.at(str_key));
			case Hash::BOOLEAN:
				return boolean(obj.at(str_key));
			case Hash::TERM:
			case Hash::TEXT:
			case Hash::STRING:
				return string(obj.at(str_key));
			case Hash::UUID:
				return uuid(obj.at(str_key));
			case Hash::DATE:
				return date(obj.at(str_key));
			case Hash::TIME:
				return time(obj.at(str_key));
			case Hash::TIMEDELTA:
				return timedelta(obj.at(str_key));
			case Hash::EWKT:
				return ewkt(obj.at(str_key));
			case Hash::POINT:
			case Hash::CIRCLE:
			case Hash::CONVEX:
			case Hash::POLYGON:
			case Hash::CHULL:
			case Hash::MULTIPOINT:
			case Hash::MULTICIRCLE:
			case Hash::MULTIPOLYGON:
			case Hash::MULTICHULL:
			case Hash::GEO_COLLECTION:
			case Hash::GEO_INTERSECTION:
				return obj;
			default:
				THROW(CastError, "Unknown cast type %s", str_key.c_str());
		}
	}

	THROW(CastError, "Expected map with one element");
}


MsgPack
Cast::cast(FieldType type, const std::string& field_value)
{
	switch (type) {
		case FieldType::INTEGER:
			try {
				return MsgPack(strict_stoll(field_value));
			} catch (const InvalidArgument& er) {
				THROW(CastError, "Value %s cannot be cast to integer [%s]", field_value.c_str(), er.what());
			} catch (const OutOfRange& er) {
				THROW(CastError, "Value %s cannot be cast to integer [%s]", field_value.c_str(), er.what());
			}
		case FieldType::POSITIVE:
			try {
				return MsgPack(strict_stoull(field_value));
			} catch (const InvalidArgument& er) {
				THROW(CastError, "Value %s cannot be cast to positive [%s]", field_value.c_str(), er.what());
			} catch (const OutOfRange& er) {
				THROW(CastError, "Value %s cannot be cast to positive [%s]", field_value.c_str(), er.what());
			}
		case FieldType::FLOAT:
			try {
				return MsgPack(strict_stod(field_value));
			} catch (const InvalidArgument& er) {
				THROW(CastError, "Value %s cannot be cast to float [%s]", field_value.c_str(), er.what());
			} catch (const OutOfRange& er) {
				THROW(CastError, "Value %s cannot be cast to float [%s]", field_value.c_str(), er.what());
			}
		case FieldType::EMPTY:
			// Try like INTEGER.
			try {
				return MsgPack(strict_stoll(field_value));
			} catch (const InvalidArgument&) {
			} catch (const OutOfRange&) { }

			// Try like POSITIVE.
			try {
				return MsgPack(strict_stoull(field_value));
			} catch (const InvalidArgument&) {
			} catch (const OutOfRange&) { }

			// Try like FLOAT
			try {
				return MsgPack(strict_stod(field_value));
			} catch (const InvalidArgument&) {
			} catch (const OutOfRange&) { }
		default:
			// Default type String.
			return MsgPack(field_value);
	}
}


int64_t
Cast::integer(const MsgPack& obj)
{
	switch (obj.getType()) {
		case MsgPack::Type::POSITIVE_INTEGER:
			return obj.u64();
		case MsgPack::Type::NEGATIVE_INTEGER:
			return obj.i64();
		case MsgPack::Type::FLOAT:
			return obj.f64();
		case MsgPack::Type::STR:
			try {
				return strict_stoll(obj.str());
			} catch (const InvalidArgument& er) {
				THROW(CastError, "Value %s cannot be cast to integer [%s]", obj.getStrType().c_str(), er.what());
			} catch (const OutOfRange& er) {
				THROW(CastError, "Value %s cannot be cast to integer [%s]", obj.getStrType().c_str(), er.what());
			}
		case MsgPack::Type::BOOLEAN:
			return obj.boolean();
		default:
			THROW(CastError, "Type %s cannot be cast to integer", obj.getStrType().c_str());
	}
}


uint64_t
Cast::positive(const MsgPack& obj)
{
	switch (obj.getType()) {
		case MsgPack::Type::POSITIVE_INTEGER:
			return obj.u64();
		case MsgPack::Type::NEGATIVE_INTEGER:
			return obj.i64();
		case MsgPack::Type::FLOAT:
			return obj.f64();
		case MsgPack::Type::STR:
			try {
				return strict_stoull(obj.str());
			} catch (const InvalidArgument& er) {
				THROW(CastError, "Value %s cannot be cast to positive [%s]", obj.getStrType().c_str(), er.what());
			} catch (const OutOfRange& er) {
				THROW(CastError, "Value %s cannot be cast to positive [%s]", obj.getStrType().c_str(), er.what());
			}
		case MsgPack::Type::BOOLEAN:
			return obj.boolean();
		default:
			THROW(CastError, "Type %s cannot be cast to positive", obj.getStrType().c_str());
	}
}


double
Cast::_float(const MsgPack& obj)
{
	switch (obj.getType()) {
		case MsgPack::Type::POSITIVE_INTEGER:
			return obj.u64();
		case MsgPack::Type::NEGATIVE_INTEGER:
			return obj.i64();
		case MsgPack::Type::FLOAT:
			return obj.f64();
		case MsgPack::Type::STR:
			try {
				return strict_stod(obj.str());
			} catch (const InvalidArgument& er) {
				THROW(CastError, "Value %s cannot be cast to float [%s]", obj.getStrType().c_str(), er.what());
			} catch (const OutOfRange& er) {
				THROW(CastError, "Value %s cannot be cast to float [%s]", obj.getStrType().c_str(), er.what());
			}
		case MsgPack::Type::BOOLEAN:
			return obj.boolean();
		default:
			THROW(CastError, "Type %s cannot be cast to float", obj.getStrType().c_str());
	}
}


std::string
Cast::string(const MsgPack& obj)
{
	switch (obj.getType()) {
		case MsgPack::Type::POSITIVE_INTEGER:
			return std::to_string(obj.u64());
		case MsgPack::Type::NEGATIVE_INTEGER:
			return std::to_string(obj.i64());
		case MsgPack::Type::FLOAT:
			return std::to_string(obj.f64());
		case MsgPack::Type::STR:
			return obj.str();
		case MsgPack::Type::BOOLEAN:
			return obj.boolean() ? "true" : "false";
		default:
			return obj.to_string();
	}
}


bool
Cast::boolean(const MsgPack& obj)
{
	switch (obj.getType()) {
		case MsgPack::Type::POSITIVE_INTEGER:
			return obj.u64() != 0;
		case MsgPack::Type::NEGATIVE_INTEGER:
			return obj.i64() != 0;
		case MsgPack::Type::FLOAT:
			return obj.f64() != 0;
		case MsgPack::Type::STR: {
			const char *value = obj.str().c_str();
			switch (value[0]) {
				case '\0':
					return false;
				case '0':
				case 'f':
				case 'F':
					if (value[1] == '\0' || strcasecmp(value, "false") == 0) {
						return false;
					}
				default:
					return true;
			}
		}
		case MsgPack::Type::BOOLEAN:
			return obj.boolean();
		default:
			THROW(CastError, "Type %s cannot be cast to boolean", obj.getStrType().c_str());
	}
}


std::string
Cast::uuid(const MsgPack& obj)
{
	if (obj.is_string()) {
		return obj.str();
	}
	THROW(CastError, "Type %s cannot be cast to uuid", obj.getStrType().c_str());
}


MsgPack
Cast::date(const MsgPack& obj)
{
	switch (obj.getType()) {
		case MsgPack::Type::POSITIVE_INTEGER:
		case MsgPack::Type::NEGATIVE_INTEGER:
		case MsgPack::Type::FLOAT:
		case MsgPack::Type::STR:
		case MsgPack::Type::MAP:
			return obj;
		default:
			THROW(CastError, "Type %s cannot be cast to date", obj.getStrType().c_str());
	}
}


MsgPack
Cast::time(const MsgPack& obj)
{
	switch (obj.getType()) {
		case MsgPack::Type::POSITIVE_INTEGER:
		case MsgPack::Type::NEGATIVE_INTEGER:
		case MsgPack::Type::FLOAT:
		case MsgPack::Type::STR:
			return obj;
		default:
			THROW(CastError, "Type %s cannot be cast to time", obj.getStrType().c_str());
	}
}


MsgPack
Cast::timedelta(const MsgPack& obj)
{
	switch (obj.getType()) {
		case MsgPack::Type::POSITIVE_INTEGER:
		case MsgPack::Type::NEGATIVE_INTEGER:
		case MsgPack::Type::FLOAT:
		case MsgPack::Type::STR:
			return obj;
		default:
			THROW(CastError, "Type %s cannot be cast to timedelta", obj.getStrType().c_str());
	}
}


std::string
Cast::ewkt(const MsgPack& obj)
{
	if (obj.is_string()) {
		return obj.str();
	}
	THROW(CastError, "Type %s cannot be cast to ewkt", obj.getStrType().c_str());
}


FieldType
Cast::getType(const std::string& cast_word)
{
	switch ((Hash)xxh64::hash(cast_word)) {
		case Hash::INTEGER:           return FieldType::INTEGER;
		case Hash::POSITIVE:          return FieldType::POSITIVE;
		case Hash::FLOAT:             return FieldType::FLOAT;
		case Hash::BOOLEAN:           return FieldType::BOOLEAN;
		case Hash::TERM:              return FieldType::TERM;
		case Hash::TEXT:              return FieldType::TEXT;
		case Hash::STRING:            return FieldType::STRING;
		case Hash::UUID:              return FieldType::UUID;
		case Hash::DATE:              return FieldType::DATE;
		case Hash::TIME:              return FieldType::TIME;
		case Hash::TIMEDELTA:         return FieldType::TIMEDELTA;
		case Hash::EWKT:              return FieldType::GEO;
		case Hash::POINT:             return FieldType::GEO;
		case Hash::CIRCLE:            return FieldType::GEO;
		case Hash::CONVEX:            return FieldType::GEO;
		case Hash::POLYGON:           return FieldType::GEO;
		case Hash::CHULL:             return FieldType::GEO;
		case Hash::MULTIPOINT:        return FieldType::GEO;
		case Hash::MULTICIRCLE:       return FieldType::GEO;
		case Hash::MULTIPOLYGON:      return FieldType::GEO;
		case Hash::MULTICHULL:        return FieldType::GEO;
		case Hash::GEO_COLLECTION:    return FieldType::GEO;
		case Hash::GEO_INTERSECTION:  return FieldType::GEO;
		case Hash::CHAI:              return FieldType::SCRIPT;
		case Hash::ECMA:              return FieldType::SCRIPT;
		default:
			THROW(CastError, "Unknown cast type %s", cast_word.c_str());
	}
}
