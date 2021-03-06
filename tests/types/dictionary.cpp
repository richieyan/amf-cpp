#include "amftest.hpp"

#include "amf.hpp"
#include "types/amfarray.hpp"
#include "types/amfbytearray.hpp"
#include "types/amfbool.hpp"
#include "types/amfdictionary.hpp"
#include "types/amfdouble.hpp"
#include "types/amfinteger.hpp"
#include "types/amfnull.hpp"
#include "types/amfobject.hpp"
#include "types/amfstring.hpp"
#include "types/amfundefined.hpp"
#include "types/amfvector.hpp"

static std::vector<v8>::iterator findElement(const v8::const_iterator& start, std::vector<v8>& parts) {
	return std::find_if(parts.begin(), parts.end(), [=] (const v8& part) {
		return std::equal(part.begin(), part.end(), start);
	});
}

static void consistsOf(std::vector<v8> parts, const v8& data) {
	auto it = data.begin();
	while (it != data.end()) {
		if (parts.empty()) {
			FAIL() << "Unexpected elements left in data: "
			       << testing::PrintToString(v8(it, data.end())) << std::endl;
			return;
		}

		std::vector<v8>::iterator found = findElement(it, parts);
		if (found == parts.end()) {
			ADD_FAILURE() << "None of the following values could be found at position "
		                << std::distance(data.begin(), it) << " of data:\n"
										<< testing::PrintToString(v8(it, data.end())) << "\n";
			for (auto part : parts)
				std::cerr << testing::PrintToString(part) << "\n";

			return;
		}

		it += found->size();
		parts.erase(found);
	}

	ASSERT_TRUE(parts.empty());
}

TEST(DictionarySerializationTest, EmptyDictionary) {
	isEqual(v8 { 0x11, 0x01, 0x00 }, AmfDictionary(false, false));
	isEqual(v8 { 0x11, 0x01, 0x00 }, AmfDictionary(true, false));
	isEqual(v8 { 0x11, 0x01, 0x01 }, AmfDictionary(false, true));
	isEqual(v8 { 0x11, 0x01, 0x01 }, AmfDictionary(true, true));
}

TEST(DictionarySerializationTest, IntegerKeys) {
	AmfDictionary d(false, false);
	d.insert(AmfInteger(3), AmfBool(false));

	isEqual(v8 {
		0x11, // AMF_DICTIONARY
		0x03, // 1 element
		0x00, // no weak keys
		0x04, 0x03, // AmfInteger 3
		0x02 // AmfBool false
	}, d);

	d = AmfDictionary(false, false);
	d.insert(AmfInteger(-16384), AmfString("foo"));
	isEqual(v8 {
		0x11, // AMF_DICTIONARY
		0x03, // 1 element
		0x00, // no weak keys
		0x04, 0xFF, 0xFF, 0xC0, 0x00,
		0x06, 0x07, 0x66, 0x6F, 0x6F
	}, d);
}

TEST(DictionarySerializationTest, BooleanKeys) {
	AmfDictionary d(false, false);
	d.insert(AmfBool(true), AmfBool(false));

	isEqual(v8 {
		0x11,
		0x03, // 1 element
		0x00, // no weak keys
		0x03, // AmfBool true
		0x02 // AmfBool false
	}, d);

	d = AmfDictionary(false, true);
	d.insert(AmfBool(false), AmfBool(true));
	isEqual(v8 {
		0x11,
		0x03, // 1 element
		0x01, // weak keys
		0x02, // AmfBool false
		0x03 // AmfBool true
	}, d);
}

TEST(DictionarySerializationTest, NumberKeys) {
	AmfDictionary d(false, true);
	d.insert(AmfDouble(-0.5), AmfInteger(3));
	isEqual(v8 {
		0x11,
		0x03, // 1 element
		0x01, // weak keys
		0x05, 0xBF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // AmfDouble -0.5
		0x04, 0x03 // AmfInteger 3
	}, d);

	d = AmfDictionary(false, false);
	d.insert(AmfDouble(1.2345678912345e+35), AmfArray());
	isEqual(v8 {
		0x11,
		0x03, // 1 element
		0x00, // no weak keys
		0x05, 0x47, 0x37, 0xC6, 0xE3, 0xC0, 0x32, 0xF7, 0x20, // AmfDouble 1.2345678912345e+35
		0x09, 0x01, 0x01 // empty AmfArray
	}, d);
}

