#ifndef MSGPACK_TYPE_JSONCPP_VALUE_HPP__
#define MSGPACK_TYPE_JSONCPP_VALUE_HPP__

#include <msgpack/object.hpp>
#include <json/value.h>

namespace msgpack {

	inline Json::Value& operator>> (object o, Json::Value& v)
	{
		switch (o.type)
		{
		case msgpack::type::BOOLEAN: v = o.via.boolean; break;;
		case msgpack::type::POSITIVE_INTEGER: v = o.via.u64; break;
		case msgpack::type::NEGATIVE_INTEGER: v = o.via.i64; break;
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
		if (v.isNull())
			return o.pack_nil();
		
		if (v.isBool())
			return v.asBool() ? o.pack_true() : o.pack_false();

		if (v.isInt())
			return o.pack_int(v.asInt());
		if (v.isUInt())
			return o.pack_unsigned_int(v.asUInt());
		if (v.isInt64())
			return o.pack_int64(v.asUInt64());
		if (v.isUInt64())
			return o.pack_uint64(v.asUInt64());
		if (v.isDouble())
			return o.pack_double(v.asDouble());
		if (v.isString())
		{
			std::string s(v.asString());
			return o.pack_raw(s.size()).pack_raw_body(s.data(), s.size());
		}

		if (v.isArray())
		{
			o.pack_array(v.size());
			Json::Value::const_iterator i = v.begin(), END = v.end();
			for (;i != END; ++i)
				o.pack(*i);
			return o;
		}

		if (v.isObject())
		{
			o.pack_map(v.size());

			Json::Value::const_iterator i = v.begin(), END = v.end();
			for (; i != END; ++i)
			{
				o.pack(i.key().asString());
				o.pack(*i);
			}
			return o;
		}
		return o;
	}

	inline void operator<< (object::with_zone& o, const Json::Value& v)
	{
		if (v.isNull())
			o.type = type::NIL;

		else if (v.isBool())
		{
			o.type = type::BOOLEAN;
			o.via.boolean = v.asBool();
		}
		else if (v.isInt())
		{
			o.type = type::NEGATIVE_INTEGER;
			o.via.i64 = v.asInt();
		}
		else if (v.isUInt())
		{
			o.type = type::POSITIVE_INTEGER;
			o.via.u64 = v.asUInt();
		}
		else if (v.isInt64())
		{
			o.type = type::NEGATIVE_INTEGER;
			o.via.i64 = v.asInt64();
		}
		else if (v.isUInt64())
		{
			o.type = type::POSITIVE_INTEGER;
			o.via.u64 = v.asUInt64();
		}
		else if (v.isDouble())
		{
			o.type = type::DOUBLE;
			o.via.dec = v.asDouble();
		}
		else if (v.isString())
		{
			o.type = type::RAW;
			std::string s(v.asString());

			char* ptr = (char*)o.zone->malloc(s.size());
			o.via.raw.ptr = ptr;
			o.via.raw.size = (uint32_t)s.size();
			memcpy(ptr, s.data(), s.size());
		}
		else if (v.isArray())
		{
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

		else if (v.isObject())
		{
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
		}
	}
}


#endif /* msgpack/type/jsoncpp/value.hpp */

