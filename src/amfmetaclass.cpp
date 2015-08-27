#include "amfmetaclass.h"

#include "serializer.hpp"
#include "deserializer.hpp"

NS_AMF_BEGIN

void AmfMetaClass::registerProperty(const std::string &name, TypeMarker marker, void* addr) {
	properties.push_back({ name, marker, addr});
}

void AmfMetaClass::serialize(Serializer& serializer) {
	properties.clear();
	__registerProperties();
	std::shared_ptr<v8> ret = std::make_shared<v8>();
	AmfItemPtr item = AmfParser::parseProperties(properties);
	serializer << (*item);
}

const std::vector<Property> & AmfMetaClass::getProperties(){
	return properties;
}

void AmfMetaClass::__convert(AmfObject* obj, Property &p) {
	switch (p.marker)
	{
	case TypeMarker::BOOL:
	{
		AmfBool& value = obj->getSealedProperty<AmfBool>(p.name);
		(*(bool*)p.addr) = value.value;
		break;
	}
	case TypeMarker::INT:
	{
		AmfInteger& value = obj->getSealedProperty<AmfInteger>(p.name);
		(*(int*)p.addr) = value.value;
		break;
	}
	case TypeMarker::STRING:
	{
		AmfString& value = obj->getSealedProperty<AmfString>(p.name);
		(*(std::string*)p.addr) = value.value;
		break;
	}
	case TypeMarker::DOUBLE:
	{
		AmfDouble& value = obj->getSealedProperty<AmfDouble>(p.name);
		(*(double*)p.addr) = value.value;
		break;
	}
	case TypeMarker::VECTOR_INT:
	{
		AmfBool& value = obj->getSealedProperty<AmfBool>(p.name);
		(*(bool*)p.addr) = value.value;
		break;
	}
	case TypeMarker::VECTOR_STRING:
	{
		AmfBool& value = obj->getSealedProperty<AmfBool>(p.name);
		(*(bool*)p.addr) = value.value;
		break;
	}
	default:
		break;
	}
}


void AmfMetaClass::deserialize(const v8& buf) {
	Deserializer d;
	AmfItemPtr item = d.deserialize(buf);
	AmfObject* obj = item.asPtr<AmfObject>();
	deserialize(obj);
}

void AmfMetaClass::deserialize(AmfObject* obj) {
	properties.clear();
	__registerProperties();
	__deserializeValue(obj);
	__exDeserialize(obj);
}

void AmfMetaClass::__deserializeValue(AmfObject* obj) {
	for (Property& p : properties) {
		__convert(obj, p);
	}
}


//Helper Class: AmfParser 
AmfItemPtr AmfParser::parseProperties(const std::vector<Property> & properties_) {
	AmfItemPtr ret(AmfObject("", true, false));
	AmfObject* obj = ret.asPtr<AmfObject>();
	for (const Property & prop : properties_) {
		obj->addSealedProperty(prop.name, parseProperty(prop));
	}
	return ret;
}

AmfItemPtr AmfParser::parseProperty(const Property &p) {
	switch (p.marker)
	{
	case TypeMarker::BOOL: {
		bool value = *(bool*)p.addr;
		return AmfItemPtr(AmfBool(value));
	}
	case TypeMarker::INT: {
		int value = *(int*)p.addr;
		return AmfItemPtr(AmfInteger(value));
	}
	case TypeMarker::STRING: {
		const std::string & value = *(std::string*)p.addr;
		return AmfItemPtr(AmfString(value));
	}

	case TypeMarker::OBJECT: {
		std::shared_ptr<AmfMetaClass> meta = *(std::shared_ptr<AmfMetaClass>*)p.addr;
		return parseProperties(meta->getProperties());
	}

	case TypeMarker::VECTOR_INT: {
		std::vector<int> & value = *(std::vector<int>*)p.addr;
		return AmfItemPtr(AmfVector<int>(value));
	}
	case TypeMarker::VECTOR_STRING: {
		std::vector<std::string> & value = *(std::vector<std::string>*)p.addr;
		AmfItemPtr item(AmfVector<AmfString>({}, false));
		AmfVector<AmfString>* vec = item.asPtr<AmfVector<AmfString>>();
		for (std::string & s : value) {
			vec->push_back(AmfString(s));
		}
		return item;
	}

	case TypeMarker::VECTOR_OBJECT: {
		std::vector<std::shared_ptr<AmfMetaClass>> & value = *(std::vector<std::shared_ptr<AmfMetaClass>>*)p.addr;
		AmfItemPtr item(AmfVector<AmfObject>({}, false));
		AmfVector<AmfObject>* vec = item.asPtr<AmfVector<AmfObject>>();
		for (std::shared_ptr<AmfMetaClass> & clazz : value) {
			AmfItemPtr obj = parseProperties(clazz->getProperties());
			vec->push_back(obj.as<AmfObject>());
		}
		return item;
	}

	default:
		break;
	}

	AmfItemPtr ptr;
	return ptr;
}


NS_AMF_END