TEST(DictionarySerializationTest, IntegerAsStringKeys) {
	AmfDictionary d(true, false);
	d.insert(AmfInteger(3), AmfBool(false));

	isEqual(v8 {
		0x11, // AMF_DICTIONARY
		0x03, // 1 element
		0x00, // no weak keys
		0x06, 0x03, 0x33, // AmfInteger 3 serialized as AmfString "3"
		0x02 // AmfBool false
	}, d);

	d = AmfDictionary(true, false);
	d.insert(AmfInteger(-16384), AmfString("foo"));
	isEqual(v8 {
		0x11, // AMF_DICTIONARY
		0x03, // 1 element
		0x00, // no weak keys
		0x06, 0x0D, 0x2D, 0x31, 0x36, 0x33, 0x38, 0x34,
		0x06, 0x07, 0x66, 0x6F, 0x6F
	}, d);
}

TEST(DictionarySerializationTest, BooleanAsStringKeys) {
	AmfDictionary d(true, false);
	d.insert(AmfBool(true), AmfBool(false));

	isEqual(v8 {
		0x11,
		0x03, // 1 element
		0x00, // no weak keys
		0x06, 0x09, 0x74, 0x72, 0x75, 0x65, // AmfString "true"
		0x02 // AmfBool false
	}, d);

	d = AmfDictionary(true, true);
	d.insert(AmfBool(false), AmfBool(true));
	isEqual(v8 {
		0x11,
		0x03, // 1 element
		0x01, // weak keys
		0x06, 0x0B, 0x66, 0x61, 0x6C, 0x73, 0x65, // AmfString "false"
		0x03 // AmfBool true
	}, d);
}

TEST(DictionarySerializationTest, NumberAsStringKeys) {
	AmfDictionary d(true, true);
	d.insert(AmfDouble(-0.5), AmfInteger(3));
	isEqual(v8 {
		0x11,
		0x03, // 1 element
		0x01, // weak keys
		0x06, 0x09, 0x2D, 0x30, 0x2E, 0x35, // AmfString "-0.5"
		0x04, 0x03 // AmfInteger 3
	}, d);

	d = AmfDictionary(true, false);
	d.insert(AmfDouble(1.2345678912345e+35), AmfArray());
	isEqual(v8 {
		0x11,
		0x03, // 1 element
		0x00, // no weak keys
		// AmfString "1.2345678912345e+35"
		0x06, 0x27, 0x31, 0x2e, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
		0x31, 0x32, 0x33, 0x34, 0x35, 0x65, 0x2b, 0x33, 0x35,
		0x09, 0x01, 0x01 // empty AmfArray
	}, d);
}

TEST(DictionarySerializationTest, NullKeys) {
	AmfDictionary n(false, false);
	n.insert(AmfNull(), AmfNull());
	isEqual(v8 {
		0x11, 0x03, 0x00, 0x01, 0x01
	}, n);

	n.asString = true;

	isEqual(v8 {
		0x11, 0x03, 0x00, 0x06, 0x09, 0x6e, 0x75, 0x6c, 0x6c, 0x01
	}, n);
}

TEST(DictionarySerializationTest, UndefinedKeys) {
	AmfDictionary d(false, false);
	d.insert(AmfUndefined(), AmfUndefined());
	isEqual(v8 {
		0x11, 0x03, 0x00, 0x00, 0x00
	}, d);

	d.asString = true;

	isEqual(v8 {
		0x11, 0x03, 0x00,
		0x06, 0x13, 0x75, 0x6e, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x64,
		0x00
	}, d);
}

TEST(DictionarySerializationTest, MultipleKeys) {
	SerializationContext ctx;
	AmfDictionary d(true, false);
	d.insert(AmfInteger(3), AmfBool(false));
	d.insert(AmfInteger(-16384), AmfString("foo"));
	consistsOf(std::vector<v8> {
		{ // header
			0x11, // AMF_DICTIONARY
			0x05, // 2 elements
			0x00, // no weak keys
		},
		{ // "3" = false
			0x06, 0x03, 0x33,
			0x02,
		},
		{ // "-16384" = "foo"
			0x06, 0x0D, 0x2D, 0x31, 0x36, 0x33, 0x38, 0x34,
			0x06, 0x07, 0x66, 0x6F, 0x6F
		}
	}, d.serialize(ctx));
}

