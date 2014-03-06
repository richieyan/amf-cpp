#include "amftest.hpp"

#include "amf.hpp"
#include "types/amfstring.hpp"

static void isEqual(const std::vector<u8>& expected, const char* value) {
	isEqual(expected, AmfString(value));
}

TEST(StringSerializationTest, EmptyString) {
	isEqual(v8 { 0x06, 0x01 }, "");
}

TEST(StringSerializationTest, Ascii) {
	isEqual(v8 { 0x06, 0x07, 0x62, 0x61, 0x72 }, "bar");
	isEqual(v8 { 0x06, 0x0D, 0x62, 0x6F, 0x6F, 0x66, 0x61, 0x72 }, "boofar");
	isEqual(v8 { 0x06, 0x07, 0x71, 0x75, 0x78 }, "qux");
	isEqual(v8 { 0x06, 0x07, 0x71, 0x75, 0x7A }, "quz");
	isEqual(v8 { 0x06, 0x09, 0x71, 0x75, 0x75, 0x78 }, "quux");
	isEqual(v8 { 0x06, 0x07, 0x22, 0x27, 0x5C }, "\"'\\");
}

TEST(StringSerialization, MultiByteLength) {
	isEqual(v8 { 0x06, 0x84, 0x59, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61 }, std::string(300, 'a').c_str());
}

TEST(StringSerializationTest, Umlaute) {
	isEqual(v8 { 0x06, 0x17, 0xC3, 0xBC, 0x6D, 0x6C, 0xC3, 0xA4, 0xC3, 0xBC, 0x74,
		           0xC3, 0xAB }, "ümläütë");
}

TEST(StringSerializationTest, Unicode) {
	isEqual(v8 { 0x06, 0x29, 0xC4, 0xA7, 0xC4, 0xB8, 0xC3, 0xB0, 0x40, 0xC3, 0xBE,
		           0xC3, 0xA6, 0xC4, 0xB8, 0xC5, 0xBF, 0xE2, 0x80, 0x9C, 0xC3, 0xB0
		         }, "ħĸð@þæĸſ“ð");
	isEqual(v8 { 0x06, 0x59, 0xE2, 0x80, 0x9D, 0x5D, 0xC2, 0xB2, 0xC2, 0xB3, 0xC2,
		           0xB6, 0xC5, 0xA7, 0xE2, 0x86, 0x93, 0xC3, 0xB8, 0xC4, 0xA7, 0xE2,
		           0x80, 0x9D, 0xE2, 0x80, 0x9C, 0xC5, 0x82, 0xC2, 0xB5, 0xC3, 0xA6,
		           0xC3, 0xB0, 0xC2, 0xB5, 0xE2, 0x86, 0x92, 0xC2, 0xB3, 0xC3, 0xB8,
		           0xC4, 0xA7 }, "”]²³¶ŧ↓øħ”“łµæðµ→³øħ");
}

static void deserializesTo(const char* value, const v8& data, int left = 0,
	DeserializationContext* ctx = nullptr) {
	deserialize<AmfString>(value, data, left, ctx);
}

TEST(StringDeserialization, EmptyString) {
	deserializesTo("", { 0x01 });
}

TEST(StringDeserialization, Ascii) {
	deserializesTo("bar", { 0x07, 0x62, 0x61, 0x72 });
	deserializesTo("boofar", { 0x0D, 0x62, 0x6F, 0x6F, 0x66, 0x61, 0x72 });
	deserializesTo("qux", { 0x07, 0x71, 0x75, 0x78 });
	deserializesTo("quz", { 0x07, 0x71, 0x75, 0x7A });
	deserializesTo("quux", { 0x09, 0x71, 0x75, 0x75, 0x78 });
	deserializesTo("\"'\\", { 0x07, 0x22, 0x27, 0x5C });
}

TEST(StringDeserialization, Umlaute) {
	deserializesTo("ümläütë", { 0x17, 0xC3, 0xBC, 0x6D, 0x6C, 0xC3, 0xA4, 0xC3, 0xBC, 0x74, 0xC3, 0xAB });
}

TEST(StringDeserialization, Unicode) {
	deserializesTo("ħĸð@þæĸſ“ð", {
		0x29, 0xC4, 0xA7, 0xC4, 0xB8, 0xC3, 0xB0, 0x40, 0xC3, 0xBE, 0xC3, 0xA6,
		0xC4, 0xB8, 0xC5, 0xBF, 0xE2, 0x80, 0x9C, 0xC3, 0xB0 });
	deserializesTo("”]²³¶ŧ↓øħ”“łµæðµ→³øħ", {
		0x59, 0xE2, 0x80, 0x9D, 0x5D, 0xC2, 0xB2, 0xC2, 0xB3, 0xC2, 0xB6, 0xC5,
		0xA7, 0xE2, 0x86, 0x93, 0xC3, 0xB8, 0xC4, 0xA7, 0xE2, 0x80, 0x9D, 0xE2,
		0x80, 0x9C, 0xC5, 0x82, 0xC2, 0xB5, 0xC3, 0xA6, 0xC3, 0xB0, 0xC2, 0xB5,
		0xE2, 0x86, 0x92, 0xC2, 0xB3, 0xC3, 0xB8, 0xC4, 0xA7 });
}

TEST(StringDeserialization, BytesLeft) {
	deserializesTo("bar", { 0x07, 0x62, 0x61, 0x72, 0x72 }, 1);
	deserializesTo("boofar", { 0x0D, 0x62, 0x6F, 0x6F, 0x66, 0x61, 0x72, 0x66,
		0x61, 0x72 }, 3);
	deserializesTo("qux", { 0x07, 0x71, 0x75, 0x78 }, 0);
}

TEST(StringDeserialization, MultiByteLength) {
	deserializesTo(std::string(300, 'a').c_str(), {
		0x84, 0x59, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61 });
}

TEST(StringDeserialization, EmptyIterator) {
	v8 empty { };
	auto begin = empty.begin();
	DeserializationContext ctx;
	ASSERT_THROW({
		AmfString::deserialize(begin, empty.end(), ctx);
	}, std::out_of_range);
}

TEST(StringDeserialization, StringContext) {
	DeserializationContext ctx;
	deserializesTo("bar", { 0x07, 0x62, 0x61, 0x72 }, 0, &ctx);
	deserializesTo("bar", { 0x00 }, 0, &ctx);
	deserializesTo("foobar", { 0x0D, 0x66, 0x6F, 0x6F, 0x62, 0x61, 0x72 }, 0, &ctx);
	deserializesTo("bar", { 0x00 }, 0, &ctx);
	deserializesTo("qux", { 0x07, 0x71, 0x75, 0x78 }, 0, &ctx);
	deserializesTo("foobar", { 0x02 }, 0, &ctx);
	deserializesTo("qux", { 0x04 }, 0, &ctx);
}
