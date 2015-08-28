#pragma once

#include <string>
#include <sstream>

#include <memory>
#include <stdint.h>

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
	INT64,
	DOUBLE,
	STRING,
	OBJECT,
	VECTOR_INT,
	VECTOR_INT64,
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
		if (root->sealedProperties.find(name) == root->sealedProperties.end()) { 
			return; 
		}
		AmfObject& b = root->getSealedProperty<AmfObject>(name);
		target->deserialize(&b);
	}
	
	template<typename T>
	void exDeserializeVector(const std::string & name, AmfObject* root, std::vector<std::shared_ptr<T>>& target) {
		AmfArray& b = root->getSealedProperty<AmfArray>(name);
		int size = (int)b.dense.size();
		for (int i = 0; i < size; i++) {
			std::shared_ptr<T> item = std::make_shared<T>();
			AmfObject* obj = b.dense.at(i).asPtr<AmfObject>();
			item->deserialize(obj);
			target.push_back(item);
		}
	}

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

	template<typename T>
	static std::string to_string(T value) {
		std::ostringstream oss;
		oss << value;
		return oss.str();
	}
private:
	static AmfItemPtr parseProperty(const Property &p);
};


NS_AMF_END