TEST(DictionarySerializationTest, Clear) {
	AmfDictionary d(false, true);
	d.insert(AmfBool(true), AmfInteger(17));
	d.clear();

	isEqual(v8 {
		0x11,
		0x01, // no elements
		0x01  // weak keys
	}, d);

	d.insert(AmfBool(false), AmfDouble(17.0));
	isEqual(v8 {
		0x11,
		0x03, // 1 element
		0x01, // weak keys
		0x02, // AmfBool false
		0x05, 0x40, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // AmfDouble 17.0
	}, d);
}

TEST(DictionarySerializationTest, ComplexObjectKeys) {
	SerializationContext ctx;
	AmfDictionary d(false);
	d.insert(AmfArray(), AmfDictionary(false));
	d.insert(AmfArray(std::vector<AmfInteger> { 1 }), AmfObject("", true, false));

	consistsOf(std::vector<v8> {
		{ // header
			0x11,
			0x05, // 2 elements
			0x00  // no weak keys
		},
		{ // [] = {}
			0x09, 0x01, 0x01, // empty array
			0x11, 0x01, 0x00 // empty dictionary
		},
		{ // [1] = {}
			0x09, 0x03, 0x01, 0x04, 0x01, // AmfArray [1]
			0x0a, 0x0b, 0x01, 0x01 // empty dynamic anonymous object
		}
	}, d.serialize(ctx));


	AmfObject obj("foo", true, false);
	obj.addDynamicProperty("prop", AmfString("val"));

	AmfVector<int> vec { { 1, 2, 3 }, false };

	d = AmfDictionary(false);
	d.insert(obj, vec);
	isEqual({
		0x11,
		0x03, // 1 element
		0x00, // no weak keys
		// key
		0x0a, // AMF_OBJECT
		0x0b, // U29O-traits | dynamic, 0 sealed properties
		0x07, 0x66, 0x6f, 0x6f, // class-name "foo"
		// dynamic-member
		0x09, 0x70, 0x72, 0x6f, 0x70, // UTF-8-vr "prop"
		0x06, 0x07, 0x76, 0x61, 0x6c, // AmfString "val"
		0x01, // end of object (UTF-8-empty)
		// value
		0x0d, 0x07, 0x00, // AmfVector<int> with 3 elements
		0x00, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x02,
		0x00, 0x00, 0x00, 0x03
	}, d);
}

TEST(DictionarySerializationTest, NumberAsStringsDoesntAffectObjects) {
	SerializationContext ctx;
	AmfDictionary d(true);
	d.insert(AmfArray(), AmfDictionary(false));
	d.insert(AmfArray(std::vector<AmfInteger> { 1 }), AmfObject("", true, false));

	consistsOf(std::vector<v8> {
		{ // header
			0x11,
			0x05, // 2 elements
			0x00  // no weak keys
		},
		{ // [] = {}
			0x09, 0x01, 0x01, // empty array
			0x11, 0x01, 0x00 // empty array
		},
		{ // [1] = {}
			0x09, 0x03, 0x01, 0x04, 0x01, // AmfArray [1]
			0x0a, 0x0b, 0x01, 0x01 // empty dynamic anonymous object
		}
	}, d.serialize(ctx));


	AmfObject obj("foo", true, false);
	obj.addDynamicProperty("prop", AmfString("val"));

	AmfVector<int> vec { { 1, 2, 3 }, false };

	d = AmfDictionary(true);
	d.insert(obj, vec);
	isEqual({
		0x11,
		0x03, // 1 element
		0x00, // no weak keys
		// key
		0x0a, // AMF_OBJECT
		0x0b, // U29O-traits | dynamic, 0 sealed properties
		0x07, 0x66, 0x6f, 0x6f, // class-name "foo"
		// dynamic-member
		0x09, 0x70, 0x72, 0x6f, 0x70, // UTF-8-vr "prop"
		0x06, 0x07, 0x76, 0x61, 0x6c, // AmfString "val"
		0x01, // end of object (UTF-8-empty)
		// value
		0x0d, 0x07, 0x00, // AmfVector<int> with 3 elements
		0x00, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x02,
		0x00, 0x00, 0x00, 0x03
	}, d);
}

TEST(DictionarySerializationTest, MultiByteLength) {
	SerializationContext ctx;
	AmfDictionary d(true, false);
	for(int i = 0; i < 300; ++i)
		d.insert(AmfInteger(i), AmfInteger(i));

	// for simplicity, only test header and total length
	v8 expected { 0x11, 0x84, 0x59, 0x00 };
	v8 actual(d.serialize(ctx));
	v8 header(actual.begin(), actual.begin() + 4);

	ASSERT_EQ(2166, actual.size());
	ASSERT_EQ(expected, header);
}

