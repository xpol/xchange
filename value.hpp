#ifndef MSGPACK_TYPE_RAPIDJSON_GENERICVALUE_HPP__
#define MSGPACK_TYPE_RAPIDJSON_GENERICVALUE_HPP__

#include "msgpack/object.hpp"

#include <rapidjson/document.h>

namespace msgpack {


	template <typename Encoding, typename Allocator>
	inline rapidjson::GenericValue<Encoding, Allocator>& operator>> (object o, rapidjson::GenericValue<Encoding, Allocator>& v)
	{
		switch (o.type)
		{
		case msgpack::type::BOOLEAN: v.SetBool(o.via.boolean); break;;
		case msgpack::type::POSITIVE_INTEGER: v.SetUint64(o.via.u64); break;
		case msgpack::type::NEGATIVE_INTEGER: v.SetInt64(o.via.i64); break;
		case msgpack::type::DOUBLE: v.SetDouble(o.via.dec); break;
		case msgpack::type::RAW: v.SetString(o.via.raw.ptr, o.via.raw.size); break;
		case msgpack::type::ARRAY:
			v.SetArray();
			msgpack::object* ptr = o.via.array.ptr;
			msgpack::object* END = ptr + o.via.array.size;
			for (; ptr < END; ++ptr)
			{
				Value element;
				ptr->convert(&element);
				v.PushBack(element);
			}
			break;
		case msgpack::type::MAP:
			v.SetObject();
			msgpack::object_kv* ptr = o.via.map.ptr;
			msgpack::object_kv* END = ptr + o.via.map.size;
			for (; ptr < END; ++ptr)
			{
				Value key, val;
				ptr->key.convert(&key);
				ptr->val.convert(&val);

				v.AddMember(key, val);
			}
			break;
		case msgpack::type::NIL:
		default:
			v.SetNull(); break;

		}
		return v;
	}

	template <typename Stream, typename Encoding, typename Allocator>
	inline packer<Stream>& operator<< (packer<Stream>& o, const rapidjson::GenericValue<Encoding, Allocator>& v)
	{
		if (v.IsNull())
			return o.pack_nil();
		
		if (v.IsTrue())
			return o.pack_true();

		if (v.IsFalse())
			return o.pack_false();

		if (v.IsInt())
			return o.pack_int(v.GetInt());
		if (v.IsUint())
			return o.pack_unsigned_int(v.GetUint());
		if (v.IsInt64())
			return o.pack_int64(v.GetUint64());
		if (v.IsUint64())
			return o.pack_uint64(v.GetUint64());
		if (v.IsDouble()||v.IsNumber())
			return o.pack_double(v.GetDouble());
		if (v.IsString())
			return o.pack_raw(v.GetStringLength()).pack_raw_body(v.GetString(), v.GetStringLength());

		if (v.IsArray())
		{
			o.pack_array(v.Size());
			rapidjson::Value::ConstValueIterator i = v.Begin(), END = v.End();
			for (;i < END; ++i)
				o.pack(*i);
			return o;
		}

		if (v.IsObject())
		{
			rapidjson::Value::ConstMemberIterator i = v.MemberBegin(), END = v.MemberEnd();
				size_t sz = 0;
			for (; i < END; ++i; ++sz)
				continue;

			o.pack_map(sz);
			i = v.MemberBegin();
			for (; i < END; ++i; ++sz)
			{
				o.pack(i->name);
				o.pack(i->value);
			}
			return o;
		}
		return o;
	}

	template <typename Encoding, typename Allocator>
	inline void operator<< (object::with_zone& o, rapidjson::GenericValue<Encoding, Allocator>& v)
	{
		if (v.IsNull())
			o.type = type::NIL;

		else if (v.IsBool())
		{
			o.type = type::BOOLEAN;
			o.via.boolean = v.GetBool();
		}
		else if (v.IsInt())
		{
			o.type = type::NEGATIVE_INTEGER;
			o.via.i64 = v.GetInt();
		}
		else if (v.IsUint())
		{
			o.type = type::POSITIVE_INTEGER;
			o.via.u64 = v.GetUint();
		}
		else if (v.IsInt64())
		{
			o.type = type::NEGATIVE_INTEGER;
			o.via.i64 = v.GetInt64();
		}
		else if (v.IsUint64())
		{
			o.type = type::POSITIVE_INTEGER;
			o.via.u64 = v.GetUint64();
		}
		else if (v.IsDouble() || v.IsNumber())
		{
			o.type = type::DOUBLE;
			o.via.dec = v.GetDouble();
		}
		else if (v.IsString())
		{
			o.type = type::RAW;
			o.via.raw.ptr = v.GetString();
			o.via.raw.size = v.GetStringLength();
		}
		else if (v.IsArray())
		{
			o.type = type::ARRAY;
			if (v.Empty()) {
				o.via.array.ptr = NULL;
				o.via.array.size = 0;
			}
			else {
				object* p = (object*)o.zone->malloc(sizeof(object)*v.Size());
				object* const pend = p + v.Size();
				o.via.array.ptr = p;
				o.via.array.size = v.Size();
				rapidjson::GenericValue<Encoding, Allocator>::ConstValueIterator it(v.Begin());
				do {
					*p = object(*it, o.zone);
					++p;
					++it;
				} while (p < pend);
			}
		}

		else if (v.IsObject())
		{
			o.type = type::MAP;
			size_t sz = v.MemberEnd() - v.MemberBegin();
			if (sz == 0) {
				o.via.map.ptr = NULL;
				o.via.map.size = 0;
			}
			else {
				object_kv* p = (object_kv*)o.zone->malloc(sizeof(object_kv)*sz);
				object_kv* const pend = p + sz;
				o.via.map.ptr = p;
				o.via.map.size = sz;
				rapidjson::Value::ConstMemberIterator it(v.MemberBegin());
				do {
					p->key = object(it->name, o.zone);
					p->val = object(it->value, o.zone);
					++p;
					++it;
				} while (p < pend);
			}
		}
	}
}


#endif /* msgpack/type/rapidjson/value.hpp */

