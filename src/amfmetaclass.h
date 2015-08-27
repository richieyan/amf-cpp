#pragma once

#include <string>
#include <memory>

#include "amf.hpp"
#include "serializer.hpp"
#include "deserializer.hpp"

#include "types/amfarray.hpp"
#include "types/amfbool.hpp"
#include "types/amfbytearray.hpp"
#include "types/amfdate.hpp"
#include "types/amfdictionary.hpp"
#include "types/amfdouble.hpp"
#include "types/amfinteger.hpp"
#include "types/amfnull.hpp"
#include "types/amfobject.hpp"
#include "types/amfstring.hpp"
#include "types/amfundefined.hpp"
#include "types/amfvector.hpp"
#include "types/amfxml.hpp"
#include "types/amfxmldocument.hpp"

#include "utils/amfitemptr.hpp"

NS_AMF_BEGIN

enum class TypeMarker : u8 {
	NONE,
	BOOL,
	INT,
	DOUBLE,
	STRING,
	OBJECT,
	VECTOR_INT,
	VECTOR_STRING,
	VECTOR_OBJECT,
};

typedef struct Property_
{
	std::string name;
	TypeMarker marker;
	void* addr;
} Property;

class AmfMetaClass{
public:

	amf::v8 serialize() {
		Serializer s;
		serialize(s);
		return s.data();
	}

	void serialize(Serializer& serializer);

	void deserialize(const v8& buf);
	void deserialize(AmfObject* buf);
	const std::vector<Property>& const getProperties();

protected:
	
	virtual void __registerProperties() = 0;
	void registerProperty(const std::string &name, TypeMarker marker, void* addr);
	void registerDictionaryProperty(const std::string &name, void* addr, TypeMarker keyMarker, TypeMarker valueMarker);

	template<typename T>
	void exDeserialize(const std::string & name, AmfObject* root, std::shared_ptr<T> target) {
		AmfObject& b = root->getSealedProperty<AmfObject>(name);
		target->deserialize(&b);
	}
	
	//template<typename T>
	//void exDeserializeVector(const std::string & name, AmfObject* root, std::vector<shared_ptr<T>> target) {
	//	AmfArray& b = root->getSealedProperty<AmfArray>(name);
	//	b.dense.size();
	//	target->deserialize(&b);
	//}

	//nest object need use exDeserialize
	virtual void __exDeserialize(AmfObject* root) {};

private:
	void __convert(AmfObject* obj, Property &p);
	void __deserializeValue(AmfObject* obj);

private:
	std::vector<Property> properties;
};

class AmfParser {
public:
	static AmfItemPtr parseProperties(const std::vector<Property> & properties_);
private:
	static AmfItemPtr parseProperty(const Property &p);
};


NS_AMF_END