TEST(DictionarySerializationTest, OverwriteKeys) {
	AmfDictionary d(true, false);
	d.insert(AmfBool(false), AmfInteger(3));

	isEqual({
		0x11, 0x03, 0x00,
		0x06, 0x0B, 0x66, 0x61, 0x6C, 0x73, 0x65,
		0x04, 0x03
	}, d);

	d.insert(AmfBool(false), AmfString("foo"));
	isEqual({
		0x11, 0x03, 0x00,
		0x06, 0x0B, 0x66, 0x61, 0x6C, 0x73, 0x65,
		0x06, 0x07, 0x66, 0x6f, 0x6f
	}, d);
}

TEST(DictionarySerializationTest, ToggleAsString) {
	AmfDictionary d(true, false);
	d.insert(AmfBool(false), AmfInteger(3));
	isEqual({
		0x11, 0x03, 0x00,
		0x06, 0x0B, 0x66, 0x61, 0x6C, 0x73, 0x65,
		0x04, 0x03
	}, d);

	d.asString = false;
	isEqual({
		0x11, 0x03, 0x00,
		0x02,
		0x04, 0x03
	}, d);
}

TEST(DictionarySerializationTest, SelfReference) {
	AmfItemPtr ptr(AmfDictionary(false, false));
	ptr.as<AmfDictionary>().values[AmfItemPtr(AmfString("x"))] = ptr;

	SerializationContext ctx;
	isEqual({
		0x11,
		0x03,
		0x00,
		0x06, 0x03, 0x78,
		0x11, 0x00
	}, ptr->serialize(ctx));
}

TEST(DictionarySerializationTest, ReferenceMembers) {
	AmfDictionary d(false, false);
	d.insert(AmfArray(), AmfDictionary(false, false));
	d.insert(AmfDictionary(false, false), AmfArray());

	// Since serialization order is not mandated, we have two possible outcomes
	// when serializing the dictionary.
	v8 data1 {
		0x11, 0x05, 0x00,
		0x09, 0x01, 0x01, 0x11, 0x01, 0x00,
		0x11, 0x04, 0x09, 0x02
	};
	v8 data2 {
		0x11, 0x05, 0x00,
		0x11, 0x01, 0x00, 0x09, 0x01, 0x01,
		0x09, 0x04, 0x11, 0x02
	};

	SerializationContext ctx;
	auto s = d.serialize(ctx);
	ASSERT_TRUE(s == data1 || s == data2);
}

TEST(DictionarySerializationTest, DifferentPropertiesNoReference) {
	AmfDictionary d(false, false);
	AmfDictionary d2(true, false);
	AmfDictionary d3(true, true);

	d.insert(AmfBool(true), AmfUndefined());
	d2.insert(AmfBool(true), AmfUndefined());
	d3.insert(AmfBool(true), AmfUndefined());

	SerializationContext ctx;
	isEqual({ 0x11, 0x03, 0x00, 0x03, 0x00 }, d.serialize(ctx));
	isEqual({
		0x11, 0x03, 0x00,
		0x06, 0x09, 0x74, 0x72, 0x75, 0x65,
		0x00
	}, d2.serialize(ctx));
	isEqual({
		0x11, 0x03, 0x01,
		0x06, 0x00,
		0x00
	}, d3.serialize(ctx));
	isEqual({ 0x11, 0x00 }, d.serialize(ctx));
	isEqual({ 0x11, 0x02 }, d2.serialize(ctx));
	isEqual({ 0x11, 0x04 }, d3.serialize(ctx));
}

TEST(DictionaryEquality, EmptyDictionary) {
	AmfDictionary d0(true);
	AmfDictionary d1(true, false);
	EXPECT_EQ(d0, d1);

	AmfDictionary d2(false);
	EXPECT_NE(d0, d2);
	AmfDictionary d3(false, true);
	EXPECT_NE(d2, d3);
	EXPECT_NE(d0, d3);
}

