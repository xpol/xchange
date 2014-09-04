#ifndef MSGPACK_TYPE_JSONCPP_VALUE_HPP__
#define MSGPACK_TYPE_JSONCPP_VALUE_HPP__

#include <msgpack/object.hpp>
#include <json/value.h>

namespace msgpack {

	inline Json::Value& operator>> (const object& o, Json::Value& v)
	{
		switch (o.type)
		{
		case msgpack::type::BOOLEAN: v = o.via.boolean; break;;
		case msgpack::type::POSITIVE_INTEGER: v = static_cast<Json::UInt64>(o.via.u64); break;
		case msgpack::type::NEGATIVE_INTEGER: v = static_cast<Json::Int64>(o.via.i64); break;
		case msgpack::type::DOUBLE: v = o.via.dec; break;
		case msgpack::type::RAW: v = Json::Value(o.via.raw.ptr, o.via.raw.ptr+o.via.raw.size); break;
		case msgpack::type::ARRAY:{
			msgpack::object* ptr = o.via.array.ptr;
			msgpack::object* END = ptr + o.via.array.size;
			for (; ptr < END; ++ptr)
			{
				Json::Value element;
				ptr->convert(&element);
				v.append(element);
			}
		}
			break;
		case msgpack::type::MAP: {
			msgpack::object_kv* ptr = o.via.map.ptr;
			msgpack::object_kv* END = ptr + o.via.map.size;
			for (; ptr < END; ++ptr)
			{
				std::string key(ptr->key.via.raw.ptr, ptr->key.via.raw.size);
				Json::Value& val = v[key];
				ptr->val.convert(&val);
			}
		}
			break;
		case msgpack::type::NIL:
		default:
			v = Json::Value::null; break;
		}
		return v;
	}

	template <typename Stream>
	inline packer<Stream>& operator<< (packer<Stream>& o, const Json::Value& v)
	{
		switch (v.type())
		{
		default:
		case Json::nullValue: return o.pack_nil();
		case Json::intValue:  return o.pack_int64(v.asInt64());
		case Json::uintValue: return o.pack_uint64(v.asUInt64());
		case Json::realValue: return o.pack_double(v.asDouble());
		case Json::stringValue:{
			std::string s(v.asString());
			return o.pack_raw(s.size()).pack_raw_body(s.data(), s.size());
		}
		case Json::booleanValue:return v.asBool() ? o.pack_true() : o.pack_false();
		case Json::arrayValue: {
			o.pack_array(v.size());
			Json::Value::const_iterator i = v.begin(), END = v.end();
			for (; i != END; ++i)
				o.pack(*i);
			return o;
		}
		case Json::objectValue:{
			o.pack_map(v.size());

			Json::Value::const_iterator i = v.begin(), END = v.end();
			for (; i != END; ++i)
			{
				o.pack(i.key().asString());
				o.pack(*i);
			}
			return o;
		}
		}
	}

	inline void operator<< (object::with_zone& o, const Json::Value& v)
	{
		switch (v.type())
		{
		default:
		case Json::nullValue:
			o.type = type::NIL;
			break;
		case Json::intValue:
			o.type = type::NEGATIVE_INTEGER;
			o.via.i64 = v.asInt64();
			break;
		case Json::uintValue:
			o.type = type::POSITIVE_INTEGER;
			o.via.u64 = v.asUInt64();
			break;
		case Json::realValue:
			o.type = type::DOUBLE;
			o.via.dec = v.asDouble();
			break;
		case Json::stringValue:		{
			o.type = type::RAW;
			std::string s(v.asString());

			char* ptr = (char*)o.zone->malloc(s.size());
			o.via.raw.ptr = ptr;
			o.via.raw.size = (uint32_t)s.size();
			memcpy(ptr, s.data(), s.size());
			break;
		}
		case Json::booleanValue:
			o.type = type::BOOLEAN;
			o.via.boolean = v.asBool();
			break;
		case Json::arrayValue:{
			o.type = type::ARRAY;
			if (v.empty()) {
				o.via.array.ptr = NULL;
				o.via.array.size = 0;
			}
			else {
				size_t sz = v.size();
				object* p = (object*)o.zone->malloc(sizeof(object)*sz);
				object* const pend = p + sz;
				o.via.array.ptr = p;
				o.via.array.size = sz;
				Json::Value::const_iterator it = v.begin();
				do {
					*p = object(*it, o.zone);
					++p;
					++it;
				} while (p < pend);
			}
		}
		case Json::objectValue:{
			o.type = type::MAP;
			if (v.empty()) {
				o.via.map.ptr = NULL;
				o.via.map.size = 0;
			}
			else {
				size_t sz = v.size();
				object_kv* p = (object_kv*)o.zone->malloc(sizeof(object_kv)*sz);
				object_kv* const pend = p + sz;
				o.via.map.ptr = p;
				o.via.map.size = sz;
				Json::Value::const_iterator it(v.begin());
				do {
					p->key = object(it.key().asString(), o.zone);
					p->val = object(*it, o.zone);
					++p;
					++it;
				} while (p < pend);
			}
			break;
		}
		}
	}
}


#endif /* msgpack/type/jsoncpp/value.hpp */

