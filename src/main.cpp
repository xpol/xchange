#include <fstream>
#include <string>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <json/json.h>

#include <msgpack.hpp>
#include "msgpack/type/rapidjson/document.hpp"
#include "msgpack/type/jsoncpp/value.hpp"

using namespace rapidjson;
using namespace msgpack;

bool read_file_contents(const std::string& filename, std::string* contents)
{
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (in.bad())
		return false;

	in.seekg(0, std::ios::end);
	contents->resize(static_cast<size_t>(in.tellg()));
	in.seekg(0, std::ios::beg);
	in.read(&contents->front(), contents->size());
	in.close();
	return true;
}
bool write_file_contents(const std::string& filename, const std::string& contents)
{
	std::ofstream of(filename, std::ios::out | std::ios::binary);
	if (of.bad())
		return false;
	of.write(contents.data(), contents.size());
	return true;
}

bool msgpack_to_rapidjson(const std::string& file)
{
	std::string contents;
	if (!read_file_contents(file, &contents))
		return false;

	msgpack::unpacked msg;
	msgpack::unpack(&msg, contents.data(), contents.size());

	rapidjson::Document doc;

	msg.get().convert(&doc);

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	return write_file_contents(file + ".json", std::string(buffer.GetString(), buffer.GetSize()));
}


bool msgpack_to_jsoncpp(const std::string& file)
{
	std::string contents;
	if (!read_file_contents(file, &contents))
		return false;

	msgpack::unpacked msg;
	msgpack::unpack(&msg, contents.data(), contents.size());

	Json::Value doc;

	msg.get().convert(&doc);

	std::ofstream of(file + ".json", std::ios::out | std::ios::binary);
	Json::StyledWriter styledWriter;
	of << styledWriter.write(doc);
	return true;
}

bool jsoncpp_to_msgpack(const std::string &filename)
{
	Json::Value doc;
	Json::Reader reader;
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	reader.parse(in, doc);

	msgpack::sbuffer sbuf;  // simple buffer
	msgpack::pack(&sbuf, doc);
	std::string mpfile(filename, 0, filename.size() - 5);
	mpfile += ".mpack";

	if (!write_file_contents(mpfile, std::string(sbuf.data(), sbuf.size())))
		return false;

	return true;
}

bool rapidjson_to_msgpack(const std::string &filename)
{
	rapidjson::Document doc;
	std::string contents;
	if (!read_file_contents(filename, &contents))
		return false;
	doc.Parse(contents.data());

	msgpack::sbuffer sbuf;  // simple buffer
	msgpack::pack(&sbuf, doc);
	std::string mpfile(filename, 0, filename.size() - 5);
	mpfile += ".mpack";

	if (!write_file_contents(mpfile, std::string(sbuf.data(), sbuf.size())))
		return false;

	return msgpack_to_rapidjson(mpfile);
}


int main(int argc, char* argv[])
{
	if (argc >= 2)
		rapidjson_to_msgpack(argv[1]);
	return 0;
}