TEST(DictionaryEquality, SimpleValues) {
	AmfDictionary d0(true), d1(true), d2(false);
	d0.insert(AmfInteger(0), AmfString("foo"));
	d1.insert(AmfInteger(0), AmfString("foo"));
	d2.insert(AmfInteger(0), AmfString("foo"));
	EXPECT_EQ(d0, d1);
	EXPECT_NE(d0, d2);

	d0.insert(AmfString("qux"), AmfByteArray(v8 { 0x00 }));
	EXPECT_NE(d0, d1);
	d1.insert(AmfString("qux"), AmfByteArray(v8 { 0x00 }));
	EXPECT_EQ(d0, d1);

	d0.insert(AmfNull(), AmfUndefined());
	d1.insert(AmfUndefined(), AmfNull());
	EXPECT_NE(d0, d1);

	d0.insert(AmfUndefined(), AmfNull());
	d1.insert(AmfNull(), AmfUndefined());
	EXPECT_EQ(d0, d1);
}

TEST(DictionaryEquality, NestedDictionary) {
	AmfDictionary d0(true), d1(true), i(false);
	i.insert(AmfNull(), AmfUndefined());
	d0.insert(i, i);
	d1.insert(i, i);
	EXPECT_EQ(d0, d1);
}

TEST(DictionaryEquality, MixedTypes) {
	AmfDictionary d0(false);
	AmfArray a;
	AmfVector<int> v;
	AmfUndefined u;
	EXPECT_NE(d0, a);
	EXPECT_NE(d0, v);
	EXPECT_NE(d0, u);
}

TEST(DictionaryDeserialization, EmptyDictionary) {
	deserialize(AmfDictionary(false, false), v8 { 0x11, 0x01, 0x00 });
	deserialize(AmfDictionary(false, true), v8 { 0x11, 0x01, 0x01 });
	deserialize(AmfDictionary(false, true), v8 { 0x11, 0x01, 0x01, 0x01 }, 1);
}

TEST(DictionaryDeserialization, IntegerKeys) {
	AmfDictionary d(false, false);
	d.insert(AmfInteger(3), AmfBool(false));
	v8 data {
		0x11, 0x03, 0x00, 0x04, 0x03, 0x02,
	};
	deserialize(d, data, 0);

	d = AmfDictionary(false, false);
	d.insert(AmfInteger(-16384), AmfString("foo"));
	data =  {
		0x11, 0x03, 0x00,
		0x04, 0xFF, 0xFF, 0xC0, 0x00,
		0x06, 0x07, 0x66, 0x6F, 0x6F
	};
	deserialize(d, data, 0);
}

TEST(DictionaryDeserialization, BoolKeys) {
	AmfDictionary d(false, false);
	d.insert(AmfBool(false), AmfInteger(3));
	v8 data {
		0x11, 0x03, 0x00, 0x02, 0x04, 0x03,
		0xff, 0xff
	};
	deserialize(d, data, 2);
}

TEST(DictionaryDeserialization, StringKeys) {
	AmfDictionary d(false, false);
	d.insert(AmfString("foo"), AmfUndefined());
	v8 data {
		0x11, 0x03, 0x00,
		0x06, 0x07, 0x66, 0x6f, 0x6f,
		0x00
	};
	deserialize(d, data, 0);
}

TEST(DictionaryDeserialization, FlashBug) {
	AmfDictionary d(false, false);
	d.insert(AmfString("x"), AmfString("g"));

	v8 data {
		0x11, 0x03, 0x00, 0x06, 0x03, 0x78, 0x06, 0x03, 0x67
	};

	// Flash deserializes this to a dictionary with .x = undefined ...
	deserialize(d, data, 0);
}

TEST(DictionaryDeserialization, ObjectKeys) {
	AmfDictionary d(false, false);
	d.insert(AmfObject("", true, false), AmfString("foo"));
	v8 data {
		0x11, 0x03, 0x00, 0x0a, 0x0b, 0x01, 0x01, 0x06, 0x07, 0x66, 0x6f, 0x6f
	};
	deserialize(d, data, 0);

	d = AmfDictionary(false, false);
	AmfObject o("", true, false);
	o.addDynamicProperty("bar", AmfInteger(1));
	d.insert(o, AmfString("foo"));
	data = {
		0x11, 0x03, 0x00, 0x0a, 0x0b, 0x01, 0x07, 0x62, 0x61, 0x72, 0x04, 0x01,
		0x01, 0x06, 0x07, 0x66, 0x6f, 0x6f
	};
	deserialize(d, data, 0);
}

TEST(DictionaryDeserialization, NestedDictionary) {
	AmfDictionary d(false, true);
	AmfDictionary val(false, true);
	val.insert(AmfString("true"), AmfBool(true));
	d.insert(AmfDictionary(false, false), val);
	v8 data {
		0x11, 0x03, 0x01,
			0x11, 0x01, 0x00,
			0x11, 0x03, 0x01,
				0x06, 0x09, 0x74, 0x72, 0x75, 0x65,
				0x03
	};
	deserialize(d, data);
}

TEST(DictionaryDeserialization, ObjectsCorrectReferenceOrder) {
	// circular object reference to d is index 0 -> { 0x0a, 0x02 } == o
	AmfDictionary d(false, false);
	AmfObject o("", true, false);
	o.addDynamicProperty("bar", AmfInteger(1));
	d.insert(o, AmfString("foo"));
	d.insert(AmfString("qux"), o);

	v8 data {
		0x11, 0x05, 0x00,
			// key 1
			0x0a, 0x0b, 0x01,
				0x07, 0x62, 0x61, 0x72,
				0x04, 0x01,
				0x01,
			// value 1
			0x06, 0x07, 0x66, 0x6f, 0x6f,
			// key 2
			0x06, 0x07, 0x71, 0x75, 0x78,
			// value 2
			0x0a, 0x02
	};
	deserialize(d, data, 0);
}

TEST(DictionaryDeserialization, NotEnoughBytes) {
	DeserializationContext ctx;

	v8 data = { 0x11 };
	auto it = data.cbegin();
	ASSERT_THROW(AmfDictionary::deserialize(it, data.cend(), ctx), std::out_of_range);

	v8 data2 = { 0x11, 0x01 };
	it = data2.cbegin();
	ASSERT_THROW(AmfDictionary::deserialize(it, data2.cend(), ctx), std::out_of_range);

	v8 data3 = { 0x11, 0x03, 0x00 };
	it = data3.cbegin();
	ASSERT_THROW(AmfDictionary::deserialize(it, data3.cend(), ctx), std::out_of_range);
}

TEST(DictionaryDeserialization, InvalidMarker) {
	DeserializationContext ctx;
	v8 data = { 0x10, 0x01, 0x00 };
	auto it = data.cbegin();
	ASSERT_THROW(AmfDictionary::deserialize(it, data.cend(), ctx), std::invalid_argument);
}

TEST(DictionaryDeserialization, SelfReference) {
	v8 data {
		0x11,
		0x03,
		0x00,
		0x06, 0x03, 0x78,
		0x11, 0x00
	};

	DeserializationContext ctx;
	auto it = data.cbegin();
	AmfItemPtr ptr = AmfDictionary::deserializePtr(it, data.cend(), ctx);
	AmfDictionary & d = ptr.as<AmfDictionary>();

	AmfItemPtr key(new AmfString("x"));
	const AmfItemPtr & inner = d.values.at(key);
	EXPECT_EQ(ptr.get(), inner.get());

	const AmfItemPtr & inner2 = inner.as<AmfDictionary>().values.at(key);
	EXPECT_EQ(ptr.get(), inner2.get());
}

TEST(DictionaryDeserialization, SelfReference2) {
	v8 data {
		0x11,
		0x05,
		0x00,
			0x00,
			0x11, 0x00,
			0x11, 0x00,
			0x00
	};

	DeserializationContext ctx;
	auto it = data.cbegin();
	AmfItemPtr ptr = AmfDictionary::deserializePtr(it, data.cend(), ctx);
	AmfDictionary & d = ptr.as<AmfDictionary>();

	const AmfItemPtr& inner = d.values.at(AmfItemPtr(AmfUndefined()));
	EXPECT_EQ(ptr.get(), inner.get());

	// TODO: segfault due to AmfDictionaryHash
	// const AmfItemPtr& inner2 = d.values.at(ptr);
	// EXPECT_EQ(AmfUndefined(), inner2.as<AmfUndefined>());
}

// Broken due to AmfDictionaryHash recursing
TEST(DictionaryDeserialization, DISABLED_SelfReference3) {
	v8 data {
		0x11, 0x05, 0x00,
		0x11, 0x00,
		0x11, 0x00,
		0x11, 0x00,
		0x00
	};

	DeserializationContext ctx;
	auto it = data.cbegin();
	AmfItemPtr ptr = AmfDictionary::deserializePtr(it, data.cend(), ctx);
}
